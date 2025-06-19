#pragma once
#include <string>
#include <map>
#include <cstdint>

struct TextRunProp {
	float fontSize = 12.0f;
	float letterSpacing = 0.0f;
	float lineSpacing = 0.0f;
	uint32_t color = 0xFF000000;
	uint32_t fontIdx = 0;
};

struct TextRun {
    std::wstring text;
	TextRunProp properties;
};
