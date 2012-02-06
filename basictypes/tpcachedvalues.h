#ifndef TPCACHEDVALUES_H
#define TPCACHEDVALUES_H

//! @class TPCachedValues
//! @brief Cached attributes for album and artist types.
//! Allows much better performance to be achieved when searching for
//! content.
class TPCachedValues
{
public:

    TPCachedValues()
    {
        clear();
    }

    void clear()
    {
        playLen = lastPlayed = playCount = -1;
    }

    int playLen;
    int lastPlayed;
    int playCount;
};

#endif // TPCACHEDVALUES_H
