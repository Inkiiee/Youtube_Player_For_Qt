#include "streamiodevice.h"

#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QTimer>

qint64 StreamIODevice::bytesAvailable() const{
    qint64 block_num = readIndex / batch_size;
    qint64 block_pos = readIndex % batch_size;
    qint64 size = -block_pos;

    for(qint64 i=block_num; virtual_buf.contains(i); i++)
        if(virtual_buf.at(i) != nullptr) size += (virtual_buf.at(i)->size());

    qDebug()<<"bytesAvailable called..........................................."<<size;
    if(size < 0) return 0;
    else return size;
}

void StreamIODevice::recv_data(qint64 start){
    qint64 block_num = start/batch_size;
    if(virtual_buf.contains(block_num) || block_num < 0) return;
    else if(start > total_length && total_length != -1) return;
    virtual_buf[block_num] = nullptr;

    QNetworkRequest stream_request(*request);
    stream_request.setRawHeader("Range", QString("bytes=%1-%2").arg(start).arg(start + (batch_size) -1).toUtf8());
    QNetworkReply* reply = stream_manager->get(stream_request);
    qDebug()<<"--------------------------------------("<<start<<" ~ "<<start + batch_size - 1<<" current buf size: "<<size()<<" block "<<block_num;

    QEventLoop loop;
    QObject::connect(reply, &QNetworkReply::finished, [this, reply, &loop](){
        if(reply->error() == QNetworkReply::NoError){
            QString contentRange = QString::fromUtf8(reply->rawHeader("Content-Range"));
            QRegularExpression regex(R"(bytes (\d+)-(\d+)/(\d+))");
            QRegularExpressionMatch match = regex.match(contentRange);

            qint64 start = match.captured(1).toLongLong();
            qint64 end = match.captured(2).toLongLong();
            qint64 total = match.captured(3).toLongLong();
            qint64 block = start / batch_size;

            qDebug() <<"start: "<<start<<" end: "<<end<<" total: "<<total;

            if(block == 0) total_length = total;

            QByteArray data = reply->readAll();
            this->appendData(block, data);
        }
        else{
            qDebug() << "Error:" << reply->error();
            qDebug() << "Error String:" << reply->errorString();
            qDebug() << "Response Code:" << reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
            qDebug() << "Content Type:" << reply->header(QNetworkRequest::ContentTypeHeader).toString();
            qDebug() << "Response Body:" << reply->readAll();
        }
        reply->deleteLater();
        loop.quit();
    });
    loop.exec();
}

bool StreamIODevice::isBlockAvailable(qint64 block){
    if(virtual_buf.contains(block) && virtual_buf[block] != nullptr)
        return true;

    else return false;
}

// 데이터 추가
void StreamIODevice::appendData(qint64 block_num, const QByteArray& data) {
    if((virtual_buf.contains(block_num) && virtual_buf[block_num] != nullptr) || data.size() == 0) return;

    if(virtual_buf[block_num] == nullptr)
        virtual_buf[block_num] = new QByteArray();
    if(virtual_buf[block_num]->isEmpty())
        virtual_buf[block_num]->append(data);
    qDebug()<<"buffer fill --------- block: "<<block_num<<" data_size: "<<(double)(virtual_buf[block_num]->size()) / (1024 * 1024);

    emit readyRead(); // 데이터가 준비되었음을 알림
}

// QIODevice 인터페이스 구현
bool StreamIODevice::isSequential() const  {
    return false; // 랜덤 접근 가능
}

qint64 StreamIODevice::size() const  {
    qDebug()<<"size called --------------------- size: "<<total_length;

    return total_length;
}

bool StreamIODevice::seek(qint64 pos)  {
    qDebug()<<"seek---------------------------("<<pos;
    qint64 block_num = pos / batch_size;

    if(!virtual_buf.contains(block_num)){
        qDebug()<<"\tout of buffer ----------- block_num: "<<block_num;
        qint64 request_index = block_num * batch_size;

        if(!virtual_buf.contains(request_index))
            recv_data(request_index);
        if(!virtual_buf.contains(request_index + batch_size))
            recv_data(request_index + batch_size);
        if(!virtual_buf.contains(request_index + batch_size))
            recv_data(request_index + batch_size * 2);

        return false;
    }else if(virtual_buf[block_num] == nullptr){
        return false;
    }
    else{
        readIndex = pos;
        return true;
    }

    return false;
}

qint64 StreamIODevice::readData(char* data, qint64 maxSize)  {
    qint64 block_num = readIndex / batch_size;
    qint64 block_pos = readIndex % batch_size;
    if(!virtual_buf.contains(block_num)){
        qint64 request_index = block_num * batch_size;

        if(!virtual_buf.contains(request_index))
            recv_data(request_index);
        if(!virtual_buf.contains(request_index + batch_size))
            recv_data(request_index + batch_size);

        return 0;
    }
    else if(virtual_buf[block_num] == nullptr) return 0;

    qint64 read_bytes = qMin(maxSize, virtual_buf[block_num]->size() - block_pos);
    memcpy(data, virtual_buf[block_num]->constData() + block_pos, read_bytes);
    readIndex += read_bytes;
    qDebug()<<"readData called ------- maxSize: "<<maxSize<<" read-bytes: "<<read_bytes<<" block: "<<block_num<<" block size: "<<bytesAvailable();

    qint64 temp = maxSize - read_bytes;
    if(temp > 0){
        qint64 next_block = block_num + 1;
        if(virtual_buf.contains(next_block) && virtual_buf[next_block] != nullptr){
            qint64 read_len = qMin(temp, virtual_buf[next_block]->size());
            memcpy(data + read_bytes, virtual_buf[next_block]->constData(), read_len);
            read_bytes += read_len;
            readIndex += read_len;

            qDebug()<<"\treadData block: "<<block_num<<" ~ "<<next_block<<" total read: "<<read_bytes<<" append read: "<<read_len;
        }
    }

    for(qint64 i=1; i<=3; i++)
        if(!virtual_buf.contains(block_num + i))
            recv_data((block_num + i) * batch_size);

    for(qint64 i=1; i<block_num-2; i++){
        if(virtual_buf.contains(i)){
            if(virtual_buf[i] != nullptr)
                delete virtual_buf[i];
            virtual_buf.erase(i);
        }
    }

    return read_bytes;
}

qint64 StreamIODevice::writeData(const char* data, qint64 maxSize) {
    Q_UNUSED(data);
    Q_UNUSED(maxSize);
    return -1; // 쓰기 지원 안 함
}

StreamIODevice::~StreamIODevice(){
    for(const auto& [key, val] : virtual_buf){
        if(val != nullptr) delete val;
    }
}
