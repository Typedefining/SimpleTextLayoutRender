#pragma once
#include "TextParagraph.h"
#include "GlyphInfo.h"
#include <vector>
#include <memory>
#include "FreeTypeFontMetrics.h"

struct FontFace;
struct PositionedGlyph {
    GlyphInfo glyph;
	uint32_t fontId;
	RectF rect;
};

struct ParagraphLayout {
    std::vector<std::vector<PositionedGlyph>> lines;
    float totalHeight = 0;
	int boundingSizeX;
	int boundingSizeY;
};

class LayoutEngine {
public:
    LayoutEngine(float maxWidth);
    ParagraphLayout layout(const TextParagraph& paragraph);

private:
	void __layout(FontFace& font, const TextParagraph& paragraph, const TextRun& run, ParagraphLayout& layout);


    float m_maxWidth;
};
