#include "admin.h"
#include <QDateTime>

// constructor for creating new admin
Admin::Admin(const QString& username, const QString& password)
    : Person(-1, username, password, "admin123@gmail.com")
{}

//constructor for LOADING from DB
Admin::Admin(int id, const QString& username, const QString& password,
             const QList<QString>& logs)
    : Person(id, username, password, "admin123@gmail.com"), actionLogs(logs)
{}

void Admin::logAction(const QString& action) {
    QString timestamp = QDateTime::currentDateTime().toString(Qt::ISODate);
    actionLogs.append(timestamp + " - " + action);
}

QList<QString> Admin::getActionLogs() const {
    return actionLogs;
}
