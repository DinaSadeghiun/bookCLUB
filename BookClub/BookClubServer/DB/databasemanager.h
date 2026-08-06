#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QString>
#include <QDebug>
#include <QCoreApplication>

class DatabaseManager {
    QSqlDatabase db;
    DatabaseManager();

public:
    static DatabaseManager& instance();
    bool initDatabase(const QString& dbName = "bookclubFinal.db");
    void closeDatabase();
    QSqlDatabase& getDatabase();

private:
    bool createTables();
    void enableForeignKeys();
};

#endif