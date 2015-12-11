#include "ShapeDisplay.h"

ShapeDisplay::ShapeDisplay(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
}

ShapeDisplay::~ShapeDisplay()
{

}

void ShapeDisplay::setShape(DISPLAY_SHAPE shape) {
	QImage img;
	switch (shape) {
	case ShapeDisplay::DISPLAY_SHAPE_NONE:
		break;
	case ShapeDisplay::DISPLAY_SHAPE_SIN:
		img.load(":/GenSv/Resources/sin.gif");
		break;
	case ShapeDisplay::DISPLAY_SHAPE_TRIANGLE:
		img.load(":/GenSv/Resources/triangle.gif");
		break;
	case ShapeDisplay::DISPLAY_SHAPE_SAW:
		img.load(":/GenSv/Resources/saw.gif");
		break;
	case ShapeDisplay::DISPLAY_SHAPE_MEANDER:
		img.load(":/GenSv/Resources/meander.gif");
		break;
	default:
		break;
	}
	ui.shapeLabel->setPixmap(QPixmap::fromImage(img));
	//repaint();
}