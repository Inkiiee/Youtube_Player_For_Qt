#ifndef EXTRACTER_H
#define EXTRACTER_H

#include <QNetworkAccessManager>
#include <map>
#include <QObject>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

class Extracter : public QObject
{
    Q_OBJECT
public:
    Extracter(QObject * parent = nullptr);
    void decrypt_n(QString& n);
    void decrypt_signature(QString& signature);

    QString get_url(const QString& base, const std::unordered_map<QString, QString>& params);
    std::unordered_map<QString, QString> parse_url_param(const QString& url);
    QString parse_base_url(const QString& url);

    QJsonArray get_yt_video_formats(const QString& video_url);
    QJsonArray search_yt_contents(const QString& text);
    QByteArray get_yt_thumbnail(const QString& thumb_url);

    QNetworkAccessManager* get_manager(){return manager;}
    QNetworkRequest& get_request(){return request;}

private:
    QNetworkAccessManager * manager;
    QNetworkCookieJar * ckj;
    QNetworkRequest request;
    std::map<QString, QString> info;
    std::map<QString, QJsonObject> video_info;
    QString base_js_url;

    QString find_n_function_name(const QString& js_code);
    int find_n_function_body(const QString& js_code, const QString& n_name);
    int find_descrypt_n_func(const QString& js_code);
    QString find_player_script_url(const QString& video_html);
    QString get_decrypted_url(const QString& sc);

    QString get_video_info(const QString& yt_vid);
    int get_yt_decrypt_funcs(const QString& urlStr);
    QJsonArray parse_video_stream_info();
    QString get_video_id(QString url);
    void fix_n_function(QString& n_function_body);

    int parse_video_info(const QString& vinfo);
    void set_header();
    void error_show(int code);
};

#endif // EXTRACTER_H
