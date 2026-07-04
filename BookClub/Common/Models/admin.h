#ifndef ADMIN_H
#define ADMIN_H

#include "person.h"

class Admin : public Person {
public:
    //constructor for creating new admin
    Admin(const QString& username, const QString& password);
    //constructor for LOADING from DB
    Admin(int id, const QString& username, const QString& password);
    ~Admin() override;
};

#endif
