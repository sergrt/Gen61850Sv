#ifndef EDITWND_H
#define EDITWND_H

#include <QDialog>
#include "ui_editwnd.h"

class EditWnd : public QWidget {
	Q_OBJECT

public:
	EditWnd(QWidget *parent = 0);
	~EditWnd();
	QString getText() const;
	void setText(const QString& text);

private:
	Ui::EditWnd ui;
};

#endif // EDITWND_H
