import { Plugin } from "@opencode/plugin"
import { promises as fs } from "node:fs"
import { join } from "node:path"

const PROJECT = "/home/e/esp/esp32_p4_jc4880p433c_bsp"

async function readText(path: string): Promise<string | undefined> {
  try {
    return await fs.readFile(path, "utf8")
  } catch {
    return undefined
  }
}

async function mtime(path: string): Promise<string | undefined> {
  try {
    const stat = await fs.stat(path)
    return stat.mtime.toISOString()
  } catch {
    return undefined
  }
}

async function gitInfo(): Promise<string> {
  const head = (await readText(join(PROJECT, ".git/HEAD")))?.trim()
  if (!head) return "sem .git/HEAD"
  if (head.startsWith("ref:")) {
    const ref = head.slice(4).trim()
    const hash = (await readText(join(PROJECT, ".git", ref)))?.trim()
    return `${ref.split("/").pop()} @ ${hash ? hash.slice(0, 7) : "?"}`
  }
  return `detached @ ${head.slice(0, 7)}`
}

export default Plugin.define({
  id: "oldskull",
  async setup(ctx) {
    await ctx.tool.transform((editor) => {
      editor.namespace({ name: "oldskull", description: "OldSkull project helpers" })
      editor.add({
        name: "status",
        description: "Report OldSkull git state, build artifact times, and newest source files",
        input: { type: "object", properties: {}, additionalProperties: false },
        options: { namespace: "oldskull", codemode: true },
        execute: async () => {
          try {
            const git = await gitInfo()
            const bin = await mtime(join(PROJECT, "build/esp_lvgl_oldskull.bin"))
            const elf = await mtime(join(PROJECT, "build/esp_lvgl_oldskull.elf"))
            const spiffs = await mtime(join(PROJECT, "build/spiffs_image.bin"))

            const mainDir = join(PROJECT, "main")
            const entries = await fs.readdir(mainDir, { withFileTypes: true })
            const sources = await Promise.all(
              entries
                .filter((e) => e.isFile() && e.name.endsWith(".c"))
                .map(async (e) => ({ name: e.name, mtime: await mtime(join(mainDir, e.name)) })),
            )
            sources.sort((a, b) => (b.mtime ?? "").localeCompare(a.mtime ?? ""))
            const newest = sources.slice(0, 5).map((s) => `${s.name} (${s.mtime})`)

            const lines = [
              `Projeto: ${PROJECT}`,
              `Git: ${git}`,
              `Binario: ${bin ?? "ausente"}`,
              `Elf: ${elf ?? "ausente"}`,
              `SPIFFS: ${spiffs ?? "ausente"}`,
              `Fontes mais novos em main/:`,
              ...newest.map((s) => `  - ${s}`),
            ]
            return { content: lines.join("\n") }
          } catch (error) {
            return { content: `oldskull_status falhou: ${String(error)}` }
          }
        },
      })
    })
  },
})
