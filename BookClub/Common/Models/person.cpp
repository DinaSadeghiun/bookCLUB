#include "person.h"

Person::Person(int id, const QString& username, const QString& password,
               const QString& email, const QDateTime& createdAt, bool isActive) {
    this->id = id;
    this->username = encryptData(username);
    this->passwordHash = hashPassword(password);
    this->email = encryptData(email);
    this->createdAt = createdAt;
    this->isActive = isActive;
}

Person::~Person() {}

// Getters
int Person::getId() const {
    return this->id;
}

QString Person::getUsername() const {
    return decryptData(this->username);
}

QString Person::getEmail() const {
    return decryptData(this->email);
}

QDateTime Person::getCreatedAt() const {
    return this->createdAt;
}

bool Person::getIsActive() const {
    return this->isActive;
}

// Security Methods
bool Person::verifyPassword(const QString& password) const {
    return this->passwordHash == hashPassword(password);
}

// Setters
void Person::setId(int id) {
    this->id = id;
}

void Person::setUsername(const QString& username) {
    this->username = encryptData(username);
}

void Person::setPassword(const QString& password) {
    this->passwordHash = hashPassword(password);
}

void Person::setEmail(const QString& email) {
    this->email = encryptData(email);
}

void Person::setIsActive(bool isActive) {
    this->isActive = isActive;
}

// Status Methods
void Person::blockAccount() {
    this->isActive = false;
}

void Person::unblockAccount() {
    this->isActive = true;
}

bool Person::isBlocked() const {
    return !this->isActive;
}


QString Person::hashPassword(const QString& password) {
    return QString(QCryptographicHash::hash(
                       password.toUtf8(),
                       QCryptographicHash::Sha256
                       ).toHex());
}

QString Person::encryptData(const QString& data) {
    if (data.isEmpty()) return "";
    const QString key = "MySecretKey2026";
    QByteArray result;
    QByteArray dataBytes = data.toUtf8();
    QByteArray keyBytes = key.toUtf8();

    for (int i = 0; i < dataBytes.size(); ++i) {
        result.append(dataBytes[i] ^ keyBytes[i % keyBytes.size()]);
    }
    return QString(result.toBase64());
}

QString Person::decryptData(const QString& encrypted) {
    if (encrypted.isEmpty()) return "";
    const QString key = "MySecretKey2026";
    QByteArray encryptedBytes = QByteArray::fromBase64(encrypted.toUtf8());
    QByteArray keyBytes = key.toUtf8();
    QByteArray result;

    for (int i = 0; i < encryptedBytes.size(); ++i) {
        result.append(encryptedBytes[i] ^ keyBytes[i % keyBytes.size()]);
    }
    return QString::fromUtf8(result);
}
