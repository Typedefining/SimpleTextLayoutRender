#include <QApplication>
#include <QImage>
#include <QPainter>
#include <QFontDatabase>
#include <QRawFont>
#include <QDebug>
#include "TextParagraph.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // 加载字体
    QFont font("微软雅黑", 20);
    QRawFont rawFont = QRawFont::fromFont(font);
    if (!rawFont.isValid()) {
        qWarning("Failed to load raw font");
        return 1;
    }

    // 构造段落列表
    QVector<TextParagraph> paragraphs;

    // 第一个段落：左对齐，带下划线
    TextParagraph para1;
    para1.align = AlignLeft;
    para1.indent = 10;
    para1.paragraphSpacing = 10;

    TextRun run1;
    run1.text = "This is a left-aligned paragraph with underline and spacing.";
    run1.font = rawFont;
    run1.fillColor = Qt::black;
    run1.underline = true;
    run1.letterSpacing = 1.0;
    para1.runs.append(run1);
    paragraphs.append(para1);

    // 第二个段落：居中，描边 + 阴影
    TextParagraph para2;
    para2.align = AlignCenter;

    TextRun run2;
    run2.text = "Centered paragraph with stroke and shadow.";
    run2.font = rawFont;
    run2.fillColor = Qt::blue;
    //run2.strokeColor = Qt::blue;
    run2.strokeWidth = 1.5;
    run2.textShadowOffset = QPointF(2, 2);
    run2.textShadowColor = Qt::red;
    para2.runs.append(run2);
    paragraphs.append(para2);

    // 第三个段落：右对齐，带背景色和删除线
    TextParagraph para3;
    para3.align = AlignRight;

    TextRun run3;
    run3.text = "哈哈哈哈哈哈哈哈哈哈哈哈哈哈哈哈哈哈哈哈哈哈哈哈哈哈哈哈哈哈哈哈哈哈哈哈哈哈哈哈哈哈哈哈哈哈哈哈哈哈哈哈哈哈哈哈哈";
    run3.font = rawFont;
    run3.fillColor = Qt::darkGreen;
    run3.backgroundColor = QColor(255, 255, 0, 160);
    run3.strikethrough = true;
    para3.runs.append(run3);
    paragraphs.append(para3);

    // 初始化布局引擎
    ParallelLayoutEngine engine(600);
    QSize totalSize = engine.layoutParagraphs(paragraphs);

    // 创建图像进行绘制
    QImage image(totalSize, QImage::Format_ARGB32_Premultiplied);
    image.fill(Qt::white);
    QPainter painter(&image);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::TextAntialiasing);

    engine.renderToPainter(painter);

    // 保存结果图像
    QString outputPath = "test_output.png";
    if (!image.save(outputPath)) {
        qWarning("Failed to save image.");
    } else {
        qDebug() << "Image saved to" << outputPath;
    }

    return 0;
}
