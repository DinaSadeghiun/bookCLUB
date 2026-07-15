#include "Services/publisherservice.h"
#include "DB/publisherrepository.h"
#include "DB/bookrepository.h"
#include "Publisher.h"
#include <algorithm>

PublisherService::PublisherService(PublisherRepository* pRepo, BookRepository* bRepo, DiscountRepository* dRepo)
    : pubRepo(pRepo), bookRepo(bRepo), discountRepo(dRepo) {}

std::optional<Publisher> PublisherService::registerPublisher(const QString& username,
                                                             const QString& password,
                                                             const QString& companyName,
                                                             const QString& securityAns) {
    QString trimmedUser = username.trimmed();
    QString trimmedCompany = companyName.trimmed();

    if (trimmedUser.isEmpty() || password.isEmpty() || trimmedCompany.isEmpty()) {
        return std::nullopt;
    }

    // Check if publisher already exists
    if (pubRepo->findByUsername(trimmedUser).has_value()) {
        return std::nullopt;
    }

    // Create new publisher
    Publisher newPub(trimmedUser, password, securityAns, trimmedCompany);

    if (pubRepo->save(newPub)) {
        return newPub;
    }

    return std::nullopt;
}

std::optional<Publisher> PublisherService::login(const QString& username, const QString& password) {
    QString trimmedUser = username.trimmed();
    if (trimmedUser.isEmpty() || password.isEmpty()) {
        return std::nullopt;
    }

    auto pubOpt = pubRepo->authenticate(trimmedUser, password);

    if (pubOpt && pubOpt->canLogin()) {
        return pubOpt;
    }
    return std::nullopt;
}

bool PublisherService::resetPasswordWithSecurityAnswer(const QString& username,
                                                       const QString& answer,
                                                       const QString& newPassword)
{
    auto trimmedUsername = username.trimmed();
    auto trimmedAnswer = answer.trimmed();
    auto trimmedNewPassword = newPassword.trimmed();

    if (trimmedUsername.isEmpty() || trimmedAnswer.isEmpty() || trimmedNewPassword.isEmpty()) {
        return false;
    }

    auto pubOpt = pubRepo->findByUsername(trimmedUsername);
    if (!pubOpt) {
        return false;
    }

      if (pubOpt->changePasswordWithSecurityAnswer(trimmedAnswer, trimmedNewPassword)) {
        return pubRepo->save(*pubOpt);
    }

    return false;
}


bool PublisherService::changePublisherPassword(int publisherId, const QString& oldPassword, const QString& newPassword) {
    auto pubOpt = pubRepo->findById(publisherId);
    if (!pubOpt) {
        return false;
    }

    if (!pubOpt->changePassword(oldPassword, newPassword)) {
        return false;
    }

    return pubRepo->save(*pubOpt);
}

bool PublisherService::changePublisherUsername(int publisherId, const QString& newUsername, const QString& password) {
    QString trimmedNewUser = newUsername.trimmed();
    auto pubOpt = pubRepo->findById(publisherId);
    if (!pubOpt) {
        return false;
    }

    auto existingPub = pubRepo->findByUsername(trimmedNewUser);
    if (existingPub && existingPub->getId() != publisherId) {
        return false;
    }

    if (!pubOpt->changeUsername(trimmedNewUser, password)) {
        return false;
    }

    return pubRepo->save(*pubOpt);
}

// add book
bool PublisherService::addNewBook(int publisherId, Book& book) {
    auto pubOpt = pubRepo->findById(publisherId);
    if (!pubOpt) {
        return false;
    }

    book.setPublisherId(publisherId);
    book.setIsAvailable(true);

    return bookRepo->save(book);
}

// remove book
bool PublisherService::removeBook(int publisherId, int bookId) {
    auto bookOpt = bookRepo->findById(bookId);
    if (!bookOpt || bookOpt->getPublisherId() != publisherId) {
        return false;
    }

    return bookRepo->remove(bookId);
}

bool PublisherService::updateBookPrice(int publisherId, int bookId, double newPrice) {
    if (newPrice < 0.0) {
        return false;
    }

    auto bookOpt = bookRepo->findById(bookId);
    if (!bookOpt || bookOpt->getPublisherId() != publisherId) {
        return false;
    }

    bookOpt->setPrice(newPrice);
    return bookRepo->save(*bookOpt);
}

QList<Book> PublisherService::getPublisherBooks(int publisherId) {
    return bookRepo->findByPublisherId(publisherId);
}

//discount
bool PublisherService::applyDiscountToBook(int publisherId, int bookId, int discountId) {
    auto bookOpt = bookRepo->findById(bookId);
    if (!bookOpt.has_value()) {
        qWarning() << "Apply Discount Failed: Book not found. ID:" << bookId;
        return false;
    }

    Book book = bookOpt.value();

    if (book.getPublisherId() != publisherId) {
        qWarning() << "Apply Discount Failed: Publisher" << publisherId << "does not own book" << bookId;
        return false;
    }

    auto discountOpt = discountRepo->findById(discountId);
    if (!discountOpt.has_value()) {
        qWarning() << "Apply Discount Failed: Discount not found. ID:" << discountId;
        return false;
    }

    book.setDiscountId(discountId);
    return bookRepo->save(book);
}

bool PublisherService::removeDiscountFromBook(int publisherId, int bookId) {
    auto bookOpt = bookRepo->findById(bookId);
    if (!bookOpt.has_value()) {
        qWarning() << "Remove Discount Failed: Book not found. ID:" << bookId;
        return false;
    }

    Book book = bookOpt.value();

    if (book.getPublisherId() != publisherId) {
        qWarning() << "Remove Discount Failed: Publisher" << publisherId << "does not own book" << bookId;
        return false;
    }

    book.removeDiscount();
    return bookRepo->save(book);
}

std::optional<SalesStats> PublisherService::calculateSalesStats(int publisherId) const {
    auto pubOpt = pubRepo->findById(publisherId);
    if (!pubOpt) {
        return std::nullopt;
    }

    const QList<Book> books = bookRepo->findByPublisherId(publisherId);
    SalesStats stats;

    if (books.isEmpty()) {
        return stats;
    }

    int totalSold = 0;
    double ratingSum = 0.0;
    int ratedBooksCount = 0;

    for (const auto& book : books) {
        totalSold += book.getSalesCount();
        if (book.getRatingCount() > 0) {
            ratingSum += book.getAverageRating();
            ratedBooksCount++;
        }
    }

    stats.totalBooksSold = totalSold;
    stats.averageRating = (ratedBooksCount > 0) ? (ratingSum / ratedBooksCount) : 0.0;

    QList<Book> sortedBooks = books;
    std::sort(sortedBooks.begin(), sortedBooks.end(), [](const Book& a, const Book& b) {
        return a.getSalesCount() > b.getSalesCount();
    });

    int limit = std::min(3, static_cast<int>(sortedBooks.size()));

    for (int i = 0; i < limit; ++i) {
        if (sortedBooks[i].getSalesCount() > 0) {
            stats.topSellingBookIds.append(sortedBooks[i].getId());
        }
    }

    int startIdx = std::max(0, static_cast<int>(sortedBooks.size()) - limit);
    for (int i = static_cast<int>(sortedBooks.size()) - 1; i >= startIdx; --i) {
        stats.lowSellingBookIds.append(sortedBooks[i].getId());
    }

    return stats;
}

double PublisherService::getPublisherRevenue(int publisherId) const {
    auto pubOpt = pubRepo->findById(publisherId);
    return pubOpt ? pubOpt->getRevenue() : 0.0;
}
