#ifndef TPWEBSOCKETIPADDRESSMASK_H
#define TPWEBSOCKETIPADDRESSMASK_H

#include <QString>
#include <QRegExp>

class TPWebSocketIPAddressMask
{
public:
    //! @brief Mask definition for allowed IP Addresses that will be served.
    //! @param regularExpression IP Mask as definition Regular Expression string.
    //! This value will be used to mask against IP address string such as a.b.c.d
    TPWebSocketIPAddressMask(const QString &regularExpression);

    bool match(const QByteArray &ipAddress);

private:

    //! Regular expression definition.
    QRegExp regularExp;
};

#endif // TPWEBSOCKETIPADDRESSMASK_H
