#include "TextParagraph.h"
#include "LayoutEngine.h"
#include <iostream>
#include <sstream>
#include "PaintDevice.h"
#include "PaintEngine.h"

int main() {


	FreeTypeFontMetrics::instance().addFace("C:/Users/Typedef/Desktop/fontTest/FZLTHK.TTF");


    std::vector<std::wstring> stringList;
    stringList.emplace_back(L"这是一个测试段落。");
    stringList.emplace_back(L"aa aaa aaaa aaaaa");
    stringList.emplace_back(L"这是一 aa aaa aaaa aaaaa 个测试段落。");

    int i = 0;
    for (const auto& str : stringList) {
        TextRun run;
        run.text = str;
        run.properties.fontIdx = 0; // 使用第一个字体
        run.properties.fontSize = 24.0f; // 字体大小
        run.properties.letterSpacing = 1.0f; // 字符间距
        run.properties.lineSpacing = 4.0f; // 行间距

        // 准备一个段落
        TextParagraph paragraph;
        paragraph.align = ParagraphAlignment::AlignLeft;
        paragraph.indent = 20.0f;
        paragraph.paragraphSpacing = 10.0f;

        paragraph.runs.push_back(run);

        // 创建布局引擎，指定最大宽度
        LayoutEngine layoutEngine(200); // 限制段落宽度为 300 像素
        ParagraphLayout layout = layoutEngine.layout(paragraph);

        // 输出排版结果
        std::cout << "Paragraph Layout Results:\n";
        std::cout << "Total height: " << layout.totalHeight << " px\n";

        int lineNum = 1;
        for (const auto& line : layout.lines) {
            std::cout << "Line " << lineNum++ << ": " << line.size() << " glyphs\n";
            for (const auto& g : line) {
                std::cout << "  Glyph Index: " << g.glyph.glyphIndex
                    << ", Position: (" << g.glyph.posX << ", " << g.glyph.posY << ")\n";
            }
        }

        PaintDevice device(512, 512);
        PaintEngine paintEngine(&device);

        device.clear(0xFFFFFFFF);


        paintEngine.drawLayout(0, 0, layout, Color{ 255, 255, 0, 0 });
        //i转字符串
        std::stringstream ss;
        ss<< i++ << "_sequence.png";

        device.saveToPNG(ss.str());
        device.clear(0xFFFFFFFF);

    }

	
	PaintDevice device(512, 512);
	PaintEngine paintEngine(&device);

    device.clear(0xFFFFFFFF);

    paintEngine.fillRect(50, 50, 100, 60, Color{200, 255, 0, 0});
    paintEngine.drawLine(10, 10, 240, 200, Color{255, 0, 128, 0});

    std::vector<Point> triangle = { {100, 100}, {150, 200}, {50, 200} };
    paintEngine.fillPolygon(triangle, Color{180, 30, 144, 255});

    device.saveToPNG("output.png");

	device.clear(0xFFFFFFFF);
    return 0;
}
