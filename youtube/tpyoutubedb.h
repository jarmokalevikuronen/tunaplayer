#ifndef TPYOUTUBEDB_H
#define TPYOUTUBEDB_H

#include <QObject>
#include "tpdbtemplate.h"
#include "tpidbase.h"
#include "tpreferencecounted.h"
#include "tpyoutubeobject.h"

//!
//! @class TPYouTubeDB
//! @brief Caches youtube specific information. Basically
//! the search results can be saved to local DB (links to those) so that
//! they can be represented in the UI like being local objects.
//!
class TPYouTubeDB : public QObject, public TPDBBase<TPYouTubeObject *>, public TPAssociativeDBItemVisitor
{
    Q_OBJECT

public:

    TPYouTubeDB(QObject *parent = 0);
    ~TPYouTubeDB();

    //! @brief Inserts a associative object into the database...
    void insertItem(TPYouTubeObject *object);

private: // from TPAssociativeDBItemVisitor

    void visitAssociativeDBItem(TPAssociativeDBItem *item);
};

#endif // TPYOUTUBEDB_H
