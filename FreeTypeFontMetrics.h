#pragma once

#include <ft2build.h>
#include FT_FREETYPE_H
#include <string>
#include <vector>
#include <unordered_map>
#include <cstdint>

struct RectF {
    double x, y, width, height;
};

class FontMetricsProvider {
public:
    virtual ~FontMetricsProvider() = default;
    virtual std::vector<uint32_t> getGlyphIndices(const std::string& text) const = 0;
    virtual std::vector<double> getAdvances(const std::vector<uint32_t>& glyphIndices) const = 0;
    virtual double ascent() const = 0;
    virtual double descent() const = 0;
    virtual RectF boundingRect(uint32_t glyphIndex) const = 0;
};

class FreeTypeFontMetrics : public FontMetricsProvider {
public:
    FreeTypeFontMetrics(const std::string& fontPath, int pixelSize);
    ~FreeTypeFontMetrics();

    std::vector<uint32_t> getGlyphIndices(const std::string& text) const override;
    std::vector<double> getAdvances(const std::vector<uint32_t>& glyphIndices) const override;
    double ascent() const override;
    double descent() const override;
    RectF boundingRect(uint32_t glyphIndex) const override;

private:
    FT_Library m_ftLibrary = nullptr;
    FT_Face m_face = nullptr;
    int m_pixelSize = 0;

    mutable std::unordered_map<uint32_t, FT_Glyph_Metrics> m_metricsCache;

    void loadGlyph(uint32_t glyphIndex) const;
};