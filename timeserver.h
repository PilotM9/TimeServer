#ifndef SERVER_TIME_H
#define SERVER_TIME_H

#include <QTcpServer>
#include <QTcpSocket>
#include <QTimer>

class TimeServer : public QTcpServer
{
    Q_OBJECT

public:
    explicit TimeServer(QObject *parent = nullptr);
    ~TimeServer();

private slots:
    void onNewConnection();
    void onReadyRead();
    void onDisconnected();
    void sendTick();
    void processTick();

private:
    QTcpSocket *clientSocket;
    QTcpSocket *mainServerSocket;
    QTimer *tickTimer;
    bool hasRequests; // Флаг, указывающий, есть ли еще заявки

    void startProcessing();  // Запуск обработки заявок
    void stopProcessing();   // Остановка обработки заявок
    void checkMainServerStatus();
    void notifyClientToSend();
};

#endif // SERVER_TIME_H
