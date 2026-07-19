#include "clienthandler.h"
#include <QJsonDocument>
#include <QJsonParseError>
#include <QDebug>

ClientHandler::ClientHandler(QTcpSocket* clientSocket, QObject* parent)
    : QObject(parent), socket(clientSocket), userId(-1) {

    connect(socket, &QTcpSocket::readyRead, this, &ClientHandler::onReadyRead);
    connect(socket, &QTcpSocket::disconnected, this, &ClientHandler::onDisconnected);
}

ClientHandler::~ClientHandler() {
    if (socket) {
        socket->close();
        socket->deleteLater();
    }
}

int ClientHandler::getUserId() const {
    return userId;
}

void ClientHandler::setUserId(int id) {
    userId = id;
}

QTcpSocket* ClientHandler::getSocket() const {
    return socket;
}

void ClientHandler::sendResponse(const QJsonObject& response) {
    if (!socket || socket->state() != QAbstractSocket::ConnectedState) {
        return;
    }
    QJsonDocument doc(response);
    QByteArray data = doc.toJson(QJsonDocument::Compact) + "\n";
    socket->write(data);
    socket->flush();
}

void ClientHandler::onReadyRead() {
    buffer.append(socket->readAll());

    int newlineIndex;
    while ((newlineIndex = buffer.indexOf('\n')) != -1) {
        QByteArray line = buffer.left(newlineIndex).trimmed();
        buffer.remove(0, newlineIndex + 1);

        if (line.isEmpty()) {
            continue;
        }

        QJsonParseError parseError;
        QJsonDocument doc = QJsonDocument::fromJson(line, &parseError);

        if (parseError.error == QJsonParseError::NoError && doc.isObject()) {
            QJsonObject request = doc.object();
            emit requestReceived(this, request);
        } else {
            qWarning() << "Invalid JSON received from client:" << parseError.errorString();

            QJsonObject errorResponse;
            errorResponse["action"] = "error";
            errorResponse["status"] = "error";
            errorResponse["message"] = "Invalid JSON format";
            sendResponse(errorResponse);
        }
    }
}

void ClientHandler::onDisconnected() {
    emit disconnected(this);
    this->deleteLater();
}

