/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2013-09-24
 * Description : Nepomuk Query class provide Nepomuk Api based implementation
 *               to query for images rating, asigned tags and comments.
 *               It also query Tags.
 *
 * Copyright (C) 2013 by Veaceslav Munteanu <veaceslav dot munteanu90 at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#ifndef DKNEPOMUKQUERY_H
#define DKNEPOMUKQUERY_H

// Qt includes

#include <QObject>

namespace Nepomuk2
{
    namespace Query
    {
        class Query;
    }
}

namespace Digikam
{

class DkNepomukService;

class NepomukQuery : public QObject
{
    Q_OBJECT

public:

    NepomukQuery(DkNepomukService* const service);

    /**
     * @brief queryImagesProperties   - query Nepomuk data for images with
     *                                  tags or rating or comment and
     *                                  sync them to digiKam.
     *                                  Query executed is syncronous
     */
    void queryImagesProperties();

    /**
     * @brief queryImagesProperties    - query all tags from Nepomuk and
     *                                   add them to digiKam
     */
    void queryTags();

private:

    /**
     * @brief buildImagePropertiesQuery   - build a query that will return
     *                                      all images that contains tags or
     *                                      ratings or comments
     * @return                            - query to be executed
     */
    Nepomuk2::Query::Query buildImagePropertiesQuery() const;

    /**
     * @brief buildTagsQuery  - build a query that will return all tags from
     *                          Nepomuk
     * @return                - query to be executed
     */
    Nepomuk2::Query::Query buildTagsQuery() const;

private:

    DkNepomukService* service;
};

} // namespace Digikam

#endif // DKNEPOMUKQUERY_H
