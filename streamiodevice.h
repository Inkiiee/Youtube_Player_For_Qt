#ifndef STREAMIODEVICE_H
#define STREAMIODEVICE_H

#include <QIODevice>
#include <QEventLoop>

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>

class StreamIODevice : public QIODevice {
    Q_OBJECT

public:
    StreamIODevice(QNetworkAccessManager * m, QNetworkRequest * r, qint64 batch = 1024*1024*3, QObject* parent = nullptr)
        : QIODevice(parent), readIndex(0), manager(m), request(r), batch_size(batch) {
        stream_manager = new QNetworkAccessManager(nullptr);
        stream_manager->setCookieJar(manager->cookieJar());
    }

    // 데이터 추가
    void appendData(qint64 block_num, const QByteArray& data) ;

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
    QNetworkRequest * request;
    qint64 batch_size;
    std::map<qint64, QByteArray*> virtual_buf;
    qint64 total_length = -1;
};
#endif // STREAMIODEVICE_H
