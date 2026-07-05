#include "discount.h"

//LOADING from DB
Discount::Discount(int id, double value, DiscountType type, QDateTime start, QDateTime end, bool active)
    : id(id),
    value(value),
    type(type),
    startDate(start),
    endDate(end),
    active(active)
{}

//creating new
Discount::Discount(double value, DiscountType type, QDateTime start, QDateTime end)
    : id(-1),
    value(value),
    type(type),
    startDate(start),
    endDate(end),
    active(true)
 {}

// Getters
int Discount::getId() const { return id; }
double Discount::getValue() const { return value; }
Discount::DiscountType Discount::getType() const { return type; }
QDateTime Discount::getStartDate() const { return startDate; }
QDateTime Discount::getEndDate() const { return endDate; }
bool Discount::isActive() const { return active; }

// Setters
void Discount::setId(int newId) {
    if (id == -1) {
        id = newId;
    }
}
void Discount::setValue(double v) { if (v >= 0) value = v; }
void Discount::setType(DiscountType t) { type = t; }
void Discount::setStartDate(const QDateTime& start) { startDate = start; }
void Discount::setEndDate(const QDateTime& end) { endDate = end; }
void Discount::setActive(bool status) { active = status; }

bool Discount::isValid() const {
    if (!active) {
        return false;
    }
    QDateTime now = QDateTime::currentDateTime();
    return (now >= startDate && now <= endDate);
}
