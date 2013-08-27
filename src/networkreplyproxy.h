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

#ifndef NETWORKREPLYPROXY_H
#define NETWORKREPLYPROXY_H

#include <QIODevice>
#include <QNetworkReply>
#include <QUrl>

class NetworkReplyProxyPrivate;

class NetworkReplyProxy : public QNetworkReply {
    Q_OBJECT
public:
    NetworkReplyProxy(QObject* parent, QNetworkReply* reply);
    ~NetworkReplyProxy();

    virtual qint64 readData(char* data, qint64 maxlen);

    // Virtual declarations
    void abort();
    void close();
    bool isSequential() const;

    void setReadBufferSize(qint64 size);
    virtual qint64 bytesAvailable() const;
    virtual qint64 bytesToWrite() const;

    virtual bool canReadLine() const
    {
        qFatal("Interceptor API: not implemented");
        return false;
    }

    virtual bool waitForReadyRead(int)
    {
        qFatal("Interceptor API: not implemented");
        return false;
    }

    virtual bool waitForBytesWritten(int)
    {
        qFatal("Interceptor API: not implemented");
        return false;
    }

signals:
    void resourceIntercepted(QUrl, QByteArray, QByteArray);

public slots:
    void applyMetaData();
    void ignoreSslErrors();
    void errorInternal(QNetworkReply::NetworkError _error);
    void readInternal();

protected:
    NetworkReplyProxyPrivate * const d_ptr;

private slots:
    void writeDataFake();

private:
    Q_DECLARE_PRIVATE(NetworkReplyProxy)

    QNetworkReply* m_reply;
    QByteArray m_data;
    QByteArray m_buffer;
};

#endif // NETWORKREPLYPROXY_H
