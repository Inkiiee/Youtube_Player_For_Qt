#include "searchpage.h"

#include "searchpage.h"

#include <QJsonDocument>
#include <QVBoxLayout>
#include <QJsonArray>
#include <QJsonValue>
#include <QJsonObject>
#include <QQmlContext>
#include <QQmlEngine>
#include <QQuickItem>
#include <QFileDialog>

SearchPage::SearchPage(WebComWidget * w, QWidget *parent)
    : QWidget(parent), wcw(w), slm(this)
{
    // QQuickWidget 생성
    quickWidget = new QQuickWidget(this);
    playlist_quick = new QQuickWidget();
    quickWidget->setResizeMode(QQuickWidget::SizeRootObjectToView);
    quickWidget->setFocusPolicy(Qt::StrongFocus); // 포커스 정책 설정
    quickWidget->setAttribute(Qt::WA_AcceptTouchEvents, true);

    this->setFocusPolicy(Qt::StrongFocus);

    quickWidget->engine()->rootContext()->setContextProperty("SearchListModel", &slm);
    quickWidget->engine()->rootContext()->setContextProperty("text_inputer", this);
    playlist_quick->engine()->rootContext()->setContextProperty("PlaylistModel", &plm);
    playlist_quick->engine()->rootContext()->setContextProperty("text_inputer", this);

    // QML 파일 로드
    quickWidget->setSource(QUrl(QStringLiteral("qrc:/search_page.qml")));
    playlist_quick->setSource(QUrl(QStringLiteral("qrc:/play_list.qml")));

    QVBoxLayout * vbox = new QVBoxLayout(this);
    vbox->addWidget(quickWidget);
    vbox->addWidget(playlist_quick);

    if (quickWidget->status() != QQuickWidget::Ready)
        qDebug() << "QML Load Error:" << quickWidget->errors();

    show_search_page();
    if(playlist_quick->status() != QQuickWidget::Ready)
        qDebug()<<"QML load Error: "<<playlist_quick->errors();

    QObject::connect(wcw, &WebComWidget::close_window, [this](){
        auto context = quickWidget->rootObject();

        auto subPlayer = context->findChild<QObject*>("sub_player");
        auto subPlayerImage = context->findChild<QObject*>("sub_player_image");
        auto subPlayerText = context->findChild<QObject*>("sub_player_text");

        if(subPlayer && subPlayerImage && subPlayerText){
            subPlayer->setProperty("visible", true);
            subPlayerImage->setProperty("source", image);
            subPlayerText->setProperty("text", title);
        }
    });

    QObject::connect(wcw, &WebComWidget::user_request, [this](int type){
        if(type == wcw->refeat){
            request_play_video(title, image, video, play_idx);
        }
        else if(type == wcw->pre_video){
            pre_video();
        }
        else if(type == wcw->next_video){
            next_video();
        }
        else if(type == wcw->show_playlist){
            show_playlist();
            wcw->close();
        }
        else if(type == wcw->end_video){
            auto qreplay_mode_button = quickWidget->rootObject()->findChild<QObject*>("replay_mode_button");

            if(qreplay_mode_button){
                int mode = qreplay_mode_button->property("mode").toInt();
                if(mode == 0) next_video();
                else if(mode == 1) refeat();
                else{
                    auto qplay_btn = quickWidget->rootObject()->findChild<QObject*>("sub_player_play_btn");
                    if(qplay_btn){
                        qplay_btn->setProperty("iconSource", "qrc:/refeat_icon.png");
                        qplay_btn->setProperty("status", 2);
                    }
                }
            }

        }
        else if(type == wcw->req_pause)
            pause();
        else if(type == wcw->req_play)
            play();
        else if(type == wcw->download_video){
            download();
        }
    });

}

void SearchPage::set_text(const QString& str){
    text = str;
    slm.clearItems();

    QJsonArray contents = wcw->search_yt_contents(text);
    for(const auto& content: contents){
        QJsonObject obj = content.toObject()["videoRenderer"].toObject();
        if(obj.isEmpty()) continue;

        QString thumbnail_url = obj["thumbnail"].toObject()["thumbnails"].toArray()[0].toObject()["url"].toString();
        QString title = obj["title"].toObject()["runs"].toArray()[0].toObject()["text"].toString();
        QString video_id = obj["videoId"].toString();

        slm.addItem(title, thumbnail_url, video_id);
    }
}

void SearchPage::request_play_video(const QString& name, const QString& image, const QString& video_id, int idx){
    wcw->play_end();
    wcw->play(video_id);

    this->image = image;
    this->title = name;
    this->video = video_id;

    play_idx = idx;
    auto qplay_btn = quickWidget->rootObject()->findChild<QObject*>("sub_player_play_btn");
    if(qplay_btn){
        qplay_btn->setProperty("iconSource", "qrc:/pause_icon.png");
        qplay_btn->setProperty("status", 0);
    }
    update_subplayer(false);
}

void SearchPage::show_search_page(){
    quickWidget->setHidden(false);
    playlist_quick->setHidden(true);
}

void SearchPage::show_playlist(){
    quickWidget->setHidden(true);
    playlist_quick->setHidden(false);
}

void SearchPage::add_video(const QString& name, const QString& image, const QString& video_id){
    plm.addItem(name, image, video_id);

    if(play_idx == -1){
        play_idx = 0;
        request_play_video(name, image, video_id, play_idx);
        wcw->show();
    }
}

void SearchPage::show_video(){
    wcw->show();
}

void SearchPage::update_subplayer(bool video_enable){
    auto context = quickWidget->rootObject();

    auto subPlayer = context->findChild<QObject*>("sub_player");
    auto subPlayerImage = context->findChild<QObject*>("sub_player_image");
    auto subPlayerText = context->findChild<QObject*>("sub_player_text");
    if(!video_enable) video_enable = subPlayer->property("visible").toBool();

    if(subPlayer && subPlayerImage && subPlayerText){
        subPlayer->setProperty("visible", video_enable);
        subPlayerImage->setProperty("source", image);
        subPlayerText->setProperty("text", title);
    }
}

void SearchPage::remove_video(int idx){
    plm.removeItem(idx);

    if(idx < play_idx) play_idx --;
    else if(idx > play_idx) return;
    else{
        if(idx <= (plm.rowCount() - 1)){
            auto video = plm.get_play_video(play_idx);
            request_play_video(video.name, video.image, video.videoId, play_idx);
        }
        else if(plm.rowCount() > 0){
            play_idx = 0;
            auto video = plm.get_play_video(play_idx);
            request_play_video(video.name, video.image, video.videoId, play_idx);
        }
        else{
            play_idx = -1;
            plm.clearItems();
        }
    }
}

void SearchPage::pause(){
    wcw->pause_video();

    auto qplay_btn = quickWidget->rootObject()->findChild<QObject*>("sub_player_play_btn");
    if(qplay_btn){
        qplay_btn->setProperty("iconSource", "qrc:/play_icon.png");
        qplay_btn->setProperty("status", 1);
    }
}
void SearchPage::play(){
    wcw->play_video();

    auto qplay_btn = quickWidget->rootObject()->findChild<QObject*>("sub_player_play_btn");
    if(qplay_btn){
        qplay_btn->setProperty("iconSource", "qrc:/pause_icon.png");
        qplay_btn->setProperty("status", 0);
    }
}
void SearchPage::refeat(){
    wcw->play_end();
    wcw->play(video);

    auto qplay_btn = quickWidget->rootObject()->findChild<QObject*>("sub_player_play_btn");
    if(qplay_btn){
        qplay_btn->setProperty("iconSource", "qrc:/pause_icon.png");
        qplay_btn->setProperty("status", 0);
    }
}
void SearchPage::pre_video(){
    if(play_idx > 0){
        auto video = plm.get_play_video(--play_idx);
        request_play_video(video.name, video.image, video.videoId, play_idx);
    }
    else{
        play_idx = plm.rowCount() - 1;
        auto video = plm.get_play_video(play_idx);
        request_play_video(video.name, video.image, video.videoId, play_idx);
    }
}
void SearchPage::next_video(){
    if(play_idx < plm.rowCount() - 1){
        auto video = plm.get_play_video(++play_idx);
        request_play_video(video.name, video.image, video.videoId, play_idx);
    }
    else{
        play_idx = 0;
        auto video = plm.get_play_video(play_idx);
        request_play_video(video.name, video.image, video.videoId, play_idx);
    }
}
void SearchPage::download(){
    QFileDialog fd;
    fd.setFileMode(QFileDialog::Directory);
    fd.setOption(QFileDialog::ShowDirsOnly);
    pause();
    fd.exec();
    play();

    QString path = fd.selectedFiles()[0];
    auto video = plm.get_play_video(play_idx);
    QString video_name = video.name;
    QString invalid_chars = "<>:/\\?*";
    for(auto ic: invalid_chars)
        video_name = video_name.remove(ic);

    wcw->download(path + "/" + video_name);
}

SearchPage::~SearchPage()
{
}
