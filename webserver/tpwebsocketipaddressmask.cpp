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

    // Not provided -> do not match.
    if (regularExp.isEmpty())
        return false;
    if (regularExp.pattern().length() == 0)
        return false;
    if (!regularExp.isValid())
        return false;

    return regularExp.exactMatch(QString(ipAddress));
}
