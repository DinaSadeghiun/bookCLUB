#ifndef USER_H
#define USER_H

#include "person.h"
#include "shoppingcart.h"
#include "personallibrary.h"

class User : public Person {
private:
    double walletBalance;
    ShoppingCart* cart;
    PersonalLibrary* library;

public:
    User(int id = 0, const QString& username = "", const QString& password = "",
         const QString& email = "", double balance = 0.0,
         const QDateTime& createdAt = QDateTime::currentDateTime(), bool isActive = true);

    ~User() override;

    QString getRole() const override;

    // Getters
    double getWalletBalance() const;
    ShoppingCart* getCart() const;
    PersonalLibrary* getLibrary() const;

    // Methods
    bool deposit(double amount);
    bool withdraw(double amount);
    bool canLogin() const;
};

#endif
