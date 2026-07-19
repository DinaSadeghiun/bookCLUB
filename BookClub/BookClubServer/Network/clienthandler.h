#ifndef CLIENTHANDLER_H
#define CLIENTHANDLER_H

#include <QObject>
#include <QTcpSocket>
#include <QJsonObject>
#include <QByteArray>

class ClientHandler : public QObject {
    Q_OBJECT
private:
    QTcpSocket* socket;
    int userId;
    QByteArray buffer;

public:
    explicit ClientHandler(QTcpSocket* clientSocket, QObject* parent = nullptr);
    ~ClientHandler() override;

    int getUserId() const;
    void setUserId(int id);
    QTcpSocket* getSocket() const;

    void sendResponse(const QJsonObject& response);

signals:
    void requestReceived(ClientHandler* handler, const QJsonObject& request);
    void disconnected(ClientHandler* handler);

private slots:
    void onReadyRead();
    void onDisconnected();
};

#endif
