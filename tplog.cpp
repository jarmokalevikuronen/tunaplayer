
#include <QDateTime>
#include "tplog.h"
#include "tpclargs.h"


static QList<QtMsgType> logTypes;

void initDebugLogging()
{
    QString logLevel = TPCLArgs::instance().arg(TPCLArgs::cliArgLogLevel, "none").toString().toLower();

    if (logLevel == "error")
        logTypes << QtCriticalMsg << QtFatalMsg;
    else if (logLevel == "warning")
        logTypes << QtWarningMsg << QtCriticalMsg << QtFatalMsg;
    else if (logLevel == "debug")
        logTypes << QtDebugMsg << QtWarningMsg << QtCriticalMsg << QtFatalMsg;
}

void tpMessageOutput(QtMsgType type, const char *msg)
{
    if (logTypes.contains(type))
    {
        QDateTime now = QDateTime::currentDateTime();
        QString stamp = now.toString("dd.MM.yyyy hh:mm:ss.zzz");
        fprintf(stdout, "%s %s\n", stamp.toAscii().constData(), msg);
        fflush(stdout);
    }
}

