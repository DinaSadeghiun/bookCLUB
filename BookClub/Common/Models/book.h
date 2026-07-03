#ifndef BOOK_H
#define BOOK_H

#include <QString>

class Book {
private:
    int id;
    int publisherId;
    int discountId; // ID-based link
    QString title;
    QString author;
    QString genre;
    QString description;
    QString coverImagePath;
    QString pdfFilePath;
    double price;
    double totalRating;
    double averageRating;
    int ratingCount;
    int salesCount;
    bool isAvailable;

    void updateAverageRating();

public:
    Book(int id = 0, int pubId = 0, const QString& title = "", double price = 0.0);
    ~Book() = default;

    // Getters
    int getId() const;
    int getPublisherId() const;
    int getDiscountId() const;
    QString getTitle() const;
    QString getAuthor() const;
    QString getGenre() const;
    QString getDescription() const;
    QString getCoverImagePath() const;
    QString getPdfFilePath() const;
    double getPrice() const;
    int getSalesCount() const;
    int getRatingCount() const;
    bool getIsAvailable() const;
    double getAverageRating() const;

    // Setters
    void setTitle(const QString& t);
    void setAuthor(const QString& a);
    void setGenre(const QString& g);
    void setDescription(const QString& d);
    void setPrice(double p);
    void setCoverImagePath(const QString& path);
    void setPdfFilePath(const QString& path);
    void setDiscountId(int dId);
    void setAvailable(bool status);

    // Methods
    void removeDiscount();
    void incrementSalesCount();
    void addRating(double rating);
};

#endif
