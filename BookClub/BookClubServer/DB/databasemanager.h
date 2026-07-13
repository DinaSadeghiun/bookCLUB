#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QString>
#include <QDebug>

class DatabaseManager {
    QSqlDatabase db;
    DatabaseManager();

public:
    static DatabaseManager& instance();
    bool initDatabase(const QString& dbName = "bookclub.db");
    void closeDatabase();
    QSqlDatabase getDatabase() const;

private:
    bool createTables();
    void enableForeignKeys();
};

#endif
