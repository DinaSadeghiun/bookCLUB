#include "bookservice.h"
#include "DB/bookrepository.h"
#include "DB/discountrepository.h"
#include "Services/userservice.h"
#include <QDateTime>
#include <QDebug>

BookService::BookService(BookRepository* repo, DiscountRepository* discRepo,
                         UserService* userSvc, QObject* parent)
    : QObject(parent), bookRepo(repo), discountRepo(discRepo), userService(userSvc)
{
    Q_ASSERT(bookRepo != nullptr);
    Q_ASSERT(discountRepo != nullptr);
    Q_ASSERT(userService != nullptr);
}

bool BookService::validateBook(const Book& book) const {
    if (!bookRepo) {
        return false;
    }
    if (book.getTitle().trimmed().isEmpty()) {
        qDebug() << "Validation Failed: Title is empty.";
        return false;
    }
    if (book.getAuthor().trimmed().isEmpty()) {
        qDebug() << "Validation Failed: Author name is empty.";
        return false;
    }
    if (book.getPrice() < 0.0) {
        qDebug() << "Validation Failed: Price cannot be negative.";
        return false;
    }
    if (book.getPublisherId() <= 0) {
        qDebug() << "Validation Failed: Invalid publisher ID.";
        return false;
    }
    return true;
}

bool BookService::addBook(Book& book) {
    if (!bookRepo) {
        return false;
    }
    if (book.getId() != -1) {
        qDebug() << "Add Book Failed: Book already has an ID.";
        return false;
    }
    if (!validateBook(book)) {
        return false;
    }
    if (bookRepo->save(book)) {
        emit bookAdded(book.getId(), book.getPublisherId());
        return true;
    }
    return false;
}

bool BookService::updateBook(Book& book) {
    if (!bookRepo) {
        return false;
    }
    if (book.getId() == -1) {
        qDebug() << "Update Book Failed: Book must have a valid ID.";
        return false;
    }
    if (!validateBook(book)) {
        return false;
    }
    if (bookRepo->save(book)) {
        emit bookUpdated(book.getId());
        return true;
    }
    return false;
}

bool BookService::removeBook(int bookId) {
    if (!bookRepo) {
        return false;
    }
    if (bookId <= 0) {
        return false;
    }
    if (bookRepo->remove(bookId)) {
        emit bookRemoved(bookId);
        return true;
    }
    return false;
}

std::optional<Book> BookService::getBookById(int id) const {
    if (!bookRepo) {
        return {};
    }
    if (id <= 0) {
        return std::nullopt;
    }
    return bookRepo->findById(id);
}

QList<Book> BookService::getAllAvailableBooks() const {
    if (!bookRepo) {
        return {};
    }
    return bookRepo->findAll();
}

QList<Book> BookService::getBooksByPublisher(int publisherId) const {
    Q_ASSERT(publisherId > 0);
    if (publisherId <= 0 || !bookRepo) {
        return QList<Book>();
    }
    return bookRepo->findByPublisherId(publisherId);
}

QList<Book> BookService::getBooksByGenre(Genre genre) const {
    if (!bookRepo) {
        return {};
    }
    return bookRepo->findByGenre(genre);
}

QList<Book> BookService::search(const QString& query) const {
    if (!bookRepo) {
        return {};
    }
    QString trimmedQuery = query.trimmed();
    if (trimmedQuery.isEmpty()) {
        return {};
    }
    return bookRepo->searchBooks(trimmedQuery);
}

//Home
QList<Book> BookService::getRecommendedBooksForUser(int userId) {
    QList<Book> result;
    if (!bookRepo || !userService) return result;

    QList<Genre> userGenres = userService->getUserFavoriteGenres(userId);
    if (userGenres.isEmpty()) {
        return bookRepo->findAll();
    }

    QSet<int> seenIds;
    for (Genre genre : std::as_const(userGenres)) {
        QList<Book> books = bookRepo->findByGenre(genre);
        for (const Book& book : std::as_const(books)) {
            if (!seenIds.contains(book.getId())) {
                seenIds.insert(book.getId());
                result.append(book);
            }
        }
    }

    return result;
}

QList<Book> BookService::getNewReleases() {
    if (!bookRepo) return {};
    QList<Book> books = bookRepo->findAll();
    std::sort(books.begin(), books.end(), [](const Book& a, const Book& b) {
        return a.getId() > b.getId();
    });
    return books;
}

QList<Book> BookService::getBestSellers() {
    if (!bookRepo) return {};
    QList<Book> books = bookRepo->findAll();
    std::sort(books.begin(), books.end(), [](const Book& a, const Book& b) {
        return a.getSalesCount() > b.getSalesCount();
    });
    return books;
}

QList<Book> BookService::getFreeBooks() {
    if (!bookRepo) return {};
    QList<Book> books = bookRepo->findAll();
    QList<Book> freeBooks;
    for (const Book& book : std::as_const(books)) {
        if (book.getPrice() == 0.0) {
            freeBooks.append(book);
        }
    }
    return freeBooks;
}

QList<Book> BookService::getPopularBooks() {
    if (!bookRepo) return {};
    QList<Book> books = bookRepo->findAll();
    std::sort(books.begin(), books.end(), [](const Book& a, const Book& b) {
        return a.getAverageRating() > b.getAverageRating();
    });
    return books;
}

std::optional<double> BookService::getBookFinalPrice(int bookId) const {
    Q_ASSERT(bookId > 0);
    if (bookId <= 0 || !bookRepo || !discountRepo) {
        return std::nullopt;
    }

    auto bookOpt = bookRepo->findById(bookId);
    if (!bookOpt.has_value()) {
        return std::nullopt;
    }

    Book book = bookOpt.value();
    int discId = book.getDiscountId();

    if (discId <= 0) {
        return book.getPrice();
    }

    auto discountOpt = discountRepo->findById(discId);
    if (!discountOpt.has_value()) {
        return book.getPrice();
    }

    Discount discount = discountOpt.value();

    QDateTime now = QDateTime::currentDateTime();
    if (!discount.getIsActive() || now < discount.getStartDate() || now > discount.getEndDate()) {
        return book.getPrice();
    }

    double originalPrice = book.getPrice();
    double finalPrice = originalPrice;

    if (discount.getType() == Discount::Percentage) {
        finalPrice = originalPrice - (originalPrice * (discount.getValue() / 100.0));
    } else if (discount.getType() == Discount::FixedAmount) {
        finalPrice = originalPrice - discount.getValue();
    }

    return (finalPrice < 0.0) ? 0.0 : finalPrice;
}

bool BookService::rateBook(int bookId, double rating) {
    Q_ASSERT(bookId > 0);
    if (bookId <= 0 || !bookRepo) {
        return false;
    }

    if (rating < 1.0 || rating > 5.0) {
        qDebug() << "Rate Book Failed: Rating must be between 1.0 and 5.0.";
        return false;
    }

    auto bookOpt = bookRepo->findById(bookId);
    if (!bookOpt.has_value()) {
        qDebug() << "Rate Book Failed: Book not found.";
        return false;
    }

    Book book = bookOpt.value();
    book.addRating(rating);
    if (bookRepo->save(book)) {
        emit bookRatingUpdated(bookId, book.getAverageRating());
        return true;
    }
    return false;
}
