/* Copyright 2011 Alexander Potashev <aspotashev@gmail.com>

   This library is free software; you can redistribute it and/or modify
   it under the terms of the GNU Library General Public License as published
   by the Free Software Foundation; either version 2 of the License or
   ( at your option ) version 3 or, at the discretion of KDE e.V.
   ( which shall act as a proxy as in section 14 of the GPLv3 ), any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/
#include "groupinfo.moc"

namespace Vkontakte
{

class GroupInfo::Private
{
public:
    int gid;

    QString name;
    QString screenName;
    bool isClosed;
    bool isAdmin;
    QString photo;
    QString photoMedium;
    QString photoBig;
};

GroupInfo::GroupInfo()
    : d(new Private)
{
    d->gid = -1;
    d->isClosed = true;
    d->isAdmin = false;
}

GroupInfo::~GroupInfo()
{
    delete d;
}

void GroupInfo::setGid(int gid)
{
    d->gid = gid;
}

int GroupInfo::gid() const
{
    return d->gid;
}

void GroupInfo::setName(const QString &name)
{
    d->name = name;
}

QString GroupInfo::name() const
{
    return d->name;
}

void GroupInfo::setScreenName(const QString &screenName)
{
    d->screenName = screenName;
}

QString GroupInfo::screenName() const
{
    return d->screenName;
}

void GroupInfo::setClosed(bool closed)
{
    d->isClosed = closed;
}

bool GroupInfo::isClosed() const
{
    return d->isClosed;
}

void GroupInfo::setAdmin(bool admin)
{
    d->isAdmin = admin;
}

bool GroupInfo::isAdmin() const
{
    return d->isAdmin;
}

void GroupInfo::setPhoto(const QString &photo)
{
    d->photo = photo;
}

QString GroupInfo::photo() const
{
    return d->photo;
}

void GroupInfo::setPhotoMedium(const QString &photoMedium)
{
    d->photoMedium = photoMedium;
}

QString GroupInfo::photoMedium() const
{
    return d->photoMedium;
}

void GroupInfo::setPhotoBig(const QString &photoBig)
{
    d->photoBig = photoBig;
}

QString GroupInfo::photoBig() const
{
    return d->photoBig;
}

} /* namespace Vkontakte */
