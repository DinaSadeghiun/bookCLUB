#ifndef PERSON_H
#define PERSON_H

#include <QString>
#include <QDateTime>

class Person {
protected:
    int id;
    QString username;
    QString password;
    QString email;
    QDateTime createdAt;
    bool isActive;

public:
    Person(int id = 0, const QString& username = "", const QString& password = "",
           const QString& email = "", const QDateTime& createdAt = QDateTime::currentDateTime(),
           bool isActive = true);

    virtual ~Person() {}

    //getters
    int getId() const;
    QString getUsername() const;
    QString getPassword() const;
    QString getEmail() const;
    QDateTime getCreatedAt() const;
    bool getIsActive() const;

    // Setters
    void setId(int newId);
    void setUsername(const QString &newUsername);
    void setPassword(const QString &newPassword);
    void setEmail(const QString &newEmail);
    void setIsActive(bool status);

    virtual QString getRole() const = 0;
};

#endif
