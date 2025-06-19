#pragma once



#include "Color.h"
#include "PaintDevice.h"
#include "LayoutEngine.h"
#include "FreeTypeFontMetrics.h"

#include <ft2build.h>
#include FT_FREETYPE_H

class PaintDevice;

class PaintEngine {
public:
    PaintEngine(PaintDevice* device) : m_device(device) {}

	void drawLayout(int x, int y, ParagraphLayout& layout, const Color& color)
	{
		auto& ffonts = FreeTypeFontMetrics::instance();
		for (auto& line : layout.lines) {
			for (auto& positioned : line) {
				ffonts.getFace(positioned.fontId).drawFace(this, positioned);
			}
		}
	}

	void drawText(int x, int y, const std::string& text,
							const std::string& fontPath, int fontSize,
							const Color& color) {
		FT_Library ft;
		if (FT_Init_FreeType(&ft)) return;

		FT_Face face;
		if (FT_New_Face(ft, fontPath.c_str(), 0, &face)) {
			FT_Done_FreeType(ft);
			return;
		}

		FT_Set_Pixel_Sizes(face, 0, fontSize);
		int penX = x, penY = y;

		for (char c : text) {
			if (FT_Load_Char(face, c, FT_LOAD_RENDER)) continue;

			FT_GlyphSlot g = face->glyph;
			for (int row = 0; row < g->bitmap.rows; ++row) {
				for (int col = 0; col < g->bitmap.width; ++col) {
					uint8_t alpha = g->bitmap.buffer[row * g->bitmap.pitch + col];
					if (alpha == 0) continue;

					Color blended = color;
					blended.a = (alpha * color.a) / 255;

					int px = penX + g->bitmap_left + col;
					int py = penY - g->bitmap_top + row;
					blendPixel(px, py, blended);
				}
			}

			penX += g->advance.x >> 6; // advance in pixels
		}

		FT_Done_Face(face);
		FT_Done_FreeType(ft);
	}


	// 计算颜色透明度混合的辅助函数
	uint8_t fpart(float x) { return (uint8_t)((x - floor(x)) * 255); }
	uint8_t rfpart(float x) { return 255 - fpart(x); }

	void drawWuLine(float x0, float y0, float x1, float y1, const Color& baseColor) {
		bool steep = abs(y1 - y0) > abs(x1 - x0);
		if (steep) {
			std::swap(x0, y0);
			std::swap(x1, y1);
		}
		if (x0 > x1) {
			std::swap(x0, x1);
			std::swap(y0, y1);
		}

		float dx = x1 - x0;
		float dy = y1 - y0;
		float gradient = dx == 0 ? 1 : dy / dx;

		// 处理第一个端点
		int xEnd = round(x0);
		float yEnd = y0 + gradient * (xEnd - x0);
		float xGap = rfpart(x0 + 0.5f);
		int xPixel1 = xEnd;
		int yPixel1 = (int)floor(yEnd);

		// 画第一个端点像素
		Color c1 = baseColor;
		c1.a = (baseColor.a * rfpart(yEnd) * xGap) / 255;
		if (steep) blendPixel(yPixel1, xPixel1, c1);
		else blendPixel(xPixel1, yPixel1, c1);

		c1.a = (baseColor.a * fpart(yEnd) * xGap) / 255;
		if (steep) blendPixel(yPixel1 + 1, xPixel1, c1);
		else blendPixel(xPixel1, yPixel1 + 1, c1);

		float intery = yEnd + gradient;

		// 处理第二个端点
		int xEnd2 = round(x1);
		float yEnd2 = y1 + gradient * (xEnd2 - x1);
		xGap = fpart(x1 + 0.5f);
		int xPixel2 = xEnd2;
		int yPixel2 = (int)floor(yEnd2);

		Color c2 = baseColor;
		c2.a = (baseColor.a * rfpart(yEnd2) * xGap) / 255;
		if (steep) blendPixel(yPixel2, xPixel2, c2);
		else blendPixel(xPixel2, yPixel2, c2);

		c2.a = (baseColor.a * fpart(yEnd2) * xGap) / 255;
		if (steep) blendPixel(yPixel2 + 1, xPixel2, c2);
		else blendPixel(xPixel2, yPixel2 + 1, c2);

		// 主循环
		for (int x = xPixel1 + 1; x < xPixel2; x++) {
			Color c3 = baseColor;
			c3.a = (baseColor.a * rfpart(intery)) / 255;
			if (steep) blendPixel((int)floor(intery), x, c3);
			else blendPixel(x, (int)floor(intery), c3);

			c3.a = (baseColor.a * fpart(intery)) / 255;
			if (steep) blendPixel((int)floor(intery) + 1, x, c3);
			else blendPixel(x, (int)floor(intery) + 1, c3);

			intery += gradient;
		}
	}


    void blendPixel(int x, int y, const Color& color) {
        if (!m_device) return;
        uint32_t dstARGB = m_device->getPixel(x, y);
        if (color.a == 255) {
            m_device->setPixel(x, y, color.toARGB());
            return;
        } else if (color.a == 0) {
            return;
        }

        Color dst = Color::fromARGB(dstARGB);
        Color blended;
        blended.r = (color.r * color.a + dst.r * (255 - color.a)) / 255;
        blended.g = (color.g * color.a + dst.g * (255 - color.a)) / 255;
        blended.b = (color.b * color.a + dst.b * (255 - color.a)) / 255;
        blended.a = 255;
        m_device->setPixel(x, y, blended.toARGB());
    }

    void fillRect(int x, int y, int w, int h, const Color& color) {
        for (int j = 0; j < h; ++j)
            for (int i = 0; i < w; ++i)
                blendPixel(x + i, y + j, color);
    }

    void drawLine(int x0, int y0, int x1, int y1, const Color& color) {
		drawWuLine(static_cast<float>(x0), static_cast<float>(y0), static_cast<float>(x1), static_cast<float>(y1), color);
        // int dx = std::abs(x1 - x0), dy = std::abs(y1 - y0);
        // int sx = x0 < x1 ? 1 : -1;
        // int sy = y0 < y1 ? 1 : -1;
        // int err = dx - dy;

        // while (true) {
        //     blendPixel(x0, y0, color);
        //     if (x0 == x1 && y0 == y1) break;
        //     int e2 = 2 * err;
        //     if (e2 > -dy) { err -= dy; x0 += sx; }
        //     if (e2 < dx)  { err += dx; y0 += sy; }
        // }
    }

    void fillPolygon(const std::vector<Point>& points, const Color& color) {
        if (points.size() < 3) return;

        int ymin = points[0].y, ymax = points[0].y;
        for (const auto& p : points) {
            ymin = std::min(ymin, p.y);
            ymax = std::max(ymax, p.y);
        }

        for (int y = ymin; y <= ymax; ++y) {
            std::vector<int> nodes;
            for (size_t i = 0, j = points.size() - 1; i < points.size(); j = i++) {
                const Point& pi = points[i];
                const Point& pj = points[j];
                if ((pi.y < y && pj.y >= y) || (pj.y < y && pi.y >= y)) {
                    int x = pi.x + (y - pi.y) * (pj.x - pi.x) / (pj.y - pi.y);
                    nodes.push_back(x);
                }
            }

            std::sort(nodes.begin(), nodes.end());
            for (size_t i = 0; i + 1 < nodes.size(); i += 2) {
                for (int x = nodes[i]; x <= nodes[i + 1]; ++x) {
                    blendPixel(x, y, color);
                }
            }
        }
    }

private:
    PaintDevice* m_device;
};