#ifndef PUBLISHER_H
#define PUBLISHER_H

#include "person.h"
#include <QString>
#include <QList>
#include "Book.h"

// Sales statistics container
struct SalesStats {
    int totalBooksSold = 0;
    double averageRating = 0.0;
    QList<int> topSellingBookIds;
    QList<int> lowSellingBookIds;
    QList<Book> topSellingBooks;
    QList<Book> lowSellingBooks;
};


class Publisher : public Person {

private:
    SalesStats salesStats;
    double revenue = 0.0;


public:
    //creat new
    Publisher(const QString& name, const QString& password, const QString& sa);

    //LOAD from DB
    Publisher(int id, const QString& name, const QString& passwordHash,
              const QDateTime& createdAt, bool isActive, const QString& sa,
              double revenue=0.0);

    ~Publisher() override = default;

    QString getRole() const override;

    // Getters
    double getRevenue() const;

    // Setters
    void setRevenue(double amount);
    void setId(int newId) override;

    // Statistics (computed externally, injected here)
    SalesStats getSalesStats() const;
    void setSalesStats(const SalesStats& stats);

    void addRevenue(double amount);

};

#endif
