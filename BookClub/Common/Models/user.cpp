#include "user.h"

//creat new
User::User(const QString& username, const QString& password, const QString& email, double balance)
    : Person(username, password, email),
    walletBalance(balance)
{
    cart = new ShoppingCart();
    library = new PersonalLibrary();
}

//LOAD from DB
User::User(int id, const QString& username, const QString& passwordHash,
           const QString& email, double balance,
           const QDateTime& createdAt, bool isActive)
    : Person(id, username, passwordHash, email, createdAt, isActive),
    walletBalance(balance)
{
    cart = new ShoppingCart(this->id);
    library = new PersonalLibrary(this->id);
}

User::~User() {
    delete this->cart;
    delete this->library;
}

//getters
QString User::getRole() const {
    return "User";
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
    if (newId == -1) {
        Person::setId(newId);
        this->cart->setUserId(newId);
        this->library->setUserId(newId);
    }
}


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


