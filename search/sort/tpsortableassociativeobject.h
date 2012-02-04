#ifndef TPSORTABLEASSOCIATIVEOBJECT_H
#define TPSORTABLEASSOCIATIVEOBJECT_H

#include <QString>


// FORWARD DECLARATIONS
class TPAssociativeObject;
class TPSortInterface;

class TPSortableAssociativeObject
{
public:

    TPSortableAssociativeObject(TPAssociativeObject *__object, TPSortInterface *__sorter);
    TPSortableAssociativeObject(const TPSortableAssociativeObject &other);

    bool operator<(const TPSortableAssociativeObject *_other) const;
    bool operator<(const TPSortableAssociativeObject &_other) const;
    bool operator==(const TPSortableAssociativeObject *_other) const;
    bool operator==(const TPSortableAssociativeObject &_other) const;
    void setCachedInt(const TPSortInterface *caller, int value) const;
    bool hasCachedIntFor(const TPSortInterface *caller) const;;
    int getCachedInt() const;
    void setCachedString(const TPSortInterface *caller, const QString string) const;
    bool hasCachedStringFor(const TPSortInterface *caller) const;
    QString getCachedString() const;
    const TPAssociativeObject& object() const;
    TPSortInterface* sorter() const;

private:

    TPAssociativeObject *_object;
    TPSortInterface *_sorter;

    mutable int cachedInt;
    mutable const TPSortInterface *cachedIntCookie;

    mutable QString cachedString;
    mutable const TPSortInterface *cachedStringCookie;
};


#endif // TPSORTABLEASSOCIATIVEOBJECT_H
