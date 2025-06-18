#pragma once

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include <vector>
#include <string>
#include <cstdint>
#include <cmath>
#include <algorithm>
#include <fstream>


class PaintDevice {
public:
    PaintDevice(int w, int h)
        : m_width(w), m_height(h), m_pixels(w * h, 0xFFFFFFFF) {}

    int width() const { return m_width; }
    int height() const { return m_height; }

    uint32_t* pixels() { return m_pixels.data(); }
    const uint32_t* pixels() const { return m_pixels.data(); }

    void clear(uint32_t argb) {
        std::fill(m_pixels.begin(), m_pixels.end(), argb);
    }

    void setPixel(int x, int y, uint32_t argb) {
        if (x < 0 || y < 0 || x >= m_width || y >= m_height) return;
        m_pixels[y * m_width + x] = argb;
    }

    uint32_t getPixel(int x, int y) const {
        if (x < 0 || y < 0 || x >= m_width || y >= m_height) return 0;
        return m_pixels[y * m_width + x];
    }

    void saveToPNG(const std::string& filename) const {
        stbi_write_png(filename.c_str(), m_width, m_height, 4, m_pixels.data(), m_width * 4);
    }

private:
    int m_width, m_height;
    std::vector<uint32_t> m_pixels;
};
