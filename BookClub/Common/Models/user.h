#ifndef USER_H
#define USER_H

#include "person.h"
#include "genre.h"
#include "shoppingcart.h"
#include "personallibrary.h"

class User : public Person {
private:
    double walletBalance;
    QList<Genre> favoriteGenres;
    ShoppingCart* cart;
    PersonalLibrary* library;

public:
    //creat new
    User(const QString& username, const QString& password, const QString& sa, double balance, QList<Genre> favs = {});

    //LOAD from DB
    User(int id, const QString& username, const QString& passwordHash, const QDateTime& createdAt,
         bool isActive, const QString& sa, double balance, QList<Genre> favs);


    ~User() override;
    User(const User& other); //copy constructor
    User& operator=(const User& other); //copt assignment operator

    QString getRole() const override;

    // Getters
    double getWalletBalance() const;
    QList<Genre> getFavoriteGenres() const;
    ShoppingCart* getCart() const;
    PersonalLibrary* getLibrary() const;

    //setter
    void setId(int newId) override;
    bool setFavoriteGenres(const QList<Genre>& favs);
    void setWalletBalance(double amount);


    // Methods
    bool deposit(double amount);
    bool withdraw(double amount);
    void addFavoriteGenre(Genre g);
    bool updateFavoriteGenres(const QList<Genre>& newGenres);
};

#endif
