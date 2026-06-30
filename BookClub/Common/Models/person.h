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
    Person(int id, QString username, QString password, QString email);
    virtual ~Person() {}

    // Getters
    int getId() const;
    QString getUsername() const;
    QString getEmail() const;
    bool getIsActive() const;

    // Setters
    void setPassword(const QString &newPassword);
    void setIsActive(bool status);

    virtual QString getRole() const = 0;
};

#endif
