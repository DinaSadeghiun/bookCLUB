#ifndef ADMIN_H
#define ADMIN_H

#include "person.h"

class Admin : public Person {
public:
    //constructor for creating new admin
    Admin(const QString& username, const QString& password, const QString& sa);

    //constructor for LOADING from DB
    Admin(int id, const QString& username, const QString& passwordHash,
          const QDateTime& createdAt, bool isActive, const QString& sa);

    ~Admin() override = default;

    QString getRole() const override;
};

#endif
