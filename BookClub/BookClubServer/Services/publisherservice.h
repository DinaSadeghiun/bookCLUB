#ifndef PUBLISHERSERVICE_H
#define PUBLISHERSERVICE_H

#include <optional>
#include <QList>
#include <QString>
#include "Publisher.h"
#include "Book.h"
#include "DB/discountrepository.h"
#include "DB/bookrepository.h"
#include "Discount.h"

// Forward declarations
class PublisherRepository;
class BookRepository;
class DiscountRepository;

class PublisherService {
private:
    PublisherRepository* pubRepo;
    BookRepository* bookRepo;
    DiscountRepository* discountRepo;

public:
    // Constructor injecting repositories via pointer
    PublisherService(PublisherRepository* pRepo, BookRepository* bRepo, DiscountRepository* dRepo);

    // Authentication & Registration
    std::optional<Publisher> registerPublisher(const QString& username,
                                               const QString& password,
                                               const QString& companyName,
                                               const QString& securityAns = "1");

    std::optional<Publisher> login(const QString& username, const QString& password);

    bool resetPasswordWithSecurityAnswer(const QString& username,
                                         const QString& answer,
                                         const QString& newPassword);


    // Profile Settings (Leveraging Person methods)
    bool changePublisherPassword(int publisherId, const QString& oldPassword, const QString& newPassword);
    bool changePublisherUsername(int publisherId, const QString& newUsername, const QString& password);

    // Book Management
    bool addNewBook(int publisherId, Book& book);
    bool updateBookPrice(int publisherId, int bookId, double newPrice);
    bool removeBook(int publisherId, int bookId);
    QList<Book> getPublisherBooks(int publisherId);
    // Discount Management
    bool applyDiscountToBook(int publisherId, int bookId, int discountId);
    bool removeDiscountFromBook(int publisherId, int bookId);


    // Stats & Financials
    std::optional<SalesStats> calculateSalesStats(int publisherId) const;
    double getPublisherRevenue(int publisherId) const;
};

#endif
