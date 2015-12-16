#pragma once

//#define QWT_NO_MEMARRAY
#include <qwt_plot.h>
#include <qwt_plot_curve.h>

#include <qwt_plot_zoomer.h>
#include <qwt_plot_panner.h>
#include <qwt_plot_magnifier.h>

#include <qwt_scale_widget.h>
namespace CHART_COLORS {
    const QColor background = QColor(200, 200, 220);
    const QColor frame =      QColor(  0,   0,   0);
    const QColor axis =       QColor(120,  80,  80);
    const QColor chart_U =    QColor( 20, 120,  20);
    const QColor chart_I =    QColor(120,  20,  20);

    const QColor grid =       QColor(0xB2, 0xB2, 0xDE);
    const QColor label =      QColor(0x42, 0x42, 0x6E);//QColor(  0x82, 0x82,  0x9E );
    const QColor labelmark =  QColor(0x42, 0x42, 0x6E);

    const QColor backgroundScale = QColor(255, 255, 255, 64);
    const QColor frameScale =      QColor(20,  60,  20, 100);
}

class CPlotData
{
public:
    QVector< double > xVal;
    QVector< double > yVal;
};

class CPlotBackground : public QwtPlotItem
{
public:
    virtual void draw( QPainter *painter, const QwtScaleMap &, const QwtScaleMap &yMap, const QRectF &canvasRect ) const;
};

class CPlotCurve : public QwtPlot {
    Q_OBJECT
public:
    enum CHART_TYPE {
        CURVE = 0,
        HISTOGRAM = 1
    };
    CPlotCurve(const CHART_TYPE ct = CURVE, QWidget * parent = NULL);
    ~CPlotCurve();

    void setData(const unsigned char curveNum, const QVector< double >& x, const QVector< double >& y);
    void doAutoScale();
    void addCurve();
    void setCurveColor(const unsigned int& curve, const QColor& color);
private:
    CHART_TYPE chartType;
    QwtPlotZoomer * zoomer;
    QwtPlotPanner * panner;
    QwtPlotMagnifier * magnifier;
    //QwtScaleMap xMap;
    //QwtScaleMap yMap;
    QVector< CPlotData > curveData;
    QVector< QwtPlotItem * > curves;
    
    virtual void paintEvent(QPaintEvent *event);
};