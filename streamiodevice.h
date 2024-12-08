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
#include <mutex>

class StreamIODevice : public QIODevice {
    Q_OBJECT

public:
    StreamIODevice(QNetworkAccessManager * m, QNetworkRequest r, qint64 batch = 1024*1024*3, QObject* p = nullptr)
        : QIODevice(p), readIndex(0), manager(m), request(r), batch_size(batch) {
        stream_manager = new QNetworkAccessManager();
        stream_manager->setCookieJar(manager->cookieJar());
        this->parent = p;
    }

    // 데이터 추가
    void appendData(qint64 block_num, const QByteArray& data) ;

    void set_total_length(qint64  length) {total_length = length;}

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

private:
    qint64 readIndex;
    QNetworkAccessManager * manager;
    QNetworkAccessManager * stream_manager;
    QNetworkRequest request;
    qint64 batch_size;
    std::map<qint64, QByteArray*> virtual_buf;
    qint64 total_length = -1;
    QObject* parent;
};

class AsyncRecvBlock : public QObject{
    QNetworkAccessManager * manager;
    QNetworkRequest * request;
    QNetworkReply * reply;
    StreamIODevice * device;
    qint64 block;

    Q_OBJECT
public:
    AsyncRecvBlock(QNetworkAccessManager * m, QNetworkRequest * r, StreamIODevice * d, qint64 b, QObject* parent = nullptr) : request(r), device(d), block(b), QObject(parent){
        manager = new QNetworkAccessManager(this);
        //manager->setCookieJar(m->cookieJar());
    }
    void start(){
        reply = manager->get(*request);

        QEventLoop loop;
        QObject::connect(reply, &QNetworkReply::finished, [this, &loop](){
            if(reply->error() == QNetworkReply::NoError){
                QByteArray data = reply->readAll();
                device->appendData(block, data);

                if(block == 0){
                    QString contentRange = QString::fromUtf8(reply->rawHeader("Content-Range"));
                    QRegularExpression regex(R"(bytes (\d+)-(\d+)/(\d+))");
                    QRegularExpressionMatch match = regex.match(contentRange);

                    qint64 total = match.captured(3).toLongLong();

                    device->set_total_length(total);
                }
            }
            else{
                qDebug() << "Error:" << reply->error();
                qDebug() << "Error String:" << reply->errorString();
                qDebug() << "Response Code:" << reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
                qDebug() << "Content Type:" << reply->header(QNetworkRequest::ContentTypeHeader).toString();
                qDebug() << "Response Body:" << reply->readAll();
            }

            loop.quit();
        });
        loop.exec();

        reply->deleteLater();
        delete this;
    }
};

#endif // STREAMIODEVICE_H
