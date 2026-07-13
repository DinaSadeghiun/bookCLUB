#ifndef DISCOUNTREPOSITORY_H
#define DISCOUNTREPOSITORY_H

#include <QList>
#include <QString>
#include <optional>
#include <QSqlDatabase>
#include "Discount.h"
class DatabaseManager;

class DiscountRepository {
    DatabaseManager* dbManager;

public:
    explicit DiscountRepository(DatabaseManager* manager);

    bool save(Discount& discount);
    bool remove(int id);
    std::optional<Discount> findById(int id) const;
    QList<Discount> findAllActive() const;
    QList<Discount> findAll() const;
};

#endif
