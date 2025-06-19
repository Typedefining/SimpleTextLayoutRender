#include "FreeTypeFontMetrics.h"
#include <stdexcept>
#include "GlyphInfo.h"
#include "LayoutEngine.h"
#include "PaintEngine.h"
#include "Color.h"

FreeTypeFontMetrics::FreeTypeFontMetrics()
{
    if (FT_Init_FreeType(&m_ftLibrary) != 0) {
        throw std::runtime_error("Failed to initialize FreeType");
    }
}

FreeTypeFontMetrics::~FreeTypeFontMetrics()
{
	for (auto& fontFace : m_fontFaces) {
		if (fontFace.face) {
			FT_Done_Face(fontFace.face);
			}
		}
	m_fontFaces.clear();

    if (m_ftLibrary) {
        FT_Done_FreeType(m_ftLibrary);
    }
}

uint32_t FreeTypeFontMetrics::addFace(const std::string& fontPath)
{
	FT_Face face;
    if (!m_ftLibrary || FT_New_Face(m_ftLibrary, fontPath.c_str(), 0, &face) != 0) {
        throw std::runtime_error("Failed to load font: " + fontPath);
    }
	FT_Set_Char_Size(face, 0, 24 * 64, 96, 96);


	m_fontFaces.emplace_back(fontPath, face);
	return static_cast<uint32_t>(m_fontFaces.size() - 1);
}

FontFace FreeTypeFontMetrics::getFace(uint32_t fontId)
{
	if (fontId >= m_fontFaces.size()) {
		throw std::out_of_range("Font ID out of range");
	}
	return m_fontFaces[fontId];
}

uint32_t FreeTypeFontMetrics::getFontId(const FontFace& font)
{
	for (uint32_t i = 0; i < m_fontFaces.size(); ++i) {
		if (m_fontFaces[i].face == font.face) {
			return i;
		}
	}
	throw std::runtime_error("Font not found in metrics provider");
}

void FontFace::drawFace(PaintEngine* paint, const PositionedGlyph& posGlyph)
{
	auto& ffonts = FreeTypeFontMetrics::instance();
	auto font = ffonts.getFace(posGlyph.fontId);
	if (!font.face)
		return;

	if (FT_Load_Glyph(font.face, posGlyph.glyph.glyphIndex, FT_LOAD_RENDER))
		return;

	FT_GlyphSlot g = face->glyph;

	auto posX = posGlyph.glyph.posX;
	auto posY = posGlyph.glyph.posY;

	for (int row = 0; row < g->bitmap.rows; ++row) {
		for (int col = 0; col < g->bitmap.width; ++col) {
			uint8_t alpha = g->bitmap.buffer[row * g->bitmap.pitch + col];
			if (alpha == 0) continue;

			int px = g->bitmap_left + col + posX;
			int py = posY - g->bitmap_top + row;

			Color color = Color::fromARGB(0xFF000000);
			color.a = (alpha * color.a) / 255;
			paint->blendPixel(px, py, color);
		}
	}
}

FontFace::FontFace(const std::string& fontPath, FT_Face ftFace)
	: IFontMetricsProvider(), path(fontPath), face(ftFace){}

FontFace::~FontFace() {

}

std::vector<uint32_t> FontFace::getGlyphIndices(const std::wstring& text)
{
    std::vector<uint32_t> glyphs;
    for (auto c : text) {
        glyphs.push_back(FT_Get_Char_Index(face, c));
    }
    return glyphs;
}

std::vector<std::pair<double, double>> FontFace::getAdvances(const std::vector<uint32_t>& glyphIndices)
{
    std::vector<std::pair<double, double>> advances;
    for (uint32_t glyphIndex : glyphIndices) {
		if (FT_Load_Glyph(face, glyphIndex, FT_LOAD_DEFAULT) != 0) {
            throw std::runtime_error("Failed to load glyph " + std::to_string(glyphIndex));
        }

		auto x = face->glyph->metrics.horiAdvance;
		auto y = face->glyph->metrics.vertAdvance;
        advances.emplace_back(x / 64.0, y / 64.0); // 26.6 fixed-point
    }
    return advances;
}

double FontFace::ascent()
{
    return face->size->metrics.ascender / 64.0;
}

double FontFace::descent()
{
    return -face->size->metrics.descender / 64.0;
}

RectF FontFace::boundingRect(uint32_t glyphIndex)
{
	if (FT_Load_Glyph(face, glyphIndex, FT_LOAD_DEFAULT) != 0) {
		throw std::runtime_error("Failed to load glyph " + std::to_string(glyphIndex));
	}

    auto& m = face->glyph->metrics;
    return {
        m.horiBearingX / 64.0,
        m.horiBearingY / 64.0 - m.height / 64.0,
        m.width / 64.0,
        m.height / 64.0
    };
}
