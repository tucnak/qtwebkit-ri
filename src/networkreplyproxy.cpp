#include <QDebug>

#include <QWebPage>
#include <QWebFrame>
#include <QWebSettings>

#include <QNetworkProxy>
#include <QNetworkReply>
#include <QNetworkCookie>

#include "networkreplyproxy.h"

NetworkReplyProxy::NetworkReplyProxy(QObject* parent, QNetworkReply* reply)
    : QNetworkReply(parent)
    , m_reply(reply)
{
    // Copying attributes state
    setOperation(m_reply->operation());
    setRequest(m_reply->request());
    setUrl(m_reply->url());

    // Handle these to forward
    connect(m_reply, SIGNAL(metaDataChanged()), SLOT(applyMetaData()));
    connect(m_reply, SIGNAL(readyRead()), SLOT(readInternal()));
    connect(m_reply, SIGNAL(error(QNetworkReply::NetworkError)), SLOT(errorInternal(QNetworkReply::NetworkError)));

    // Signal forwarding
    connect(m_reply, SIGNAL(finished()), SIGNAL(finished()));
    connect(m_reply, SIGNAL(uploadProgress(qint64,qint64)), SIGNAL(uploadProgress(qint64,qint64)));
    connect(m_reply, SIGNAL(downloadProgress(qint64,qint64)), SIGNAL(downloadProgress(qint64,qint64)));

    connect(this, SIGNAL(finished()), this, SLOT(writeDataFake()));

    // Data proxy
    setOpenMode(ReadOnly);
}

NetworkReplyProxy::~NetworkReplyProxy()
{
    delete m_reply;
}

void NetworkReplyProxy::writeDataFake()
{
    QByteArray httpHeader;
    QList<QByteArray> headers = rawHeaderList();
    foreach(QByteArray header, headers) {
        if (header.toLower() == "content-encoding"
            || header.toLower() == "transfer-encoding"
            || header.toLower() == "content-length"
            || header.toLower() == "connection")
            continue;

        // special case for cookies: we need to generate separate lines
        // QNetworkCookie::toRawForm is a bit broken and we have to do this
        // ourselves. some simple heuristic here
        if (header.toLower() == "set-cookie") {
            QList<QNetworkCookie> cookies = QNetworkCookie::parseCookies(rawHeader(header));
            foreach (QNetworkCookie cookie, cookies) {
                httpHeader += "set-cookie: " + cookie.toRawForm() + "\r\n";
            }
        } else {
            httpHeader += header + ": " + rawHeader(header) + "\r\n";
        }
    }
    httpHeader += "content-length: " + QByteArray::number(m_data.size()) + "\r\n";
    httpHeader += "\r\n";

    if(m_reply->error() != QNetworkReply::NoError) {
        return;
    }

    emit resourceIntercepted(url(), m_data, httpHeader);
}

qint64 NetworkReplyProxy::readData(char* data, qint64 maxlen)
{
    qint64 size = qMin(maxlen, qint64(m_buffer.size()));
    memcpy(data, m_buffer.constData(), size);
    m_buffer.remove(0, size);

    return size;
}

void NetworkReplyProxy::applyMetaData()
{
    QList<QByteArray> headers = m_reply->rawHeaderList();
    foreach(QByteArray header, headers)
        setRawHeader(header, m_reply->rawHeader(header));

    setHeader(QNetworkRequest::ContentTypeHeader,
              m_reply->header(QNetworkRequest::ContentTypeHeader));
    setHeader(QNetworkRequest::ContentLengthHeader,
              m_reply->header(QNetworkRequest::ContentLengthHeader));
    setHeader(QNetworkRequest::LocationHeader,
              m_reply->header(QNetworkRequest::LocationHeader));
    setHeader(QNetworkRequest::LastModifiedHeader,
              m_reply->header(QNetworkRequest::LastModifiedHeader));
    setHeader(QNetworkRequest::SetCookieHeader,
              m_reply->header(QNetworkRequest::SetCookieHeader));

    setAttribute(QNetworkRequest::HttpStatusCodeAttribute,
                 m_reply->attribute(QNetworkRequest::HttpStatusCodeAttribute));
    setAttribute(QNetworkRequest::HttpReasonPhraseAttribute,
                 m_reply->attribute(QNetworkRequest::HttpReasonPhraseAttribute));
    setAttribute(QNetworkRequest::RedirectionTargetAttribute,
                 m_reply->attribute(QNetworkRequest::RedirectionTargetAttribute));
    setAttribute(QNetworkRequest::ConnectionEncryptedAttribute,
                 m_reply->attribute(QNetworkRequest::ConnectionEncryptedAttribute));
    setAttribute(QNetworkRequest::CacheLoadControlAttribute,
                 m_reply->attribute(QNetworkRequest::CacheLoadControlAttribute));
    setAttribute(QNetworkRequest::CacheSaveControlAttribute,
                 m_reply->attribute(QNetworkRequest::CacheSaveControlAttribute));
    setAttribute(QNetworkRequest::SourceIsFromCacheAttribute,
                 m_reply->attribute(QNetworkRequest::SourceIsFromCacheAttribute));
    setAttribute(QNetworkRequest::DoNotBufferUploadDataAttribute,
                 m_reply->attribute(QNetworkRequest::DoNotBufferUploadDataAttribute));

    emit metaDataChanged();
}
