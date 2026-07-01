#ifndef BOOK_H
#define BOOK_H

#include <QString>
#include "discount.h"

class Book {
private:
    int id;
    QString title;
    QString author;
    QString genre;
    QString description;
    double price;
    Discount discount;
    bool isAvailable;
    int publisherId;
    double averageRating;
    QString coverImagePath;
    QString pdfFilePath;

public:
    Book(int id = 0,
         const QString& title = "",
         const QString& author = "",
         const QString& genre = "",
         const QString& description = "",
         double price = 0.0,
         const Discount& discount = Discount(),
         bool isAvailable = true,
         int publisherId = 0,
         double averageRating = 0.0,
         const QString& coverImagePath = "",
         const QString& pdfFilePath = "");

    // Getters
    int getId() const;
    QString getTitle() const;
    QString getAuthor() const;
    QString getGenre() const;
    QString getDescription() const;
    double getPrice() const;
    Discount getDiscount() const;
    bool getIsAvailable() const;
    int getPublisherId() const;
    double getAverageRating() const;
    QString getCoverImagePath() const;
    QString getPdfFilePath() const;

    // Setters
    void setTitle(const QString& title);
    void setAuthor(const QString& author);
    void setGenre(const QString& genre);
    void setDescription(const QString& description);
    void setPrice(double price);
    void setDiscount(const Discount& d);
    void setIsAvailable(bool available);
    void setAverageRating(double rating);
    void setCoverImagePath(const QString& path);
    void setPdfFilePath(const QString& path);

    double getFinalPrice() const;
    bool hasDiscount() const;
    bool isFree() const;
};
#endif