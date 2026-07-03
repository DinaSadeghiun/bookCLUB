#include "book.h"

Book::Book(int id, int pubId, const QString& title, double price)
    : id(id), publisherId(pubId), discountId(-1), title(title), price(price),
    totalRating(0.0), averageRating(0.0), ratingCount(0), salesCount(0), isAvailable(true) {}

//helper
void Book::updateAverageRating() {
    averageRating = (ratingCount > 0) ? (totalRating / ratingCount) : 0.0;
}

int Book::getId() const { return id; }
int Book::getPublisherId() const { return publisherId; }
int Book::getDiscountId() const { return discountId; }
QString Book::getTitle() const { return title; }
QString Book::getAuthor() const { return author; }
QString Book::getGenre() const { return genre; }
QString Book::getDescription() const { return description; }
QString Book::getCoverImagePath() const { return coverImagePath; }
QString Book::getPdfFilePath() const { return pdfFilePath; }
double Book::getPrice() const { return price; }
int Book::getSalesCount() const { return salesCount; }
int Book::getRatingCount() const { return ratingCount; }
double Book::getAverageRating() const { return averageRating; }
bool Book::getIsAvailable() const { return isAvailable; }

void Book::setTitle(const QString& t) { title = t; }
void Book::setAuthor(const QString& a) { author = a; }
void Book::setGenre(const QString& g) { genre = g; }
void Book::setDescription(const QString& d) { description = d; }
void Book::setPrice(double p) { price = p; }
void Book::setCoverImagePath(const QString& path) { coverImagePath = path; }
void Book::setPdfFilePath(const QString& path) { pdfFilePath = path; }

void Book::setDiscountId(int dId) { discountId = dId; }

void Book::setAvailable(bool status) { isAvailable = status; }

void Book::removeDiscount() { discountId = -1; }

void Book::incrementSalesCount() { salesCount++; }

void Book::addRating(double rating) {
    if (rating >= 1.0 && rating <= 5.0) {
        totalRating += rating;
        ratingCount++;
        updateAverageRating();
    }
}
