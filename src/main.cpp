#include <cstdint>
#include <cstring>
#include "pl/Gloss.h"
#include "pl/cpp/Signature.hpp"
#include "pl/cpp/Patch.hpp"
#include "pl/cpp/mod/RegisterHelper.hpp"

static const std::string GFX_GAMMA_SIGNATURE = "? ? ? 52 ? ? ? ? ? ? ? 91 ? ? ? F9 ? ? ? 38 ? ? ? F8 ? ? ? 91 ? ? ? ? ? ? ? 91 ? ? ? F8 ? ? ? F9 ? ? ? F8";

constexpr uint32_t MOV_W8_10   = 0x52800148;
constexpr uint32_t SCVTF_S2_W8 = 0x1E220102;

constexpr ptrdiff_t OFFSET_MOVK = 12;
constexpr ptrdiff_t OFFSET_FMOV = 16;

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

        if (*reinterpret_cast<uint32_t*>(base + OFFSET_FMOV) != 0x1E2E1002)
            return false;

        uint32_t mov_w8_10 = MOV_W8_10;
        uint32_t scvtf_s2_w8 = SCVTF_S2_W8;

        return ::pl_patch_write_bytes(base + OFFSET_MOVK, reinterpret_cast<const uint8_t*>(&mov_w8_10), sizeof(mov_w8_10), "better_brightness_movk")
            && ::pl_patch_write_bytes(base + OFFSET_FMOV, reinterpret_cast<const uint8_t*>(&scvtf_s2_w8), sizeof(scvtf_s2_w8), "better_brightness_fmov");
    }
};

static BetterBrightnessMod mod;
PL_REGISTER_MOD(BetterBrightnessMod, mod)
