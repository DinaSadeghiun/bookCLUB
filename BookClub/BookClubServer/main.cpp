#include <QCoreApplication>
#include "DB/databasemanager.h"
#include "DB/PublisherRepository.h"
#include "Publisher.h"
#include <QDebug>

#ifdef Q_OS_WIN
#include <windows.h>
#endif

int main(int argc, char *argv[]) {
    QCoreApplication a(argc, argv);

#ifdef Q_OS_WIN
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif

    // ۱. راه‌اندازی دیتابیس
    DatabaseManager& dbManager = DatabaseManager::instance();
    if (!dbManager.initDatabase("bookclub-final.db")) {
        qDebug() << "Failed to open database!";
        return -1;
    }

    PublisherRepository pubRepo;

    // ۲. تست ثبت‌نام (Save یک ناشر جدید)
    qDebug() << "--- Testing Publisher Save ---";
    Publisher newPub(-1, "nashr_e_no", "1234", QDateTime::currentDateTime(), true, "Tehran", "Nashr-e No");
    newPub.setRevenue(500000.0);

    if (pubRepo.save(newPub)) {
        qDebug() << "Publisher saved successfully! Assigned ID:" << newPub.getId();
    } else {
        qDebug() << "Failed to save publisher!";
    }

    // ۳. تست پیدا کردن ناشر (Find by Username)
    qDebug() << "\n--- Testing Find by Username ---";
    auto foundPub = pubRepo.findByUsername("nashr_e_no");
    if (foundPub) {
        qDebug() << "Found Publisher:" << foundPub->getCompanyName();
        qDebug() << "Revenue:" << foundPub->getRevenue();
    } else {
        qDebug() << "Publisher not found!";
    }

    // ۴. تست آپدیت (Update)
    if (foundPub) {
        qDebug() << "\n--- Testing Update ---";
        foundPub->setRevenue(750000.0); // تغییر موجودی
        if (pubRepo.save(*foundPub)) {
            qDebug() << "Publisher updated! New Revenue:" << pubRepo.findByUsername("nashr_e_no")->getRevenue();
        }
    }

    return 0; // چون فعلاً فقط تست کنسولی است
}
