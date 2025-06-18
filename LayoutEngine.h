#pragma once
#include "TextParagraph.h"
#include "GlyphInfo.h"
#include <vector>

struct PositionedGlyph {
    GlyphInfo glyph;
    float baselineY;
};

struct ParagraphLayout {
    std::vector<std::vector<PositionedGlyph>> lines;
    float totalHeight = 0;
};

class LayoutEngine {
public:
    LayoutEngine(float maxWidth);
    ParagraphLayout layout(const TextParagraph& paragraph);

private:
    float m_maxWidth;
};
