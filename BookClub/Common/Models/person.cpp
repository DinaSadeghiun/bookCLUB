#include "person.h"

//creat new
Person::Person(const QString& username, const QString& password, const QString& sa)
    : id(-1),
    username(encryptData(username)),
    createdAt(QDateTime::currentDateTime()),
    isActive(true),
    securityAnswer(sa)
{
    this->passwordHash = hashPassword(password);
}

//LOAD from DB
Person::Person(int id, const QString& username, const QString& passwordHash,
               const QDateTime& createdAt, bool isActive, const QString& sa)
    : id(id),
    username(username),
    passwordHash(passwordHash),
    createdAt(createdAt),
    isActive(isActive),
    securityAnswer(sa)
{}


// Getters
int Person::getId() const { return id; }
QString Person::getUsername() const { return decryptData(username); }
QDateTime Person::getCreatedAt() const { return createdAt; }
bool Person::getIsActive() const { return isActive; }
QString Person::getPasswordHash() const { return passwordHash; }
QString Person::getSecurityAnswer() const { return securityAnswer; }


// Security Methods
bool Person::verifyPassword(const QString& password) const {
    return this->passwordHash == hashPassword(password);
}

// Setters
void Person::setId(int newId) {
    if (id == -1) {
        id= newId;
    }
}

void Person::setUsername(const QString& username) {
    this->username = encryptData(username);
}

void Person::setPassword(const QString& password) {
    this->passwordHash = hashPassword(password);
}

void Person::setIsActive(bool isActive) {
    this->isActive = isActive;
}

void Person::setSecurityAnswer(const QString& sa) {
    securityAnswer = sa;
}


// Status Methods
void Person::active() {
    this->isActive = true;
}

void Person::deactive() {
    this->isActive = false;
}

bool Person::canLogin() const {
    return isActive;
}



//Account Managemenet
bool Person::changePassword(const QString& oldPassword, const QString& newPassword) {
    if (!verifyPassword(oldPassword)) {
        return false;
    }

    if (newPassword.trimmed().isEmpty()) {
        return false;
    }

    setPassword(newPassword);
    return true;
}

bool Person::changeUsername(const QString& newUsername, const QString& password) {
    if (!verifyPassword(password)) {
        return false;
    }

    if (newUsername.trimmed().isEmpty()) {
        return false;
    }

    this->username = encryptData(newUsername);
    return true;
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
