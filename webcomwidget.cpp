#include "webcomwidget.h"
#include "streamiodevice.h"

#include <QSslConfiguration>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QRegularExpression>
#include <QSsl>
#include <QSslSocket>
#include <QMessageBox>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QNetworkCookieJar>
#include <QNetworkCookie>
#include <QQmlEngine>
#include <QJSValue>
#include <QMediaPlayer>
#include <QVideoWidget>
#include <QAudioOutput>
#include <QVBoxLayout>
#include <QTimer>
#include <QSlider>
#include <QIcon>
#include <QFile>

#include <memory>

std::map<int, QString> error_code;
const int err_no_error = 0;
const int err_load_js = 1;
const int err_find_funcs = 2;
const int err_find_decrypt_func = 3;
const int err_find_sts = 4;
const int err_search_vid = 5;
const int err_search_video_info = 6;
const int err_invalid_json = 7;
const int err_find_json_data = 8;
const int err_reply = 9;
const int err_find_base_js = 10;
const int err_play_status = 11;
const int err_find_n_function_name = 12;
const int err_find_n_function_body = 13;
const int err_decrypt_n = 14;

void init_error(){
    error_code[0] = "No Error :)";
    error_code[1] = "Failed load java script from youtube player";
    error_code[2] = "Not found decrypt functions(reverse, splice, swap) from java script";
    error_code[3] = "Not found decrypt function process from java script";
    error_code[4] = "Not found sts value from java script";
    error_code[5] = "Youtube video id search failed";
    error_code[6] = "Youtube video information search failed";
    error_code[7] = "Failed read json";
    error_code[8] = "Not found data from json";
    error_code[9] = "Rely Error";
    error_code[10] = "Not found player script(base.js or player.js)";
    error_code[11] = "Playability status is not ok";
    error_code[12] = "Not found n decrypt function name";
    error_code[13] = "Not found n decrypt function body";
    error_code[14] = "Failed decrypt n parameter";
}

#include <QIODevice>
#include <QByteArray>
#include <QDebug>

QByteArray WebComWidget::get_yt_thumbnail(const QString& thumb_url){
    QNetworkAccessManager man;
    QNetworkRequest request_content{QUrl(thumb_url)};

    QByteArray image;
    QString url = thumb_url;
    auto reply = man.get(request_content);
    QEventLoop loop;

    if(url.startsWith("//")) url = "https:" + url;

    QObject::connect(reply, &QNetworkReply::finished, [this, &loop, reply, &image, url](){
        if(reply->error() == QNetworkReply::NoError)
            image = reply->readAll();
        else
            qDebug()<<"thumbnail load error: "<<reply->error()<<" url: "<<url;
        reply->deleteLater();
        loop.quit();
    });
    loop.exec();

    return image;
}

QJsonArray WebComWidget::search_yt_contents(const QString& text){
    QString urlStr = "https://www.youtube.com/youtubei/v1/search";

    std::unordered_map<QString, QString> params;
    //params["key"] = "AIzaSyAO_FJ2SlqU8Q4STEHLGCilw_Y9_11qcW8";
    params["key"] = "AIzaSyA8eiZmM1FaDVjRy-df2KTyQ_vz_yYM39w";

    set_header();
    QNetworkRequest request_content;
    request_content.setRawHeader("User-Agent", "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/130.0.0.0 Safari/537.36");
    request_content.setUrl(QUrl(get_url(urlStr, params)));

    QJsonObject client_info;
    client_info["clientName"] = "WEB";
    client_info["clientVersion"] = "2.20241113";

    QJsonObject context;
    context["client"] = client_info;

    QJsonObject post_data;
    post_data["context"] = context;
    post_data["query"] = text;

    QJsonArray answer;

    auto reply = manager->post(request_content, QJsonDocument(post_data).toJson());
    QEventLoop loop;
    QObject::connect(reply, &QNetworkReply::finished, [this, reply, &answer, &loop](){
        if(reply->error() == QNetworkReply::NoError){
            QJsonObject respone = QJsonDocument::fromJson(reply->readAll()).object();
            auto&& temp = respone["contents"].toObject();
            temp = temp["twoColumnSearchResultsRenderer"].toObject();
            temp = temp["primaryContents"].toObject();
            temp = temp["sectionListRenderer"].toObject();
            temp = temp["contents"].toArray()[0].toObject()["itemSectionRenderer"].toObject();
            answer = temp["contents"].toArray();
        }
        else qDebug()<<reply->error();

        reply->deleteLater();
        loop.quit();
    });
    loop.exec();

    return answer;
}

//에러가 발생시 이를 알려주고 프로그램 종료
void WebComWidget::error_show(int code){
    QMessageBox::information(this, "WebComWidget", error_code[code]);
    exit(code);
}

//n 파라미터를 복호화하는 함수의 이름을 가져온다.
QString WebComWidget::find_n_function_name(const QString& js_code){
    QString n_func_name = "";
    QString regexPattern = R"((?x)
        (?:
            \.get\("n"\)\)&&\(b=|
            (?:
                b=String\.fromCharCode\(110\)|
                (?P<str_idx>[a-zA-Z0-9_$.]+)&&\(b="nn"\[\+(?P=str_idx)\]
            )
            (?:
                ,[a-zA-Z0-9_$]+\(a\))?,c=a\.
                (?:
                    get\(b\)|
                    [a-zA-Z0-9_$]+\[b\]\|\|null
                )\)&&\(c=|
            \b(?P<var>[a-zA-Z0-9_$]+)=
        )(?P<nfunc>[a-zA-Z0-9_$]+)(?:\[(?P<idx>\d+)\])?\([a-zA-Z]\)
        (?(var),[a-zA-Z0-9_$]+\.set\("n"\,(?P=var)\),(?P=nfunc)\.length))";

    QRegularExpression re(regexPattern);
    QRegularExpressionMatch match = re.match(js_code);

    if (match.hasMatch()) {
        QString func = match.captured("nfunc");
        QString idx = match.captured("idx");

        QString regexPattern = QString(R"(%1\s*=\s*\[(.*?)\];)").arg(func);
        QRegularExpression re(regexPattern);
        auto match = re.match(js_code);

        n_func_name = match.captured(1);
    }

    return n_func_name;
}

//n 파라미터를 복호화하는 함수의 몸체를 가져와 info에 저장한다.
int WebComWidget::find_n_function_body(const QString& js_code, const QString& n_name){
    QString n_function_pattern = QString(R"((%1)\s*=\s*function\s*\(([^)]*)\)\s*\{([\s\S]*?)\})").arg(n_name);
    QRegularExpression nf_re(n_function_pattern);
    auto n_func_match = nf_re.match(js_code);
    if(!n_func_match.hasMatch()) return err_find_n_function_body;

    auto n_function_sidx = js_code.indexOf(n_func_match.captured(0));
    decltype(n_function_sidx) n_function_eidx = n_function_sidx;
    while(js_code[n_function_eidx] != '{') n_function_eidx++;
    size_t counter = 0;

    while(true){
        if(js_code[n_function_eidx] == '{') ++counter;
        else if(js_code[n_function_eidx] == '}') --counter;
        n_function_eidx++;

        if(counter <= 0){
            if(js_code[n_function_eidx] != ';') ++ counter;
            else break;
        }
    }

    int n_function_len = n_function_eidx - n_function_sidx;
    QString n_function_body = js_code.mid(n_function_sidx, n_function_len);
    info["n_decrypt"] = n_name + "__(n_decrypt_function)__" + n_function_body + ";";

    return err_no_error;
}

//n 파라미터를 복호화하는 함수를 가져와 info에 저장하도록 시킨다.
int WebComWidget::find_descrypt_n_func(const QString& js_code){
    QString n_name = find_n_function_name(js_code);
    if(n_name.isEmpty()) return err_find_n_function_name;

    int error = find_n_function_body(js_code, n_name);
    if(error != err_no_error) return error;

    return err_no_error;
}

//n 파라미터를 복호화한다.
void WebComWidget::decrypt_n(QString& n){
    auto token = info["n_decrypt"].split("__(n_decrypt_function)__");
    QString n_name = token[0];
    QString n_body = token[1];

    QQmlEngine engine;
    engine.evaluate(n_body);

    QJSValue multiplyFunction = engine.globalObject().property(n_name);
    QJSValueList args;
    args << n; // JavaScript 함수에 전달할 인수

    QJSValue result = multiplyFunction.call(args);
    n = result.toString();
}

//쿠키를 보여준다.
void WebComWidget::print_cookies(){
    auto cookies = ckj->cookiesForUrl(QUrl("https://www.youtube.com"));

    qDebug()<<"cookies";
    for(auto cookie: cookies)
        qDebug()<<cookie.name()<<" "<<cookie.value();
}

//url의 파라미터를 파싱한다.
std::unordered_map<QString, QString> WebComWidget::parse_url_param(const QString& url){
    int sidx = url.indexOf("?") + 1;
    auto params_tokens = url.mid(sidx).split("&");
    std::unordered_map<QString, QString> params;

    for(const auto& p: params_tokens){
        auto split_p = p.split("=");
        params[split_p[0]] = QUrl::fromPercentEncoding(split_p[1].toUtf8());
    }

    return params;
}

//base url을 가져온다. (ex -> https://www.youtube.com)
QString WebComWidget::parse_base_url(const QString& url){
    int idx = url.indexOf("?");
    return url.mid(0, idx);
}

//비디오 정보를 가져온다.
int WebComWidget::parse_video_info(const QString& vinfo){
    QJsonDocument video_json = QJsonDocument::fromJson(vinfo.toUtf8());
    if(video_json.isNull()) return err_search_video_info;

    QJsonObject info_obj = video_json.object();
    video_info["playabilityStatus"] = info_obj["playabilityStatus"].toObject();
    video_info["streamingData"] = info_obj["streamingData"].toObject();
    video_info["videoDetails"] = info_obj["videoDetails"].toObject();

    if(video_info["playabilityStatus"]["status"].toString() != "OK"){
        error_code[err_play_status] = "Playability status is not OK (" + video_info["playabilityStatus"]["reason"].toString() + ")";
        return err_play_status;
    }

    return err_no_error;
}

//베이스와 params를 기반으로
QString WebComWidget::get_url(const QString& base, const std::unordered_map<QString, QString>& params){
    QString url = base;
    int i = 0;

    for(const auto& pair: params){
        const auto& param = pair.first + "=" + QString::fromUtf8(QUrl::toPercentEncoding(pair.second));

        if(i != 0) url += "&";
        else url += "?";
        url+=param;

        i++;
    }
    return url;
}

//기본 헤더를 생성한다.
void WebComWidget::set_header(){
    if(!request.hasRawHeader("User-Agent"))
        request.setRawHeader("User-Agent", "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/130.0.0.0 Safari/537.36");
    if(!request.hasRawHeader("Accept"))
        request.setRawHeader("Accept", "*/*");
    if(!request.hasRawHeader("Referer"))
        request.setRawHeader("Referer", "https://www.youtube.com/");
    if(!request.hasRawHeader("Origin"))
        request.setRawHeader("Origin", "https://www.youtube.com");
    if(!request.hasRawHeader("Connection"))
        request.setRawHeader("Connection", "keep-alive");
}

//signatureCipher를 해독한다.
void WebComWidget::decrypt_signature(QString& signature){
    auto reverse = [&signature](){std::reverse(signature.begin(), signature.end());};
    auto splice = [&signature](int b){signature = signature.remove(0, b);};
    auto swap = [&signature](int b){
        auto c = signature[0];
        signature[0] = signature[b % signature.length()];
        signature[b % signature.length()] = c;
    };

    QString decrypt_process = info["decrypt_routine"];
    for(auto process: decrypt_process.split(";")){
        int sidx = process.indexOf("(") + 1;
        int eidx = process.indexOf(")");
        int len = eidx - sidx;
        QString param = process.mid(sidx, len);

        if(process.startsWith("r")) reverse();
        else if(process.startsWith("sw")) swap(param.toInt());
        else if(process.startsWith("s")) splice(param.toInt());
    }
}

//signature와 n값을 모두 해독한 스트림 URL을 리턴한다.
QString WebComWidget::get_decrypted_url(const QString& sc){
    auto s_sp_url = sc.split("&");
    QString s = s_sp_url[0].mid(s_sp_url[0].indexOf("=") + 1);
    QString sp = s_sp_url[1].mid(s_sp_url[1].indexOf("=") + 1);
    QString url = s_sp_url[2].mid(s_sp_url[2].indexOf("=") + 1);
    s = QUrl::fromPercentEncoding(s.toUtf8());
    url = QUrl::fromPercentEncoding(url.toUtf8());

    QString base_url = parse_base_url(url);
    auto params = parse_url_param(url);

    decrypt_signature(s);
    params[sp] = s;

    QString n = params["n"];
    decrypt_n(n);
    params["n"] = n;

    return get_url(base_url, params);
}

//비디오 페이지에서 base.js(플레이어 스크립트)의 URL을 가져온다.
QString WebComWidget::find_player_script_url(const QString& video_html){
    //정규표현식으로 플레이어 자바스크립트를 찾는다.
    QRegularExpression regex(R"(<script\s+src=["']([^"']*\/(?:base|player)\.js)["'])");
    auto match_url = regex.match(video_html);
    if(!match_url.hasMatch())
        return "";

    //상대 URL을 절대 URL로 변환
    auto player_script_url = "https://www.youtube.com" + match_url.captured(1);
    qDebug()<<"find player script: "<<player_script_url;

    return player_script_url;
}

//유튜브의 비디오 id를 url에서 추출한다.
QString WebComWidget::get_video_id(QString url){
    // 정규표현식을 사용하여 유튜브 비디오 ID 추출
    QRegularExpression regex(R"(https?://(?:www\.)?(?:youtube\.com/(?:watch\?v=|embed/|v/|.+\?v=)|youtu\.be/)([\w-]{11}))");
    QRegularExpressionMatch match = regex.match(url);

    if (match.hasMatch())
        return match.captured(1);  // 첫 번째 캡처 그룹에서 비디오 ID 반환
    else
        return "";  // 비디오 ID가 없으면 빈 문자열 반환
}

//video 관련 간략한 정보를 가져온다.
QString WebComWidget::get_video_info(const QString& yt_vid){
    QString urlStr = "https://www.youtube.com/youtubei/v1/player";
    QString vinfo = "";

    std::unordered_map<QString, QString> params;
    //params["key"] = "AIzaSyAO_FJ2SlqU8Q4STEHLGCilw_Y9_11qcW8";
    params["key"] = "AIzaSyA8eiZmM1FaDVjRy-df2KTyQ_vz_yYM39w";

    urlStr = get_url(urlStr, params);

    QUrl url(urlStr);
    qDebug()<<"get video info request url: "<<urlStr;
    QNetworkRequest request(url);
    set_header();
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QJsonObject client_info;
    client_info["clientName"] = "WEB";
    client_info["clientVersion"] = "2.20241113";

    QJsonObject context;
    context["client"] = client_info;

    QJsonObject signature_time_stamp;
    signature_time_stamp["signatureTimestamp"] = info["sts"];
    QJsonObject content_playback_context;
    content_playback_context["contentPlaybackContext"] = signature_time_stamp;

    QJsonObject post_data;
    post_data["videoId"] = yt_vid;
    post_data["context"] = context;
    post_data["playbackContext"] = content_playback_context;

    qDebug()<<post_data;

    std::unique_ptr<QNetworkReply> reply(manager->post(request, QJsonDocument(post_data).toJson()));
    QEventLoop loop;
    int error = err_no_error;

    QObject::connect(reply.get(), &QNetworkReply::finished, [&loop, &reply, &vinfo](){
        if (reply->error() == QNetworkReply::NoError)
            vinfo = QString::fromUtf8(reply->readAll());

        loop.quit();
    });
    loop.exec();

    return vinfo;
}

//sts와 signature cipher 해독 루틴을 가져온다.
int WebComWidget::get_yt_decrypt_funcs(const QString& urlStr){
    //base.js에 유튜브 player에 관한 정보 및 decrypt 함수가 있다.
    QUrl url(urlStr);
    qDebug()<<"youtube player base.js url: "<<urlStr;
    QNetworkRequest request(url);
    set_header();

    std::unique_ptr<QNetworkReply> reply(manager->get(request));
    QString js_code = "";

    QEventLoop loop;
    QObject::connect(reply.get(), &QNetworkReply::finished, [&reply, &loop, &js_code]() {
        if (reply->error() == QNetworkReply::NoError) {
            QByteArray responseData = reply->readAll();
            js_code = QString::fromUtf8(responseData);
        }
        loop.quit();
    });
    loop.exec();

    if(js_code == ""){
        qDebug()<<error_code[err_load_js];
        return err_load_js;
    }

    // 정규 표현식: 함수 이름, 매개변수, 본문 추출
    QString reverse_function;
    QString splice_function;
    QString swap_function;
    QRegularExpression regex("(\\w+)\\s*:\\s*function\\s*\\(([^)]*)\\)\\s*\\{([^}]*)\\}");
    auto match_list = regex.globalMatch(js_code);

    //reverse, swap, splice 함수의 이름 찾기
    for(const auto& content: match_list){
        auto function_name = content.captured(1);
        auto parameters = content.captured(2);
        auto body = content.captured(3);

        auto first_p = parameters.split(",")[0];
        if(body.contains(first_p+".reverse()"))
            reverse_function = function_name;
        else if(body.contains(first_p + ".splice(0,"))
            splice_function = function_name;
        else if(body.contains(first_p+"[0]") && body.contains(first_p+".length"))
            swap_function = function_name;
    }

    if(reverse_function.isEmpty() || splice_function.isEmpty() || swap_function.isEmpty()){
        qDebug()<<error_code[err_find_funcs];
        return err_find_funcs;
    }

    qDebug()<<"reverse name: "<<reverse_function;
    qDebug()<<"swap name: "<<swap_function;
    qDebug()<<"splice name: "<<splice_function;

    // 정규 표현식: signature cipher를 해독하는 부분을 찾기
    QRegularExpression decrypt_func_regex("(\\w+)\\s*=\\s*function\\s*\\(([^)]*)\\)\\s*\\{([^}]*)\\}");
    auto decrypt_func_list = decrypt_func_regex.globalMatch(js_code);

    //찾은 함수의 이름으로 decrypt 함수 구문 찾기
    for(const auto& content: decrypt_func_list){
        auto function_name = content.captured(1);
        auto parameters = content.captured(2);
        auto body = content.captured(3);
        auto first_p = parameters.split(",")[0];

        if(body.contains(first_p+".split") && (body.contains(reverse_function) || body.contains(swap_function)\
            || body.contains(splice_function)) && body.contains(first_p+".join")){
            QString parse_routine = "";
            auto routine = body.split(";");
            for(const auto& command: routine){
                // 정규 표현식 정의: 파라미터 추출
                QRegularExpression regex(R"(\(([^)]*)\))");
                auto match = regex.match(command);

                auto ps = match.captured(1).split(",");
                if(command.contains(reverse_function))
                    parse_routine += "r;";
                else if(command.contains(swap_function)){
                    QString p = ps[0] != first_p ? ps[0] : ps[1];
                    parse_routine += "sw(" + p + ");";
                }
                else if(command.contains(splice_function)){
                    QString p = ps[0] != first_p ? ps[0] : ps[1];
                    parse_routine += "s(" + p + ");";
                }
            }

            //r: reverse, s(a): splice(0, a), sw(a): swap(0, a)
            info["decrypt_routine"] = parse_routine;
            qDebug()<<"find signature decrypt function: "<<function_name;
            qDebug()<<"decrypt function: "<<info["decrypt_routine"];
            break;
        }
    }
    if(info.find("decrypt_routine") == info.end()){
        qDebug()<<error_code[err_find_decrypt_func];
        return err_find_decrypt_func;
    }

    QRegularExpression sts_regex(R"((?:signatureTimestamp|sts):(\d{5}))");
    auto sts_match = sts_regex.match(js_code);
    if(!sts_match.hasMatch()){
        qDebug()<<error_code[err_find_sts];
        return err_find_sts;
    }

    QString sts_val = sts_match.captured(1);
    qDebug()<<"find sts: "<<sts_val;
    info["sts"] = sts_val;

    int error = err_no_error;
    error = find_descrypt_n_func(js_code);

    return error;
}

QJsonArray WebComWidget::parse_video_stream_info(){
    QJsonArray formats;
    for(const auto& obj : video_info["streamingData"]["formats"].toArray()){
        auto format = obj.toObject();
        QString decrypted_url;

        if(format.contains("signatureCipher"))
            decrypted_url = get_decrypted_url(format["signatureCipher"].toString());
        else{
            decrypted_url = format["url"].toString();
            auto params = parse_url_param(decrypted_url);
            QString base_url = parse_base_url(decrypted_url);
            QString n = params["n"];
            decrypt_n(n);
            params["n"] = n;
            decrypted_url = get_url(base_url, params);
        }

        format["decryptedUrl"] = decrypted_url;
        formats.append(format);
    }
    for(const auto& obj: video_info["streamingData"]["adaptiveFormats"].toArray()){
        auto format = obj.toObject();
        QString decrypted_url;

        if(format.contains("signatureCipher"))
            decrypted_url = get_decrypted_url(format["signatureCipher"].toString());
        else
            decrypted_url = format["url"].toString();

        format["decryptedUrl"] = decrypted_url;
        formats.append(format);
    }

    return formats;
}

QJsonArray WebComWidget::get_yt_video_formats(const QString& video_url){
    //플레이어 스크립트가 없는 경우, 비디오 페이지에서 플레이어 스크립트를 가져온다.
    if(base_js_url == ""){
        QUrl url(video_url);
        request.setUrl(url);
        set_header();
        QNetworkReply * reply = manager->get(request);

        QEventLoop loop;

        QObject::connect(reply, &QNetworkReply::finished, [reply, &loop, this]() {
            if (reply->error() == QNetworkReply::NoError) {
                QByteArray responseData = reply->readAll();
                QString response = QString::fromUtf8(responseData);

                QString player_script_url = find_player_script_url(response);
                if(player_script_url.isEmpty()) error_show(err_find_base_js);

                //해당 플레이어 스크립트에서 descrypt functions(n & signature)과 sts값을 읽어온다.
                auto error = get_yt_decrypt_funcs(player_script_url);
                if(error != err_no_error) error_show(error);

                base_js_url = player_script_url;
            }
            loop.quit();
        });
        loop.exec();
        reply->deleteLater();
    }

    //비디오 정보를 가져온다.
    auto vinfo = get_video_info(get_video_id(video_url));
    if(vinfo == "") error_show(err_search_video_info);

    int error = parse_video_info(vinfo);
    if(error != err_no_error) error_show(error);

    //영상의 formats을 가져온다.
    auto formats = parse_video_stream_info();
    return formats;
}

int WebComWidget::play(const QString& video_id){
    QString base = "https://www.youtube.com/watch";
    std::unordered_map<QString, QString> params;
    params["v"] = video_id;

    QString video_url = get_url(base, params);
    auto formats = get_yt_video_formats(video_url);
    QString url = formats[0].toObject()["decryptedUrl"].toString();

    qDebug()<<url;
    request.setUrl(QUrl(url));
    set_header();

    auto format = formats[0].toObject();
    QString str_length = format["contentLength"].toString();
    QString mime = format["mimeType"].toString().split(";")[0];
    QString ext = mime.split('/')[1];
    if(!ext.isEmpty()) extention = ext;

    StreamIODevice * device = new StreamIODevice(manager, &request, batch_size, this);

    player->setSourceDevice(device);
    player->play();

    return 0;
}

WebComWidget::WebComWidget(QWidget *parent)
    : QWidget(parent){
    QVBoxLayout * vbox = new QVBoxLayout(this);
    player = new QMediaPlayer(this);
    QVideoWidget * video = new QVideoWidget(this);
    QAudioOutput * audio = new QAudioOutput(this);
    player->setVideoOutput(video);
    player->setAudioOutput(audio);

    QWidget * hbox_widget = new QWidget(this);
    QHBoxLayout * hbox = new QHBoxLayout(hbox_widget);
    slider = new QSlider(Qt::Horizontal);
    play_time = new QLabel("00:00", this);
    video_len = new QLabel("00:00", this);

    QRect rect = this->rect();
    slider->setGeometry(rect.x() +10, rect.y() + rect.height(), rect.width() - 20, 20);

    play_btn = new QPushButton(this);
    QPushButton * pre_video_btn = new QPushButton(this);
    QPushButton * next_video_btn = new QPushButton(this);
    QPushButton * check_list_btn = new QPushButton(this);
    download_btn = new QPushButton(this);
    pre_video_btn->setIcon(QIcon(":/pre_video_icon.png"));
    next_video_btn->setIcon(QIcon(":/next_video_icon"));
    check_list_btn->setIcon(QIcon(":/list_check_icon.png"));
    download_btn->setIcon(QIcon(":/download_icon.png"));

    QObject::connect(download_btn, &QPushButton::clicked, [this](){
        emit user_request(download_video);
        download_btn->setEnabled(false);
    });
    QObject::connect(slider, &QSlider::sliderPressed, [this](){
        isChange = true;
    });
    QObject::connect(slider, &QSlider::sliderReleased, [this](){
        StreamIODevice * device = (StreamIODevice*)(player->sourceDevice());
        if(device == nullptr) return;

        double trans_rate = (double)(device->size()) / player->duration();
        if(trans_rate < 0) return;

        player->stop();
        qint64 batch = device->get_batch_size();
        qint64 position = slider->sliderPosition();
        qint64 offset = position * trans_rate;

        if(!device->seek(offset)){
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
        }

        emit user_request(req_play);
        player->setPosition(position);
        isChange = false;
    });
    QObject::connect(player, &QMediaPlayer::positionChanged, [this](qint64 position){
        if(!isChange)
            slider->setSliderPosition(position);
        int sec = position / 1000;
        int min = sec / 60;
        sec = sec % 60;
        play_time->setText(QString::asprintf("%02d:%02d",min, sec));

        if(player->position() >= player->duration()){
            play_btn->setIcon(QIcon(":/refeat_icon.png"));
            emit user_request(end_video);
        }
    });
    QObject::connect(player, &QMediaPlayer::durationChanged, [this](qint64 duration){
        slider->setRange(0, duration);
        int sec = duration / 1000;
        int min = sec / 60;
        sec = sec % 60;
        video_len->setText(QString::asprintf("%02d:%02d",min, sec));

        play_btn->setIcon(QIcon(":/pause_icon.png"));
    });
    QObject::connect(play_btn, &QPushButton::clicked, [this](){
        if(player->isPlaying())
            emit user_request(req_pause);
        else if(player->position() >= player->duration())
            emit user_request(refeat);
        else
            emit user_request(req_play);
    });
    QObject::connect(pre_video_btn, &QPushButton::clicked, [this](){ emit user_request(pre_video);});
    QObject::connect(next_video_btn, &QPushButton::clicked, [this](){ emit user_request(next_video);});
    QObject::connect(check_list_btn, &QPushButton::clicked, [this](){ emit user_request(show_playlist);});

    QSizePolicy sp;
    sp.setVerticalPolicy(QSizePolicy::Fixed);
    sp.setHorizontalPolicy(QSizePolicy::Expanding);
    hbox_widget->setSizePolicy(sp);
    vbox->addWidget(video);
    hbox->addWidget(pre_video_btn);
    hbox->addWidget(play_btn);
    hbox->addWidget(next_video_btn);
    hbox->addWidget(play_time);
    hbox->addWidget(slider);
    hbox->addWidget(video_len);
    hbox->addWidget(download_btn);
    hbox->addWidget(check_list_btn);
    vbox->addWidget(hbox_widget);

    manager = new QNetworkAccessManager(this);
    ckj = new QNetworkCookieJar(this);
    manager->setCookieJar(ckj);

    init_error();
    resize(600, 400);
}

void WebComWidget::play_end(){
    player->stop();
    auto device = player->sourceDevice();
    if(device != nullptr) delete device;
    player->setSourceDevice(nullptr);
}

void WebComWidget::pause_video(){
    play_btn->setIcon(QIcon(":/play_icon.png"));
    player->pause();
}
void WebComWidget::play_video(){
    play_btn->setIcon(QIcon(":/pause_icon.png"));
    player->play();
}

void WebComWidget::download(const QString& name){
    QString file_name = name + "." + extention;
    download_file.setFileName(file_name);
    if(!download_file.open(QFile::WriteOnly)){
        QMessageBox::information(this, "error", "file create fail");
        return;
    }

    download_reply = manager->get(request);
    QObject::connect(download_reply, &QNetworkReply::finished, [this](){
        download_file.write(download_reply->readAll());
        download_file.close();
        download_reply->deleteLater();
        download_btn->setEnabled(true);

        QMessageBox::information(this, "information", "file downloaded");
    });
}

WebComWidget::~WebComWidget()
{
}
