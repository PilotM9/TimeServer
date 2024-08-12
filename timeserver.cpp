#include "TimeServer.h"
#include <QDebug>

TimeServer::TimeServer(QObject *parent)
    : QTcpServer(parent), clientSocket(nullptr), mainServerSocket(nullptr), hasRequests(false)
{
    tickTimer = new QTimer(this);
    connect(tickTimer, &QTimer::timeout, this, &TimeServer::sendTick);

    connect(this, &QTcpServer::newConnection, this, &TimeServer::onNewConnection);
    if (!listen(QHostAddress::Any, 1235)) {
        qDebug() << "TimeServer could not start!";
    } else {
        qDebug() << "TimeServer started!";
    }
}

TimeServer::~TimeServer()
{
    if (clientSocket) {
        clientSocket->disconnectFromHost();
        clientSocket->deleteLater();
    }
    if (mainServerSocket) {
        mainServerSocket->disconnectFromHost();
        mainServerSocket->deleteLater();
    }
}

void TimeServer::onNewConnection()
{
    if (clientSocket) {
        qDebug() << "Disconnecting old client";
        clientSocket->disconnectFromHost();
        clientSocket->deleteLater();
    }

    clientSocket = nextPendingConnection();
    connect(clientSocket, &QTcpSocket::readyRead, this, &TimeServer::onReadyRead);
    connect(clientSocket, &QTcpSocket::disconnected, this, &TimeServer::onDisconnected);
    qDebug() << "Client connected";
}

void TimeServer::onReadyRead()
{
    QByteArray data = clientSocket->readAll();
    if (data == "START") {
        startProcessing();
    } else if (data == "NO_MORE_REQUESTS") {
        stopProcessing();
    }
}

void TimeServer::onDisconnected()
{
    qDebug() << "Client disconnected";
    if (clientSocket) {
        clientSocket->deleteLater();
        clientSocket = nullptr;
    }
}

void TimeServer::startProcessing()
{
    hasRequests = true;
    tickTimer->start(128); // Начинаем тикание каждые 128 миллисекунд
    qDebug() << "Started processing requests.";
}

void TimeServer::stopProcessing()
{
    hasRequests = false;
    tickTimer->stop(); // Останавливаем тикание
    qDebug() << "Stopped processing requests.";
}

void TimeServer::sendTick()
{
    if (clientSocket && hasRequests) {
        clientSocket->write("TICK");
        clientSocket->flush();
        qDebug() << "Tick sent.";
    }
}

void TimeServer::processTick()
{
    // Ваш код обработки тиков
    qDebug() << "Processing tick.";
}
