#ifndef ADMINSERVICE_H
#define ADMINSERVICE_H

#include <QString>
#include <optional>
#include "Admin.h"
#include "DB/adminrepository.h"

class AdminService {
private:
    AdminRepository* adminRepo;

public:
    explicit AdminService(AdminRepository* repo);
    QString registerAdmin(const QString& username, const QString& password, const QString& securityAnswer);
    std::optional<Admin> loginAdmin(const QString& username, const QString& password);
    bool resetPasswordWithSecurityAnswer(const QString& username, const QString& answer, const QString& newPassword);
};

#endif
