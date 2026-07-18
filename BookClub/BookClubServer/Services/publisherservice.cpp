#include "Services/publisherservice.h"
#include "DB/publisherrepository.h"
#include "DB/bookrepository.h"
#include "Publisher.h"
#include <algorithm>

PublisherService::PublisherService(PublisherRepository* pRepo, BookRepository* bRepo, DiscountRepository* dRepo, QObject* parent)
    : QObject(parent), pubRepo(pRepo), bookRepo(bRepo), discountRepo(dRepo)
{
    Q_ASSERT(pubRepo != nullptr);
    Q_ASSERT(bookRepo != nullptr);
    Q_ASSERT(discountRepo != nullptr);
}


QString PublisherService::registerPublisher(const QString& username,
                                            const QString& password,
                                            const QString& companyName,
                                            const QString& securityAns)
{
    if (!pubRepo) {
        return "DATABASE_ERROR";
    }

    QString trimmedUser = username.trimmed();
    QString trimmedCompany = companyName.trimmed();
    QString trimmedAns = securityAns.trimmed();

    if (trimmedUser.isEmpty() || password.isEmpty() || trimmedCompany.isEmpty() || trimmedAns.isEmpty()) {
        return "EMPTY_FIELDS";
    }

    auto existingPub = pubRepo->findByUsername(trimmedUser);
    if (existingPub.has_value()) {
        return "USERNAME_TAKEN";
    }

    Publisher newPub(trimmedUser, password, trimmedAns, trimmedCompany);

    if (pubRepo->save(newPub)) {
        emit publisherRegistered(newPub.getId());
        return "SUCCESS";
    } else {
        return "DATABASE_ERROR";
    }
}


std::optional<Publisher> PublisherService::login(const QString& username, const QString& password) {
    if (!pubRepo) {
        return std::nullopt;
    }
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
    if (!pubRepo) {
        return false;
    }
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
        if (pubRepo->save(*pubOpt)) {
            emit publisherCredentialsChanged(pubOpt->getId());
            return true;
        }
    }
    return false;
}


bool PublisherService::changePublisherPassword(int publisherId, const QString& oldPassword, const QString& newPassword) {
    Q_ASSERT(publisherId > 0);
    if (publisherId <= 0 || !pubRepo) {
        return false;
    }
    auto pubOpt = pubRepo->findById(publisherId);
    if (!pubOpt) {
        return false;
    }

    if (!pubOpt->changePassword(oldPassword, newPassword)) {
        return false;
    }

    if (pubRepo->save(*pubOpt)) {
        emit publisherCredentialsChanged(publisherId);
        return true;
    }
    return false;
}

bool PublisherService::changePublisherUsername(int publisherId, const QString& newUsername, const QString& password) {
    Q_ASSERT(publisherId > 0);
    if (publisherId <= 0 || !pubRepo) {
        return false;
    }
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

    if (pubRepo->save(*pubOpt)) {
        emit publisherCredentialsChanged(publisherId);
        return true;
    }
    return false;

}

// add book
bool PublisherService::addNewBook(int publisherId, Book& book) {
    Q_ASSERT(publisherId > 0);
    if (publisherId <= 0 || !pubRepo || !bookRepo) {
        return false;
    }
    auto pubOpt = pubRepo->findById(publisherId);
    if (!pubOpt) {
        return false;
    }

    book.setPublisherId(publisherId);
    book.setIsAvailable(true);

    if (bookRepo->save(book)) {
        emit bookAdded(publisherId, book.getId());
        return true;
    }
    return false;
}

// remove book
bool PublisherService::removeBook(int publisherId, int bookId) {
    Q_ASSERT(publisherId > 0);
    Q_ASSERT(bookId > 0);
    if (publisherId <= 0 || bookId <= 0 || !bookRepo) {
        return false;
    }
    auto bookOpt = bookRepo->findById(bookId);
    if (!bookOpt || bookOpt->getPublisherId() != publisherId) {
        return false;
    }

    if (bookRepo->remove(bookId)) {
        emit bookRemoved(publisherId, bookId);
        return true;
    }
    return false;
}

//add revenue
bool PublisherService::addRevenue(int publisherId, double amount) {
    Q_ASSERT(publisherId > 0);
    Q_ASSERT(amount > 0);
        if (publisherId <= 0 || amount <= 0 || !pubRepo) {
        return false;
    }
    auto optPub = pubRepo->findById(publisherId);
    if (!optPub.has_value()) {
        qDebug() << "Publisher not found for ID:" << publisherId;
        return false;
    }

    Publisher pub = optPub.value();

    pub.addRevenue(amount);

    return pubRepo->save(pub);
}

bool PublisherService::updateBookPrice(int publisherId, int bookId, double newPrice) {
    Q_ASSERT(publisherId > 0);
    Q_ASSERT(bookId > 0);
    if (publisherId <= 0 || bookId <= 0 || newPrice < 0.0 || !bookRepo) {
        return false;
    }
    if (newPrice < 0.0) {
        return false;
    }

    auto bookOpt = bookRepo->findById(bookId);
    if (!bookOpt || bookOpt->getPublisherId() != publisherId) {
        return false;
    }

    bookOpt->setPrice(newPrice);
    bookOpt->setPrice(newPrice);
    if (bookRepo->save(*bookOpt)) {
        emit bookPriceUpdated(bookId, newPrice);
        return true;
    }
    return false;
}

QList<Book> PublisherService::getPublisherBooks(int publisherId) {
    Q_ASSERT(publisherId > 0);
    if (publisherId <= 0 || !bookRepo) {
        return {};
    }
    return bookRepo->findByPublisherId(publisherId);
}

//discount
bool PublisherService::applyDiscountToBook(int publisherId, int bookId, int discountId) {
    Q_ASSERT(publisherId > 0);
    Q_ASSERT(bookId > 0);
    Q_ASSERT(discountId > 0);
    if (publisherId <= 0 || bookId <= 0 || discountId <= 0 || !bookRepo || !discountRepo) {
        return false;
    }
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
    if (bookRepo->save(book)) {
        emit discountApplied(bookId, discountId);
        return true;
    }
    return false;
}

bool PublisherService::removeDiscountFromBook(int publisherId, int bookId) {
    Q_ASSERT(publisherId > 0);
    Q_ASSERT(bookId > 0);
    if (publisherId <= 0 || bookId <= 0 || !bookRepo) {
        return false;
    }
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
    if (bookRepo->save(book)) {
        emit discountRemoved(bookId);
        return true;
    }
    return false;
}

std::optional<SalesStats> PublisherService::calculateSalesStats(int publisherId) const {
    Q_ASSERT(publisherId > 0);
    if (publisherId <= 0 || !pubRepo || !bookRepo) {
        return std::nullopt;
    }
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
    Q_ASSERT(publisherId > 0);
    if (publisherId <= 0 || !pubRepo) {
        return 0.0;
    }
    auto pubOpt = pubRepo->findById(publisherId);
    return pubOpt ? pubOpt->getRevenue() : 0.0;
}
