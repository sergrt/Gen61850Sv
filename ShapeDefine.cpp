#include "ShapeDefine.h"
#include "genParams.h"

const int COLUMN_K    = 0;
const int COLUMN_AK   = 1;
const int COLUMN_PHIK = 2;

ShapeDefine::ShapeDefine(QWidget *parent)
	: QWidget(parent) {
	ui.setupUi(this);

	editWnd = new EditWnd(this);
	editWnd->hide();

	ui.tableWidget->setColumnCount(3);
	ui.tableWidget->setHorizontalHeaderLabels(QStringList() << tr("N") << tr("Амплитуда") << tr("Угол"));
	ui.tableWidget->setColumnWidth(COLUMN_K, 50);
	ui.tableWidget->setColumnWidth(COLUMN_AK, 110);
	ui.tableWidget->setColumnWidth(COLUMN_PHIK, 115);
	ui.tableWidget->horizontalHeader()->setStretchLastSection(true);

	loadShape();
	
	QObject::connect(ui.bnEdit, &QPushButton::clicked, this, &ShapeDefine::onEditClick);
}

ShapeDefine::~ShapeDefine() {
	delete editWnd;
}

void ShapeDefine::saveShape() {
	SGenParams p;
	vector<double> Ak, phik;
	for (int i = 0; i < ui.tableWidget->rowCount(); ++i) {
		Ak.push_back(ui.tableWidget->item(i, COLUMN_AK)->text().toDouble());
		phik.push_back(ui.tableWidget->item(i, COLUMN_PHIK)->text().toDouble());
	}
	p.saveShape(Ak, phik);
}
void ShapeDefine::loadShape() {
	while (ui.tableWidget->rowCount() > 0)
		ui.tableWidget->removeRow(0);

	SGenParams p;
	for (int i = 0; i < p.Ak.size(); ++i) {
		ui.tableWidget->insertRow(ui.tableWidget->rowCount());
		QTableWidgetItem* item_i = new QTableWidgetItem();
		item_i->setText(QString("%1").arg(i));
		ui.tableWidget->setItem(ui.tableWidget->rowCount() - 1, COLUMN_K, item_i);

		QTableWidgetItem* item_A = new QTableWidgetItem();
		item_A->setText(QString("%1").arg(p.Ak[i]));
		ui.tableWidget->setItem(ui.tableWidget->rowCount() - 1, COLUMN_AK, item_A);

		QTableWidgetItem* item_phi = new QTableWidgetItem();
		item_phi->setText(QString("%1").arg(p.phik[i]));
		ui.tableWidget->setItem(ui.tableWidget->rowCount() - 1, COLUMN_PHIK, item_phi);
	}
}

void ShapeDefine::onEditClick() {
	int curIndex = ui.tableWidget->currentRow();
	int curColumn = ui.tableWidget->currentColumn();

	QTableWidgetItem* item = ui.tableWidget->item(curIndex, curColumn);
	if (item) {
		editWnd->setText(item->text());
		editWnd->setGeometry(this->geometry());
		editWnd->show();	
	}
}

void ShapeDefine::onApplyValueClick() {
	editWnd->hide();
	int curIndex = ui.tableWidget->currentRow();
	int curColumn = ui.tableWidget->currentColumn();

	QTableWidgetItem* item = ui.tableWidget->item(curIndex, curColumn);
	item->setText(QString("%1").arg(editWnd->getText()));
	saveShape();
}

void ShapeDefine::onCancelValueClick() {
	editWnd->hide();
}