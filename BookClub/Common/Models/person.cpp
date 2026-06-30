#include "person.h"

Person::Person(int id, const QString& username, const QString& password, const QString& email, const QDateTime& createdAt, bool isActive)
    : id(id), username(username), password(password), email(email), createdAt(createdAt), isActive(isActive) {}

int Person::getId() const { return id; }
QString Person::getUsername() const { return username; }
QString Person::getPassword() const { return password; }
QString Person::getEmail() const { return email; }
QDateTime Person::getCreatedAt() const { return createdAt; }
bool Person::getIsActive() const { return isActive; }

void Person::setId(int newId) { id = newId; }
void Person::setUsername(const QString &newUsername) { username = newUsername; }
void Person::setPassword(const QString &newPassword) { password = newPassword; }
void Person::setEmail(const QString &newEmail) { email = newEmail; }
void Person::setIsActive(bool status) { isActive = status; }
