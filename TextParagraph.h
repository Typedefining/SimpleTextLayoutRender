#pragma once
#include <QString>
#include <QColor>
#include <QVector>
#include <QPointF>
#include <QRectF>
#include <QRawFont>
#include <QImage>
#include <QPainter>
#include <QSize>

struct TextRun
{
	QString text;
	QRawFont font;
	QColor fillColor = Qt::black;
	QColor strokeColor = Qt::transparent;
	QColor backgroundColor = Qt::transparent;
	qreal strokeWidth = 0.0;
	qreal letterSpacing = 0.0;
	qreal lineSpacing = 0.0;
	bool underline = false;
	bool strikethrough = false;
	QPointF textShadowOffset;
	QColor textShadowColor = Qt::transparent;
};

enum ParagraphAlignment
{
	AlignLeft,
	AlignCenter,
	AlignRight
};

struct TextParagraph
{
	QVector<TextRun> runs;
	ParagraphAlignment align = AlignLeft;
	qreal indent = 0.0;
	qreal paragraphSpacing = 4.0;
};

struct PositionedGlyph
{
	QRawFont font;
	quint32 glyphIndex = 0;
	QPointF pos;
	QColor fillColor;
	QColor strokeColor;
	QColor backgroundColor;
	QRectF background;
	qreal strokeWidth = 0;
	bool underline = false;
	bool strikethrough = false;
	QPointF shadowOffset;
	QColor shadowColor;
};

struct ParagraphLayout
{
	QVector<QVector<PositionedGlyph>> lines;
	qreal totalHeight = 0;
	QSize boundingSize;
};

ParagraphLayout layoutParagraph(const TextParagraph &paragraph, int maxWidth);

class ParagraphRenderUnit
{
public:
	ParagraphRenderUnit() = default;
	void setLayout(const ParagraphLayout &layout) { m_layout = layout; }
	void setOffset(QPoint offset) { m_offset = offset; }
	QSize size() const { return m_layout.boundingSize; }
	QPoint offset() const { return m_offset; }
	void render(QPainter &painter) const;

private:
	ParagraphLayout m_layout;
	QPoint m_offset;
};

class ParallelLayoutEngine
{
public:
	ParallelLayoutEngine(int maxWidth);
	QSize layoutParagraphs(const QVector<TextParagraph> &paragraphs);
	void renderToPainter(QPainter &painter, const QRect &rect = QRect());

private:
	int m_maxWidth;
	bool m_dirty = true;
	QSize m_totalSize;

	QVector<TextParagraph> m_paragraphs;
	QVector<ParagraphRenderUnit> m_renderUnits;

	QHash<quint64, ParagraphLayout> m_layoutCache;

	void updateLayout();
	quint64 hashParagraph(const TextParagraph &paragraph);
};
