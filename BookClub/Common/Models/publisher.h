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
    Publisher();
    Publisher(int id, const QString& username, const QString& password,
              const QString& email, const QString& companyName);

    ~Publisher() override = default;

    QString getRole() const override;

    // Getters
    QString getCompanyName() const { return companyName; }
    QList<int> getBookIds() const { return bookIds; }
    QList<int> getDiscountIds() const { return discountIds; }

    // Setters
    void setCompanyName(const QString& name) { this->companyName = name; }

    // Book management
    void addBook(int bookId);
    void removeBook(int bookId);
    bool hasBook(int bookId) const;

    // Discount management
    void addDiscount(int discountId);
    void removeDiscount(int discountId);
    bool hasDiscount(int discountId) const;

    // Statistics (computed externally, injected here)
    SalesStats getSalesStats() const { return salesStats; }
    void setSalesStats(const SalesStats& stats) { this->salesStats = stats; }


};

#endif
