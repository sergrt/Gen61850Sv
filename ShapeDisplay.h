#ifndef SHAPEDISPLAY_H
#define SHAPEDISPLAY_H

#include <QWidget>
#include "ui_ShapeDisplay.h"

class ShapeDisplay : public QWidget {
    Q_OBJECT

public:
    ShapeDisplay(QWidget *parent = 0);
    ~ShapeDisplay();

    enum DISPLAY_SHAPE {
        DISPLAY_SHAPE_NONE = 0,
        DISPLAY_SHAPE_SIN,
        DISPLAY_SHAPE_TRIANGLE,
        DISPLAY_SHAPE_SAW,
        DISPLAY_SHAPE_MEANDER
    };

    void setShape(DISPLAY_SHAPE shape);
private:
    Ui::ShapeDisplay ui;
};

#endif // SHAPEDISPLAY_H
