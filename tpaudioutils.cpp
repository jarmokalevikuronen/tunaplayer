/*
This file is part of tunaplayer project
Copyright (C) 2012  Jarmo Kuronen <jarmok@iki.fi>

This is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This software is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this software; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include "tpaudioutils.h"
#include "tplog.h"


bool TPAudioUtils::artistNameMappingInitialized = false;
QMap<QString, QString> TPAudioUtils::artistNameMapping;


bool TPAudioUtils::albumsEqual(const TPAlbum &_1, const QString name2)
{
    QString name1 = _1.getName();

    return name1.toLower().compare(name2.toLower()) == 0;
}

bool TPAudioUtils::albumsEqual(const TPAlbum &_1, const TPAlbum &_2)
{
    QString name1 = _1.getName();
    QString name2 = _2.getName();
    QString artist1 = _1.getArtist()->getName();
    QString artist2 = _2.getArtist()->getName();

    // TODO: This is fairly simple, add more complex algorithm
    // That strips possible white spaces and others.
    if (name1.toLower().compare(name2.toLower()) == 0)
        if (artist1.toLower().compare(artist2.toLower()) == 0)
            return true;

    return false;
}

bool TPAudioUtils::album1BeforeAlbum2(const TPAlbum &_1, const TPAlbum &_2)
{    
    // Detect order originally by year if such info is available on both
    int year1 = _1.getYear();
    int year2 = _2.getYear();

    if (year1 != AlbumYearNotAvailable && year2 != AlbumYearNotAvailable)
        return year1 < year2;

    // Secondly use the alphabetic ordering of albums.
    QString name1 = _1.getName().toLower();
    QString name2 = _2.getName().toLower();

    return name1.compare(name2) < 0;
}

bool TPAudioUtils::artist1BeforeArtist2(const TPArtist &_1, const TPArtist &_2)
{
    QString name1 = _1.getName().toLower();
    QString name2 = _2.getName().toLower();

    return name1.compare(name2) < 0;
}

bool TPAudioUtils::artistsEqual(const TPArtist &_1, QString name)
{
    return _1.getName().toLower().compare(name.toLower()) == 0;
}

bool TPAudioUtils::artistsEqual(QString name1, QString name2)
{
    return name1.toLower().compare(name2.toLower()) == 0;
}

const QString TPAudioUtils::normalizeArtistName(const QString &originalName)
{
    static const QString a("a ");
    static const QString the("the ");

    if (!artistNameMappingInitialized)
    {
        buildArtistNameMapping();
        artistNameMappingInitialized = true;
    }

    QMap<QString, QString>::iterator it = artistNameMapping.find(originalName.trimmed());
    if (it != artistNameMapping.end())
    {
/*        DEBUG() << "NORMALIZE: " << originalName << " mapped to " << it.value();*/
        return it.value();
    }

    QString lcName = originalName.toLower();

    if (lcName.startsWith(a) && originalName.length() > a.length())
        return originalName.mid(a.length()).trimmed();
    else if (lcName.startsWith(the) && originalName.length() > the.length())
        return originalName.mid(the.length()).trimmed();

    return originalName.trimmed();
}

void TPAudioUtils::buildArtistNameMapping()
{
    QFile f(TPPathUtils::getArtistNameMappingConfigFile());

    if (f.open(QIODevice::ReadOnly))
    {
        while (!f.atEnd())
        {
            QByteArray line = f.readLine();
            if (line.length())
            {
                QString wc = QString::fromUtf8(line.constData());
                QStringList items = wc.split("=");
                if (items.length() == 2)
                {
                    QString key = items.at(0).trimmed();
                    QString value = items.at(1).trimmed();

                    if (key.length() && value.length() && key.at(0) != '#' && key != value)
                    {
                        artistNameMapping[key] = value;
                    }
                }
            }
        }
        f.close();
    }
}


