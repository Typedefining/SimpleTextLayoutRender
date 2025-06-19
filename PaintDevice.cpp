#include "PaintDevice.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include <vector>
#include <string>
#include <cstdint>
#include <cmath>
#include <algorithm>
#include <fstream>


void PaintDevice::saveToPNG(const std::string& filename) const {
	stbi_write_png(filename.c_str(), m_width, m_height, 4, m_pixels.data(), m_width * 4);
}
