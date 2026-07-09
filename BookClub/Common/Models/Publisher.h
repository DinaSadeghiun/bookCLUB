#ifndef PUBLISHER_H
#define PUBLISHER_H

#include "person.h"
#include <QString>
#include <QList>

// Sales statistics container
struct SalesStats {
    int totalBooksSold = 0;
    double averageRating = 0.0;
    QList<int> topSellingBookIds;
    QList<int> lowSellingBookIds;
};


class Publisher : public Person {

private:
    QString companyName;
   // QList<int> bookIds;
    //QList<int> discountIds;
    SalesStats salesStats;
    double revenue = 0.0;


public:
    //creat new
    Publisher(const QString& name, const QString& password, const QString& sa, const QString& companyName);

    //LOAD from DB
    Publisher(int id, const QString& name, const QString& passwordHash,
              const QDateTime& createdAt, bool isActive, const QString& sa, const QString& companyName);

    ~Publisher() override = default;

    QString getRole() const override;

    // Getters
    QString getCompanyName() const;
    double getRevenue() const;
    //QList<int> getBookIds() const;
    //QList<int> getDiscountIds() const;

    // Setters
    void setCompanyName(const QString& name);
    void setRevenue(double amount);
    void setId(int newId) override;

    // Book management
    void addBook(int bookId);
    void removeBook(int bookId);
    bool hasBook(int bookId) const;

    // Discount management
   /* void addDiscount(int discountId);
    void removeDiscount(int discountId);
    bool hasDiscount(int discountId) const;*/

    // Statistics (computed externally, injected here)
    SalesStats getSalesStats() const;
    void setSalesStats(const SalesStats& stats);

    void addRevenue(double amount);

};

#endif
