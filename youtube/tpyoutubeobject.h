#ifndef TPYOUTUBEOBJECT_H
#define TPYOUTUBEOBJECT_H

#include "tpdbtemplate.h"
#include "tpidbase.h"
#include "tpreferencecounted.h"
#include "tpassociative.h"

class TPYouTubeObject : public TPAssociativeObject, public TPReferenceCounted, public TPIdBase
{
public:

    TPYouTubeObject(const QString primaryKey);
    TPYouTubeObject(TPAssociativeDBItem *item);

    const QString getString(const QString key, const QString defaultValue="") const;
    QVariantMap toMap(QStringList *filteredKeys);

};

#endif // TPYOUTUBEOBJECT_H
