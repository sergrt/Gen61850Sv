#include "PlotCurve.h"

#include <QPainter>

#include <qwt_symbol.h>
#include <qwt_scale_map.h>
#include <qwt_plot_grid.h>
#include <qwt_plot_histogram.h>
#include <qwt_series_data.h>
#include <qwt_scale_engine.h>

void CPlotBackground::draw( QPainter *painter, const QwtScaleMap &, const QwtScaleMap &yMap, const QRectF &canvasRect ) const {
    QColor c(CHART_COLORS::background);
    QRectF r = canvasRect;
    painter->fillRect( r, c );
}

CPlotCurve::CPlotCurve( const CHART_TYPE ct, QWidget * parent )
    : QwtPlot(parent) {
    chartType = ct;
    setAutoReplot( false );
    
    // Set up grid
    QwtPlotGrid * g	= new QwtPlotGrid();
    g->setPen( QPen( CHART_COLORS::grid ) );
    g->attach( this );
    
    // Set up background
    CPlotBackground * bg = new CPlotBackground;
    bg->attach( this );
    
    //addCurve();
    //setAutoReplot( true );

    // Set up magnifier
    magnifier = new QwtPlotMagnifier( canvas() );
    magnifier->setAxisEnabled( QwtPlot::yLeft, true );
    magnifier->setAxisEnabled( QwtPlot::xBottom, true );
    magnifier->setMouseButton( Qt::NoButton );
    
    // Set up zoomer
    zoomer = new QwtPlotZoomer( QwtPlot::xBottom, QwtPlot::yLeft, canvas() );
    zoomer->zoom( 0 );
    zoomer->setEnabled( true );

    // RightButton: zoom out by 1
    // Ctrl+RightButton: zoom out to full size
    zoomer->setMousePattern( QwtEventPattern::MouseSelect2,
        Qt::RightButton, Qt::ControlModifier );
    zoomer->setMousePattern( QwtEventPattern::MouseSelect3,
        Qt::RightButton );

    // Set up panner
    panner = new QwtPlotPanner( canvas() );
    panner->setMouseButton( Qt::MidButton );
    panner->setEnabled( true );

    addCurve();

    //this->setAxisTitle(QwtPlot::xBottom, tr("t, сек"));
    axisScaleEngine(QwtPlot::xBottom)->setAttribute(QwtScaleEngine::Floating,true);
    setAutoReplot( true );
}

void CPlotCurve::addCurve()
{
    curveData.push_back(CPlotData());
    
    //
    //  define curve styles
    //
    if (chartType == CURVE) {
        QwtPlotCurve * curve = new QwtPlotCurve();	

        curve->setSymbol( new QwtSymbol( QwtSymbol::NoSymbol, Qt::NoBrush, QPen( Qt::black ), QSize( 5, 5 ) ) );
        curve->setRenderHint(QwtPlotItem::RenderAntialiased);


        curve->setStyle( QwtPlotCurve::Lines );
        //curve->setCurveAttribute( QwtPlotCurve::Fitted ); 

        //curve->setYAxis( QwtPlot::yLeft );
        //curve->setXAxis( QwtPlot::xBottom );

        // Attach data
        curve->setRawSamples( curveData[ 0 ].xVal.data(), curveData[ 0 ].yVal.data(), curveData[ 0 ].xVal.size() );

        if ( curves.count() == 0 ) {
            curve->setYAxis( QwtPlot::yLeft );
        } else {
            // Убрал вторую ось, чтобы совпадали нули и вообще :)
            curve->setYAxis( QwtPlot::yRight );
            this->enableAxis( QwtPlot::yRight, true );
        }

        curve->setVisible( true );
        curve->attach( this );
        
        curves.push_back( curve );
    }
    else if ( chartType == HISTOGRAM )
    {
        QwtPlotHistogram * h = new QwtPlotHistogram();
        h->setStyle( QwtPlotHistogram::Columns );
        QwtColumnSymbol * symbol = new QwtColumnSymbol( QwtColumnSymbol::Box );
        symbol->setFrameStyle( QwtColumnSymbol::Plain );

        h->setSymbol( symbol );
        
        QVector<QwtIntervalSample> samples( curveData[ 0 ].xVal.size() );

        h->setData( new QwtIntervalSeriesData( samples ) );
        curves.push_back( h );
        h->setVisible( true );
        h->attach( this );
    }
}

CPlotCurve::~CPlotCurve(void) {
}

void CPlotCurve::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    painter.setRenderHint( QPainter::Antialiasing, curves[ 0 ]->testRenderHint( QwtPlotItem::RenderAntialiased ) );	
    replot();
}
void CPlotCurve::setData(const unsigned char curveNum, const QVector< double >& x, const QVector< double >& y) {
    if (curveNum >= curves.size())
        throw(tr("Случилось что-то нехорошее"));

    curveData[curveNum].xVal = x;
    curveData[curveNum].yVal = y;

    if (chartType == CURVE) {
        QwtPlotCurve* curve = dynamic_cast<QwtPlotCurve*>(curves[curveNum]);
        if (curve) {
            curve->setRawSamples(curveData[curveNum].xVal.data(), curveData[curveNum].yVal.data(), x.size());
            zoomer->setZoomBase(false);
        }
    } else if (chartType == HISTOGRAM) {
        QwtPlotHistogram* histogram = dynamic_cast<QwtPlotHistogram*>(curves[ curveNum ]);
        if (histogram) {
            QVector<QwtIntervalSample> samples(curveData[curveNum].xVal.size());
            for (int i  = 0; i < curveData[ curveNum ].xVal.size() - 1; i++) {
                QwtInterval interval(curveData[curveNum].xVal[i], curveData[curveNum].xVal[i + 1]);
                interval.setBorderFlags(QwtInterval::ExcludeMaximum);
                samples[i] = QwtIntervalSample(curveData[curveNum].yVal[i], interval);
            }
            histogram->setData(new QwtIntervalSeriesData(samples));
            zoomer->setZoomBase(false);
        }
    }
    repaint();
}

void CPlotCurve::doAutoScale() {
    setAxisAutoScale(QwtPlot::xBottom, true);
    setAxisAutoScale(QwtPlot::yLeft, true);
    setAxisAutoScale(QwtPlot::yRight, true);
    repaint();
}

void CPlotCurve::setCurveColor(const unsigned int& curve, const QColor& color) {
    if (curves.count() > curve) {
        if (chartType == CURVE) {
            ((QwtPlotCurve*)curves.at(curve))->setPen(color);
        } else { //chartType == HISTOGRAM
            QPalette p = palette();
            p.setColor(QPalette::Window, color);
            //((QwtPlotHistogram*)curves.at(curve))->setBrush(QBrush(color));
            QwtColumnSymbol * symbol = new QwtColumnSymbol(QwtColumnSymbol::Box);
            symbol->setFrameStyle(QwtColumnSymbol::Plain);
            symbol->setPalette(p);
            ((QwtPlotHistogram*)curves.at(curve))->setSymbol(symbol);
        }
    }
}