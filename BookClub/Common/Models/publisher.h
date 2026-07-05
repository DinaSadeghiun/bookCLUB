#ifndef PUBLISHER_H
#define PUBLISHER_H

#include "person.h"
#include <QString>
#include <QList>

// Sales statistics container
struct SalesStats {
    double totalRevenue = 0.0;
    int totalBooksSold = 0;
    double averageRating = 0.0;
    QList<int> topSellingBookIds;
    QList<int> lowSellingBookIds;
};


class Publisher : public Person {

private:
    QString companyName;
    QList<int> bookIds;
    QList<int> discountIds;
    SalesStats salesStats;

public:
    //creat new
    Publisher(QString name, QString password, QString companyName);

    //LOAD from DB
    Publisher(int id, QString name, QString password, QString companyName);

    ~Publisher() override = default;

    QString getRole() const override;

    // Getters
    QString getCompanyName() const;
    QList<int> getBookIds() const;
    QList<int> getDiscountIds() const;

    // Setters
    void setCompanyName(const QString& name);
    void setId(int newId) override;

    // Book management
    void addBook(int bookId);
    void removeBook(int bookId);
    bool hasBook(int bookId) const;

    // Discount management
    void addDiscount(int discountId);
    void removeDiscount(int discountId);
    bool hasDiscount(int discountId) const;

    // Statistics (computed externally, injected here)
    SalesStats getSalesStats() const;
    void setSalesStats(const SalesStats& stats);

};

#endif
