#ifndef DISCOUNT_H
#define DISCOUNT_H

#include <QDateTime>

class Discount {
public:
    enum DiscountType {
        Percentage,
        FixedAmount
    };

private:
    int id;
    double value;
    DiscountType type;
    QDateTime startDate;
    QDateTime endDate;
    bool active;

public:
    Discount(int id = 0,
             double value = 0.0,
             DiscountType type = Percentage,
             QDateTime start = QDateTime(),
             QDateTime end = QDateTime(),
             bool active = true);

    // Getters
    int getId() const;
    double getValue() const;
    DiscountType getType() const;
    QDateTime getStartDate() const;
    QDateTime getEndDate() const;
    bool isActive() const;

    // Setters
    void setValue(double v);
    void setType(DiscountType t);
    void setStartDate(const QDateTime& start);
    void setEndDate(const QDateTime& end);
    void setActive(bool status);

    bool isValid() const;
};

#endif
