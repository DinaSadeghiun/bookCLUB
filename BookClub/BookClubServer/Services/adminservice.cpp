#include "adminservice.h"

AdminService::AdminService(AdminRepository* repo) : adminRepo(repo) {}

QString AdminService::registerAdmin(const QString& username, const QString& password, const QString& securityAnswer) {
    if (username.trimmed().isEmpty() || password.trimmed().isEmpty() || securityAnswer.trimmed().isEmpty()) {
        return "EMPTY_FIELDS";
    }

    // Check if admin already exists
    if (adminRepo->findByUsername(username).has_value()) {
        return "USERNAME_TAKEN";
    }

    // Create and save new admin
    Admin newAdmin(username, password, securityAnswer);

    if (adminRepo->save(newAdmin)) {
        return "SUCCESS";
    }

    return "DATABASE_ERROR";
}

std::optional<Admin> AdminService::loginAdmin(const QString& username, const QString& password) {
    auto adminOpt = adminRepo->findByUsername(username);

    if (adminOpt && adminOpt->verifyPassword(password)) {
        return adminOpt;
    }

    return std::nullopt;
}

bool AdminService::resetPasswordWithSecurityAnswer(const QString& username,
                                                   const QString& answer,
                                                   const QString& newPassword)
{
    auto trimmedUsername = username.trimmed();
    auto trimmedAnswer = answer.trimmed();
    auto trimmedNewPassword = newPassword.trimmed();

    if (trimmedUsername.isEmpty() || trimmedAnswer.isEmpty() || trimmedNewPassword.isEmpty()) {
        return false;
    }

    auto adminOpt = adminRepo->findByUsername(trimmedUsername);
    if (!adminOpt) {
        return false;
    }

    if (adminOpt->changePasswordWithSecurityAnswer(trimmedAnswer, trimmedNewPassword)) {
        return adminRepo->save(*adminOpt);
    }

    return false;
}
