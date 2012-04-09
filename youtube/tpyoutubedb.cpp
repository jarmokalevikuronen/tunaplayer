#include "tpyoutubedb.h"
#include "tpschemes.h"
#include "tppathutils.h"
#include "tplog.h"

TPYouTubeDB::TPYouTubeDB(QObject *parent) : QObject(parent),
        TPDBBase<TPYouTubeObject *>(this, TPPathUtils::getYoutubeDbFilename(), schemeYoutube)
{
    db->visitItems(this);
}

TPYouTubeDB::~TPYouTubeDB()
{
    DEBUG() << "YOUTUBE: ~TPYouTubeDB";
    decAllObjects();
}

void TPYouTubeDB::insertItem(TPYouTubeObject *object)
{
    if (!object)
        return;

    TPAssociativeDBItem *item = object->detachDbItem();
    if (item)
    {
        if (db->item(item->primaryKey()))
            delete item;
        else
        {
            db->addItem(item);
            visitAssociativeDBItem(item);
        }
    }

    object->dec();

    save();
}

void TPYouTubeDB::visitAssociativeDBItem(TPAssociativeDBItem *item)
{
    if (item->intValue(objectAttrCreated) == 0)
        item->setIntValue(objectAttrCreated, TPUtils::currentEpoch());

    objects.insertObject(new TPYouTubeObject(item));
}
