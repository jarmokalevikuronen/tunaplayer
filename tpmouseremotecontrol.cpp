#include "tpmouseremotecontrol.h"
#include "tplog.h"
#include <QFile>
#include <stdint.h>
#include <linux/input.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include "json.h"

#define CREATE_KEY_ITEM(Key)    { Key, #Key }

typedef struct Key2ActionMap
{
    int         key;
    const char *keystr;
} Key2ActionMap;

static const Key2ActionMap keyActionMap[] =
{
    CREATE_KEY_ITEM(KEY_0),
    CREATE_KEY_ITEM(KEY_1),
    CREATE_KEY_ITEM(KEY_2),
    CREATE_KEY_ITEM(KEY_3),
    CREATE_KEY_ITEM(KEY_4),
    CREATE_KEY_ITEM(KEY_5),
    CREATE_KEY_ITEM(KEY_6),
    CREATE_KEY_ITEM(KEY_7),
    CREATE_KEY_ITEM(KEY_8),
    CREATE_KEY_ITEM(KEY_9),
    CREATE_KEY_ITEM(KEY_UP),
    CREATE_KEY_ITEM(KEY_DOWN),
    CREATE_KEY_ITEM(KEY_LEFT),
    CREATE_KEY_ITEM(KEY_RIGHT),
    CREATE_KEY_ITEM(KEY_ESC),
    CREATE_KEY_ITEM(KEY_SPACE),
    CREATE_KEY_ITEM(KEY_ENTER)
};

static const QString prefixPlaylist("PLAYLIST://");

#define ELEMENTS(x) (sizeof(x)/sizeof((x)[0]))


TPMouseRemoteControl::TPMouseRemoteControl(const QString inp, QObject *parent) :
        QObject(parent), inputDev(inp)
{
    fd = -1;
    sn = 0;
}

TPMouseRemoteControl::~TPMouseRemoteControl()
{
    delete sn;

    if (fd > 0)
        close(fd);
}

bool TPMouseRemoteControl::start(const QString inp)
{
    if (sn)
        return false;

    if (inp.length())
        inputDev = inp;

    if (!inputDev.length())
        return false;

    fd = open(inputDev.toUtf8().constData(), O_RDONLY);
    if (fd < 0)
    {
        ERROR() << "REMOTECON: Failed to open " << inputDev;
        return false;
    }

    sn = new QSocketNotifier(fd, QSocketNotifier::Read, this);
    connect(sn, SIGNAL(activated(int)), this, SLOT(readEvent()));

    return true;
}

void TPMouseRemoteControl::readEvent()
{
    struct input_event ev[64];

    int rc = read(fd, ev, sizeof(struct input_event) * 64);
    if (rc < 0)
    {
        ERROR() << "REMCO: read failed with " << rc << " -> DISABLING";
        sn->deleteLater();
        sn = 0;
        close(fd);
        fd = -1;
    }

    for (unsigned int i = 0; i < (rc / sizeof(struct input_event)); i++)
    {
        if (ev[i].type == EV_KEY && ev[i].value == 1 /* =PRESSED */)
        {
            //
            // Firstly, process all normal key(s)
            //
            QMap<int, QString>::iterator it =
                    controlActions.find(ev[i].code);
            if (it != controlActions.end())
            {
                emit remoteControlCommand(it.value());
                return;
            }

            //
            // No match. process playlist action(s).
            //
            it = playlistActions.find(ev[i].code);
            if (it != playlistActions.end())
            {
                emit remoteControlPlaylistRequest(it.value());
                return;
            }
        }
    }
}


// processRemcoConfigItem
bool TPMouseRemoteControl::processRemcoConfigItem(QVariant &item)
{
    QVariantMap map = item.toMap();

    if (!map.contains("key") || !map.contains("action"))
    {
        ERROR() << "Invalid remco config item: " << item.toString();

        return false;
    }

    QString key = map.value("key").toString();
    QString action = map.value("action").toString();

    int code = -1;
    for (int i=0; i<(int)ELEMENTS(keyActionMap) && code < 0; ++i)
    {
        if (    keyActionMap[i].keystr &&
                key == keyActionMap[i].keystr)
            code = keyActionMap[i].key;
    }

    if (code < 0)
    {
        ERROR() << "REMCO: Unrecognized key: " << key;
        return false;
    }


    if (action.startsWith(prefixPlaylist))
    {
        QString playlistName = action.mid(prefixPlaylist.length());

        playlistActions.insert(code, playlistName);

        DEBUG() << "REMCO: PlaylistItem: " << code << "=" << playlistName;

        return true;
    }
    else if (action == remoteControlCommandStopPlay)
    {
        controlActions.insert(code, action);
    }
    else if (action == remoteControlCommandVolumeUp)
    {
        controlActions.insert(code, action);
    }
    else if (action == remoteControlCommandVolumeDown)
    {
        controlActions.insert(code, action);
    }
    else if (action == remoteControlCommandNext)
    {
        controlActions.insert(code, action);
    }
    else
    {
        ERROR() << "REMCO: Unrecognized action: " << action;
    }

    DEBUG() << "REMCO: ActionItem: " << code << "=" << action;

    return true;
}


// loadKeyboardConfig
bool TPMouseRemoteControl::loadKeyboardConfig(const QString filename)
{
    /*
      {
        items: [
            {
                key: KEY_0
                action: PLAYLIST:radionova.m3u
            }
        ]
      }




      */

    QFile fin(filename);

    if (!fin.open(QIODevice::ReadOnly))
    {
        ERROR() << "REMCO: failed to open: " << filename;
        return false;
    }

    QString jsonSerialized = QString::fromUtf8(fin.readAll());
    fin.close();

    if (!jsonSerialized.length())
        return false;

    QtJson::Json j;

    bool ok = false;
    QVariantMap tree = j.parse(jsonSerialized, ok).toMap();
    if (!ok)
    {
        ERROR() << "REMCO: Failed to parse: " << filename;
        return false;
    }

    QVariantList itemList = tree.value("items").toList();

    foreach (QVariant item, itemList)
    {
        processRemcoConfigItem(item);
    }

    DEBUG() << "REMCO: Successfully parsed: " << filename;

    return true;
}
