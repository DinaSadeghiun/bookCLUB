#include "book.h"

Book::Book(int id, const QString& title, const QString& author,
           const QString& genre, const QString& description,
           double price, const Discount& discount, bool isAvailable,
           int publisherId, double averageRating,
           const QString& coverImagePath, const QString& pdfFilePath)
    : id(id), title(title), author(author), genre(genre),
    description(description), price(price), discount(discount),
    isAvailable(isAvailable), publisherId(publisherId),
    averageRating(averageRating), coverImagePath(coverImagePath),
    pdfFilePath(pdfFilePath) {}

// Getters
int Book::getId() const { return id; }
QString Book::getTitle() const { return title; }
QString Book::getAuthor() const { return author; }
QString Book::getGenre() const { return genre; }
QString Book::getDescription() const { return description; }
double Book::getPrice() const { return price; }
Discount Book::getDiscount() const { return discount; }
bool Book::getIsAvailable() const { return isAvailable; }
int Book::getPublisherId() const { return publisherId; }
double Book::getAverageRating() const { return averageRating; }
QString Book::getCoverImagePath() const { return coverImagePath; }
QString Book::getPdfFilePath() const { return pdfFilePath; }

// Setters
void Book::setTitle(const QString& title) { this->title = title; }
void Book::setAuthor(const QString& author) { this->author = author; }
void Book::setGenre(const QString& genre) { this->genre = genre; }
void Book::setDescription(const QString& description) { this->description = description; }
void Book::setPrice(double price) { if (price >= 0) this->price = price; }
void Book::setDiscount(const Discount& d) { this->discount = d; }
void Book::setIsAvailable(bool available) { this->isAvailable = available; }
void Book::setAverageRating(double rating) { if (rating >= 0 && rating <= 5) this->averageRating = rating; }
void Book::setCoverImagePath(const QString& path) { this->coverImagePath = path; }
void Book::setPdfFilePath(const QString& path) { this->pdfFilePath = path; }


double Book::getFinalPrice() const {
    if (!discount.isValid()) {
        return price;
    }

    if (discount.getType() == Discount::Percentage) {
        return price * (1.0 - (discount.getValue() / 100.0));
    }
    else {
        double finalPrice = price - discount.getValue();
        return (finalPrice > 0.0) ? finalPrice : 0.0;
    }
}

bool Book::hasDiscount() const {
    return discount.isValid() && discount.getValue() > 0.0;
}

bool Book::isFree() const {
    return price == 0.0 || getFinalPrice() == 0.0;
}
