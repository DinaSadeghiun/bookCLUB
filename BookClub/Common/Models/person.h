#ifndef PERSON_H
#define PERSON_H

#include <QString>
#include <QDateTime>
#include <QCryptographicHash>
#include <QByteArray>

class Person {
protected:
    int id;
    QString username;
    QString passwordHash;
    QString email;
    QDateTime createdAt;
    bool isActive;

public:
    Person(int id = 0, const QString& username = "", const QString& password = "",
           const QString& email = "", const QDateTime& createdAt = QDateTime::currentDateTime(),
           bool isActive = true);

    virtual ~Person();

    // Getters
    int getId() const;
    QString getUsername() const;
    QString getEmail() const;
    QDateTime getCreatedAt() const;
    bool getIsActive() const;

    // Password verification
    bool verifyPassword(const QString& password) const;

    // Setters
    void setId(int id);
    void setUsername(const QString& username);
    void setPassword(const QString& password);
    void setEmail(const QString& email);
    void setIsActive(bool isActive);

    // Methods
    void blockAccount();
    void unblockAccount();
    bool isBlocked() const;

    // Pure virtual
    virtual QString getRole() const = 0;

protected:
    static QString hashPassword(const QString& password);
    static QString encryptData(const QString& data);
    static QString decryptData(const QString& encrypted);
};

#endif
