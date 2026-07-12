#ifndef GENRE_H
#define GENRE_H

#include <QString>

enum class Genre {
    Fiction = 0,
    NonFiction,
    Mystery,
    Romance,
    SciFi,
    Fantasy,
    Biography,
    History,
    SelfHelp,
    Poetry,
    Children,
    Other,
    Unknown
};

QString genreToString(Genre genre);
Genre stringToGenre(const QString& genreString);

#endif
