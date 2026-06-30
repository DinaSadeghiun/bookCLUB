#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QString>

class DatabaseManager {
private:
    QSqlDatabase db;
    DatabaseManager();

public:
    static DatabaseManager& instance();
    bool initDatabase(const QString& dbName = "bookclub.db");
    bool registerUser(const QString& username, const QString& password, const QString& email, const QString& role);
};

#endif
