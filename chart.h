#pragma once
#include <QWidget>

class CChart : public QWidget {
	Q_OBJECT
public:
    CChart();
	void paintEvent(QPaintEvent* event);
    QVector<double> dataU;
    QVector<double> dataI;
};
