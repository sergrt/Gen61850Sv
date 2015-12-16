#pragma once
#include <QTcpServer>
#include <QTcpSocket>

#include "scpi.h"

class TcpServer : public QObject {
    Q_OBJECT
public:
    TcpServer(QObject *parent);
    ~TcpServer();
    
private:
    QTcpServer tcpServer;
    QTcpSocket *client;
    QByteArray dataBuffer;

    QStringList splitCmd(const QString& cmd);
    void parseBuffer();
public slots:
    void onNewConnection();
    void onClientDisconnect();
    void onDataArrived();

    void tcpResponse(QByteArray *data);
signals:
    void setVpp(double Vpp);
    void setIpp(double Ipp);
    void setF(double F);
    void setPhi(double phi);
};

