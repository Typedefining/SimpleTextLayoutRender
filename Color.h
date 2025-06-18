#pragma once

#include <cstdint>

struct Color {
    uint8_t a, r, g, b;

    uint32_t toARGB() const {
        return (a << 24) | (r << 16) | (g << 8) | b;
    }

    static Color fromARGB(uint32_t argb) {
        return {
            static_cast<uint8_t>((argb >> 24) & 0xFF),
            static_cast<uint8_t>((argb >> 16) & 0xFF),
            static_cast<uint8_t>((argb >> 8) & 0xFF),
            static_cast<uint8_t>((argb) & 0xFF)
        };
    }
};

struct Point {
    int x, y;
};