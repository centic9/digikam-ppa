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
#include "photopostjob.moc"
#include "mpform.h"

#include <qjson/parser.h>
#include <KIO/Job>
#include <KDebug>
#include <KLocale>

namespace Vkontakte
{

PhotoPostJob::PhotoPostJob(Vkontakte::UploadPhotosJob::Dest dest, const QString &url, const QStringList &files)
{
    m_url = url;
    m_files = files;
    m_dest = dest;

    setCapabilities(KJob::Killable);

    m_ok = true;
    if (files.size() <= 0 || files.size() > 5)
        m_ok = false;
}

void PhotoPostJob::handleError(const QVariant &data)
{
    const QVariantMap errorMap = data.toMap();
    int error_code = errorMap["error_code"].toInt();
    const QString error_msg = errorMap["error_msg"].toString();
    kWarning() << "An error of type" << error_code << "occurred:" << error_msg;
    setError(KJob::UserDefinedError);
    setErrorText(i18n(
        "The VKontakte server returned an error "
        "of type <i>%1</i> in reply to uploading to URL %2: <i>%3</i>",
        error_code, m_url, error_msg));
}

void PhotoPostJob::start()
{
    if (!m_ok)
    {
        setError(UserDefinedError);
        setErrorText("Internal error");
        emitResult();
    }

    MPForm form;
    switch (m_dest)
    {
        case Vkontakte::UploadPhotosJob::DEST_ALBUM:
            // "file1" .. "file5"
            for (int i = 0; i < m_files.size(); i ++)
                if (!form.addFile(QString("file%1").arg(i + 1), m_files[i]))
                {
                    m_ok = false;
                    break;
                }
                break;
        case Vkontakte::UploadPhotosJob::DEST_PROFILE:
        case Vkontakte::UploadPhotosJob::DEST_WALL:
            // "photo"
            if (!form.addFile(QString("photo"), m_files[0]))
                m_ok = false;
            break;
        default:
            m_ok = false;
            break;
    }
    form.finish();

    if (!m_ok)
    {
        setError(UserDefinedError);
        setErrorText("Could not attach file");
        emitResult();
    }


    KUrl url(m_url);
    kDebug() << "Starting request" << url;
    KIO::StoredTransferJob *job = KIO::storedHttpPost(form.formData(), url, KIO::HideProgressInfo);
    job->addMetaData("content-type", form.contentType());

    m_job = job;
    connect(job, SIGNAL(result(KJob*)), this, SLOT(jobFinished(KJob*)));
    job->start();
}

void PhotoPostJob::jobFinished(KJob *kjob)
{
    KIO::StoredTransferJob *job = dynamic_cast<KIO::StoredTransferJob *>(kjob);
    Q_ASSERT(job);
    if (job && job->error())
    {
        setError(job->error());
        setErrorText(KIO::buildErrorString(error(), job->errorText()));
        kWarning() << "Job error: " << job->errorString();
    }
    else
    {
        kDebug() << "Got data: " << QString::fromAscii(job->data().data());
        QJson::Parser parser;
        bool ok;
        const QVariant data = parser.parse(job->data(), &ok);
        if (ok)
        {
            const QVariant error = data.toMap()["error"];
            if (error.isValid())
                handleError(error);
            else
                handleData(data);
        }
        else
        {
            kWarning() << "Unable to parse JSON data: " << QString::fromAscii(job->data().data());
            setError(KJob::UserDefinedError);
            setErrorText(i18n("Unable to parse data returned by the VKontakte server: %1", parser.errorString()));
        }
    }
    emitResult();
    m_job = 0;
}

void PhotoPostJob::handleData(const QVariant &data)
{
    m_response = data.toMap();
}

QVariantMap PhotoPostJob::response() const
{
    return m_response;
}

} /* namespace Vkontakte */