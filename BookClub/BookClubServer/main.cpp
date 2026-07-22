#include <QCoreApplication>
#include "Network/bookclubserver.h"
#include "DB/databasemanager.h"
#include "DB/bookrepository.h"
#include "DB/discountrepository.h"
#include "DB/publisherrepository.h"
#include "Services/bookservice.h"
#include "Book.h"
#include "Publisher.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    // ۱. مقداردهی اولیه دیتابیس
    DatabaseManager db = DatabaseManager::instance();
    if (!db.initDatabase("bookclub.db")) {
        qDebug() << "Failed to open database!";
        return -1;
    }

    // ۲. تعریف ریپازیتوری‌ها
    BookRepository bookRepo(&db);
    DiscountRepository discRepo(&db);
    PublisherRepository pubRepo(&db);

    // ۳. ایجاد یک ناشر تستی (اگر وجود ندارد) چون کتاب بدون ناشر نمیتواند باشد
    if (pubRepo.findAll().isEmpty()) {
        Publisher p("admin_pub", "1234", "axford", "company");
        pubRepo.save(p);
        qDebug() << "Test Publisher created.";
    }

    // ۴. ایجاد کتاب تستی با استفاده از Service (برای رعایت منطق بیزنس)
    // ۴. ایجاد کتاب تستی با استفاده از Service (برای رعایت منطق بیزنس)
    BookService bookService(&bookRepo, &discRepo);

    if (bookService.getAllAvailableBooks().isEmpty()) {
        // ایجاد شیء کتاب با استفاده از سازنده پارامتردار
        Book b1("C++ Modern Guide", "Stroustrup", 450000, Genre::SciFi, 1);
        b1.setIsAvailable(true);
        bookService.addBook(b1);

        Book b2("The Little Prince", "Antoine de Saint-Exupéry", 120000, Genre::Fantasy, 1);
        b2.setIsAvailable(true);
        bookService.addBook(b2);

        qDebug() << "Sample books added to DB.";
    }


    // ۵. اجرای سرور
    BookClubServer* server = new BookClubServer(&a);
    if (!server->startServer(12345)) {
        return -1;
    }

    return a.exec();
}
