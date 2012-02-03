#include "tpwebsocketipaddressmask.h"

static const QByteArray localhost("127.0.0.1");

TPWebSocketIPAddressMask::TPWebSocketIPAddressMask(const QString &regexp) : regularExp(regexp)
{
    regularExp.setPatternSyntax(QRegExp::Wildcard);
}

bool TPWebSocketIPAddressMask::match(const QByteArray &ipAddress)
{
    //
    // Localhost implicitly allowed.
    //
    if (ipAddress == localhost)
        return true;

    return regularExp.exactMatch(QString(ipAddress));
}
