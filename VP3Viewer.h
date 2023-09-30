#pragma once

#include <QWidget>

class VP3Embroidery;

class VP3Viewer : public QWidget
{
	Q_OBJECT;
public:
	VP3Viewer(QWidget* parent = nullptr);
	~VP3Viewer();

	void setEmbroidery(std::unique_ptr<VP3Embroidery> embroidery);

protected:
	QSize sizeHint() const override;
	QSize minimumSizeHint() const override;
	void paintEvent(QPaintEvent* event) override;

private:
	std::unique_ptr<VP3Embroidery> embroidery;
};
