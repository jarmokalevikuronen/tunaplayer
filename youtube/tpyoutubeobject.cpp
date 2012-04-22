#include "tpyoutubeobject.h"
#include "tpschemes.h"

TPYouTubeObject::TPYouTubeObject(const QString primaryKey) : TPAssociativeObject(primaryKey), TPIdBase(schemeYoutube, primaryKey)
{
    setString(objectAttrScheme, schemeYoutube);
}

TPYouTubeObject::TPYouTubeObject(TPAssociativeDBItem *item) : TPAssociativeObject(item), TPIdBase(schemeYoutube, item->primaryKey())
{
    setString(objectAttrScheme, schemeYoutube);
}

const QString TPYouTubeObject::getString(const QString key, const QString defaultValue) const
{
    if (key == objectAttrIdentifier)
        return QString(const_cast<TPYouTubeObject *>(this)->identifier());
    return TPAssociativeObject::getString(key, defaultValue);
}

QVariantMap TPYouTubeObject::toMap(QStringList *filteredKeys)
{
    QVariantMap values = TPAssociativeObject::toMap(filteredKeys);

    if (filteredKeys)
    {
        if (!filteredKeys->contains(objectAttrIdentifier))
            values.insert(objectAttrIdentifier, getString(objectAttrIdentifier));
    }
    else
    {
        values.insert(objectAttrIdentifier, getString(objectAttrIdentifier));
    }

    return values;
}
