#include "tpmouseremotecontrol.h"
#include "tplog.h"

#include <stdint.h>
#include <linux/input.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>

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
//    DEBUG() << "REMOTECON: readEvent";

    struct input_event ev[64];

    int rc = read(fd, ev, sizeof(struct input_event) * 64);
    if (rc < 0)
    {
        ERROR() << "REMOTECON: read failed with " << rc << " -> DISABLING";
        sn->deleteLater();
        sn = 0;
        close(fd);
        fd = -1;
    }

    for (unsigned int i = 0; i < (rc / sizeof(struct input_event)); i++)
    {
//        DEBUG() << "REMOTECON: Event=" << ev[i].type;

        if (ev[i].type == EV_KEY)
        {
            switch (ev[i].code)
            {
                case BTN_LEFT:
                {
                    // Cache button states... if state information would be needed later on.
                    btnStates.insert(ev[i].code, ev[i].value ? true : false);

                    if (ev[i].value)
                    {                        
                        DEBUG() << "REMOTECON: LEFT_DOWN";
                    }
                    else
                    {
                        DEBUG() << "REMOTECON: LEFT_UP";
                        emit remoteControlCommand(remoteControlCommandStopPlay);
                    }
                }
                break;

                case BTN_RIGHT:
                {
                    // Cache button states.
                    btnStates.insert(ev[i].code, ev[i].value ? true : false);

                    if (ev[i].value)
                    {
                        DEBUG() << "REMOTECON: RIGHT_DOWN";
                    }
                    else
                    {
                        DEBUG() << "REMOTECON: RIGHT_UP";
                        emit remoteControlCommand(remoteControlCommandNext);
                    }
                }
                break;

                case BTN_MIDDLE:
                {
                    // Cache button states.
                    btnStates.insert(ev[i].code, ev[i].value ? true : false);

                    if (ev[i].value)
                    {
                        DEBUG() << "REMOTECON: MIDDLE_DOWN";
                    }
                    else
                    {
                        DEBUG() << "REMOTECON: MIDDLE_UP";
                    }
                }
                break;
            }
        }
        else if (ev[i].type == EV_REL)
        {
            if (ev[i].code == REL_WHEEL)
            {
                if (ev[i].value < 0)
                {
                    DEBUG() << "REMOTECON: WHEEL_DOWN";
                    emit remoteControlCommand(remoteControlCommandVolumeDown);
                }
                else if (ev[i].value > 0)
                {
                    DEBUG() << "REMOTECON: WHEEL_UP";
                    emit remoteControlCommand(remoteControlCommandVolumeUp);
                }
            }
        }
    }
}
