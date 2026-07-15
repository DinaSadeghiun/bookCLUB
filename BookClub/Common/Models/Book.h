#ifndef BOOK_H
#define BOOK_H

#include <QString>
#include "genre.h"
class Book {
private:
    int id;
    int publisherId;
    int discountId; // ID-based link
    QString title;
    QString author;
    Genre genre;
    QString description;
    QString coverImagePath;
    QString pdfFilePath;
    double price;
    double totalRating;
    int ratingCount;
    int salesCount;
    bool isAvailable;

public:
    //creating new book
    Book(const QString& title, const QString& author, double price, Genre genre, int publisherId = 0);
    //LOADING from DB
    Book(int id, int publisherId, int discountId,
         const QString& title, const QString& author, Genre genre,
         const QString& description, const QString& coverImagePath,
         const QString& pdfFilePath, double price,
         double totalRating, int ratingCount, int salesCount, bool isAvailable);

    ~Book() = default;

    // Getters
    int getId() const;
    int getPublisherId() const;
    int getDiscountId() const;
    QString getTitle() const;
    QString getAuthor() const;
    Genre getGenre() const;
    QString getDescription() const;
    QString getCoverImagePath() const;
    QString getPdfFilePath() const;
    double getPrice() const;
    int getSalesCount() const;
    double getTotalRating() const;
    int getRatingCount() const;
    bool getIsAvailable() const;
    double getAverageRating() const;

    double getDiscountAmount() const; //***
    double getFinalPrice() const;

    // Setters
    void setId(int id);
    void setTitle(const QString& t);
    void setAuthor(const QString& a);
    void setGenre(Genre g);
    void setDescription(const QString& d);
    void setPrice(double p);
    void setCoverImagePath(const QString& path);
    void setPdfFilePath(const QString& path);
    void setDiscountId(int dId);
    void setIsAvailable(bool status);
    void setPublisherId(int newId);
    void setTotalRating(double rating);
    void setRatingCount(int count);


    // Methods
    void removeDiscount();
    void incrementSalesCount();
    void addRating(double rating);
};

#endif
