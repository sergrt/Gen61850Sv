#include "editwnd.h"
#include "ShapeDefine.h"
EditWnd::EditWnd(QWidget *parent)
	: QWidget(parent) {
	ui.setupUi(this);

	QObject::connect(ui.bnSave, &QPushButton::clicked, (ShapeDefine*)parent, &ShapeDefine::onApplyValueClick);
	QObject::connect(ui.bnCancel, &QPushButton::clicked, (ShapeDefine*)parent, &ShapeDefine::onCancelValueClick);
}

EditWnd::~EditWnd() {

}

QString EditWnd::getText() const {
	return ui.leValue->text();
}

void EditWnd::setText(const QString& text) {
	ui.leValue->setText(text);
}
