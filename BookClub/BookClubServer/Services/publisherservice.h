#ifndef PUBLISHERSERVICE_H
#define PUBLISHERSERVICE_H

#include <QObject>
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

class PublisherService : public QObject {
    Q_OBJECT
private:
    PublisherRepository* pubRepo;
    BookRepository* bookRepo;
    DiscountRepository* discountRepo;

public:
    explicit PublisherService(PublisherRepository* pRepo,
                              BookRepository* bRepo,
                              DiscountRepository* dRepo,
                              QObject* parent = nullptr);

    // Authentication & Registration
    QString registerPublisher(const QString& username,
                              const QString& password,
                              const QString& securityAns = "1");


    std::optional<Publisher> login(const QString& username, const QString& password);
    bool verifyPublisherSecurityAnswer(const QString& username, const QString& answer) const;

    bool resetPasswordWithSecurityAnswer(const QString& username,
                                         const QString& answer,
                                         const QString& newPassword);

    std::optional<Publisher> getPublisherById(int publisherId) const;
    std::optional<Publisher> getPublisherByPublishername(const QString& publisherUsername) const;

    // Profile Settings
    bool changePublisherPassword(int publisherId, const QString& oldPassword, const QString& newPassword);
    bool changePublisherUsername(int publisherId, const QString& newUsername, const QString& password);
    bool changeSecurityAnswer(int publisherId, const QString& newAnswer, const QString& password);

    // Book Management
    bool addNewBook(int publisherId, Book& book);
    bool updateBookPrice(int publisherId, int bookId, double newPrice);
    bool updateBookDetailsByPublisher(int publisherId, const Book& book);
    bool removeBook(int publisherId, int bookId);
    bool activateBook(int publisherId, int bookId);
    bool addRevenue(int publisherId, double amount);
    QList<Book> getPublisherBooks(int publisherId);
    // Discount Management
    bool applyDiscountToBook(int publisherId, int bookId, int discountId);
    bool removeDiscountFromBook(int publisherId, int bookId);


    // Stats & Financials
    std::optional<SalesStats> calculateSalesStats(int publisherId) const;
    double getPublisherRevenue(int publisherId) const;

signals:
    void publisherRegistered(int publisherId);
    void publisherCredentialsChanged(int publisherId);
    void bookAdded(int publisherId, int bookId);
    void bookRemoved(int publisherId, int bookId);
    void bookActivated(int publisherId, int bookId);
    void bookPriceUpdated(int bookId, double newPrice);
    void discountApplied(int bookId, int discountId);
    void discountRemoved(int bookId);
};

#endif
