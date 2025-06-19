#include "LayoutEngine.h"
#include "FreeTypeFontMetrics.h"

#include <cctype>   // isspace
#include <unordered_set>
#include <locale>

// 判断是否是空白字符（包括空格、制表符、换行等）
bool isWhitespace(uint32_t codepoint) {
    return codepoint == ' ' || codepoint == '\t' || codepoint == '\n' || codepoint == '\r' || codepoint == '\f' || codepoint == '\v';
}

// 判断是否是常见标点符号
bool isPunctuation(uint32_t codepoint) {
    // 简单示例，扩展可根据需求添加更多符号
    static const std::unordered_set<uint32_t> punctuations = {
        '.', ',', '!', '?', ':', ';', '-', '(', ')', '[', ']', '{', '}', '\"', '\'', L'—', L'–', L'…'
    };
    return punctuations.find(codepoint) != punctuations.end();
}

// 判断是否是英文字符
bool isEnglishChar(uint32_t codepoint) {
    return (codepoint >= 'A' && codepoint <= 'Z') || (codepoint >= 'a' && codepoint <= 'z');
}

// 判断是否是中文字符
bool isChineseChar(uint32_t codepoint) {
    return (codepoint >= 0x4E00 && codepoint <= 0x9FFF); // Unicode范围：中文汉字
}

// 判断是否是单词边界符
bool isWordBoundary(uint32_t codepoint) {
    return isWhitespace(codepoint) || isPunctuation(codepoint);
}

LayoutEngine::LayoutEngine(float maxWidth)
    : m_maxWidth(maxWidth) {}

void LayoutEngine::__layout(FontFace& font, const TextParagraph& paragraph, const TextRun& run, ParagraphLayout& layout)
{
	std::vector<PositionedGlyph> line;
	double y = 0;
	double lineHeight = 0;
	double x = paragraph.indent;

	int currentCharIdx = 0;

	auto finalizeLine = [&]() {
		// 判断当前断行位置是否是英文单词中间，是则回退到单词边界处
		if (!line.empty()) {
			auto lastGlyph = line.back();
			while (currentCharIdx >= 0 && isEnglishChar(run.text[currentCharIdx]) && !isWordBoundary(run.text[currentCharIdx])) {
				line.pop_back();
				currentCharIdx--;
			}


			if (isWordBoundary(run.text[currentCharIdx]))
				currentCharIdx++;


			layout.lines.emplace_back(line);
			y += lineHeight + paragraph.paragraphSpacing;
			line.clear();
			x = paragraph.indent;
			lineHeight = 0;
		}
	};

	std::vector<uint32_t> glyphs = font.getGlyphIndices(run.text);
	std::vector<std::pair<double, double>> advances = font.getAdvances(glyphs);

	for (; currentCharIdx < run.text.size();) {
		uint32_t glyphIndex = glyphs[currentCharIdx];
		double advanceX = advances[currentCharIdx].first + font.letterSpacing;
		if (x + advanceX > m_maxWidth && !line.empty()) {
			finalizeLine();
			continue;
		}

		PositionedGlyph g;
		g.fontId = run.properties.fontIdx;
		g.glyph.glyphIndex = glyphIndex;
		g.glyph.posX = x;
		g.glyph.posY = y + font.ascent();

		RectF bounds = font.boundingRect(glyphIndex);
		g.rect = RectF(x, y, bounds.width, font.ascent() + font.descent());

		line.emplace_back(g);
		++currentCharIdx;
		x += advanceX;
		lineHeight = std::max(lineHeight, font.ascent() + font.descent() + font.lineSpacing);
	}

	if (!line.empty()) {
		finalizeLine();
	}

	layout.totalHeight = y;
	layout.boundingSizeX = m_maxWidth;
	layout.boundingSizeY = y;
}

ParagraphLayout LayoutEngine::layout(const TextParagraph& paragraph) {
    ParagraphLayout layout;
    float y = 0;

    for (const auto& run : paragraph.runs) {
        auto font = FreeTypeFontMetrics::instance().getFace(run.properties.fontIdx);
        if (!font.face) continue;
		if (run.text.empty()) continue;

		font.setSize(run.properties.fontSize);
		font.setLetterSpacing(run.properties.letterSpacing < 0 ? 0 : run.properties.letterSpacing);
		font.setLineSpacing(run.properties.lineSpacing);

		__layout(font, paragraph, run, layout);
    }

    layout.totalHeight = y;
    return layout;
}
