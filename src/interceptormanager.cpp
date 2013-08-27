#include <QtWebKit>
#include <QDebug>
#include <QRegExp>

#include "interceptormanager.h"
#include "networkreplyproxy.h"

class InterceptorAccessManagerPrivate
{
public:
    InterceptorAccessManagerPrivate()
    {
        urlRegExp.setPatternSyntax(QRegExp::Wildcard);
        urlRegExp.setPattern("*");
    }

    ~InterceptorAccessManagerPrivate() {}

    QRegExp urlRegExp;
};

InterceptorAccessManager::InterceptorAccessManager(QObject *parent) :
    QNetworkAccessManager(parent),
    d_ptr(new InterceptorAccessManagerPrivate)
{
}

InterceptorAccessManager::~InterceptorAccessManager()
{
    delete d_ptr;
}

QRegExp InterceptorAccessManager::urlMask() const
{
    Q_D(const InterceptorAccessManager);
    return d->urlRegExp;
}

void InterceptorAccessManager::setUrlMask(const QRegExp &mask)
{
    Q_D(InterceptorAccessManager);
    d->urlRegExp = mask;
}

bool InterceptorAccessManager::isResourceNeeded(const QNetworkRequest &request)
{
    Q_D(InterceptorAccessManager);

    if (d->urlRegExp.exactMatch(request.url().toString()))
        return true;

    return false;
}

QNetworkReply *InterceptorAccessManager::createRequest(Operation op, const QNetworkRequest &request,
                                                       QIODevice *outgoingData)
{
    QNetworkReply *real = QNetworkAccessManager::createRequest(op, request, outgoingData);
    if (isResourceNeeded(request)) {
        NetworkReplyProxy *proxy = new NetworkReplyProxy(this, real);
        emit pendingReplyProxy(proxy);
        return proxy;
    }
    return real;
}
