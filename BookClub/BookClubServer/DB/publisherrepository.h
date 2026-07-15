#ifndef PUBLISHERREPOSITORY_H
#define PUBLISHERREPOSITORY_H

#include <QSqlDatabase>
#include <QSqlQuery>
#include <optional>
#include "Publisher.h"

inline const QString ROLE_PUBLISHER = "Publisher";
class DatabaseManager;

class PublisherRepository {
public:
    explicit PublisherRepository(DatabaseManager* manager);

    bool save(Publisher& pub);
    std::optional<Publisher> findById(int id) const;
    std::optional<Publisher> findByUsername(const QString& username) const;
    std::optional<Publisher> authenticate(const QString& username, const QString& password);
    bool remove(int id);

private:
    DatabaseManager* dbManager;
    Publisher fromQuery(QSqlQuery& q) const;
};

#endif
