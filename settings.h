#ifndef _SETTINGS_H_
#define _SETTINGS_H_

#include <qpen.h>
#include <qbrush.h>

class Settings
{
public:
    enum UpdateType
    {
        RepaintCanvas,
        Replot
    };

    Settings()
    {
        grid.pen = Qt::NoPen;

        curve.brush = Qt::NoBrush;
        curve.numPoints = 20;
        curve.paintAttributes = 0;
        curve.renderHint = 0;
        curve.lineSplitting = true;

        canvas.useBackingStore = false;
        canvas.paintOnScreen = false;
        canvas.immediatePaint = true;

        updateType = RepaintCanvas;
        updateInterval = 100;
	sensor_callback = NULL;
    }

    struct gridSettings
    {
        QPen pen;
    } grid;

    struct curveSettings
    {
        QPen pen;
        QBrush brush;
        uint numPoints;
        int paintAttributes;
        int renderHint;
        bool lineSplitting;
    } curve;

    struct canvasSettings
    {
        bool useBackingStore;
        bool paintOnScreen;
        bool immediatePaint;
    } canvas;

    UpdateType updateType;
    int updateInterval;

    double (*sensor_callback)(void);
};

#endif
