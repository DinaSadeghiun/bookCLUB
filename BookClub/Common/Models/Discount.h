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
    bool isActive;

public:
    //creating new discount
    Discount(double value = 0.0, DiscountType type = Percentage,
             QDateTime start = QDateTime(), QDateTime end = QDateTime());

    //LOADING from DB
    Discount(int id, double value, DiscountType type,
             QDateTime start, QDateTime end, bool isActive);

    // Getters
    int getId() const;
    double getValue() const;
    DiscountType getType() const;
    QDateTime getStartDate() const;
    QDateTime getEndDate() const;
    bool getIsActive() const;

    // Setters
    void setId(int newId);
    void setValue(double v);
    void setType(DiscountType t);
    void setStartDate(const QDateTime& start);
    void setEndDate(const QDateTime& end);
    void setIsActive(bool status);

    bool isValid() const;

    static QString typeToString(DiscountType t);
    static DiscountType stringToType(const QString& str);
};

#endif
