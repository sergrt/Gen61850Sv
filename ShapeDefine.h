#ifndef SHAPEDEFINE_H
#define SHAPEDEFINE_H

#include <QWidget>
#include "ui_ShapeDefine.h"
#include "editwnd.h"

class ShapeDefine : public QWidget {
    Q_OBJECT

public:
    ShapeDefine(QWidget *parent = 0);
    ~ShapeDefine();

private:
    Ui::ShapeDefine ui;
    EditWnd *editWnd;

    void saveShape();
    void loadShape();
public slots:	
    void onEditClick();
    void onApplyValueClick();
    void onCancelValueClick();
};

#endif // SHAPEDEFINE_H
