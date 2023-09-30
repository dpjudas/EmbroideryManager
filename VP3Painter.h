#pragma once

class VP3Embroidery;
class QPainter;

class VP3Painter
{
public:
	static void paint(VP3Embroidery* embroidery, QPainter* painter, int width, int height);
};
