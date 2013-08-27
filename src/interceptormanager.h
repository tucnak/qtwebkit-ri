/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2013 Illya Kovalevskyy (illya.kovalevskyy@gmail.com)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.

 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */

#ifndef INTERCEPTORNAM_H
#define INTERCEPTORNAM_H

#include <QNetworkAccessManager>

class InterceptorAccessManagerPrivate;
class NetworkReplyProxy;

class InterceptorAccessManager : public QNetworkAccessManager
{
    Q_OBJECT
public:
    explicit InterceptorAccessManager(QObject *parent = 0);
    ~InterceptorAccessManager();

    QRegExp urlMask() const;
    void setUrlMask(const QRegExp &mask);

signals:
    void pendingReplyProxy(NetworkReplyProxy*);

protected:
    InterceptorAccessManagerPrivate * const d_ptr;

    bool isResourceNeeded(const QNetworkRequest &request);
    
protected:
    QNetworkReply *createRequest(Operation op, const QNetworkRequest &request,
                                 QIODevice *outgoingData);

private:
    Q_DECLARE_PRIVATE(InterceptorAccessManager)
    
};

#endif // INTERCEPTORNAM_H
