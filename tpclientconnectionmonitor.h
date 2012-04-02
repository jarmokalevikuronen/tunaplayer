#ifndef TPCLIENTCONNECTIONMONITOR_H
#define TPCLIENTCONNECTIONMONITOR_H

#include <QObject>

class TPClientConnectionMonitor : public QObject
{
    Q_OBJECT
public:
    explicit TPClientConnectionMonitor(QObject *parent = 0);

signals:

public slots:

    //!
    //! @brief Called once a client is connected, total amount of clients
    //! is given as parameter.
    //!
    void onClientConnected(int count);

    //!
    //! @brief Called once client is disconnected, total amount of remaining
    //! clients is attached.
    //!
    void onClientDisconnected(int count);
};

#endif // TPCLIENTCONNECTIONMONITOR_H
