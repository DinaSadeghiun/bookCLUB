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
    QDateTime createdAt;
    bool isActive;

public:
    Person(int id, const QString& username, const QString& passwordHash,
                    const QDateTime& createdAt = QDateTime::currentDateTime(), bool isActive = true);

    Person(const QString& username, const QString& password);

    virtual ~Person() = default;

    // Getters
    int getId() const;
    QString getUsername() const;
    QDateTime getCreatedAt() const;
    bool getIsActive() const;

    // Password verification
    bool verifyPassword(const QString& password) const;

    // Setters
    virtual void setId(int newId);
    void setUsername(const QString& username);
    void setPassword(const QString& password);
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

protected:
    static QString hashPassword(const QString& password);
    static QString encryptData(const QString& data);
    static QString decryptData(const QString& encrypted);
};

#endif
