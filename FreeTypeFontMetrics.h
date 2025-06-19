#pragma once

#include <ft2build.h>
#include FT_FREETYPE_H
#include <string>
#include <vector>
#include <unordered_map>
#include <cstdint>
#include "TextRun.h"

class PaintEngine;
class IFontMetricsProvider;
struct GlyphInfo;
struct PositionedGlyph;


struct RectF {
    double x, y, width, height;
};

class IFontMetricsProvider {
public:
    virtual ~IFontMetricsProvider() = default;
    virtual std::vector<uint32_t> getGlyphIndices(const std::wstring& text) = 0;
    virtual std::vector<std::pair<double, double>> getAdvances(const std::vector<uint32_t>& glyphIndices) = 0;
    virtual double ascent() = 0;
    virtual double descent() = 0;
    virtual RectF boundingRect(uint32_t glyphIndex) = 0;
};

struct FontFace : public IFontMetricsProvider {
	std::string path;
	FT_Face face = nullptr;
	float fontSize = 12.0f;
	float letterSpacing = 12.0f;
	float lineSpacing = 12.0f;

	FontFace(const std::string& fontPath, FT_Face ftFace);

	virtual ~FontFace();

	void setSize(float pixelSize) {
		fontSize = pixelSize;
	}
	void setLetterSpacing(float pixelSize) {
		letterSpacing = pixelSize;
	}
	void setLineSpacing(float pixelSize) {
		lineSpacing = pixelSize;
	}

	void drawFace(PaintEngine* paint, const PositionedGlyph& info);
	virtual std::vector<uint32_t> getGlyphIndices(const std::wstring& text) override;
    virtual std::vector<std::pair<double, double>> getAdvances(const std::vector<uint32_t>& glyphIndices) override;
    virtual double ascent() override;
    virtual double descent() override;
    virtual RectF boundingRect(uint32_t glyphIndex) override;
};


class FreeTypeFontMetrics {
    FreeTypeFontMetrics();
public:
    ~FreeTypeFontMetrics();

	static FreeTypeFontMetrics& instance() {
		static FreeTypeFontMetrics instance;
		return instance;
	}

	uint32_t addFace(const std::string& fontPath);
	FontFace getFace(uint32_t fontId);
	uint32_t getFontId(const FontFace& font);

private:
    FT_Library m_ftLibrary = nullptr;
    std::vector<FontFace> m_fontFaces;
    int m_pixelSize = 0;

    mutable std::unordered_map<uint32_t, FT_Glyph_Metrics> m_metricsCache;

};