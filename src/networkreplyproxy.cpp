#include <QDebug>

#include <QWebPage>
#include <QWebFrame>
#include <QWebSettings>

#include <QNetworkProxy>
#include <QNetworkReply>
#include <QNetworkCookie>

#include "networkreplyproxy.h"

class NetworkReplyProxyPrivate
{
public:
    NetworkReplyProxyPrivate() {}

    ~NetworkReplyProxyPrivate()
    {
        delete reply;
    }

    QNetworkReply* reply;
    QByteArray data;
    QByteArray buffer;
};

NetworkReplyProxy::NetworkReplyProxy(QObject* parent, QNetworkReply* reply) :
    QNetworkReply(parent),
    d_ptr(new NetworkReplyProxyPrivate)
{
    Q_D(NetworkReplyProxy);
    d->reply = reply;

    // Copying attributes state
    setOperation(d->reply->operation());
    setRequest(d->reply->request());
    setUrl(d->reply->url());

    // Handle these to forward
    connect(d->reply, SIGNAL(metaDataChanged()), SLOT(applyMetaData()));
    connect(d->reply, SIGNAL(readyRead()), SLOT(readInternal()));
    connect(d->reply, SIGNAL(error(QNetworkReply::NetworkError)), SLOT(errorInternal(QNetworkReply::NetworkError)));

    // Signal forwarding
    connect(d->reply, SIGNAL(finished()), SIGNAL(finished()));
    connect(d->reply, SIGNAL(uploadProgress(qint64,qint64)), SIGNAL(uploadProgress(qint64,qint64)));
    connect(d->reply, SIGNAL(downloadProgress(qint64,qint64)), SIGNAL(downloadProgress(qint64,qint64)));

    connect(this, SIGNAL(finished()), this, SLOT(writeDataFake()));

    // Data proxy
    setOpenMode(ReadOnly);
}

NetworkReplyProxy::~NetworkReplyProxy()
{
    delete d_ptr;
}

qint64 NetworkReplyProxy::readData(char* data, qint64 maxlen)
{
    Q_D(NetworkReplyProxy);

    qint64 size = qMin(maxlen, qint64(d->buffer.size()));
    memcpy(data, d->buffer.constData(), size);
    d->buffer.remove(0, size);

    return size;
}

void NetworkReplyProxy::abort()
{
    Q_D(NetworkReplyProxy);

    d->reply->abort();
}

void NetworkReplyProxy::close()
{
    Q_D(NetworkReplyProxy);

    d->reply->close();
}

bool NetworkReplyProxy::isSequential() const
{
    Q_D(const NetworkReplyProxy);

    return d->reply->isSequential();
}

void NetworkReplyProxy::setReadBufferSize(qint64 size)
{
    Q_D(NetworkReplyProxy);

    QNetworkReply::setReadBufferSize(size); d->reply->setReadBufferSize(size);
}

qint64 NetworkReplyProxy::bytesAvailable() const
{
    Q_D(const NetworkReplyProxy);

    return d->buffer.size() + QIODevice::bytesAvailable();
}

qint64 NetworkReplyProxy::bytesToWrite() const
{
    return -1;
}

void NetworkReplyProxy::applyMetaData()
{
    Q_D(NetworkReplyProxy);

    QList<QByteArray> headers = d->reply->rawHeaderList();
    foreach(QByteArray header, headers)
        setRawHeader(header, d->reply->rawHeader(header));

    setHeader(QNetworkRequest::ContentTypeHeader,
              d->reply->header(QNetworkRequest::ContentTypeHeader));
    setHeader(QNetworkRequest::ContentLengthHeader,
              d->reply->header(QNetworkRequest::ContentLengthHeader));
    setHeader(QNetworkRequest::LocationHeader,
              d->reply->header(QNetworkRequest::LocationHeader));
    setHeader(QNetworkRequest::LastModifiedHeader,
              d->reply->header(QNetworkRequest::LastModifiedHeader));
    setHeader(QNetworkRequest::SetCookieHeader,
              d->reply->header(QNetworkRequest::SetCookieHeader));

    setAttribute(QNetworkRequest::HttpStatusCodeAttribute,
                 d->reply->attribute(QNetworkRequest::HttpStatusCodeAttribute));
    setAttribute(QNetworkRequest::HttpReasonPhraseAttribute,
                 d->reply->attribute(QNetworkRequest::HttpReasonPhraseAttribute));
    setAttribute(QNetworkRequest::RedirectionTargetAttribute,
                 d->reply->attribute(QNetworkRequest::RedirectionTargetAttribute));
    setAttribute(QNetworkRequest::ConnectionEncryptedAttribute,
                 d->reply->attribute(QNetworkRequest::ConnectionEncryptedAttribute));
    setAttribute(QNetworkRequest::CacheLoadControlAttribute,
                 d->reply->attribute(QNetworkRequest::CacheLoadControlAttribute));
    setAttribute(QNetworkRequest::CacheSaveControlAttribute,
                 d->reply->attribute(QNetworkRequest::CacheSaveControlAttribute));
    setAttribute(QNetworkRequest::SourceIsFromCacheAttribute,
                 d->reply->attribute(QNetworkRequest::SourceIsFromCacheAttribute));
    setAttribute(QNetworkRequest::DoNotBufferUploadDataAttribute,
                 d->reply->attribute(QNetworkRequest::DoNotBufferUploadDataAttribute));

    emit metaDataChanged();
}

void NetworkReplyProxy::ignoreSslErrors()
{
    Q_D(NetworkReplyProxy);

    d->reply->ignoreSslErrors();
}

void NetworkReplyProxy::errorInternal(QNetworkReply::NetworkError _error)
{
    setError(_error, errorString());
    emit error(_error);
}

void NetworkReplyProxy::readInternal()
{
    Q_D(NetworkReplyProxy);

    QByteArray data = d->reply->readAll();
    d->data += data;
    d->buffer += data;
    emit readyRead();
}

void NetworkReplyProxy::writeDataFake()
{
    Q_D(NetworkReplyProxy);

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
    httpHeader += "content-length: " + QByteArray::number(d->data.size()) + "\r\n";
    httpHeader += "\r\n";

    if(d->reply->error() != QNetworkReply::NoError) {
        return;
    }

    emit resourceIntercepted(url(), d->data, httpHeader);
}
