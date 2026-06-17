#include <cstdint>
#include <cstring>
#include <sys/mman.h>
#include "pl/Gloss.h"
#include "pl/Signature.h"

static const char* GFX_GAMMA_SIGNATURE = "? ? ? 52 ? ? ? ? ? ? ? 91 ? ? ? F9 ? ? ? 38 ? ? ? F8 ? ? ? 91 ? ? ? ? ? ? ? 91 ? ? ? F8 ? ? ? F9 ? ? ? F8";

constexpr uint32_t MOV_W8_10   = 0x52800148;
constexpr uint32_t SCVTF_S2_W8 = 0x1E220102;

constexpr ptrdiff_t OFFSET_MOVK = 12;
constexpr ptrdiff_t OFFSET_FMOV = 16;

static bool PatchMemory(void* addr, uint32_t insn) {
    uintptr_t page_start = (uintptr_t)addr & ~(uintptr_t)4095;
    size_t    page_size  = (sizeof(insn) + 4095) & ~(size_t)4095;
    if (mprotect((void*)page_start, page_size, PROT_READ | PROT_WRITE | PROT_EXEC) != 0)
        return false;
    memcpy(addr, &insn, sizeof(insn));
    __builtin___clear_cache((char*)addr, (char*)addr + sizeof(insn));
    mprotect((void*)page_start, page_size, PROT_READ | PROT_EXEC);
    return true;
}

static bool PatchGfxGamma() {
    uintptr_t base = pl::signature::pl_resolve_signature(GFX_GAMMA_SIGNATURE, "libminecraftpe.so");
    if (base == 0)
        return false;

    if (*reinterpret_cast<uint32_t*>(base + OFFSET_FMOV) != 0x1E2E1002)
        return false;

    return PatchMemory(reinterpret_cast<void*>(base + OFFSET_MOVK), MOV_W8_10)
        && PatchMemory(reinterpret_cast<void*>(base + OFFSET_FMOV), SCVTF_S2_W8);
}

__attribute__((constructor))
void BetterBrightness_Init() {
    GlossInit(true);
    PatchGfxGamma();
}
