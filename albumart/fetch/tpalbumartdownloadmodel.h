#ifndef TPALBUMARTDOWNLOADMODEL_H
#define TPALBUMARTDOWNLOADMODEL_H

#include <QObject>
#include <QAbstractListModel>

class TPAlbumArtDownloadModel : public QAbstractListModel
{
    Q_OBJECT
public:
    explicit TPAlbumArtDownloadModel(QObject *parent = 0);

signals:

public slots:

};

#endif // TPALBUMARTDOWNLOADMODEL_H
