#include "Genre.h"

QString genreToString(Genre genre) {
    switch(genre) {
    case Genre::Fiction: return "Fiction";
    case Genre::NonFiction: return "Non-Fiction";
    case Genre::Mystery: return "Mystery";
    case Genre::Romance: return "Romance";
    case Genre::SciFi: return "Science Fiction";
    case Genre::Fantasy: return "Fantasy";
    case Genre::Biography: return "Biography";
    case Genre::History: return "History";
    case Genre::SelfHelp: return "Self-Help";
    case Genre::Poetry: return "Poetry";
    case Genre::Children: return "Children";
    case Genre::Other: return "Other";
    case Genre::Unknown: return "Unknown";

    default: return "Unknown";
    }
}


Genre stringToGenre(const QString& genreString) {
    if (genreString == "Fiction") return Genre::Fiction;
    else if (genreString == "Non-Fiction") return Genre::NonFiction;
    else if (genreString == "Mystery") return Genre::Mystery;
    else if (genreString == "Romance") return Genre::Romance;
    else if (genreString == "Science Fiction") return Genre::SciFi;
    else if (genreString == "Fantasy") return Genre::Fantasy;
    else if (genreString == "Biography") return Genre::Biography;
    else if (genreString == "History") return Genre::History;
    else if (genreString == "Self-Help") return Genre::SelfHelp;
    else if (genreString == "Poetry") return Genre::Poetry;
    else if (genreString == "Children") return Genre::Children;
    else if (genreString == "Other") return Genre::Other;
    else return Genre::Unknown;
}