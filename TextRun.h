#pragma once
#include <string>
#include <vector>
#include <cstdint>

struct TextRun {
    std::wstring text;
    std::string fontFilePath;
    float fontSize = 12.0f;
    float letterSpacing = 0.0f;
    float lineSpacing = 0.0f;
};
