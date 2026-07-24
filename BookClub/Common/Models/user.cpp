#include "user.h"

//creat new
User::User(const QString& username, const QString& password, const QString& sa, QList<Genre> favs)
    : Person(username, password, sa),
    favoriteGenres(favs)
{
    cart = new ShoppingCart(this->id);
    library = new PersonalLibrary(this->id);
}

//LOAD from DB
User::User(int id, const QString& username, const QString& passwordHash, const QDateTime& createdAt,
           bool isActive, const QString& sa, QList<Genre> favs)
    : Person(id, username, passwordHash, createdAt, isActive, sa),
    favoriteGenres(favs)
{
    cart = new ShoppingCart(this->id);
    library = new PersonalLibrary(this->id);
}

User::~User() {
    delete this->cart;
    delete this->library;
}

// Deep Copy
User::User(const User& other)
    : Person(other), favoriteGenres(other.favoriteGenres)
{
    this->cart = new ShoppingCart(*(other.cart));
    this->library = new PersonalLibrary(*(other.library));
}

// Copy Assignment Operator
User& User::operator=(const User& other) {
    if (this != &other) {
        Person::operator=(other);

        this->favoriteGenres = other.favoriteGenres;

        delete this->cart;
        delete this->library;

        this->cart = other.cart ? new ShoppingCart(*(other.cart)) : nullptr;
        this->library = other.library ? new PersonalLibrary(*(other.library)) : nullptr;
    }
    return *this;
}

// Move Constructor
User::User(User&& other) noexcept
    : Person(std::move(other)),
    favoriteGenres(std::move(other.favoriteGenres)),
    cart(other.cart),
    library(other.library)
{
    other.cart = nullptr;
    other.library = nullptr;
}

// Move Assignment Operator
User& User::operator=(User&& other) noexcept {
    if (this != &other) {
        Person::operator=(std::move(other));

        this->favoriteGenres = std::move(other.favoriteGenres);

        delete this->cart;
        delete this->library;

        this->cart = other.cart;
        this->library = other.library;

        other.cart = nullptr;
        other.library = nullptr;
    }
    return *this;
}

//getters
QString User::getRole() const {
    return "User";
}

QList<Genre> User::getFavoriteGenres() const {
    return favoriteGenres;
}

ShoppingCart* User::getCart() const {
    return this->cart;
}

PersonalLibrary* User::getLibrary() const {
    return this->library;
}

//setter
void User::setId(int newId) {
    if (id == -1) {
        id = newId;
        this->cart->setUserId(newId);
        this->library->setUserId(newId);
    }
}

bool User::setFavoriteGenres(const QList<Genre>& genres) {
    if (genres.isEmpty() || genres.size() > 3) return false;
    favoriteGenres = genres;
    return true;
}

//methods
void User::addFavoriteGenre(Genre g) {
    if (!favoriteGenres.contains(g)) favoriteGenres.append(g);
}
