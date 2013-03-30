#include "tpclientconnectionmonitor.h"
#include "tppathutils.h"
#include "tplog.h"
#include <unistd.h>

TPClientConnectionMonitor::TPClientConnectionMonitor(QObject *parent) :
    QObject(parent)
{
}

void TPClientConnectionMonitor::onClientConnected(int count)
{
    if (count == 1)
    {
        // First client connected, lets try to make
        // the disk spin early on by creating, and removing,
        // a dummy file from the each and every place we are handling
        // for information.
/*        QStringList triggerFiles = TPPathUtils::getMediaTriggerFiles();

        foreach(QString file, triggerFiles)
        {
            DEBUG() << "CLIENTCONNECTIONMONITOR: Creating a trigger file: " << file;

            QFile f(file);

            if (f.open(QIODevice::WriteOnly))
            {
                f.write("wallywashere");
                fdatasync(f.handle());
                f.close();
                QFile::remove(file);
            }
            else
            {
                ERROR() << "CLIENTCONNECTIONMONITOR: Failed to create file: " << file;
            }
        }*/
    }
}

void TPClientConnectionMonitor::onClientDisconnected(int count)
{
    Q_UNUSED(count);
}
