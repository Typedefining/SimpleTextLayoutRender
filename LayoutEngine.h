#pragma once
#include "TextParagraph.h"
#include "GlyphInfo.h"
#include <vector>
#include "Font.h"
#include <memory>


struct PositionedGlyph {
    GlyphInfo glyph;
    std::shared_ptr<Font> font;
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
