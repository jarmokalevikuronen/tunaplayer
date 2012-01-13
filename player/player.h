#ifndef PLAYER_H_
#define PLAYER_H_

#include <QObject>
#include <QString>

class PlayerEng : public QObject
{
Q_OBJECT

public:

    explicit PlayerEng(QObject *parent);
    ~PlayerEng();
};


#endif

