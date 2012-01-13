#include "player.h"
#include <QDebug>
#include <mpg123.h>


PlayerEng::PlayerEng(QObject *parent) : QObject(parent)
{
    int result = mpg123_init();
    qDebug() << "mpg123_init = " << result;
    const char **decoders = mpg123_supported_decoders();
    while (*decoders)
    {
        qDebug() << "Decoder: " << *decoders;
        ++decoders;
    }

}

PlayerEng::~PlayerEng()
{
    mpg123_exit();
}
