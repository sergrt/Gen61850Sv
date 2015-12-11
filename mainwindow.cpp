#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "helpers.h"
#include <QNetworkInterface>
#include <QTest>

const QString macPropName = "Mac";

#include <QTimer>
#include <QTextCodec>
MainWindow::MainWindow(QWidget *parent) :
#ifndef Q_OS_WIN
    QMainWindow(parent, Qt::CustomizeWindowHint | Qt::FramelessWindowHint),
#else
    QMainWindow(parent),
#endif
    ui(new Ui::MainWindow), scpiServer(this)
{

    ui->setupUi(this);

    chartUI = new CPlotCurve(CPlotCurve::CURVE, this);

    QPalette palU = chartUI->axisWidget(QwtPlot::yLeft)->palette();
    QPalette palI = chartUI->axisWidget(QwtPlot::yLeft)->palette();

    palU.setColor(QPalette::WindowText, CHART_COLORS::chart_U);
    palU.setColor(QPalette::Text, CHART_COLORS::chart_U);
    palI.setColor(QPalette::WindowText, CHART_COLORS::chart_I);
    palI.setColor(QPalette::Text, CHART_COLORS::chart_I);

    
    chartUI->addCurve();
    chartUI->setCurveColor(0, QColor(CHART_COLORS::chart_U));
    chartUI->setCurveColor(1, QColor(CHART_COLORS::chart_I));
    //chartUI->setAxisTitle(QwtPlot::xBottom, tr("t"));
    chartUI->setAxisTitle(QwtPlot::yLeft, tr("Ua, В"));
    chartUI->setAxisTitle(QwtPlot::yRight, tr("Ia, А"));
    chartUI->axisWidget(QwtPlot::yLeft)->setPalette(palU);
    chartUI->axisWidget(QwtPlot::yRight)->setPalette(palI);



    ui->frameChart->setLayout(new QGridLayout());
    ui->frameChart->layout()->setMargin(0);
    //ui->frameChart->layout()->addWidget(chart);
    ui->frameChart->layout()->addWidget(chartUI);

    QVBoxLayout *layout = new QVBoxLayout();
    layout->setMargin(0);
    ui->frameShape->setLayout(layout);

    shapeDisplay = new ShapeDisplay();	
    layout->addWidget(shapeDisplay);

    shapeDefine = new ShapeDefine();
    shapeDefine->setVisible(false);
    

    
    shapeDisplay->setVisible(true);
    
    settings.load();
    // Generator MACs
    QList< QNetworkInterface > ifs = QNetworkInterface::allInterfaces();
    for (int i = 0; i < ifs.count(); i++) {
        if (ifs.at(i).hardwareAddress().length() == 17 &&	// MAC 6 bytes
            (ifs.at(i).hardwareAddress() == settings.macFront || ifs.at(i).hardwareAddress() == settings.macRear) //mac allowed
            ) {
            QString ipStr = "---.---.---.---";
            for (int x = 0; x < ifs.at(i).addressEntries().count(); x++) {
                if (ifs.at(i).addressEntries().at(x).ip().protocol() == QAbstractSocket::IPv4Protocol) {
                    ipStr = ifs.at(i).addressEntries().at(x).ip().toString();
                    break;
                }
            }
            //QString s = ifs.at(i).hardwareAddress() + " (" +  ifs.at(i).humanReadableName() + ", ip = " + ipStr + ")";
            QString s = ifs.at(i).hardwareAddress() + " (";
            if (ifs.at(i).hardwareAddress() == settings.macFront)
                s += settings.frontName;
            if (ifs.at(i).hardwareAddress() == settings.macRear)
                s += settings.rearName;
            s += ")";
            ui->genMac->addItem(s);
            ui->genMac->setProperty(QString(macPropName + "%1").arg(ui->genMac->count() - 1).toLocal8Bit(), ifs.at(i).hardwareAddress());

            ui->captureMac->addItem(s);
            ui->captureMac->setProperty(QString(macPropName + "%1").arg(ui->captureMac->count() - 1).toLocal8Bit(), ifs.at(i).hardwareAddress());
        }
    }
    
    settingsToUi();

    
    QFont font = this->font();
    font.setPixelSize(16);
    this->setFont(font);
    setFontToChildControls(this, font);
    shapeDefine->setFont(font);

    stopGenThread = true;
    genThread = NULL;
    stopCaptureThread = true;
    captureThread = NULL;
    setGeneratorIsRunning(false); // For update UI
    setCaptureIsRunning(false);

    genParams = new SGenParams(settings);

    splash = new Splash(this);
    splash->show();
    srand(time(NULL));
    QTimer::singleShot(rand() % 400 + 50, this, SLOT(splashStep()));



#ifdef WIN32
    const int shiftY = 100;
#else
    const int shiftY = 0;
#endif
    move(0, shiftY);

    virtualKeyboard = new QVirtualKeyboard(this);
    virtualKeyboard->show();

    virtualKeyboard->resize(this->frameGeometry().width(), virtualKeyboard->frameGeometry().height());
    virtualKeyboard->move(0, this->frameGeometry().height() - virtualKeyboard->frameGeometry().height() + 1 + shiftY);

    // Install event filters for all QLineEdit
    installEventFilters(this);
    setFixedSize(size());
    //qDebug() << size();

    virtualKeyboard->collapse();

#ifdef Q_OS_WIN
    virtualKeyboard->hide();
#endif



    QObject::connect(ui->genStartStop, SIGNAL(clicked()), this, SLOT(onGenStartStopClick()));
    QObject::connect(ui->captureStartStop, SIGNAL(clicked()), this, SLOT(onCaptureStartStopClick()));

    QObject::connect(ui->signalShapeSin, &QRadioButton::clicked, this, &MainWindow::onShapeChanged);
    QObject::connect(ui->signalShapeSaw, &QRadioButton::clicked, this, &MainWindow::onShapeChanged);
    QObject::connect(ui->signalShapeTriangle, &QRadioButton::clicked, this, &MainWindow::onShapeChanged);
    QObject::connect(ui->signalShapeMeander, &QRadioButton::clicked, this, &MainWindow::onShapeChanged);
    QObject::connect(ui->signalShapeDefined, &QRadioButton::clicked, this, &MainWindow::onShapeChanged);


    ui->bnApply->setEnabled(false);
    QObject::connect(ui->signalAmplitudeUa, &QLineEdit::textChanged, this, &MainWindow::onParamsChanged);
    QObject::connect(ui->signalAmplitudeUb, &QLineEdit::textChanged, this, &MainWindow::onParamsChanged);
    QObject::connect(ui->signalAmplitudeUc, &QLineEdit::textChanged, this, &MainWindow::onParamsChanged);
    QObject::connect(ui->signalAmplitudeUn, &QLineEdit::textChanged, this, &MainWindow::onParamsChanged);

    QObject::connect(ui->signalAmplitudeIa, &QLineEdit::textChanged, this, &MainWindow::onParamsChanged);
    QObject::connect(ui->signalAmplitudeIb, &QLineEdit::textChanged, this, &MainWindow::onParamsChanged);
    QObject::connect(ui->signalAmplitudeIc, &QLineEdit::textChanged, this, &MainWindow::onParamsChanged);
    QObject::connect(ui->signalAmplitudeIn, &QLineEdit::textChanged, this, &MainWindow::onParamsChanged);

    QObject::connect(ui->signalPhaseUa, &QLineEdit::textChanged, this, &MainWindow::onParamsChanged);
    QObject::connect(ui->signalPhaseUb, &QLineEdit::textChanged, this, &MainWindow::onParamsChanged);
    QObject::connect(ui->signalPhaseUc, &QLineEdit::textChanged, this, &MainWindow::onParamsChanged);
    QObject::connect(ui->signalPhaseUn, &QLineEdit::textChanged, this, &MainWindow::onParamsChanged);

    QObject::connect(ui->signalPhaseIa, &QLineEdit::textChanged, this, &MainWindow::onParamsChanged);
    QObject::connect(ui->signalPhaseIb, &QLineEdit::textChanged, this, &MainWindow::onParamsChanged);
    QObject::connect(ui->signalPhaseIc, &QLineEdit::textChanged, this, &MainWindow::onParamsChanged);
    QObject::connect(ui->signalPhaseIn, &QLineEdit::textChanged, this, &MainWindow::onParamsChanged);

    QObject::connect(ui->signalFrequency, &QLineEdit::textChanged, this, &MainWindow::onParamsChanged);

    QObject::connect(ui->bnApply, &QPushButton::clicked, this, &MainWindow::onApplyClick);

}

MainWindow::~MainWindow()
{
    uiToSettings();
    settings.save();

    delete genParams;
    delete ui;
}

void MainWindow::onParamsChanged() {
    ui->bnApply->setEnabled(true);
}

void MainWindow::onApplyClick() {
    
    uiToSettings();
    genParams->Ua_A = ui->signalAmplitudeUa->text().toDouble();
    genParams->Ub_A = ui->signalAmplitudeUb->text().toDouble();
    genParams->Uc_A = ui->signalAmplitudeUc->text().toDouble();
    genParams->Un_A = ui->signalAmplitudeUn->text().toDouble();

    genParams->Ia_A = ui->signalAmplitudeIa->text().toDouble();
    genParams->Ib_A = ui->signalAmplitudeIb->text().toDouble();
    genParams->Ic_A = ui->signalAmplitudeIc->text().toDouble();
    genParams->In_A = ui->signalAmplitudeIn->text().toDouble();

    genParams->freq = ui->signalFrequency->text().toDouble();

    genParams->Ua_G = ui->signalPhaseUa->text().toDouble();
    genParams->Ub_G = ui->signalPhaseUb->text().toDouble();
    genParams->Uc_G = ui->signalPhaseUc->text().toDouble();
    genParams->Un_G = ui->signalPhaseUn->text().toDouble();

    genParams->Ia_G = ui->signalPhaseIa->text().toDouble();
    genParams->Ib_G = ui->signalPhaseIb->text().toDouble();
    genParams->Ic_G = ui->signalPhaseIc->text().toDouble();
    genParams->In_G = ui->signalPhaseIn->text().toDouble();

    settings.save();
    ui->bnApply->setEnabled(false);
}

void MainWindow::uiToSettings() {    
    if (ui->signalShapeSin->isChecked())
        settings.signalShape = SGL_SHAPE_SIN;
    else if (ui->signalShapeTriangle->isChecked())
        settings.signalShape = SGL_SHAPE_TRIANGLE;
    else if (ui->signalShapeMeander->isChecked())
        settings.signalShape = SGL_SHAPE_MEANDER;
    else if (ui->signalShapeSaw->isChecked())
        settings.signalShape = SGL_SHAPE_SAW;
    else if (ui->signalShapeDefined->isChecked())
        settings.signalShape = SGL_SHAPE_DEFINED;

    settings.signalAmplitudeUa = ui->signalAmplitudeUa->text().toDouble();
    settings.signalAmplitudeUb = ui->signalAmplitudeUb->text().toDouble();
    settings.signalAmplitudeUc = ui->signalAmplitudeUc->text().toDouble();
    settings.signalAmplitudeUn = ui->signalAmplitudeUn->text().toDouble();

    settings.signalAmplitudeIa = ui->signalAmplitudeIa->text().toDouble();
    settings.signalAmplitudeIb = ui->signalAmplitudeIb->text().toDouble();
    settings.signalAmplitudeIc = ui->signalAmplitudeIc->text().toDouble();
    settings.signalAmplitudeIn = ui->signalAmplitudeIn->text().toDouble();
    settings.signalFrequency = ui->signalFrequency->text().toDouble();
    settings.signalPhaseUa = ui->signalPhaseUa->text().toDouble();
    settings.signalPhaseUb = ui->signalPhaseUb->text().toDouble();
    settings.signalPhaseUc = ui->signalPhaseUc->text().toDouble();
    settings.signalPhaseUn = ui->signalPhaseUn->text().toDouble();
    settings.signalPhaseIa = ui->signalPhaseIa->text().toDouble();
    settings.signalPhaseIb = ui->signalPhaseIb->text().toDouble();
    settings.signalPhaseIc = ui->signalPhaseIc->text().toDouble();
    settings.signalPhaseIn = ui->signalPhaseIn->text().toDouble();

    settings.destinationMac = ui->destinationMac->text();
    settings.genStreamId = ui->genStreamId->text();

    if (ui->discrete80->isChecked())
        settings.signalDiscrete = DISCRETE_80;
    else if (ui->discrete256->isChecked())
        settings.signalDiscrete = DISCRETE_256;
    else if (ui->discrete640->isChecked())
        settings.signalDiscrete = DISCRETE_640;
    else if (ui->discrete1280->isChecked())
        settings.signalDiscrete = DISCRETE_1280;

    settings.genMac = getGenMacFromUi();

    settings.captureStreamId = ui->captureStreamId->text();
    settings.captureMac = getCaptureMacFromUi();

    //settings.signal1stHarmonicA = ui->stHarmA->text().toDouble();
    //settings.signal1stHarmonicPhi = ui->stHarmPhi->text().toDouble();
}

void MainWindow::settingsToUi() {
    ui->signalShapeSin->setChecked(settings.signalShape == SGL_SHAPE_SIN);
    ui->signalShapeTriangle->setChecked(settings.signalShape == SGL_SHAPE_TRIANGLE);
    ui->signalShapeMeander->setChecked(settings.signalShape == SGL_SHAPE_MEANDER);
    ui->signalShapeSaw->setChecked(settings.signalShape == SGL_SHAPE_SAW);
    ui->signalShapeDefined->setChecked(settings.signalShape == SGL_SHAPE_DEFINED);
    onShapeChanged();

    ui->signalAmplitudeUa->setText(doubleToStr(settings.signalAmplitudeUa));
    ui->signalAmplitudeUb->setText(doubleToStr(settings.signalAmplitudeUb));
    ui->signalAmplitudeUc->setText(doubleToStr(settings.signalAmplitudeUc));
    ui->signalAmplitudeUn->setText(doubleToStr(settings.signalAmplitudeUn));
    ui->signalAmplitudeIa->setText(doubleToStr(settings.signalAmplitudeIa));
    ui->signalAmplitudeIb->setText(doubleToStr(settings.signalAmplitudeIb));
    ui->signalAmplitudeIc->setText(doubleToStr(settings.signalAmplitudeIc));
    ui->signalAmplitudeIn->setText(doubleToStr(settings.signalAmplitudeIn));
    ui->signalFrequency->setText(doubleToStr(settings.signalFrequency));
    ui->signalPhaseUa->setText(doubleToStr(settings.signalPhaseUa));
    ui->signalPhaseUb->setText(doubleToStr(settings.signalPhaseUb));
    ui->signalPhaseUc->setText(doubleToStr(settings.signalPhaseUc));
    ui->signalPhaseUn->setText(doubleToStr(settings.signalPhaseUn));
    ui->signalPhaseIa->setText(doubleToStr(settings.signalPhaseIa));
    ui->signalPhaseIb->setText(doubleToStr(settings.signalPhaseIb));
    ui->signalPhaseIc->setText(doubleToStr(settings.signalPhaseIc));
    ui->signalPhaseIn->setText(doubleToStr(settings.signalPhaseIn));

    ui->destinationMac->setText(settings.destinationMac);
    ui->genStreamId->setText(settings.genStreamId);

    ui->discrete80->setChecked(settings.signalDiscrete == DISCRETE_80);
    ui->discrete256->setChecked(settings.signalDiscrete == DISCRETE_256);
    ui->discrete640->setChecked(settings.signalDiscrete == DISCRETE_640);
    ui->discrete1280->setChecked(settings.signalDiscrete == DISCRETE_1280);

    for (int i = 0; i < ui->genMac->count(); i++) {
        QString macI = getGenMacFromUi(i);
        if (settings.genMac == macI) {
            ui->genMac->setCurrentIndex(i);
            break;
        }
    }

    ui->captureStreamId->setText(settings.captureStreamId);
    for (int i = 0; i < ui->captureMac->count(); i++) {
        QString macI = getCaptureMacFromUi(i);
        if (settings.captureMac == macI) {
            ui->captureMac->setCurrentIndex(i);
            break;
        }
    }

    //ui->signalPhaseIa->setText(doubleToStr(settings.signalPhaseIa));

    //ui->stHarmA->setText(doubleToStr(settings.signal1stHarmonicA));
    //ui->stHarmPhi->setText(doubleToStr(settings.signal1stHarmonicPhi));
}

QString MainWindow::getGenMacFromUi(const int index) const {
    int i = index == -1 ? ui->genMac->currentIndex() : index;
    return ui->genMac->property(QString(macPropName + "%1").arg(i).toLocal8Bit()).toString();
}

QString MainWindow::getCaptureMacFromUi(const int index) const {
    int i = index == -1 ? ui->captureMac->currentIndex() : index;
    return ui->captureMac->property(QString(macPropName + "%1").arg(i).toLocal8Bit()).toString();
}

void MainWindow::setGeneratorIsRunning(bool isRunning) {
    QString bnTxt = isRunning ? tr("Остановить") : tr("Запустить");
    ui->genStartStop->setText(bnTxt);
    ui->groupShape->setEnabled(!isRunning);
    ui->group61850->setEnabled(!isRunning);
    shapeDefine->setEnabled(!isRunning);

    QIcon img(isRunning?":/GenSv/Resources/stop_button.gif":":/GenSv/Resources/start_button.gif");
    ui->genStartStop->setIcon(img);
}

void MainWindow::setCaptureIsRunning(bool isRunning) {
    QString bnTxt = isRunning ? tr("Остановить") : tr("Запустить");
    ui->captureStartStop->setText(bnTxt);
    ui->groupCaptureParams->setEnabled(!isRunning);

    QIcon img(isRunning?":/GenSv/Resources/stop_button.gif":":/GenSv/Resources/start_button.gif");
    ui->captureStartStop->setIcon(img);
}

void MainWindow::onGenStartStopClick() {
    stopGenThread = !stopGenThread;
    if (stopGenThread) {
        while(genThread->isRunning())
            QTest::qWait( 100 );
         delete genThread;
         genThread = NULL;
    } else {
        if (genThread == NULL) {
            uiToSettings();
            settings.save();
            *genParams = SGenParams(settings);
            ui->bnApply->setEnabled(false);
            genThread = new CGenerator(this, &stopGenThread, genParams/*, &syncObj*/);
            genThread->start();
        }
    }

}

void MainWindow::onCaptureStartStopClick() {
    stopCaptureThread = !stopCaptureThread;
    if (stopCaptureThread) {
        while(captureThread->isRunning())
            QTest::qWait( 100 );
         delete captureThread;
         captureThread = NULL;
    } else {
        if (captureThread == NULL) {
            uiToSettings();
            captureThread = new CCapture(this, &stopCaptureThread);
            captureThread->init(settings.captureStreamId, settings.captureMac);
            captureThread->start(QThread::HighPriority);
        }
    }
}

void MainWindow::displayData(vector<SVDecoded>* data, bool* ready) {
    QVector<double> x, U, I;
    int cnt = 0;
    for (unsigned int i = 0; i < data->size(); ++i) {
        for (int j = 0; j < data->at(i).Ua.size(); ++j) {
            U.push_back(data->at(i).Ua.at(j));
            I.push_back(data->at(i).Ia.at(j));
            x.push_back(cnt++);
        }
    }
        
    chartUI->setData(0, x, U);
    chartUI->setData(1, x, I);
    chartUI->doAutoScale();

    *ready = true;
}

void MainWindow::onShapeChanged() {
    //ui->groupAngles->setEnabled(ui->signalShapeSin->isChecked());

    ShapeDisplay::DISPLAY_SHAPE shape = ShapeDisplay::DISPLAY_SHAPE_NONE;

    if (ui->signalShapeDefined->isChecked()) {
        ui->frameShape->layout()->removeWidget(shapeDisplay);
        ui->frameShape->layout()->addWidget(shapeDefine);
        
        shapeDefine->setVisible(true);
        shapeDisplay->setVisible(false);
    } else {
        ui->frameShape->layout()->removeWidget(shapeDefine);
        ui->frameShape->layout()->addWidget(shapeDisplay);

        shapeDefine->setVisible(false);
        shapeDisplay->setVisible(true);

        if (ui->signalShapeSin->isChecked())
            shape = ShapeDisplay::DISPLAY_SHAPE_SIN;
        if (ui->signalShapeTriangle->isChecked())
            shape = ShapeDisplay::DISPLAY_SHAPE_TRIANGLE;
        if (ui->signalShapeSaw->isChecked())
            shape = ShapeDisplay::DISPLAY_SHAPE_SAW;
        if (ui->signalShapeMeander->isChecked())
            shape = ShapeDisplay::DISPLAY_SHAPE_MEANDER;
    
        shapeDisplay->setShape(shape);
    }
}

void MainWindow::setFontToChildControls(QObject* w, const QFont& f) {
    QObjectList ql = w->children();
    for (int i = 0; i < ql.count(); i++) {		
        if (ql[i]->isWidgetType()) {
            setFontToChildControls(ql[i], f);
            QWidget * w = dynamic_cast<QWidget *>(ql[i]);
            if (w) {
                const QString wName = w->metaObject()->className(); // Оптимизировано по результатам проверки статическим анализатором кода
                //qDebug( w->metaObject()->className() );
                /*if (wName == "QLineEdit") */{
                    w->setFont(f);
                }
            }
        }
    }
}

void MainWindow::installEventFilters(QObject* w) {
    QObjectList ql = w->children();
    for (int i = 0; i < ql.count(); i++) {		
        if (ql[i]->isWidgetType()) {
            installEventFilters(ql[i]);
            QWidget * w = dynamic_cast<QWidget *>(ql[i]);
            if (w) {
                const QString wName = w->metaObject()->className(); // Оптимизировано по результатам проверки статическим анализатором кода
                //qDebug( w->metaObject()->className() );
                if (wName == "QLineEdit" || 
                    (wName == "QComboBox" && static_cast<QComboBox*>(w)->isEditable()) ||
                    wName == "QTableWidget") {
                        w->installEventFilter(this);
                }
                if (wName == "QLineEdit") {
                    ((QLineEdit*)w)->setCursor(Qt::ArrowCursor);
                }
            }
        }
    }
}

void MainWindow::onKeyPressed(int key) {
    //key = 0x000000FF & key;
    std::string stdStr;
    stdStr.push_back((unsigned char)key);

    QString text(QTextCodec::codecForName("CP1251")->toUnicode(stdStr.data()));// = QString::fromLocal8Bit(ba)
    QKeyEvent *e = new QKeyEvent(QEvent::KeyPress, key, Qt::NoModifier, text);
    QCoreApplication::postEvent(this->focusWidget(), e);
}

bool MainWindow::eventFilter(QObject *object, QEvent *event) {
    QWidget* widget = qobject_cast<QWidget*>(object);
    if (widget != NULL) {
        QString widgetName = widget->metaObject()->className();
        if (widgetName == "QTableWidget") {
            if (event->type() == QEvent::FocusIn) {
                widget->setStyleSheet(QString("QTableWidget QLineEdit {background: #BBDDBB;}"));
            } else if(event->type() == QEvent::FocusOut && !virtualKeyboard->isActiveWindow()) {
                QString bgColorName = palette().color(QPalette::Normal, QPalette::Base).name();
                widget->setStyleSheet(QString("QTableWidget QLineEdit {background:" + bgColorName + ";}"));
            }
        } else {
            if (event->type() == QEvent::FocusIn) {
                widget->setStyleSheet(QString("background: #BBDDBB;"));
            } else if(event->type() == QEvent::FocusOut && !virtualKeyboard->isActiveWindow()) {
                QString bgColorName = palette().color(QPalette::Normal, QPalette::Base).name();
                widget->setStyleSheet(QString("background:" + bgColorName + ";"));
            }
        }
    }
    /*
    QLineEdit* edit = qobject_cast<QLineEdit*>(object);
    
    if (edit != NULL) {
        if (event->type() == QEvent::FocusIn) {
            edit->setStyleSheet(QString("background: #BBDDBB;"));
        } else if(event->type() == QEvent::FocusOut && !virtualKeyboard->isActiveWindow()) {
            QString bgColorName = palette().color(QPalette::Normal, QPalette::Base).name();
            edit->setStyleSheet(QString("background:" + bgColorName + ";"));
        }
    }
    */
    return false;
}

void MainWindow::splashStep() {
    if (!splash->step())
        splash->hide();
    else
        QTimer::singleShot(rand() % 400 + 50, this, SLOT(splashStep()));
}

void MainWindow::tcpSetVpp(double Vpp) {
    genParams->Ua_A = genParams->Ub_A = genParams->Uc_A = genParams->Un_A = Vpp;
    settings.signalAmplitudeUa = 
        settings.signalAmplitudeUb = 
        settings.signalAmplitudeUc = 
        settings.signalAmplitudeUn = Vpp;
    settingsToUi();
}
void MainWindow::tcpSetIpp(double Ipp) {
    genParams->Ia_A = genParams->Ib_A = genParams->Ic_A = genParams->In_A = Ipp;
    settings.signalAmplitudeIa = 
        settings.signalAmplitudeIb = 
        settings.signalAmplitudeIc = 
        settings.signalAmplitudeIn = Ipp;
    settingsToUi();
}
void MainWindow::tcpSetF(double F) {
    genParams->freq = F;
    settings.signalFrequency = F;
    settingsToUi();
}
void MainWindow::tcpSetPhi(double phi) {
    genParams->Ua_G = 
        genParams->Ub_G = 
        genParams->Uc_G = 
        genParams->Un_G = phi;
    genParams->Ia_G = 
        genParams->Ib_G = 
        genParams->Ic_G = 
        genParams->In_G = phi;

    settings.signalPhaseUa = 
        settings.signalPhaseUb = 
        settings.signalPhaseUc = 
        settings.signalPhaseUn = 
        settings.signalPhaseIa = 
        settings.signalPhaseIb = 
        settings.signalPhaseIc = 
        settings.signalPhaseIn = phi;

    settingsToUi();
}
