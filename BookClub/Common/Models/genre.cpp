#include "genre.h"

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
    default: return "Unknown";
    }
}
