#include "user.h"

//creat new
User::User(const QString& username, const QString& password, const QString& sa, double balance, QList<Genre> favs)
    : Person(username, password, sa),
    walletBalance(balance),
    favoriteGenres(favs)
{
    cart = new ShoppingCart(this->id);
    library = new PersonalLibrary(this->id);
}

//LOAD from DB
User::User(int id, const QString& username, const QString& passwordHash, const QDateTime& createdAt,
           bool isActive, const QString& sa, double balance, QList<Genre> favs)
    : Person(id, username, passwordHash, createdAt, isActive, sa),
    walletBalance(balance),
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
    : Person(other), walletBalance(other.walletBalance), favoriteGenres(other.favoriteGenres)
{
    this->cart = new ShoppingCart(*(other.cart));
    this->library = new PersonalLibrary(*(other.library));
}

// Copy Assignment Operator
User& User::operator=(const User& other) {
    if (this != &other) { \
        Person::operator=(other);

        this->walletBalance = other.walletBalance;
        this->favoriteGenres = other.favoriteGenres;

        delete this->cart;
        delete this->library;

        this->cart = new ShoppingCart(*(other.cart));
        this->library = new PersonalLibrary(*(other.library));
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

double User::getWalletBalance() const {
    return this->walletBalance;
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

void User::setWalletBalance(double amount) {
    walletBalance = amount;
}
//methods
bool User::deposit(double amount) {
    if (amount > 0) {
        this->walletBalance += amount;
        return true;
    }
    return false;
}

bool User::withdraw(double amount) {
    if (amount > 0 && this->walletBalance >= amount) {
        this->walletBalance -= amount;
        return true;
    }
    return false;
}

void User::addFavoriteGenre(Genre g) {
    if (!favoriteGenres.contains(g)) favoriteGenres.append(g);
}

bool User::updateFavoriteGenres(const QList<Genre>& newGenres) {
    if (newGenres.isEmpty() || newGenres.size() > 3) return false;
    favoriteGenres = newGenres;
    return true;
}
