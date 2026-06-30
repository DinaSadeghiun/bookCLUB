#include "databasemanager.h"
#include <QDebug>

DatabaseManager::DatabaseManager() {}

DatabaseManager& DatabaseManager::instance() {
    static DatabaseManager instance;
    return instance;
}

bool DatabaseManager::initDatabase(const QString& dbName) {
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(dbName);

    if (!db.open()) {
        qDebug() << "Error opening database:" << db.lastError().text();
        return false;
    }

    QSqlQuery query;
    QString createUsersTable = "CREATE TABLE IF NOT EXISTS Users ("
                               "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                               "username TEXT UNIQUE, "
                               "password TEXT, "
                               "email TEXT, "
                               "role TEXT)";

    if (!query.exec(createUsersTable)) {
        qDebug() << "Error creating table:" << query.lastError().text();
        return false;
    }

    qDebug() << "Database initialized successfully.";
    return true;
}

bool DatabaseManager::registerUser(const QString& username, const QString& password, const QString& email, const QString& role) {
    QSqlQuery query;
    query.prepare("INSERT INTO Users (username, password, email, role) VALUES (:username, :password, :email, :role)");
    query.bindValue(":username", username);
    query.bindValue(":password", password);
    query.bindValue(":email", email);
    query.bindValue(":role", role);

    return query.exec();
}
