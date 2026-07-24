#ifndef USER_H
#define USER_H
#include "QString"
#include "person.h"
#include "genre.h"
#include "shoppingcart.h"
#include "personallibrary.h"

class User : public Person {
private:
    QList<Genre> favoriteGenres;
    ShoppingCart* cart;
    PersonalLibrary* library;

public:
    //creat new
    User(const QString& username, const QString& password, const QString& sa, QList<Genre> favs = {});

    //LOAD from DB
    User(int id, const QString& username, const QString& passwordHash, const QDateTime& createdAt,
         bool isActive, const QString& sa, QList<Genre> favs={});


    ~User() override;
    User(const User& other); //copy constructor
    User& operator=(const User& other); //copt assignment operator

    // MOVE SEMANTICS
    User(User&& other) noexcept;
    User& operator=(User&& other) noexcept;

    QString getRole() const override;

    // Getters
    QList<Genre> getFavoriteGenres() const;
    ShoppingCart* getCart() const;
    PersonalLibrary* getLibrary() const;

    //setter
    void setId(int newId) override;
    bool setFavoriteGenres(const QList<Genre>& favs);

    // Methods
    void addFavoriteGenre(Genre g);

};

#endif
