#ifndef PLAYER_H
#define PLAYER_H

#include <QMediaPlayer>
#include <QFile>
#include <QWidget>
#include <QQuickWidget>
#include <QJsonArray>

#include "extracter.h"
#include "itag.h"

class Player : public QWidget
{
    Q_OBJECT
public:
    enum userReq{
        refeat = 0,
        pre_video,
        next_video,
        show_playlist,
        end_video,
        req_pause,
        req_play,
        download_video
    };
    Player(QWidget* parent = nullptr);

    int play(const QString& video_id);
    void play_end();
    void pause_video();
    void play_video();
    void download(const QString& name);
    void set_play_btn_icon_from_player(int code);
    void set_title_from_player(const QString& title);

    //player_quick.qml과 통신을 하기 위한 파트로 대부분 player_quick.qml의 상호작용의 처리(요청)를 담당한다.
    Q_INVOKABLE void set_position(double pos);
    Q_INVOKABLE bool canChange(){return !isBlocking;}
    Q_INVOKABLE void request_pre_video(){emit user_request(pre_video);}
    Q_INVOKABLE void request_next_video(){emit user_request(next_video);}
    Q_INVOKABLE void request_play_video(){emit user_request(req_play);}
    Q_INVOKABLE void request_pause_video(){emit user_request(req_pause);}
    Q_INVOKABLE void request_repeat_video(){emit user_request(refeat);}
    Q_INVOKABLE void request_download_video(){emit user_request(download_video);}
    Q_INVOKABLE void request_show_playlist(){emit user_request(show_playlist);}

    QJsonArray search(const QString& text){return extracter.search_yt_contents(text);}

signals:
    void close_window();
    void user_request(int type);

protected:
    void closeEvent(QCloseEvent * ce) override{
        emit close_window();
    }
    void keyPressEvent(QKeyEvent *event) override {
        if (event->key() == Qt::Key_Back) {
            request_show_playlist();
            event->accept();
        } else {
            QWidget::keyPressEvent(event);
        }
    }
private:
    QMediaPlayer * player;
    QQuickWidget * video_quick;
    Extracter extracter;
    int batch_size = 1024 * 1024 * 2;
    std::vector<Itag> formats_info;

    QFile download_file;
    QNetworkReply * download_reply;
    QString extension = "";
    bool isBlocking = false;
};

#endif // PLAYER_H
