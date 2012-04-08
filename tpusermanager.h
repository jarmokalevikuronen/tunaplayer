#ifndef TPUSERMANAGER_H
#define TPUSERMANAGER_H

#include <QString>
#include <QVector>
#include "tpassociativemeta.h"

/*
Tagging mechanism in tunaplayer

.johndoe.restrict.tt
 => Bydefault, all files within this folder, and its subfolders are hidden
   for user johndoe.

.johndoe.allow.tt
 => Bydefault, all files within this folder, and its subfolders are shown
   for user johndoe.

*/


//! @class TPUserTag
//! @brief Maps a directory path to a some users.
class TPUserTag
{
public:

    //! @brief What kind of
    enum AccessType {
        Invalid,
        Allow,
        Restrict
    }accessType;

    //! @brief C++ constructor used when creating object from the empty token file. (john.deny.tt)
    explicit TPUserTag(const QString &path, const QString &tagFilename);

    //! Directory where to tag.
    QString path;

    //! Tags name - specifies whether access is to be granted or restricted.
    QString tag;

    inline bool allowedAccess() const
    {
        return accessType == Allow;
    }

    //! Checks that the tag is actually valid.
    inline bool isValid() const
    {
        return accessType != Invalid && path.length() && tag.length();
    }

    //! Checks whether the objects are valid and have equal definition(s).
    inline bool equals(const TPUserTag &other) const
    {
        return isValid() && other.isValid() && accessType == other.accessType && tag == other.tag && path == other.path;
    }
};


//! @class TPGenericTag
//! @brief handles a files with name generic.tt that allow
//! defining user profile type of things for a specific folder
//! Example
//! @begincode
//! allow=john
//! deny=kelly|rock
//! @endcode
//! Above would allow "john" to see the contents but restrict it from
//! kelly and rock.
//!
class TPGenericTag
{
public:

    TPGenericTag(const QString &filename);

    //! Will be filled by constructor - contains the inner tags.
    QList<TPUserTag *> usertags;
};

//! @class TPTagManager
//! @brief Simple utility class to manage tags associated.
class TPUserManager
{
public:

    //! c++ constructor
    TPUserManager();

    //! c++ destructor
    ~TPUserManager();

    //! @brief Adds a tag to list.
    //! @return true if succeeded, false on failure.
    bool insertTagFile(const QString &tagFilename);

    //! @brief Returns all tags for a specific audio file.
    QStringList tagsForFile(const QString &filename);

    //! @brief Creates a single string from the tags specific for a file.
    //! constructed in following manner [TAG]|[TAG2]|[TAG3]
    QString tagStringForFile(const QString &filename, const QString pre="<", const QString post=">", const QString delimiter=userTokensDelimiter);

    //! @brief Returns all the known and configured tags.
    QStringList allTags();

    //! Checks whether tag managers are equal.
    bool equals(const TPUserManager &other);

private:

    //! Chceks whether _this_ object contains a equal tag definition.
    bool hasEqualTag(const TPUserTag &otherTag);

private:

    //! Array of all tag definitions.
    QVector<TPUserTag *> tags;
};

#endif // TPTAGMANAGER_H
