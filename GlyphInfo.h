#pragma once
#include <cstdint>
#include <string>

struct GlyphInfo {
    uint32_t glyphIndex;
    float advanceX;
    float posX, posY;
};
