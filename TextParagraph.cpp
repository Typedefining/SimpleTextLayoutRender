#include "TextParagraph.h"
#include <QCryptographicHash>
#include <QPainterPath>
#include <QRegularExpression>
#include <algorithm>


ParagraphLayout layoutParagraph(const TextParagraph& paragraph, int maxWidth)
{
	ParagraphLayout result;
	qreal y = 0;
	qreal lineHeight = 0;
	qreal x = paragraph.indent;
	QVector<PositionedGlyph> currentLine;

	auto finalizeLine = [&]() {
		if (!currentLine.isEmpty()) {
			result.lines.append(currentLine);
			y += lineHeight + paragraph.paragraphSpacing;
			currentLine.clear();
			x = paragraph.indent;
			lineHeight = 0;
		}
		};

	for (const auto& run : paragraph.runs) {
		const QRegularExpression re(R"((\S+|\s+))");
		auto it = re.globalMatch(run.text);
		while (it.hasNext()) {
			const auto match = it.next();
			QString word = match.captured(0);

			QVector<quint32> glyphs = run.font.glyphIndexesForString(word);
			QVector<QPointF> advances = run.font.advancesForGlyphIndexes(glyphs);

			qreal wordWidth = 0;
			for (int i = 0; i < advances.size(); ++i)
				wordWidth += advances[i].x() + run.letterSpacing;

			if (x + wordWidth > maxWidth && !currentLine.isEmpty())
				finalizeLine();

			for (int i = 0; i < glyphs.size(); ++i) {
				quint32 glyphIndex = glyphs[i];
				qreal advance = run.font.advancesForGlyphIndexes({ glyphIndex }).last().x() + run.letterSpacing;
				if (x + advance > maxWidth && !currentLine.isEmpty()) {
					finalizeLine();
				}

				PositionedGlyph g;
				g.font = run.font;
				g.glyphIndex = glyphIndex;
				g.pos = QPointF(x, y + run.font.ascent());
				g.fillColor = run.fillColor;
				g.strokeColor = run.strokeColor;
				g.strokeWidth = run.strokeWidth;
				g.backgroundColor = run.backgroundColor;
				g.underline = run.underline;
				g.strikethrough = run.strikethrough;
				g.shadowOffset = run.textShadowOffset;
				g.shadowColor = run.textShadowColor;

				QRectF bounds = run.font.boundingRect(glyphIndex);
				g.background = QRectF(x, y, bounds.width(), run.font.ascent() + run.font.descent());

				currentLine.append(g);
				x += advance;
				lineHeight = std::max(lineHeight, run.font.ascent() + run.font.descent() + run.lineSpacing);
			}
		}
	}

	if (!currentLine.isEmpty()) {
		finalizeLine();
	}

	result.totalHeight = y;
	result.boundingSize = QSize(maxWidth, int(y));
	return result;
}

quint64 ParallelLayoutEngine::hashParagraph(const TextParagraph& paragraph)
{
	QByteArray data;
	for (const auto& run : paragraph.runs)
	{
		data.append(run.text.toUtf8());
		data.append(reinterpret_cast<const char*>(&run.letterSpacing), sizeof(run.letterSpacing));
		data.append(reinterpret_cast<const char*>(&run.lineSpacing), sizeof(run.lineSpacing));
		data.append(reinterpret_cast<const char*>(&run.strokeWidth), sizeof(run.strokeWidth));
		data.append(reinterpret_cast<const char*>(&run.underline), sizeof(run.underline));
		data.append(reinterpret_cast<const char*>(&run.strikethrough), sizeof(run.strikethrough));
		data.append(reinterpret_cast<const char*>(&run.textShadowOffset), sizeof(run.textShadowOffset));
		data.append(reinterpret_cast<const char*>(&run.fillColor), sizeof(run.fillColor));
		data.append(reinterpret_cast<const char*>(&run.strokeColor), sizeof(run.strokeColor));
		data.append(reinterpret_cast<const char*>(&run.backgroundColor), sizeof(run.backgroundColor));
		data.append(reinterpret_cast<const char*>(&run.textShadowColor), sizeof(run.textShadowColor));
	}

	data.append(reinterpret_cast<const char*>(&paragraph.align), sizeof(paragraph.align));
	data.append(reinterpret_cast<const char*>(&paragraph.indent), sizeof(paragraph.indent));
	data.append(reinterpret_cast<const char*>(&paragraph.paragraphSpacing), sizeof(paragraph.paragraphSpacing));

	return qHash(QCryptographicHash::hash(data, QCryptographicHash::Sha256));
}

ParallelLayoutEngine::ParallelLayoutEngine(int maxWidth) : m_maxWidth(maxWidth) {}

QSize ParallelLayoutEngine::layoutParagraphs(const QVector<TextParagraph>& paragraphs)
{
	m_paragraphs = paragraphs;
	m_dirty = true;
	updateLayout();
	return m_totalSize;
}

void ParallelLayoutEngine::updateLayout()
{
	m_renderUnits.clear();
	m_totalSize = QSize(0, 0);
	m_layoutCache.reserve(m_paragraphs.size());

	int x = 0, y = 0;
	int maxHeightInRow = 0;

	for (const auto& paragraph : m_paragraphs)
	{
		quint64 key = hashParagraph(paragraph);
		ParagraphLayout layout;

		if (m_layoutCache.contains(key))
		{
			layout = m_layoutCache[key];
		}
		else
		{
			layout = layoutParagraph(paragraph, m_maxWidth);
			m_layoutCache.insert(key, layout);
		}

		QSize sz = layout.boundingSize;
		if (x + sz.width() > m_maxWidth)
		{
			x = 0;
			y += maxHeightInRow;
			maxHeightInRow = 0;
		}

		ParagraphRenderUnit unit;
		unit.setLayout(layout);
		unit.setOffset(QPoint(x, y));
		m_renderUnits.append(unit);

		x += sz.width();
		maxHeightInRow = std::max(maxHeightInRow, sz.height());
	}

	y += maxHeightInRow;
	m_totalSize = QSize(m_maxWidth, y);
	m_dirty = false;
}

void ParallelLayoutEngine::renderToPainter(QPainter& painter, const QRect&)
{
	for (const auto& unit : m_renderUnits)
	{
		painter.save();
		painter.translate(unit.offset());
		unit.render(painter);
		painter.restore();
	}
}

void ParagraphRenderUnit::render(QPainter& painter) const
{
	for (const auto& line : m_layout.lines)
	{
		for (const auto& glyph : line)
		{
			if (glyph.backgroundColor.alpha() > 0)
			{
				painter.fillRect(glyph.background, glyph.backgroundColor);
			}

			QPainterPath path = glyph.font.pathForGlyph(glyph.glyphIndex);
			QTransform t;
			t.translate(glyph.pos.x(), glyph.pos.y());
			QPainterPath finalPath = t.map(path);

			if (glyph.shadowColor.alpha() > 0)
			{
				QTransform shadowTransform;
				shadowTransform.translate(glyph.shadowOffset.x(), glyph.shadowOffset.y());
				QPainterPath shadowPath = shadowTransform.map(finalPath);
				painter.setPen(Qt::NoPen);
				painter.setBrush(glyph.shadowColor);
				painter.drawPath(shadowPath);
			}

			if (glyph.strokeWidth > 0 && glyph.strokeColor.alpha() > 0)
			{
				painter.setPen(QPen(glyph.strokeColor, glyph.strokeWidth));
				painter.setBrush(Qt::NoBrush);
				painter.drawPath(finalPath);
			}

			if (glyph.fillColor.alpha() > 0)
			{
				painter.setPen(Qt::NoPen);
				painter.setBrush(glyph.fillColor);
				painter.drawPath(finalPath);
			}

			if (glyph.underline || glyph.strikethrough)
			{
				QPen pen(glyph.fillColor, 1.5);
				painter.setPen(pen);
				qreal baseline = glyph.pos.y();
				qreal x1 = glyph.pos.x();
				qreal x2 = x1 + glyph.font.advancesForGlyphIndexes({ glyph.glyphIndex }).last().x();

				if (glyph.underline)
				{
					painter.drawLine(QPointF(x1, baseline + 2), QPointF(x2, baseline + 2));
				}

				if (glyph.strikethrough)
				{
					painter.drawLine(QPointF(x1, baseline - glyph.font.ascent() * 0.4), QPointF(x2, baseline - glyph.font.ascent() * 0.4));
				}
			}
		}
	}
}