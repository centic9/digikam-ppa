/* Copyright 2010 Thomas McGuire <mcguire@kde.org>
   Copyright 2011 Alexander Potashev <aspotashev@gmail.com>

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
#include "friendlistjob.moc"

#include <qjson/qobjecthelper.h>

namespace Vkontakte
{

class FriendListJob::Private
{
public:
    QList<UserInfoPtr> list;
};

// http://vkontakte.ru/developers.php?o=-1&p=friends.get
FriendListJob::FriendListJob(const QString &accessToken, int uid)
    : VkontakteJob(accessToken, "friends.get")
    , d(new Private)
{
    if (uid != -1)
        addQueryItem("uid", QString::number(uid));
    addQueryItem("fields", UserInfo::allQueryFields().join(","));
}

FriendListJob::~FriendListJob()
{
    delete d;
}

QList<UserInfoPtr> FriendListJob::list() const
{
    return d->list;
}

void FriendListJob::handleData(const QVariant &data)
{
    foreach(const QVariant &user, data.toList())
    {
        UserInfoPtr userInfo(new UserInfo());
        QJson::QObjectHelper::qvariant2qobject(user.toMap(), userInfo.data());
        d->list.append(userInfo);
    }
}

} /* namespace Vkontakte */