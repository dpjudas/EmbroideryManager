
#include "Precomp.h"
#include "VP3Viewer.h"
#include "VP3Embroidery.h"
#include "VP3Painter.h"
#include <QPainter>
#include <QPainterPath>

VP3Viewer::VP3Viewer(QWidget* parent) : QWidget(parent)
{
}

VP3Viewer::~VP3Viewer()
{
}

QSize VP3Viewer::sizeHint() const
{
	return QSize(400, 400);
}

QSize VP3Viewer::minimumSizeHint() const
{
	return QSize(100, 100);
}

void VP3Viewer::paintEvent(QPaintEvent* event)
{
	QPainter painter(this);

	if (embroidery)
	{
		VP3Painter::paint(embroidery.get(), &painter, width(), height());
	}
	else
	{
		painter.fillRect(0, 0, width(), height(), QColor(160, 160, 160));
	}
}

void VP3Viewer::setEmbroidery(std::unique_ptr<VP3Embroidery> newEmbroidery)
{
	embroidery = std::move(newEmbroidery);
	update();
}
