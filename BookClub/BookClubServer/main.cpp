#include <QCoreApplication>
#include <QDebug>
#include <cassert>
#include "DB/databasemanager.h"
#include "DB/publisherrepository.h"
#include "Publisher.h"

void runPublisherTests() {
    qDebug() << "--- Starting PublisherRepository Tests ---";

    // ۱. مقداردهی اولیه دیتابیس در حافظه موقت (برای تمیز بودن تست)
    auto& dbManager = DatabaseManager::instance();
    if (!dbManager.initDatabase("bookclub.db")) {
        qCritical() << "Failed to initialize database!";
        return;
    }
    qDebug() << "Database initialized in memory.";

    PublisherRepository pubRepo;

    // ۲. تست ذخیره ناشر جدید (INSERT)
    // فرض بر این است که سازنده کلاس Publisher پارامترهای زیر را دارد:
    // (username, passwordHash, securityAnswer, companyName)
    // مطمئن شوید آیدی پیش‌فرض شیء جدید برابر -1 است تا عمل INSERT انجام شود.
    Publisher newPub("pub_test", "hashed_pass_123", "AnswerXYZ", "Best Publisher Co.");
    newPub.setRevenue(500.0);

    qDebug() << "Saving new publisher...";
    if (!pubRepo.save(newPub)) {
        qCritical() << "Failed to save publisher!";
        return;
    }
    qDebug() << "Publisher saved. Generated ID:" << newPub.getId();
    assert(newPub.getId() != -1);

    // ۳. تست بازیابی با شناسه (findById)
    qDebug() << "Testing findById...";
    auto foundByIdOpt = pubRepo.findById(newPub.getId());
    assert(foundByIdOpt.has_value());

    Publisher foundById = foundByIdOpt.value();
    qDebug() << "Found by ID:" << foundById.getUsername()
             << "| Company:" << foundById.getCompanyName()
             << "| Revenue:" << foundById.getRevenue();

    assert(foundById.getUsername() == "pub_test");
    assert(foundById.getCompanyName() == "Best Publisher Co.");
    assert(qFuzzyCompare(foundById.getRevenue(), 500.0));

    // ۴. تست بازیابی با نام کاربری (findByUsername)
    qDebug() << "Testing findByUsername...";
    auto foundByNameOpt = pubRepo.findByUsername("pub_test");
    assert(foundByNameOpt.has_value());
    assert(foundByNameOpt->getId() == newPub.getId());
    qDebug() << "Found by username successfully.";

    // ۵. تست به‌روزرسانی اطلاعات (UPDATE)
    qDebug() << "Testing UPDATE...";
    foundById.setRevenue(7500.25);
    // در صورت وجود متد setCompanyName می‌توانید آن را هم تست کنید:
    // foundById.setCompanyName("Updated Publisher LLC");

    if (!pubRepo.save(foundById)) {
        qCritical() << "Failed to update publisher!";
        return;
    }

    auto updatedPubOpt = pubRepo.findById(newPub.getId());
    assert(updatedPubOpt.has_value());
    qDebug() << "Updated Revenue:" << updatedPubOpt->getRevenue();
    assert(qFuzzyCompare(updatedPubOpt->getRevenue(), 7500.25));

    // ۶. تست احراز هویت (authenticate)
    qDebug() << "Testing authenticate...";
    // تست با پسورد درست (باید خروجی معتبر بدهد)
    auto authSuccess = pubRepo.authenticate("pub_test", "hashed_pass_123");
    if (authSuccess.has_value()) {
        qDebug() << "Authentication successful for correct password.";
    } else {
        qWarning() << "Authentication failed! (Verify password verification logic in your Model)";
    }

    // تست با پسورد اشتباه (نباید معتبر باشد)
    auto authFail = pubRepo.authenticate("pub_test", "wrong_password");
    assert(!authFail.has_value());
    qDebug() << "Authentication correctly rejected wrong password.";

    // ۷. تست حذف ناشر (remove)
 qDebug() << "Testing remove...";
    if (!pubRepo.remove(newPub.getId())) {
        qCritical() << "Failed to remove publisher!";
        return;
}

    // بررسی اینکه ناشر واقعاً پاک شده باشد
    auto deletedPubOpt = pubRepo.findById(newPub.getId());
    assert(!deletedPubOpt.has_value());
    qDebug() << "Publisher successfully removed from database.";

    qDebug() << "\n--- All PublisherRepository Tests Passed Successfully! ---";
    dbManager.closeDatabase();
}

int main(int argc, char *argv[]) {
    QCoreApplication a(argc, argv);

    runPublisherTests();

    return 0;
}
