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
    //creat new
    User(const QString& username, const QString& password,  double balance);

    //LOAD from DB
    User(int id, const QString& username, const QString& passwordHash,
                double balance,
               const QDateTime& createdAt, bool isActive);


    ~User() override;

    QString getRole() const override;

    // Getters
    double getWalletBalance() const;
    ShoppingCart* getCart() const;
    PersonalLibrary* getLibrary() const;

    //setter
    void setId(int newId) override;

    // Methods
    bool deposit(double amount);
    bool withdraw(double amount);
};

#endif
