#include "Publisher.h"

//creat new
Publisher::Publisher(const QString& name, const QString& password, const QString& sa, const QString& companyName)
    : Person(name, password, sa),
    companyName(companyName)
{}

//LOAD from DB
Publisher::Publisher(int id, const QString& name, const QString& passwordHash,
                     const QDateTime& createdAt, bool isActive, const QString& sa,
                     const QString& companyName, double revenue)
    : Person(id, name, passwordHash, createdAt, isActive, sa),
    companyName(companyName),
    revenue(revenue)
{}


QString Publisher::getRole() const {
    return "Publisher";
}

// Getters
QString Publisher::getCompanyName() const { return companyName; }
double Publisher::getRevenue() const { return revenue; }

// Setters
void Publisher::setCompanyName(const QString& name) { this->companyName = name; }
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
