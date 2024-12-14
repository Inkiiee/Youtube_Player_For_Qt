#include "player.h"
#include "streamiodevice.h"

#include <QNetworkReply>
#include <QNetworkReply>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QAudioOutput>
#include <QQmlContext>
#include <QQmlEngine>
#include <QQuickItem>
#include <QTimer>
#include <QJsonDocument>

Player::Player(QWidget* parent):QWidget(parent){
    QVBoxLayout * vbox = new QVBoxLayout(this);
    player = new QMediaPlayer(this);
    video_quick = new QQuickWidget(this);

    video_quick->setResizeMode(QQuickWidget::SizeRootObjectToView);
    video_quick->setFocusPolicy(Qt::StrongFocus); // 포커스 정책 설정
    video_quick->setAttribute(Qt::WA_AcceptTouchEvents, true);

    video_quick->engine()->rootContext()->setContextProperty("player", this);
    video_quick->setSource(QUrl(QStringLiteral("qrc:/player_quick.qml")));
    if (video_quick->status() != QQuickWidget::Ready)
        qDebug() << "QML Load Error:" << video_quick->errors();

    vbox->addWidget(video_quick);

    auto video = video_quick->rootObject()->findChild<QObject*>("video_output");
    auto audio = video_quick->rootObject()->findChild<QAudioOutput*>("audio_output");
    player->setVideoOutput(video);
    player->setAudioOutput(audio);

    QObject::connect(player, &QMediaPlayer::durationChanged, [this](qint64 duration){
        auto slider = video_quick->rootObject()->findChild<QObject*>("video_slider");
        auto play_time_text = video_quick->rootObject()->findChild<QObject*>("play_time_text");

        slider->setProperty("to", duration);

        duration/=1000;
        int min = duration / 60;
        int sec = duration % 60;
        play_time_text->setProperty("endTime", QString::asprintf("%02d:%02d",min, sec));
    });
    QObject::connect(player, &QMediaPlayer::positionChanged, [this](qint64 position){
        auto slider = video_quick->rootObject()->findChild<QObject*>("video_slider");
        slider->setProperty("value", position);

        auto play_time_text = video_quick->rootObject()->findChild<QObject*>("play_time_text");
        position/=1000;
        int min = position / 60;
        int sec = position % 60;
        play_time_text->setProperty("playTime", QString::asprintf("%02d:%02d",min, sec));

        if(player->position() == player->duration())
            emit user_request(end_video);
    });
}

void Player::set_position(double pos){
    StreamIODevice * device = (StreamIODevice*)(player->sourceDevice());
    if(device == nullptr) return;

    double trans_rate = (double)(device->size()) / player->duration();
    if(trans_rate < 0) return;

    qint64 batch = device->get_batch_size();
    qint64 position = (qint64)pos;
    qint64 offset = position * trans_rate;

    if(!device->seek(offset)){
        player->stop();
        isBlocking = true;
        qint64 block = offset/batch;
        QEventLoop loop;
        QTimer timer;
        QObject::connect(&timer, &QTimer::timeout, this, [&timer, &loop, device, block](){
            if(device->isBlockAvailable(block)){
                timer.stop();
                loop.quit();
            }
        });
        timer.start(100);
        loop.exec();
        isBlocking = false;
    }

    emit user_request(req_play);
    player->setPosition(position);
}

//유튜브의 video id를 넣으면 이를 기반으로 데이터를 가져옵니다.
int Player::play(const QString& video_id){
    QString base = "https://www.youtube.com/watch";
    std::unordered_map<QString, QString> params;
    params["v"] = video_id;

    QString video_url = extracter.get_url(base, params);
    auto formats = extracter.get_yt_video_formats(video_url);
    formats_info.clear();

    for(const auto format: formats)
        formats_info.push_back(Itag(format.toObject()["itag"].toInt()));

    QString url = formats[0].toObject()["decryptedUrl"].toString();

    qDebug()<<url;
    extracter.get_request().setUrl(QUrl(url));

    auto format = formats[0].toObject();
    QString str_length = format["contentLength"].toString();
    QString mime = format["mimeType"].toString().split(";")[0];
    QString ext = mime.split('/')[1];
    if(!ext.isEmpty()) extension = ext;

    StreamIODevice * device = new StreamIODevice(extracter.get_manager(), extracter.get_request(), batch_size, player);
    device->seek(0);

    QObject::connect(device, &StreamIODevice::readyRead, this, [this](){
        qDebug()<<"data ready!!!!!";
        player->setSourceDevice((QIODevice*)(sender()));
        player->play();
        set_play_btn_icon_from_player(req_play);
    });

    return 0;
}

void Player::play_end(){
    player->stop();
    auto device = player->sourceDevice();
    if(device != nullptr) delete device;
    player->setSourceDevice(nullptr);
}

void Player::pause_video(){
    player->pause();
    set_play_btn_icon_from_player(req_pause);
}
void Player::play_video(){
    player->play();
    set_play_btn_icon_from_player(req_play);
}

void Player::download(const QString& name){
    QString file_name = name + "." + extension;
    qDebug()<<file_name;
    download_file.setFileName(file_name);
    if(!download_file.open(QFile::WriteOnly)){
        QMessageBox::information(this, "error", "file create fail");
        return;
    }

    download_reply = extracter.get_manager()->get(extracter.get_request());
    QObject::connect(download_reply, &QNetworkReply::finished, [this](){
        download_file.write(download_reply->readAll());
        download_file.close();
        download_reply->deleteLater();

        QMessageBox::information(this, "information", "file downloaded");
    });
}

void Player::set_play_btn_icon_from_player(int code){
    auto play_icon = video_quick->rootObject()->findChild<QObject*>("play_icon");
    QString icon_source;
    int state;

    if(code == req_play || code == pre_video || code == next_video || code == refeat){state = 0; icon_source = "qrc:/icons/video_pause_icon.png";}
    else if(code == req_pause){state = 1; icon_source = "qrc:/icons/play_fill_icon.png";}
    else if(code == end_video) {state = 2; icon_source = "qrc:/icons/video_again_icon.png";}

    play_icon->setProperty("imageSource", icon_source);
    play_icon->setProperty("state", state);
}

void Player::set_title_from_player(const QString& title){
    auto video_title = video_quick->rootObject()->findChild<QObject*>("video_title");
    video_title->setProperty("text", title);
}
