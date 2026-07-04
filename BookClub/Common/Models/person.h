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
    Person(int id, const QString& username, const QString& passwordHash,
                   const QString& email, const QDateTime& createdAt, bool isActive);

    Person(const QString& username, const QString& password, const QString& email);

    virtual ~Person() = default;

    // Getters
    int getId() const;
    QString getUsername() const;
    QString getEmail() const;
    QDateTime getCreatedAt() const;
    bool getIsActive() const;

    // Password verification
    bool verifyPassword(const QString& password) const;

    // Setters
    void setId(int newId);
    void setUsername(const QString& username);
    void setPassword(const QString& password);
    void setEmail(const QString& email);
    void setIsActive(bool isActive);

    // Methods
    void active();
    void deactive();
    bool canLogin() const;

    // Pure virtual
    virtual QString getRole() const = 0;

    // Account management
    bool changePassword(const QString& oldPassword, const QString& newPassword);
    bool changeUsername(const QString& newUsername, const QString& password);
    bool changeEmail(const QString& newEmail, const QString& password);

protected:
    static QString hashPassword(const QString& password);
    static QString encryptData(const QString& data);
    static QString decryptData(const QString& encrypted);
};

#endif
