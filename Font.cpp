#include "Font.h"
#include <stdexcept>
#include <sstream>

static FT_Library ftLibrary = nullptr;

Font::Font(const std::string& filePath, float size) {
    if (!ftLibrary) {
        FT_Init_FreeType(&ftLibrary);
    }

    if (FT_New_Face(ftLibrary, filePath.c_str(), 0, &m_face)) {
        m_face = nullptr;
        return;
    }

    m_size = size;
    FT_Set_Char_Size(m_face, 0, static_cast<FT_F26Dot6>(size * 64), 0, 0);
}

Font::~Font() {
    if (m_face) {
        FT_Done_Face(m_face);
    }
}

bool Font::isValid() const {
    return m_face != nullptr;
}

float Font::ascent() const {
    return m_face ? static_cast<float>(m_face->size->metrics.ascender) / 64.0f : 0.0f;
}

float Font::descent() const {
    return m_face ? static_cast<float>(-m_face->size->metrics.descender) / 64.0f : 0.0f;
}

float Font::height() const {
    return m_face ? static_cast<float>(m_face->size->metrics.height) / 64.0f : 0.0f;
}

Font::WordShape Font::shapeWord(const std::wstring& word, float letterSpacing) const {
    WordShape result;
    float x = 0.0f;

    for (size_t i = 0; i < word.size(); ++i) {
        auto c = word[i];
        FT_UInt glyphIndex = FT_Get_Char_Index(m_face, c);
        if (FT_Load_Glyph(m_face, glyphIndex, FT_LOAD_DEFAULT)) {
            continue;
        }

        float advance = m_face->glyph->advance.x / 64.0f;
        result.glyphs.push_back({ glyphIndex, advance, x, 0 });
        x += advance + letterSpacing;
    }

    result.totalAdvance = x;
    return result;
}

std::vector<GlyphInfo> Font::shapeText(const std::wstring& text, float letterSpacing, bool keepWords) const {
    std::vector<GlyphInfo> result;

    std::wistringstream iss(text);
    std::wstring word;
    float x = 0.0f;

    while (iss >> word) {
        auto shaped = shapeWord(word, letterSpacing);
        for (auto& g : shaped.glyphs) {
            g.posX += x;
            result.push_back(g);
        }
        x += shaped.totalAdvance + letterSpacing;
    }

    return result;
}
