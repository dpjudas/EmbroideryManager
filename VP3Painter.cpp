
#include "Precomp.h"
#include "VP3Painter.h"
#include "VP3Embroidery.h"
#include <QPainter>
#include <QPainterPath>

void VP3Painter::paint(VP3Embroidery* embroidery, QPainter* painter, int width, int height)
{
	if (width == 0 || height == 0)
		return;

	painter->fillRect(0, 0, width, height, QColor(160, 160, 160));

	int border = 16;
	int w = (embroidery->maxX - embroidery->minX) / 1000.0;
	int h = (embroidery->maxY - embroidery->minY) / 1000.0;
	double zoomW = (width - border) / (double)w;
	double zoomH = (height - border) / (double)h;
	double zoom = std::min(zoomW, zoomH);

	double centerX = width / 2.0;
	double centerY = height / 2.0;

	painter->setBrush(Qt::NoBrush);
	painter->setRenderHint(QPainter::Antialiasing, true);

	/*
	// Margin
	{
		QPainterPath path;
		path.moveTo(centerX + embroidery->minX / 1000.0 * zoom, centerY + embroidery->minY / 1000.0 * zoom);
		path.lineTo(centerX + embroidery->maxX / 1000.0 * zoom, centerY + embroidery->minY / 1000.0 * zoom);
		path.lineTo(centerX + embroidery->maxX / 1000.0 * zoom, centerY + embroidery->maxY / 1000.0 * zoom);
		path.lineTo(centerX + embroidery->minX / 1000.0 * zoom, centerY + embroidery->maxY / 1000.0 * zoom);
		path.closeSubpath();

		QPen blackPen;
		blackPen.setWidth(1.0f);
		blackPen.setColor(QColor(127, 127, 127));
		blackPen.setStyle(Qt::DashLine);
		painter->setPen(blackPen);
		painter->drawPath(path);
	}
	*/

	for (const VP3Design& design : embroidery->designs)
	{
		for (const VP3Thread& thread : design.threads)
		{
			QPainterPath path;

			double x = (design.centerX + thread.startX) / 1000.0;
			double y = -(design.centerY + thread.startY) / 1000.0;
			path.moveTo(centerX + x * zoom, centerY + y * zoom);

			for (const Vp3StitchPoint& point : thread.stitchRun.stitches)
			{
				if (point.type == VP3StitchType::stitch)
				{
					x += point.dx / 10.0;
					y += point.dy / 10.0;
					path.lineTo(centerX + x * zoom, centerY + y * zoom);
				}
				else
				{
					x += point.dx / 10.0;
					y += point.dy / 10.0;
					path.moveTo(centerX + x * zoom, centerY + y * zoom);
				}
			}

			QPen pen;
			pen.setWidth(1.0f);
			pen.setColor(QColor(thread.colors.front().red, thread.colors.front().green, thread.colors.front().blue));
			painter->setPen(pen);
			painter->drawPath(path);
		}
	}
}
