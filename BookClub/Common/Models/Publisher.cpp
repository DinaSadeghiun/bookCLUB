#include "Publisher.h"

//creat new
Publisher::Publisher(const QString& name, const QString& password, const QString& sa)
    : Person(name, password, sa),
{}

//LOAD from DB
Publisher::Publisher(int id, const QString& name, const QString& passwordHash,
                     const QDateTime& createdAt, bool isActive, const QString& sa, double revenue)
    : Person(id, name, passwordHash, createdAt, isActive, sa),
    revenue(revenue)
{}


QString Publisher::getRole() const {
    return "Publisher";
}

// Getters
double Publisher::getRevenue() const { return revenue; }

// Setters
void Publisher::setId(int newId) {
    if (id == -1){
    id = newId;
    }
}
void Publisher::setRevenue(double amount) { revenue = amount; }

// Statistics (computed externally, injected here)
SalesStats Publisher::getSalesStats() const { return salesStats; }
void Publisher::setSalesStats(const SalesStats& stats) { this->salesStats = stats; }

void Publisher::addRevenue(double amount) { revenue += amount; }
