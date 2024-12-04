#ifndef WEBCOMWIDGET_H
#define WEBCOMWIDGET_H

#include <QWidget>
#include <QNetworkAccessManager>
#include <QLineEdit>
#include <QPushButton>
#include <map>
#include <QMediaPlayer>
#include <QSlider>
#include <QLabel>
#include <QFile>

class WebComWidget : public QWidget
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
    WebComWidget(QWidget *parent = nullptr);
    ~WebComWidget();
    QString base_js_url;

    void decrypt_n(QString& n);
    void decrypt_signature(QString& signature);
    void play_end();
    void print_cookies();

    void pause_video();
    void play_video();
    void download(const QString& name);

    QJsonArray get_yt_video_formats(const QString& video_url);
    QJsonArray search_yt_contents(const QString& text);
    QByteArray get_yt_thumbnail(const QString& thumb_url);
    int play(const QString& video_id);
signals:
    void close_window();
    void user_request(int type);

protected:
    void closeEvent(QCloseEvent * ce) override{
        emit close_window();
        QWidget::closeEvent(ce);
    }

private:
    QNetworkAccessManager * manager;
    QNetworkCookieJar * ckj;
    QNetworkRequest request;
    QMediaPlayer * player;
    QSlider * slider;
    QLabel * play_time;
    QLabel * video_len;
    QPushButton * play_btn;
    QPushButton * download_btn;
    std::map<QString, QString> info;
    std::map<QString, QJsonObject> video_info;
    int batch_size = 1024 * 1024 * 3;
    int i = 0;
    bool isChange = false;
    QNetworkReply * download_reply;
    QFile download_file;

    QString find_n_function_name(const QString& js_code);
    int find_n_function_body(const QString& js_code, const QString& n_name);
    int find_descrypt_n_func(const QString& js_code);
    QString find_player_script_url(const QString& video_html);
    QString get_decrypted_url(const QString& sc);

    std::unordered_map<QString, QString> parse_url_param(const QString& url);
    QString parse_base_url(const QString& url);
    QString get_url(const QString& base, const std::unordered_map<QString, QString>& params);
    QString get_video_info(const QString& yt_vid);
    int get_yt_decrypt_funcs(const QString& urlStr);
    QJsonArray parse_video_stream_info();
    QString get_video_id(QString url);
    QString extention = "mp4";

    int parse_video_info(const QString& vinfo);
    void set_header();
    void error_show(int code);
};
#endif // WEBCOMWIDGET_H
