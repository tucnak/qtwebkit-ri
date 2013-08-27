#include <QDebug>
#include <QRegExp>

#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    interceptor(new InterceptorAccessManager(this))
{
    ui->setupUi(this);
    ui->webView->page()->setNetworkAccessManager(interceptor);
    interceptor->setUrlMask(QRegExp("*.png", Qt::CaseInsensitive, QRegExp::Wildcard));

    connect(ui->webView, &QWebView::urlChanged,
            this, &MainWindow::updateUrl);

    connect(interceptor, &InterceptorAccessManager::pendingReplyProxy,
            this, &MainWindow::processCustomReply);

    connect(ui->pushButton, &QPushButton::clicked,
            this, &MainWindow::loadPage);

    connect(ui->lineEdit, &QLineEdit::returnPressed,
            this, &MainWindow::loadPage);

    ui->webView->load(QUrl("http://google.com/"));
}

void MainWindow::loadPage()
{
    ui->webView->load(ui->lineEdit->text());
}

void MainWindow::processCustomReply(NetworkReplyProxy *proxy)
{
    connect(proxy, &NetworkReplyProxy::resourceIntercepted,
            this, &MainWindow::handleReplyData);
}

void MainWindow::handleReplyData(const QUrl &url, const QByteArray &data,
                                 const QByteArray &)
{
    qDebug() << "[RECIEVER]" << url << data.toBase64().right(8);
}

void MainWindow::updateUrl(const QUrl &url)
{
    ui->lineEdit->setText(url.toString());
}

MainWindow::~MainWindow()
{
    delete ui;
}
