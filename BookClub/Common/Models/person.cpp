#include "person.h"

Person::Person(int id, QString username, QString password, QString email)
    : id(id), username(username), password(password), email(email) {
    this->createdAt = QDateTime::currentDateTime();
    this->isActive = true;
}

int Person::getId() const { return id; }
QString Person::getUsername() const { return username; }
QString Person::getEmail() const { return email; }
bool Person::getIsActive() const { return isActive; }

void Person::setPassword(const QString &newPassword) {
    password = newPassword;
}

void Person::setIsActive(bool status) {
    isActive = status;
}
