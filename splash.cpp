#include "splash.h"
#include "ui_splash.h"

#include <QThread>

//const int STEP = 6;
#include "helpers.h"

Splash::Splash(QWidget *parent) :
    QWidget(parent), ui(new Ui::Splash) {
    ui->setupUi(this);
    ui->label->setText(tr("IEC61850-9.2LE Software Generator\nВерсия ") + getAppVersion());
}

Splash::~Splash() {
    delete ui;
}

bool Splash::step() {
    int pos = ui->progressBar->value() + rand() % 15 + 1;
    ui->progressBar->setValue(pos);
    //this->repaint();
    return (pos < 100);
}
