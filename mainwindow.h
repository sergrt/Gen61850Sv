#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "settings.h"
#include "generator.h"
#include "capture.h"
#include "PlotCurve.h"

#include "VirtualKeyboard.h"
#include "ShapeDisplay.h"
#include "ShapeDefine.h"


#include "splash.h"
#include "TcpServer.h"

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    
private:
    Ui::MainWindow *ui;
    CSettings settings;
	void uiToSettings();
	void settingsToUi();
    QString getGenMacFromUi(const int index = -1) const;
    QString getCaptureMacFromUi(const int index = -1) const;

	TcpServer scpiServer;

    CGenerator *genThread;
    CCapture *captureThread;
	SGenParams *genParams;
	//CSyncObject syncObj;
	bool stopGenThread;
	bool stopCaptureThread;

	CPlotCurve *chartUI;

	QVirtualKeyboard *virtualKeyboard;
	void installEventFilters(QObject* w);
	bool eventFilter(QObject *object, QEvent *event);
	void setFontToChildControls(QObject* w, const QFont& f); //нужно чтобы шрифт не менялся при потере фокуса

	ShapeDisplay* shapeDisplay;
	ShapeDefine* shapeDefine;
	Splash *splash;
public slots:
    void setGeneratorIsRunning(bool isRunning);
    void setCaptureIsRunning(bool isRunning);
    void onGenStartStopClick();
    void onCaptureStartStopClick();
	void displayData(vector<SVDecoded>* data, bool* ready);
	void onShapeChanged();

	void onKeyPressed(int key);
	void splashStep();

	void tcpSetVpp(double Vpp);
	void tcpSetIpp(double Ipp);
	void tcpSetF(double F);
	void tcpSetPhi(double phi);

	void onParamsChanged();
	void onApplyClick();
};

#endif // MAINWINDOW_H
