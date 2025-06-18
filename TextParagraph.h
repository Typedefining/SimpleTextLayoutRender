#pragma once
#include "TextRun.h"
#include <vector>

enum class ParagraphAlignment {
    AlignLeft,
    AlignCenter,
    AlignRight
};

struct TextParagraph {
    std::vector<TextRun> runs;
    ParagraphAlignment align = ParagraphAlignment::AlignLeft;
    float indent = 0.0f;
    float paragraphSpacing = 4.0f;
};
