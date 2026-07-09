#include "admin.h"
#include <QDateTime>

// constructor for creating new admin
Admin::Admin(const QString& username, const QString& password, const QString& sa)
    : Person(username, password, sa)
{}

//constructor for LOADING from DB
Admin::Admin(int id, const QString& username, const QString& passwordHash,
             const QDateTime& createdAt, bool isActive, const QString& sa)
    : Person(id, username, passwordHash, createdAt, isActive, sa)
{}

QString Admin::getRole() const  {
    return "Admin";
}
