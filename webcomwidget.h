#ifndef WEBCOMWIDGET_H
#define WEBCOMWIDGET_H

#include <QWidget>
#include <QNetworkAccessManager>
#include <QLineEdit>
#include <QPushButton>
#include <queue>
#include <map>

QT_BEGIN_NAMESPACE
namespace Ui {
class WebComWidget;
}
QT_END_NAMESPACE

class WebComWidget : public QWidget
{
    Q_OBJECT

public:
    WebComWidget(QWidget *parent = nullptr);
    ~WebComWidget();
    QString base_js_url;

    void decrypt_n(QString& n);
    void decrypt_signature(QString& signature);

    void print_cookies();

    QJsonArray get_yt_video_formats(const QString& video_url);
    std::unordered_map<QString, QString> parse_url_param(const QString& url);
    QString parse_base_url(const QString& url);
    QString get_url(const QString& base, const std::unordered_map<QString, QString>& params);
    QString get_video_info(const QString& yt_vid);
    int get_yt_decrypt_funcs(const QString& urlStr);
    QJsonArray parse_video_stream_info();
    QString get_video_id(QString url);
private:
    Ui::WebComWidget *ui;
    QNetworkAccessManager * manager;
    QLineEdit * url_edit;
    QPushButton * get_btn;
    QNetworkCookieJar * ckj;
    QNetworkRequest request;

    std::map<QString, QString> info;
    std::map<QString, QJsonObject> video_info;

    QString find_n_function_name(const QString& js_code);
    int find_n_function_body(const QString& js_code, const QString& n_name);
    int find_descrypt_n_func(const QString& js_code);
    QString find_player_script_url(const QString& video_html);
    QString get_decrypted_url(const QString& sc);

    int parse_video_info(const QString& vinfo);
    void set_header();
    void error_show(int code);
    std::queue<QString> play_segments;
    int batch_size = 1024 * 1024;
    int i = 0;
};
#endif // WEBCOMWIDGET_H
