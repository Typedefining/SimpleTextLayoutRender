#include "LayoutEngine.h"
#include "Font.h"

LayoutEngine::LayoutEngine(float maxWidth)
    : m_maxWidth(maxWidth) {}

ParagraphLayout LayoutEngine::layout(const TextParagraph& paragraph) {
    ParagraphLayout layout;
    float y = 0;

    for (const auto& run : paragraph.runs) {
        Font font(run.fontFilePath, run.fontSize);
        if (!font.isValid()) continue;

        auto glyphs = font.shapeText(run.text, run.letterSpacing, true);
        std::vector<PositionedGlyph> line;
        float x = paragraph.indent;
        float lineHeight = font.height();

        for (const auto& g : glyphs) {
            if (x + g.advanceX > m_maxWidth && !line.empty()) {
                layout.lines.push_back(line);
                y += lineHeight + run.lineSpacing;
                line.clear();
                x = paragraph.indent;
            }

            line.push_back({ g, y + font.ascent() });
            x += g.advanceX + run.letterSpacing;
        }

        if (!line.empty()) {
            layout.lines.push_back(line);
            y += lineHeight + run.lineSpacing;
        }

        y += paragraph.paragraphSpacing;
    }

    layout.totalHeight = y;
    return layout;
}
