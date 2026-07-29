#include "DB/bookrepository.h"
#include "DB/databasemanager.h"
#include <QSqlError>
#include <QVariant>
#include <QDebug>

BookRepository::BookRepository(DatabaseManager* manager)
    : dbManager(manager) {}


Book BookRepository::fromQuery(QSqlQuery& q) const {
    int id = q.value("id").toInt();
    int pubId = q.value("publisher_id").toInt();
    int discId = q.value("discount_id").isNull() ? -1 : q.value("discount_id").toInt();
    QString title = q.value("title").toString();
    QString author = q.value("author").toString();
    Genre genre = static_cast<Genre>(q.value("genre").toInt());
    QString desc = q.value("description").toString();
    QString cover = q.value("cover_image_path").toString();
    QString pdf = q.value("pdf_file_path").toString();
    double price = q.value("price").toDouble();
    double totRating = q.value("total_rating").toDouble();
    int ratingCnt = q.value("rating_count").toInt();
    int salesCnt = q.value("sales_count").toInt();
    bool avail = q.value("is_available").toInt() != 0;

    return Book(id, pubId, discId, title, author, genre, desc, cover, pdf, price, totRating, ratingCnt, salesCnt, avail);
}

bool BookRepository::save(Book& book) {
    QSqlDatabase db = dbManager->getDatabase();
    QSqlQuery q(db);

    if (book.getId() == -1) {
        // INSERT
        q.prepare("INSERT INTO Books (publisher_id, discount_id, title, author, genre, "
                  "description, cover_image_path, pdf_file_path, price, total_rating, "
                  "rating_count, sales_count, is_available) "
                  "VALUES (:pub_id, :disc_id, :title, :author, :genre, :desc, "
                  ":cover, :pdf, :price, :tot_rating, :rating_cnt, :sales_cnt, :avail)");

        q.bindValue(":pub_id", book.getPublisherId());
        q.bindValue(":disc_id", book.getDiscountId() == -1 ? QVariant() : book.getDiscountId());
        q.bindValue(":title", book.getTitle());
        q.bindValue(":author", book.getAuthor());
        q.bindValue(":genre", static_cast<int>(book.getGenre()));
        q.bindValue(":desc", book.getDescription());
        q.bindValue(":cover", book.getCoverImagePath());
        q.bindValue(":pdf", book.getPdfFilePath());
        q.bindValue(":price", book.getPrice());
        q.bindValue(":tot_rating", book.getTotalRating());
        q.bindValue(":rating_cnt", book.getRatingCount());
        q.bindValue(":sales_cnt", book.getSalesCount());
        q.bindValue(":avail", book.getIsAvailable() ? 1 : 0);

        if (!q.exec()) {
            qDebug() << "Books Insert Failed:" << q.lastError().text();
            return false;
        }

        book.setId(q.lastInsertId().toInt());
        return true;
    } else {
        // UPDATE
        q.prepare("UPDATE Books SET publisher_id = :pub_id, discount_id = :disc_id, "
                  "title = :title, author = :author, genre = :genre, description = :desc, "
                  "cover_image_path = :cover, pdf_file_path = :pdf, price = :price, "
                  "total_rating = :tot_rating, rating_count = :rating_cnt, "
                  "sales_count = :sales_cnt, is_available = :avail "
                  "WHERE id = :id");

        q.bindValue(":id", book.getId());
        q.bindValue(":pub_id", book.getPublisherId());
        q.bindValue(":disc_id", book.getDiscountId() == -1 ? QVariant() : book.getDiscountId());
        q.bindValue(":title", book.getTitle());
        q.bindValue(":author", book.getAuthor());
        q.bindValue(":genre", static_cast<int>(book.getGenre()));
        q.bindValue(":desc", book.getDescription());
        q.bindValue(":cover", book.getCoverImagePath());
        q.bindValue(":pdf", book.getPdfFilePath());
        q.bindValue(":price", book.getPrice());
        q.bindValue(":tot_rating", book.getTotalRating());
        q.bindValue(":rating_cnt", book.getRatingCount());
        q.bindValue(":sales_cnt", book.getSalesCount());
        q.bindValue(":avail", book.getIsAvailable() ? 1 : 0);

        if (!q.exec()) {
            qDebug() << "Books Update Failed:" << q.lastError().text();
            return false;
        }
        if (q.numRowsAffected() <= 0) {
            qDebug() << "Books Update: No rows affected for id" << book.getId();
            return false;
        }
        return true;
    }
}

bool BookRepository::incrementSalesCount(int bookId) {
    QSqlDatabase db = dbManager->getDatabase();
    QSqlQuery q(db);
    q.prepare("UPDATE Books SET sales_count = sales_count + 1 WHERE id = :id");
    q.bindValue(":id", bookId);
    return q.exec() && q.numRowsAffected() > 0;
}

std::optional<Book> BookRepository::findById(int id) {
    QSqlDatabase db = dbManager->getDatabase();
    QSqlQuery q(db);

    q.prepare("SELECT * FROM Books WHERE id = :id");
    q.bindValue(":id", id);

    if (q.exec()) {
        if (q.next()) {
            return fromQuery(q);
        }
    } else {
        qDebug() << "findById Query Failed (Book):" << q.lastError().text();
    }
    return std::nullopt;
}

QList<Book> BookRepository::findAll() {
    QSqlDatabase db = dbManager->getDatabase();
    QList<Book> list;
    QSqlQuery q(db);
    q.prepare("SELECT * FROM Books WHERE is_available = 1");

    if (q.exec()) {
        while (q.next()) {
            list.append(fromQuery(q));
        }
    } else {
        qDebug() << "findAll Query Failed (Book):" << q.lastError().text();
    }
    return list;
}

QList<Book> BookRepository::findByPublisherId(int publisherId) {
    QSqlDatabase db = dbManager->getDatabase();
    QList<Book> list;
    QSqlQuery q(db);
    q.prepare("SELECT * FROM Books WHERE publisher_id = ?");
    q.addBindValue(publisherId);

    if (q.exec()) {
        while (q.next()) {
            list.append(fromQuery(q));
        }
    } else {
        qDebug() << "findByPublisherId Query Failed:" << q.lastError().text();
    }
    return list;
}

QList<Book> BookRepository::findByGenre(Genre genre) {
    QList<Book> list;
    QSqlDatabase db = dbManager->getDatabase();
    QSqlQuery q(db);
    q.prepare("SELECT * FROM Books WHERE genre = ? AND is_available = 1");
    q.addBindValue(static_cast<int>(genre));

    if (q.exec()) {
        while (q.next()) {
            list.append(fromQuery(q));
        }
    } else {
        qDebug() << "findByGenre Query Failed:" << q.lastError().text();
    }
    return list;
}

bool BookRepository::remove(int id) {
    QSqlDatabase db = dbManager->getDatabase();
    QSqlQuery q(db);
    q.prepare("UPDATE Books SET is_available = 0 WHERE id = ?");
    q.addBindValue(id);
    return q.exec() && q.numRowsAffected() > 0;
}

bool BookRepository::activate(int id) {
    QSqlDatabase db = dbManager->getDatabase();
    QSqlQuery q(db);
    q.prepare("UPDATE Books SET is_available = 1 WHERE id = ?");
    q.addBindValue(id);
    return q.exec() && q.numRowsAffected() > 0;
}

//search by name or author name
QList<Book> BookRepository::searchBooks(const QString& query) {
    QList<Book> list;
    QSqlDatabase db = dbManager->getDatabase();
    QSqlQuery q(db);

    q.prepare("SELECT * FROM Books "
              "WHERE (title LIKE :query1 OR author LIKE :query2) "
              "AND is_available = 1");

    QString searchPattern = "%" + query.trimmed() + "%";
    q.bindValue(":query1", searchPattern);
    q.bindValue(":query2", searchPattern);

    if (q.exec()) {
        while (q.next()) list.append(fromQuery(q));
    } else {
        qDebug() << "searchBooks Query Failed:" << q.lastError().text();
    }
    return list;
}


