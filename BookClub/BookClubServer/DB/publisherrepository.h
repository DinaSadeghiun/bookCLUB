#ifndef PUBLISHERREPOSITORY_H
#define PUBLISHERREPOSITORY_H

#include <QSqlDatabase>
#include <QSqlQuery>
#include <optional>
#include "Publisher.h"

class PublisherRepository {
public:
    explicit PublisherRepository(const QString& connectionName = "");

    bool save(Publisher& pub);
    std::optional<Publisher> findById(int id) const;
    std::optional<Publisher> findByUsername(const QString& username) const;
    std::optional<Publisher> authenticate(const QString& username, const QString& password);
    bool remove(int id);

private:
    QString connName;
    Publisher fromQuery(QSqlQuery& q) const;
};

#endif
