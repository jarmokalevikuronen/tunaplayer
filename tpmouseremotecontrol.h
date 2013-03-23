#ifndef TPMOUSEREMOTECONTROL_H
#define TPMOUSEREMOTECONTROL_H

#include <QObject>
#include <QString>
#include <QSocketNotifier>
#include <QMap>


static const QString remoteControlCommandStopPlay("STOPPLAY");
static const QString remoteControlCommandVolumeUp("VOLUMEUP");
static const QString remoteControlCommandVolumeDown("VOLUMEDOWN");
static const QString remoteControlCommandNext("NEXT");
static const QString remoteControlCommandHalt("HALT");

//!
//! @class TPMouseRemoteControl
//! @brief Simple mouse remote control implementation.
//! Mouse presses are mapped to following actions/events
//!
//! LEFT-MOUSE:  TOGGLE PLAY/STOP
//! RIGHT-MOUSE: NEXT
//! WHEEL_UP: VOLUME UP
//! WHEEL_DOWN: VOLUME DOWN
//!
//!
class TPMouseRemoteControl : public QObject
{
    Q_OBJECT
public:

    TPMouseRemoteControl(const QString inp = QString(), QObject *parent = 0);
    ~TPMouseRemoteControl();

    bool loadKeyboardConfig(const QString filename);

    bool start(const QString inp = QString());

signals:

    void remoteControlCommand(const QString remcoCommand);
    void remoteControlPlaylistRequest(const QString playlistName);

private slots:

    void readEvent();

private:

    bool processRemcoConfigItem(QVariant &item);

private:

    //! Maps a button identifier to its currently known state. true = pressed, false = released.
    QMap<int, bool> btnStates;

    //! Socket notifier to get notifications on changes in input dev
    QSocketNotifier *sn;

    //! Name of the input device to open.
    QString inputDev;

    //! Input handle.
    int fd;

    // Mapping from keycodes to playlist item(s).
    QMap<int, QString> playlistActions;

    // Mapping from keycodes to control actions.
    QMap<int, QString> controlActions;
};

#endif // TPMOUSEREMOTECONTROL_H
