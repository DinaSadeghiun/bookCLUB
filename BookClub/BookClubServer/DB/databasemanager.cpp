#include "databasemanager.h"

DatabaseManager::DatabaseManager() {}

DatabaseManager& DatabaseManager::instance() {
    static DatabaseManager inst;
    return inst;
}

void DatabaseManager::closeDatabase() {
    if (db.isOpen()) db.close();
}

bool DatabaseManager::initDatabase(const QString& dbName) {
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(dbName);
    if (!db.open()) {
        qCritical() << "DB open failed:" << db.lastError().text();
        return false;
    }
    enableForeignKeys();
    return createTables();
}

void DatabaseManager::enableForeignKeys() {
    QSqlQuery q;
    q.exec("PRAGMA foreign_keys = ON;");
}

bool DatabaseManager::createTables() {
    QSqlQuery q;

    // persons (base for User/Publisher/Admin)
    if (!q.exec(R"(
        CREATE TABLE IF NOT EXISTS Persons (
            id              INTEGER PRIMARY KEY AUTOINCREMENT,
            username        TEXT    UNIQUE NOT NULL,
            password_hash   TEXT    NOT NULL,
            role            TEXT    NOT NULL CHECK(role IN ('User','Publisher','Admin')),
            created_at      INTEGER NOT NULL,
            is_active       INTEGER DEFAULT 1,
            security_answer TEXT NOT NULL
        )
    )")) {
        qCritical() << "Persons:" << q.lastError().text(); return false;
    }

    // users extra data
    if (!q.exec(R"(
        CREATE TABLE IF NOT EXISTS Users (
            person_id       INTEGER PRIMARY KEY,
            wallet_balance  REAL    DEFAULT 0.0,
            FOREIGN KEY(person_id) REFERENCES Persons(id) ON DELETE CASCADE
        )
    )")) {
        qCritical() << "Users:" << q.lastError().text(); return false;
    }

    // user genre preferences (junction)
    if (!q.exec(R"(
        CREATE TABLE IF NOT EXISTS UserGenres (
            user_id INTEGER NOT NULL,
            genre   INTEGER NOT NULL,
            PRIMARY KEY(user_id, genre),
            FOREIGN KEY(user_id) REFERENCES Users(person_id) ON DELETE CASCADE
        )
    )")) {
        qCritical() << "UserGenres:" << q.lastError().text(); return false;
    }

    // publishers extra data
    if (!q.exec(R"(
        CREATE TABLE IF NOT EXISTS Publishers (
            person_id     INTEGER PRIMARY KEY,
            company_name  TEXT NOT NULL,
            total_revenue REAL DEFAULT 0.0,
            FOREIGN KEY(person_id) REFERENCES Persons(id) ON DELETE CASCADE
        )
    )")) {
        qCritical() << "Publishers:" << q.lastError().text(); return false;
    }

    // discounts (must be before Books)
    if (!q.exec(R"(
        CREATE TABLE IF NOT EXISTS Discounts (
            id         INTEGER PRIMARY KEY AUTOINCREMENT,
            value      REAL    NOT NULL,
            type       INTEGER NOT NULL CHECK(type IN (0,1)),
            start_date INTEGER NOT NULL,
            end_date   INTEGER NOT NULL,
            is_active  INTEGER DEFAULT 1
        )
    )")) {
        qCritical() << "Discounts:" << q.lastError().text(); return false;
    }

    // books
    if (!q.exec(R"(
        CREATE TABLE IF NOT EXISTS Books (
            id               INTEGER PRIMARY KEY AUTOINCREMENT,
            publisher_id     INTEGER NOT NULL,
            discount_id      INTEGER DEFAULT NULL,
            title            TEXT    NOT NULL,
            author           TEXT    NOT NULL,
            genre            INTEGER NOT NULL,
            description      TEXT,
            cover_image_path TEXT,
            pdf_file_path    TEXT,
            price            REAL    DEFAULT 0.0,
            total_rating     INTEGER DEFAULT 0,
            rating_count     INTEGER DEFAULT 0,
            sales_count      INTEGER DEFAULT 0,
            is_available     INTEGER DEFAULT 1,
            FOREIGN KEY(publisher_id) REFERENCES Publishers(person_id),
            FOREIGN KEY(discount_id)  REFERENCES Discounts(id) ON DELETE SET NULL
        )
    )")) {
        qCritical() << "Books:" << q.lastError().text(); return false;
    }

    // shopping carts
    if (!q.exec(R"(
        CREATE TABLE IF NOT EXISTS ShoppingCarts (
            id         INTEGER PRIMARY KEY AUTOINCREMENT,
            user_id    INTEGER UNIQUE NOT NULL,
            created_at INTEGER NOT NULL,
            FOREIGN KEY(user_id) REFERENCES Users(person_id) ON DELETE CASCADE
        )
    )")) {
        qCritical() << "ShoppingCarts:" << q.lastError().text(); return false;
    }

    if (!q.exec(R"(
        CREATE TABLE IF NOT EXISTS CartItems (
            cart_id INTEGER NOT NULL,
            book_id INTEGER NOT NULL,
            PRIMARY KEY(cart_id, book_id),
            FOREIGN KEY(cart_id) REFERENCES ShoppingCarts(id) ON DELETE CASCADE,
            FOREIGN KEY(book_id) REFERENCES Books(id) ON DELETE CASCADE
        )
    )")) {
        qCritical() << "CartItems:" << q.lastError().text(); return false;
    }

    // purchase history
    if (!q.exec(R"(
        CREATE TABLE IF NOT EXISTS PurchasedBooks (
            user_id      INTEGER NOT NULL,
            book_id      INTEGER NOT NULL,
            purchased_at INTEGER NOT NULL,
            PRIMARY KEY(user_id, book_id),
            FOREIGN KEY(user_id) REFERENCES Users(person_id) ON DELETE CASCADE,
            FOREIGN KEY(book_id) REFERENCES Books(id) ON DELETE CASCADE
        )
    )")) {
        qCritical() << "PurchasedBooks:" << q.lastError().text(); return false;
    }

    // wishlist
    if (!q.exec(R"(
        CREATE TABLE IF NOT EXISTS Wishlist (
            user_id  INTEGER NOT NULL,
            book_id  INTEGER NOT NULL,
            added_at INTEGER NOT NULL,
            PRIMARY KEY(user_id, book_id),
            FOREIGN KEY(user_id) REFERENCES Users(person_id) ON DELETE CASCADE,
            FOREIGN KEY(book_id) REFERENCES Books(id) ON DELETE CASCADE
        )
    )")) {
        qCritical() << "Wishlist:" << q.lastError().text(); return false;
    }

    // personal shelves
    if (!q.exec(R"(
        CREATE TABLE IF NOT EXISTS CustomShelves (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            user_id INTEGER NOT NULL,
            name    TEXT    NOT NULL,
            FOREIGN KEY(user_id) REFERENCES Users(person_id) ON DELETE CASCADE
        )
    )")) {
        qCritical() << "CustomShelves:" << q.lastError().text(); return false;
    }

    if (!q.exec(R"(
        CREATE TABLE IF NOT EXISTS ShelfBooks (
            shelf_id INTEGER NOT NULL,
            book_id  INTEGER NOT NULL,
            PRIMARY KEY(shelf_id, book_id),
            FOREIGN KEY(shelf_id) REFERENCES CustomShelves(id) ON DELETE CASCADE,
            FOREIGN KEY(book_id)  REFERENCES Books(id) ON DELETE CASCADE
        )
    )")) {
        qCritical() << "ShelfBooks:" << q.lastError().text(); return false;
    }

    // comments & ratings
    if (!q.exec(R"(
        CREATE TABLE IF NOT EXISTS Comments (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            user_id INTEGER NOT NULL,
            book_id INTEGER NOT NULL,
            text    TEXT    NOT NULL,
            rating  INTEGER NOT NULL CHECK(rating BETWEEN 1 AND 5),
            date    INTEGER NOT NULL,
            FOREIGN KEY(user_id) REFERENCES Users(person_id) ON DELETE CASCADE,
            FOREIGN KEY(book_id) REFERENCES Books(id) ON DELETE CASCADE
        )
    )")) {
        qCritical() << "Comments:" << q.lastError().text(); return false;
    }

    // notifications
    if (!q.exec(R"(
        CREATE TABLE IF NOT EXISTS Notifications (
            id              INTEGER PRIMARY KEY AUTOINCREMENT,
            type            INTEGER NOT NULL CHECK(type IN (0,1,2,3)),
            recipient_id    INTEGER NOT NULL,
            related_book_id INTEGER,
            message         TEXT    NOT NULL,
            created_at      INTEGER NOT NULL,
            is_read         INTEGER DEFAULT 0,
            FOREIGN KEY(recipient_id) REFERENCES Persons(id) ON DELETE CASCADE,
            FOREIGN KEY(related_book_id) REFERENCES Books(id) ON DELETE SET NULL
        )
    )")) {
        qCritical() << "Notifications:" << q.lastError().text(); return false;
    }

    // orders
    if (!q.exec(R"(
        CREATE TABLE IF NOT EXISTS Orders (
            id              INTEGER PRIMARY KEY AUTOINCREMENT,
            user_id         INTEGER NOT NULL,
            order_date      INTEGER NOT NULL,
            raw_price       REAL    NOT NULL,
            discount_amount REAL    NOT NULL DEFAULT 0.0,
            final_price     REAL    NOT NULL,
            FOREIGN KEY(user_id) REFERENCES Users(person_id) ON DELETE CASCADE
        )
    )")) {
        qCritical() << "Orders:" << q.lastError().text(); return false;
    }

    if (!q.exec(R"(
        CREATE TABLE IF NOT EXISTS OrderBooks (
            order_id INTEGER NOT NULL,
            book_id  INTEGER NOT NULL,
            price    REAL    NOT NULL,
            PRIMARY KEY(order_id, book_id),
            FOREIGN KEY(order_id) REFERENCES Orders(id) ON DELETE CASCADE,
            FOREIGN KEY(book_id)  REFERENCES Books(id)
        )
    )")) {
        qCritical() << "OrderBooks:" << q.lastError().text(); return false;
    }

    qDebug() << "All tables created successfully.";
    return true;
}
