#include "FreeTypeFontMetrics.h"
#include <stdexcept>

FreeTypeFontMetrics::FreeTypeFontMetrics(const std::string& fontPath, int pixelSize)
    : m_pixelSize(pixelSize)
{
    if (FT_Init_FreeType(&m_ftLibrary) != 0) {
        throw std::runtime_error("Failed to initialize FreeType");
    }

    if (FT_New_Face(m_ftLibrary, fontPath.c_str(), 0, &m_face) != 0) {
        throw std::runtime_error("Failed to load font: " + fontPath);
    }

    FT_Set_Pixel_Sizes(m_face, 0, pixelSize);
}

FreeTypeFontMetrics::~FreeTypeFontMetrics()
{
    if (m_face) {
        FT_Done_Face(m_face);
    }
    if (m_ftLibrary) {
        FT_Done_FreeType(m_ftLibrary);
    }
}

std::vector<uint32_t> FreeTypeFontMetrics::getGlyphIndices(const std::string& text) const
{
    std::vector<uint32_t> glyphs;
    for (unsigned char c : text) {
        glyphs.push_back(FT_Get_Char_Index(m_face, c));
    }
    return glyphs;
}

std::vector<double> FreeTypeFontMetrics::getAdvances(const std::vector<uint32_t>& glyphIndices) const
{
    std::vector<double> advances;
    for (uint32_t glyphIndex : glyphIndices) {
        loadGlyph(glyphIndex);
        auto& m = m_metricsCache[glyphIndex];
        advances.push_back(m.horiAdvance / 64.0); // 26.6 fixed-point
    }
    return advances;
}

double FreeTypeFontMetrics::ascent() const
{
    return m_face->size->metrics.ascender / 64.0;
}

double FreeTypeFontMetrics::descent() const
{
    return -m_face->size->metrics.descender / 64.0;
}

RectF FreeTypeFontMetrics::boundingRect(uint32_t glyphIndex) const
{
    loadGlyph(glyphIndex);
    auto& m = m_metricsCache[glyphIndex];
    return {
        m.horiBearingX / 64.0,
        m.horiBearingY / 64.0 - m.height / 64.0,
        m.width / 64.0,
        m.height / 64.0
    };
}

void FreeTypeFontMetrics::loadGlyph(uint32_t glyphIndex) const
{
    if (m_metricsCache.find(glyphIndex) == m_metricsCache.end()) {
        if (FT_Load_Glyph(m_face, glyphIndex, FT_LOAD_DEFAULT) != 0) {
            throw std::runtime_error("Failed to load glyph " + std::to_string(glyphIndex));
        }
        m_metricsCache[glyphIndex] = m_face->glyph->metrics;
    }
}