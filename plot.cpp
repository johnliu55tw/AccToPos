#include <qglobal.h>
#include <qwt_painter.h>
#include <qwt_plot_canvas.h>
#include <qwt_plot_grid.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_layout.h>
#include <qwt_scale_widget.h>
#include <qwt_scale_draw.h>
#include "plot.h"
#include "settings.h"
#include <QDebug>

Plot::Plot( QWidget *parent ):
    QwtPlot( parent ),
    d_interval( 10.0 ), // seconds
    d_timerId( -1 )
{
    // Assign a title
    setTitle( "Sensor Plot" );

    setCanvasBackground( Qt::white );

    //alignScales();

    // Insert grid
    d_grid = new QwtPlotGrid();
    d_grid->attach( this );

    // Insert curve
    d_curve = new QwtPlotCurve( "Data Moving Right" );
    d_curve->setPen( QPen( Qt::black ) );
    d_curve->attach( this );

    // Axis
    setAxisTitle( QwtPlot::xBottom, "Seconds" );
    setAxisScale( QwtPlot::xBottom, -d_interval, 0.0 );

    setAxisTitle( QwtPlot::yLeft, "Values" );
    //setAxisScale( QwtPlot::yLeft, -1.0, 1.0 );

    d_clock.start();

    setSettings( d_settings );
}

//
//  Set a plain canvas frame and align the scales to it
//
void Plot::alignScales()
{
    // The code below shows how to align the scales to
    // the canvas frame, but is also a good example demonstrating
    // why the spreaded API needs polishing.

    canvas()->setFrameStyle( QFrame::Box | QFrame::Plain );
    canvas()->setLineWidth( 1 );

    for ( int i = 0; i < QwtPlot::axisCnt; i++ )
    {
        QwtScaleWidget *scaleWidget = axisWidget( i );
        if ( scaleWidget )
            scaleWidget->setMargin( 0 );

        QwtScaleDraw *scaleDraw = axisScaleDraw( i );
        if ( scaleDraw )
            scaleDraw->enableComponent( QwtAbstractScaleDraw::Backbone, false );
    }

    plotLayout()->setAlignCanvasToScales( true );
}

void Plot::setSettings( const Settings &s )
{
    if ( d_timerId >= 0 )
        killTimer( d_timerId );

    d_timerId = startTimer( s.updateInterval );

    d_grid->setPen( s.grid.pen );
    d_grid->setVisible( s.grid.pen.style() != Qt::NoPen );

    d_curve->setPen( s.curve.pen );
    d_curve->setBrush( s.curve.brush );

    d_curve->setPaintAttribute( QwtPlotCurve::ClipPolygons,
        s.curve.paintAttributes & QwtPlotCurve::ClipPolygons );
    d_curve->setRenderHint( QwtPlotCurve::RenderAntialiased,
        s.curve.renderHint & QwtPlotCurve::RenderAntialiased );

    canvas()->setAttribute( Qt::WA_PaintOnScreen, s.canvas.paintOnScreen );

    canvas()->setPaintAttribute(
        QwtPlotCanvas::BackingStore, s.canvas.useBackingStore );
    canvas()->setPaintAttribute(
        QwtPlotCanvas::ImmediatePaint, s.canvas.immediatePaint );

    QwtPainter::setPolylineSplitting( s.curve.lineSplitting );

    circular_buffer.clear();
    x_sample.clear();
    int circularBufferSize = 10 / ((double)s.updateInterval/1000) ;
    circular_buffer.resize(circularBufferSize);
    x_sample.resize(circularBufferSize);
    
    for (QVector<double>::iterator it=circular_buffer.begin();
	 it != circular_buffer.end(); it++)
	*it = 0;

    double increment = (double)s.updateInterval / 1000;
    double base = increment;
    for (QVector<double>::iterator it=x_sample.begin();
	 it != x_sample.end(); it++) {
	 base -= increment;
	 *it = base;
    }

    d_curve->setSamples(x_sample, circular_buffer);
    sensor_callback = s.sensor_callback;

    d_settings = s;
}

void Plot::timerEvent( QTimerEvent * )
{
    circular_buffer.pop_back();
    double data = sensor_callback();
    circular_buffer.push_front(data);
    d_curve->setSamples(x_sample, circular_buffer);
    replot();
    /*
    switch( d_settings.updateType )
    {
        case Settings::RepaintCanvas:
        {
            // the axes in this example doesn't change. So all we need to do
            // is to repaint the canvas.

            canvas()->replot();
            break;
        }
        default:
        {
            replot();
        }
    }
    */
}
