#ifndef STREAMIODEVICE_H
#define STREAMIODEVICE_H

#include <QIODevice>
#include <QEventLoop>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <map>
#include <QRegularExpression>
#include <QRegularExpressionMatch>

class StreamIODevice : public QIODevice {
    Q_OBJECT

public:
    StreamIODevice(QNetworkAccessManager * m, QNetworkRequest r, qint64 batch = 1024*1024*3, QObject* p = nullptr)
        : QIODevice(p),readIndex(0), manager(m), request(r), batch_size(batch) {
        stream_manager = new QNetworkAccessManager();
        stream_manager->setCookieJar(manager->cookieJar());
    }

    // QIODevice 인터페이스 구현
    bool isSequential() const override ;

    qint64 size() const override ;

    bool seek(qint64 pos) override ;

    qint64 readData(char* data, qint64 maxSize) override ;

    qint64 writeData(const char* data, qint64 maxSize) override ;

    qint64 bytesAvailable() const override;

    ~StreamIODevice();

    bool isBlockAvailable(qint64 block);

    void recv_data(qint64 start);

    qint64 get_batch_size(){return batch_size;}

public slots: void appendData(qint64 block_num, QByteArray data) ;

public slots: void set_total_length(qint64  length) {total_length = length;}

private:
    qint64 readIndex;
    QNetworkAccessManager * manager;
    QNetworkAccessManager * stream_manager;
    QNetworkRequest request;
    qint64 batch_size;
    std::map<qint64, QByteArray*> virtual_buf;
    qint64 total_length = -1;
};

class AsyncRecvBlock : public QObject{
    QNetworkAccessManager * manager;
    QNetworkRequest request;
    QNetworkReply * reply;
    qint64 block;

    Q_OBJECT
public:
    AsyncRecvBlock(QNetworkAccessManager * m, QNetworkRequest * r, qint64 b, QObject* parent = nullptr) : block(b), QObject(parent){
        manager = new QNetworkAccessManager(this);
        manager->setCookieJar(m->cookieJar());
        request.setUrl(r->url());

        for(const auto& header: r->rawHeaderList()){
            QByteArray value = r->rawHeader(header);
            request.setRawHeader(header, value);
        }

        qDebug()<<"ready async recv block: "<<block;
    }
    void start(){
        qDebug()<<"start async recv block: "<<block;
        reply = manager->get(request);
        qDebug()<<"wait block: "<<block;
        connect(reply, &QNetworkReply::finished, this, &AsyncRecvBlock::process_reply);
    }

signals:
    void data_recv(qint64 block, QByteArray data);
    void length_recv(qint64 len);

public slots:
    void process_reply(){
        QNetworkReply * reply = (QNetworkReply*)(sender());
        qDebug()<<"recv complete. block: "<<block;
        if(reply->error() == QNetworkReply::NoError){
            QByteArray data = reply->readAll();
            data_recv(block, data);

            if(block == 0){
                QString contentRange = QString::fromUtf8(reply->rawHeader("Content-Range"));
                QRegularExpression regex(R"(bytes (\d+)-(\d+)/(\d+))");
                QRegularExpressionMatch match = regex.match(contentRange);

                qint64 total = match.captured(3).toLongLong();
                emit length_recv(total);
            }
        }
        else{
            qDebug() << "Error:" << reply->error();
            qDebug() << "Error String:" << reply->errorString();
            qDebug() << "Response Code:" << reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
            qDebug() << "Content Type:" << reply->header(QNetworkRequest::ContentTypeHeader).toString();
            qDebug() << "Response Body:" << reply->readAll();
        }

        reply->deleteLater();
    }
};

#endif // STREAMIODEVICE_H
