#include "chart.h"
#include <QPainter>
#include "helpers.h"
#include <limits>
#include <cmath>
const QColor colorBg = QColor(150,150,180);
const QColor colorAxis = QColor(90,90,110);
const QColor colorChartU = QColor(50,60,70);
const QColor colorChartI = QColor(90,60,50);

CChart::CChart() {
    /*
    data.push_back(10.0);
    data.push_back(20.0);
    data.push_back(30.0);
    data.push_back(20.0);
    data.push_back(10.0);
    */
}

void CChart::paintEvent(QPaintEvent* event) {
	QPainter painter(this);

    painter.setRenderHint(QPainter::Antialiasing);
    /*
    QRectF rectangle(10.0, 20.0, 80.0, 60.0);
    int startAngle = 30 * 16;
    int spanAngle = 120 * 16;
    painter.drawArc(rectangle, startAngle, spanAngle);
    */
    QBrush brushBg(colorBg);
    painter.fillRect(painter.window(), brushBg);
    if (dataU.count() > 1 && dataI.count() > 1) {
        const int borderX = 5;
        double dx = (double)(painter.window().width() - 2 * borderX) / (double)(dataU.count() - 1);
        if (dx < 1.0)
            dx = 1.0;


        double minY = std::numeric_limits<double>::max();//DBL_MAX;
        double maxY = std::numeric_limits<double>::min();//DBL_MIN;
        for (int x = 0; x < dataU.count(); x++) {
            if (dataU.at(x) > maxY)
                maxY = dataU.at(x);
            if (dataU.at(x) < minY)
                minY = dataU.at(x);
        }

        for (int x = 0; x < dataI.count(); x++) {
            if (dataI.at(x) > maxY)
                maxY = dataI.at(x);
            if (dataI.at(x) < minY)
                minY = dataI.at(x);
        }

        // Adjust max and min vals
        double adjY = (maxY - minY) * 0.01;
        maxY += adjY;
        minY -= adjY;

        //yScale = painter.window().height() - (maxY - miny);

        int height = painter.window().height();
        double difY = maxY - minY;

        // Draw axes
        QPen penAxis(colorAxis);
        painter.setPen(penAxis);
        penAxis.setWidthF(2);
        double xCur = borderX;
        double yCur = maxY;
        painter.drawLine(xCur, yCur, xCur, minY);

        xCur = borderX;
        yCur = height - round(height * (0.0 - minY) / difY);
        painter.drawLine(xCur, yCur, dx * dataU.count(), yCur);




        QPen penChart(colorChartU);
        penChart.setWidthF(1.5);
        painter.setPen(penChart);
        xCur = borderX;
        yCur = height - round(height * (dataU.at(0) - minY) / difY);

        double yZero = height - round(height * (0.0 - minY) / difY);
        for (int x = 1; x < dataU.count()-1; x++) {
            double xNew = xCur + dx;
            double yNew = height - round(height * (dataU.at(x) - minY) / difY);
            painter.drawLine(xCur, yCur, xNew, yNew);


            /*
            if (!(x%100)) {
                painter.drawLine(xCur, yZero - 3, xCur, yZero + 3);
            }
            */

            xCur = xNew;
            yCur = yNew;
        }

        painter.setPen(penAxis);
        xCur = borderX;
        yCur = 0;//height - round(height * (0.0 - minY) / difY);
        painter.drawLine(xCur, yCur, xCur, height);

        penChart = QPen(colorChartI);
        penChart.setWidthF(1.5);
        painter.setPen(penChart);
        xCur = borderX;
        yCur = height - round(height * (dataI.at(0) - minY) / difY);
        for (int x = 1; x < dataI.count()-1; x++) {
            double xNew = xCur + dx;
            double yNew = height - round(height * (dataI.at(x) - minY) / difY);
            painter.drawLine(xCur, yCur, xNew, yNew);
            xCur = xNew;
            yCur = yNew;
        }
    }
} 
