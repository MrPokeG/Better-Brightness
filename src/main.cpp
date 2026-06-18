#include <cstdint>
#include <cstring>
#include "pl/Gloss.h"
#include "pl/cpp/Signature.hpp"
#include "pl/cpp/Patch.hpp"
#include "pl/cpp/mod/RegisterHelper.hpp"

static const std::string GFX_GAMMA_SIGNATURE = "? ? ? 52 ? ? ? 2F ? ? ? 1E ? ? ? 72 ? ? ? 1E ? ? ? D1 03 01 27 1E ? ? ? D1 E0 03 15 AA ? ? ? 52 ? ? ? 52";

constexpr uint32_t FMOV_S2_ORIGINAL = 0x1E2E1002;
constexpr uint32_t FMOV_S2_25       = 0x1E273002;
constexpr ptrdiff_t OFFSET_FMOV     = 16;

class BetterBrightnessMod {
public:
    bool load() {
        GlossInit(true);
        return PatchGfxGamma();
    }

private:
    bool PatchGfxGamma() {
        uintptr_t base = pl::signature::resolveSignature(GFX_GAMMA_SIGNATURE, "libminecraftpe.so");
        if (base == 0)
            return false;

        if (*reinterpret_cast<uint32_t*>(base + OFFSET_FMOV) != FMOV_S2_ORIGINAL)
            return false;

        uint32_t patch = FMOV_S2_25;
        return ::pl_patch_write_bytes(base + OFFSET_FMOV, reinterpret_cast<const uint8_t*>(&patch), sizeof(patch), "better_brightness_fmov");
    }
};

static BetterBrightnessMod mod;
PL_REGISTER_MOD(BetterBrightnessMod, mod)
