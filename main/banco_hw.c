/* banco_hw.c — switches fisicos de banco no P4 (JC4880P443C).
   GPIO 29,30,31,32,33,34,35,49 -> BANCO 1..8 (na ordem).
   Fecham no GND (pull-up interno, ativo baixo), com debounce.
   Cada toque seleciona o banco: a tela reflete o estado atual
   (mesmo caminho do "USAR" do painel). */
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "bsp/esp-bsp.h"
#include "palco.h"

static const char *TAG = "banco_hw";

/* sequencia pedida: 29..35 depois 49 (8 bancos) */
static const gpio_num_t PINO_BANCO[8] = {
    GPIO_NUM_29, GPIO_NUM_30, GPIO_NUM_31, GPIO_NUM_32,
    GPIO_NUM_33, GPIO_NUM_34, GPIO_NUM_35, GPIO_NUM_49
};

#define DEBOUNCE_MS 50

void banco_hw_init(void)
{
    for (int i = 0; i < 8; i++) {
        gpio_config_t io = {
            .pin_bit_mask = 1ULL << PINO_BANCO[i],
            .mode = GPIO_MODE_INPUT,
            .pull_up_en = GPIO_PULLUP_ENABLE,
            .pull_down_en = GPIO_PULLDOWN_DISABLE,
            .intr_type = GPIO_INTR_DISABLE,
        };
        gpio_config(&io);
    }
    ESP_LOGI(TAG, "switches de banco: GPIO 29-35,49 -> BANCOS 1-8");
}

/* tarefa: le os pinos, debounce, seleciona o banco (tela reflete) */
static void tarefa_banco(void *arg)
{
    bool solto[8];
    for (int i = 0; i < 8; i++) solto[i] = true;

    while (1) {
        for (int i = 0; i < 8; i++) {
            bool apertado = gpio_get_level(PINO_BANCO[i]) == 0;
            if (apertado && solto[i]) {          /* borda de descida */
                solto[i] = false;
                vTaskDelay(pdMS_TO_TICKS(DEBOUNCE_MS));
                if (gpio_get_level(PINO_BANCO[i]) == 0) {
                    ESP_LOGI(TAG, "pino GPIO%d -> BANCO %d", PINO_BANCO[i], i + 1);
                    if (bsp_display_lock(1000)) {
                        banco_selecionar(i);
                        bsp_display_unlock();
                    }
                }
            } else if (!apertado && !solto[i]) {
                solto[i] = true;                 /* soltou */
            }
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

void banco_hw_iniciar(void)
{
    banco_hw_init();
    xTaskCreate(tarefa_banco, "banco_hw", 4096, NULL, 5, NULL);
}
