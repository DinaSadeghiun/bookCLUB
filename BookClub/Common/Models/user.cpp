#include "user.h"

User::User(int id, const QString& username, const QString& password,
           const QString& email, double balance, const QDateTime& createdAt, bool isActive)
    : Person(id, username, password, email, createdAt, isActive)
{
    // اصلاح ۱: جلوگیری از بالانس منفی در زمان ساخت
    this->walletBalance = (balance >= 0) ? balance : 0.0;

    this->cart = new ShoppingCart(id);
    this->library = new PersonalLibrary(id);
}

User::~User() {
    delete this->cart;
    delete this->library;
}

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

bool User::canLogin() const {
    return this->isActive && !this->isBlocked();
}
