#ifndef VIRTUALKEYBOARD_H
#define VIRTUALKEYBOARD_H

#include <QWidget>
#include "ui_VirtualKeyboard.h"

class QVirtualKeyboard : public QWidget
{
	Q_OBJECT //-V524

public:
	QVirtualKeyboard(QWidget *parent = 0);
	~QVirtualKeyboard();

	void collapse();
	void restore();
public slots:
	void onBnClicked();
signals:
	void onKeyPressed(int key);
private:
	enum VIRTUALKEYBOARD_LAYOUT {
		VIRTUALKEYBOARD_LAYOUT_EN = 0,
		VIRTUALKEYBOARD_LAYOUT_RU
	} keyLayout;

	Ui::QVirtualKeyboard ui;
	bool shiftPressed;
	void connectAllButtons(QObject* o);
	void setAllButtonCodes();
	void setAllButtonLabels(QObject* obj);
	void setAllButtonFont(QObject* obj, const QFont& font);
	QPushButton* bnSpecialRestore;
};

#endif // VIRTUALKEYBOARD_H
