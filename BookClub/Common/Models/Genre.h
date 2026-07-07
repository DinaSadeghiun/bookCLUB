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
    Other
};

QString genreToString(Genre genre);

#endif
