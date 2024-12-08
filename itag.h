#ifndef ITAG_H
#define ITAG_H

#include <QString>
#include <map>

struct VideoInfo {
    QString container;
    QString content;
    QString resolution;
    QString bitrate;
    QString codec;
    QString protocol;
};

class Itag
{
public:
    Itag(int itag);
    int itag;

    QString get_container();
    QString get_content();
    QString get_resolution();
    QString get_bitrate();
    QString get_codec();
    QString get_protocol();
private:
    static std::map<int, VideoInfo> videoInfoMap;
};

#endif // ITAG_H
