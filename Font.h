#pragma once
#include <ft2build.h>
#include FT_FREETYPE_H
#include <string>
#include <unordered_map>
#include <vector>
#include "GlyphInfo.h"

class Font {
public:
    Font(const std::string& filePath, float size);
    ~Font();

    bool isValid() const;
    float ascent() const;
    float descent() const;
    float height() const;

    std::vector<GlyphInfo> shapeText(const std::wstring& text, float letterSpacing = 0.0f, bool keepWords = true) const;

private:
    FT_Face m_face = nullptr;
    float m_size = 0.0f;

    struct WordShape {
        std::vector<GlyphInfo> glyphs;
        float totalAdvance;
    };

    WordShape shapeWord(const std::wstring& word, float letterSpacing) const;
};
