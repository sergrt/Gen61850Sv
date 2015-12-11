#include "TcpServer.h"
#include "mainwindow.h"
#include "helpers.h"

const unsigned short SCPI_PORT = 5025;
const QString delimeter = "\n";

TcpServer::TcpServer(QObject *parent) {
	QObject::connect(&tcpServer, &QTcpServer::newConnection, this, &TcpServer::onNewConnection);

	QObject::connect(this, &TcpServer::setVpp, (MainWindow*)parent, &MainWindow::tcpSetVpp);
	QObject::connect(this, &TcpServer::setIpp, (MainWindow*)parent, &MainWindow::tcpSetIpp);
	QObject::connect(this, &TcpServer::setF, (MainWindow*)parent, &MainWindow::tcpSetF);
	QObject::connect(this, &TcpServer::setPhi, (MainWindow*)parent, &MainWindow::tcpSetPhi);

	//client = nullptr;
	client = NULL;
	tcpServer.listen(QHostAddress::Any, SCPI_PORT);
}

TcpServer::~TcpServer() {

}

void TcpServer::onNewConnection() {
	client = tcpServer.nextPendingConnection();
	dataBuffer.clear();

	QObject::connect(client, &QTcpSocket::readyRead, this, &TcpServer::onDataArrived);
	QObject::connect(client, &QTcpSocket::disconnected, this, &TcpServer::onClientDisconnect);

    client->setSocketOption(QAbstractSocket::LowDelayOption, QVariant(1));
}

void TcpServer::onClientDisconnect() {
	//client = nullptr;
	client = NULL;
	dataBuffer.clear();
}

void TcpServer::onDataArrived() {
	//if (client != nullptr) {
	if (client != NULL) {
        qint64 bytesReceived = client->bytesAvailable();

        if (bytesReceived > 0) {
            QByteArray data = client->readAll();
            dataBuffer.append(data);

            //Буфер изменился, пытаемся обработать команду
            parseBuffer();
        }
    }
}




void TcpServer::parseBuffer() {
	int delimeterPos = -1;
	const int delimeterLen = delimeter.length();

	while ((delimeterPos = dataBuffer.indexOf(delimeter)) != -1) {		
		QByteArray commandsPack = dataBuffer.left(delimeterPos);
		QStringList lastCommand;

		QList<QByteArray> commandsList = commandsPack.split(';');

		for (int i = 0; i < commandsList.size(); ++i) {
			QString cmd = commandsList[i];
			//log->put("Parsing command \"" + cmd + "\"");

			bool cmdRes = false;
			//if (cmd.compare("*idn?", Qt::CaseInsensitive) == 0) {
			if (SCPI::commandCompare(cmd, "*idn?", "*idn?", false, NULL, false, NULL)) {
				//log->put("Processing *IDN?");
				QString response = "IEC61850-9.2LE Generator, GenDv, 00006, " + getAppVersion() + delimeter;
				client->write(response.toLatin1());
				cmdRes = true;
			}
			// Разбор команд
			if (cmd[0] == ':') {
				lastCommand.clear();
				cmd.remove(0, 1);
			}

			QStringList cmdList = splitCmd(cmd);
			if (!lastCommand.isEmpty()) {
				for (int x = lastCommand.size() - 1; x >= 0; --x)
					cmdList.push_front(lastCommand[x]);
			}
			
			int x = 0;

			QString argument;
			QString suffix;
			if (SCPI::scpiParse(cmdList, "SET", "SET", true, &suffix, true, &argument, lastCommand, x)) {
				if (suffix.compare("U", Qt::CaseInsensitive) == 0) {
					emit setVpp(argument.toDouble());
					cmdRes = true;
				} else if (suffix.compare("I", Qt::CaseInsensitive) == 0) {
					emit setIpp(argument.toDouble());
					cmdRes = true;
				} else if (suffix.compare("F", Qt::CaseInsensitive) == 0) {
					emit setF(argument.toDouble());
					cmdRes = true;
				} else if (suffix.compare("PHI", Qt::CaseInsensitive) == 0) {
					emit setPhi(argument.toDouble());
					cmdRes = true;
				}
			}
			
			
			if (!cmdRes) {
				//log->put("Unknown command \"" + cmd + "\"");
				client->write(QString("Unknown command \"" + cmd + "\"" + delimeter).toLatin1());
			}
		}
		dataBuffer.remove(0, delimeterPos + delimeterLen);
		//log->put("Log remain: " + QString(dataBuffer));
	}
}

void TcpServer::tcpResponse(QByteArray *data) {
	//if (client != nullptr) {
	if (client != NULL) {
        QByteArray dataCopy = *data + delimeter.toLatin1();
        client->write(dataCopy.data(), dataCopy.size());
	}
}
QStringList TcpServer::splitCmd(const QString& cmd) {
	QStringList res;
	bool isInString = false;
	QString curCmd;

	for (int i = 0; i < cmd.length(); ++i) {
		if (cmd[i] == ':' && isInString == false) {
			res.push_back(curCmd);
			curCmd = "";
		} else {
			curCmd.push_back(cmd[i]);
			if (cmd[i] =='\"' || cmd[i] == '\'') {
				isInString = !isInString;
			}
		}		
	}
	if (curCmd.length() > 0)
		res.push_back(curCmd);
	
	return res;
}
