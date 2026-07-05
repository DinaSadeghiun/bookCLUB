#include "admin.h"
#include <QDateTime>

// constructor for creating new admin
Admin::Admin(const QString& username, const QString& password)
    : Person(-1, username, password)
{}

//constructor for LOADING from DB
Admin::Admin(int id, const QString& username, const QString& password)
    : Person(id, username, password)
{}

QString Admin::getRole() const  {
    return "Admin";
}
