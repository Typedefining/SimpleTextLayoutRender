#include "TextParagraph.h"
#include "LayoutEngine.h"
#include <iostream>
#include "PaintDevice.h"
#include "PaintEngine.h"

int main() {
    // 准备一个段落
    TextParagraph paragraph;
    paragraph.align = ParagraphAlignment::AlignLeft;
    paragraph.indent = 20.0f;
    paragraph.paragraphSpacing = 10.0f;

    // 添加一个文字 run
    TextRun run;
	run.text = L"这是一个测试段落。";
    run.fontFilePath = "C:/Users/Typedef/Desktop/fontTest/FZLTHK.TTF"; // 请确保该路径存在并修改为你本地的字体路径
    run.fontSize = 24.0f;
    run.letterSpacing = 1.0f;
    run.lineSpacing = 4.0f;
    paragraph.runs.push_back(run);

    // 创建布局引擎，指定最大宽度
    LayoutEngine layoutEngine(300); // 限制段落宽度为 300 像素
    ParagraphLayout layout = layoutEngine.layout(paragraph);

    // 输出排版结果
    std::cout << "Paragraph Layout Results:\n";
    std::cout << "Total height: " << layout.totalHeight << " px\n";

    int lineNum = 1;
    for (const auto& line : layout.lines) {
        std::cout << "Line " << lineNum++ << ": " << line.size() << " glyphs\n";
        for (const auto& g : line) {
            std::cout << "  Glyph Index: " << g.glyph.glyphIndex
                      << ", AdvanceX: " << g.glyph.advanceX
                      << ", Position: (" << g.glyph.posX << ", " << g.baselineY << ")\n";
        }
    }

    PaintDevice device(256, 256);
    PaintEngine paintEngine(&device);

    device.clear(0xFFFFFFFF);

    paintEngine.fillRect(50, 50, 100, 60, Color{200, 255, 0, 0});
    paintEngine.drawLine(10, 10, 240, 200, Color{255, 0, 128, 0});

    std::vector<Point> triangle = { {100, 100}, {150, 200}, {50, 200} };
    paintEngine.fillPolygon(triangle, Color{180, 30, 144, 255});

    device.saveToPNG("output.png");

	device.clear(0xFFFFFFFF);

	paintEngine.drawLayout(0, 0, layout, Color{255, 255, 0, 0});
	device.saveToPNG("sequence.png");

    return 0;
}
