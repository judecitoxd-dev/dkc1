#include "dk1/ppu_effects.h"

static uint16_t channel_math(uint16_t a, uint16_t b, bool subtract, bool half) {
    int value = subtract ? (int)a - (int)b : (int)a + (int)b;
    if (value < 0) value = 0;
    if (value > 31) value = 31;
    if (half) value >>= 1;
    return (uint16_t)value;
}

uint16_t dk1_ppu_color_math_bgr555(uint16_t main_color, uint16_t sub_color,
                                    bool subtract, bool half) {
    uint16_t r = channel_math(main_color & 31u, sub_color & 31u, subtract, half);
    uint16_t g = channel_math((main_color >> 5u) & 31u, (sub_color >> 5u) & 31u, subtract, half);
    uint16_t b = channel_math((main_color >> 10u) & 31u, (sub_color >> 10u) & 31u, subtract, half);
    return (uint16_t)(r | (g << 5u) | (b << 10u));
}

Dk1Rgba8 dk1_ppu_apply_brightness(uint16_t color_bgr555, uint8_t brightness) {
    Dk1Rgba8 full = dk1_snes_bgr555_to_rgba(color_bgr555, false);
    Dk1Rgba8 out;
    if (brightness > 15u) brightness = 15u;
    out.r = (uint8_t)(((uint16_t)full.r * brightness + 7u) / 15u);
    out.g = (uint8_t)(((uint16_t)full.g * brightness + 7u) / 15u);
    out.b = (uint8_t)(((uint16_t)full.b * brightness + 7u) / 15u);
    out.a = 255u;
    return out;
}

static bool in_window(uint8_t x, uint8_t left, uint8_t right) {
    return left <= right ? (x >= left && x <= right) : false;
}

static bool apply_window(bool enabled, bool inverted, bool inside) {
    return enabled ? (inverted ? !inside : inside) : false;
}

static uint8_t selections(const Dk1PpuWindowState *state, Dk1PpuLayer layer) {
    if (layer == DK1_PPU_LAYER_BG1) return state->w12sel & 0x0Fu;
    if (layer == DK1_PPU_LAYER_BG2) return (state->w12sel >> 4u) & 0x0Fu;
    if (layer == DK1_PPU_LAYER_BG3) return state->w34sel & 0x0Fu;
    if (layer == DK1_PPU_LAYER_BG4) return (state->w34sel >> 4u) & 0x0Fu;
    if (layer == DK1_PPU_LAYER_OBJ) return state->wobjsel & 0x0Fu;
    return (state->wobjsel >> 4u) & 0x0Fu;
}

static uint8_t logic_code(const Dk1PpuWindowState *state, Dk1PpuLayer layer) {
    if (layer <= DK1_PPU_LAYER_BG4) return (uint8_t)((state->wbglog >> ((uint8_t)layer * 2u)) & 3u);
    return (uint8_t)((state->wobjlog >> (layer == DK1_PPU_LAYER_OBJ ? 0u : 2u)) & 3u);
}

bool dk1_ppu_window_result(const Dk1PpuWindowState *state, Dk1PpuLayer layer,
                           uint8_t x) {
    uint8_t sel;
    bool w1, w2, e1, e2;
    if (state == NULL || layer > DK1_PPU_LAYER_BACKDROP) return false;
    sel = selections(state, layer);
    e1 = (sel & 2u) != 0u;
    e2 = (sel & 8u) != 0u;
    w1 = apply_window(e1, (sel & 1u) != 0u, in_window(x, state->wh0, state->wh1));
    w2 = apply_window(e2, (sel & 4u) != 0u, in_window(x, state->wh2, state->wh3));
    if (!e1) return e2 ? w2 : false;
    if (!e2) return w1;
    switch (logic_code(state, layer)) {
        case 0: return w1 || w2;
        case 1: return w1 && w2;
        case 2: return w1 != w2;
        default: return w1 == w2;
    }
}

int dk1_ppu_mode1_rank(Dk1PpuLayer layer, uint8_t priority,
                       bool bg3_high_priority) {
    priority &= 3u;
    if (layer == DK1_PPU_LAYER_OBJ) {
        static const int object_rank[4] = {2, 5, 8, 11};
        return object_rank[priority];
    }
    if (layer == DK1_PPU_LAYER_BG1) return priority ? 10 : 7;
    if (layer == DK1_PPU_LAYER_BG2) return priority ? 9 : 6;
    if (layer == DK1_PPU_LAYER_BG3) {
        if (bg3_high_priority) return priority ? 12 : 3;
        return priority ? 4 : 1;
    }
    if (layer == DK1_PPU_LAYER_BG4) return priority ? 4 : 1;
    return 0;
}

bool dk1_ppu_select_mode1(const Dk1PpuCandidate *candidates, uint8_t count,
                          bool bg3_high_priority, Dk1PpuCandidate *selected) {
    uint8_t i;
    int best_rank = -1;
    if (candidates == NULL || selected == NULL) return false;
    selected->opaque = false;
    for (i = 0; i < count; ++i) {
        int rank;
        if (!candidates[i].opaque) continue;
        rank = dk1_ppu_mode1_rank(candidates[i].layer, candidates[i].priority, bg3_high_priority);
        if (rank > best_rank) {
            best_rank = rank;
            *selected = candidates[i];
        }
    }
    return best_rank >= 0;
}
