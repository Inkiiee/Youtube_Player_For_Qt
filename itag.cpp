#include "itag.h"

std::map<int, VideoInfo> Itag::videoInfoMap = {
    {5, { "flv", "audio/video", "240p", "64k", "acodec:mp3;vcodec:h263", "GoogleDrive" }},
    {6, { "flv", "audio/video", "270p", "64k", "acodec:mp3;vcodec:h263", "GoogleDrive" }},
    {13, { "3gp", "audio/video", "-", "-", "acodec:aac;vcodec:mp4v", "GoogleDrive" }},
    {17, { "3gp", "audio/video", "144p", "24k", "acodec:aac;vcodec:mp4v", "GoogleDrive" }},
    {18, { "mp4", "audio/video", "360p", "96k", "acodec:aac;vcodec:h264", "GoogleDrive" }},
    {22, { "mp4", "audio/video", "720p", "192k", "acodec:aac;vcodec:h264", "GoogleDrive" }},
    {34, { "flv", "audio/video", "360p", "128k", "acodec:aac;vcodec:h264", "GoogleDrive" }},
    {35, { "flv", "audio/video", "480p", "128k", "acodec:aac;vcodec:h264", "GoogleDrive" }},
    {36, { "3gp", "audio/video", "180p", "-", "acodec:aac;vcodec:mp4v", "GoogleDrive" }},
    {37, { "mp4", "audio/video", "1080p", "192k", "acodec:aac;vcodec:h264", "GoogleDrive" }},
    {38, { "mp4", "audio/video", "3072p", "192k", "acodec:aac;vcodec:h264", "GoogleDrive" }},
    {43, { "webm", "audio/video", "360p", "128k", "acodec:vorbis;vcodec:vp8", "GoogleDrive" }},
    {44, { "webm", "audio/video", "480p", "128k", "acodec:vorbis;vcodec:vp8", "GoogleDrive" }},
    {45, { "webm", "audio/video", "720p", "192k", "acodec:vorbis;vcodec:vp8", "GoogleDrive" }},
    {46, { "webm", "audio/video", "1080p", "192k", "acodec:vorbis;vcodec:vp8", "GoogleDrive" }},
    {59, { "mp4", "audio/video", "480p", "128k", "acodec:aac;vcodec:h264", "GoogleDrive"}},
    {78, { "mp4", "audio/video", "480p", "128k", "acodec:aac;vcodec:h264", "GoogleDrive"}},
    {82, { "mp4", "audio/video", "360p", "128k", "acodec:aac;vcodec:h264", "3D" }},
    {83, { "mp4", "audio/video", "480p", "128k", "acodec:aac;vcodec:h264", "3D" }},
    {84, { "mp4", "audio/video", "720p", "192k", "acodec:aac;vcodec:h264", "3D" }},
    {85, { "mp4", "audio/video", "1080p", "192k", "acodec:aac;vcodec:h264", "3D" }},
    {91, { "mp4", "audio/video", "144p", "48k", "acodec:aac;vcodec:h264", "Apple HTTP Live Streaming" }},
    {92, { "mp4", "audio/video", "240p", "48k", "acodec:aac;vcodec:h264", "Apple HTTP Live Streaming" }},
    {93, { "mp4", "audio/video", "360p", "128k", "acodec:aac;vcodec:h264", "Apple HTTP Live Streaming" }},
    {94, { "mp4", "audio/video", "480p", "128k", "acodec:aac;vcodec:h264", "Apple HTTP Live Streaming" }},
    {95, { "mp4", "audio/video", "720p", "256k", "acodec:aac;vcodec:h264", "Apple HTTP Live Streaming" }},
    {96, { "mp4", "audio/video", "1080p", "256k", "acodec:aac;vcodec:h264", "Apple HTTP Live Streaming" }},
    {100, { "webm", "audio/video", "360p", "128k", "acodec:vorbis;vcodec:vp8", "3D" }},
    {101, { "webm", "audio/video", "480p", "192k", "acodec:vorbis;vcodec:vp8", "3D" }},
    {102, { "webm", "audio/video", "720p", "192k", "acodec:vorbis;vcodec:vp8", "3D" }},
    {132, { "mp4", "audio/video", "240p", "48k", "acodec:aac;vcodec:h264", "Apple HTTP Live Streaming" }},
    {133, { "mp4", "video", "240p", "-", "vcodec:h264", "DASH" }},
    {134, { "mp4", "video", "360p", "-", "vcodec:h264", "DASH" }},
    {135, { "mp4", "video", "480p", "-", "vcodec:h264", "DASH" }},
    {136, { "mp4", "video", "720p", "-", "vcodec:h264", "DASH" }},
    {137, { "mp4", "video", "1080p", "-", "vcodec:h264", "DASH" }},
    {138, { "mp4", "video", "2160p60", "-", "vcodec:h264", "DASH" }},
    {139, { "m4a", "audio", "-", "48k", "acodec:aac", "DASH" }},
    {140, { "m4a", "audio", "-", "128k", "acodec:aac", "DASH" }},
    {141, { "m4a", "audio", "-", "256k", "acodec:aac", "DASH" }},
    {151, { "mp4", "audio/video", "72p", "24k", "acodec:aac;vcodec:h264", "Apple HTTP Live Streaming" }},
    {160, { "mp4", "video", "144p", "-", "vcodec:h264", "DASH" }},
    {167, { "webm", "video", "360p", "-", "vcodec:vp8", "DASH" }},
    {168, { "webm", "video", "480p", "-", "vcodec:vp8", "DASH" }},
    {169, { "webm", "video", "720p", "-", "vcodec:vp8", "DASH" }},
    {170, { "webm", "video", "1080p", "-", "vcodec:vp8", "DASH" }},
    {171, { "webm", "audio", "-", "128k", "acodec:vorbis", "DASH" }},
    {172, { "webm", "audio", "-", "256k", "acodec:vorbis", "DASH" }},
    {212, { "mp4", "video", "480p", "-", "vcodec:h264", "DASH" }},
    {218, { "webm", "video", "480p", "-", "vcodec:vp8", "DASH" }},
    {219, { "webm", "video", "480p", "-", "vcodec:vp8", "DASH" }},
    {242, { "webm", "video", "240p", "-", "vcodec:vp9", "DASH" }},
    {243, { "webm", "video", "360p", "-", "vcodec:vp9", "DASH" }},
    {244, { "webm", "video", "480p", "-", "vcodec:vp9", "DASH" }},
    {245, { "webm", "video", "480p", "-", "vcodec:vp9", "DASH" }},
    {246, { "webm", "video", "480p", "-", "vcodec:vp9", "DASH" }},
    {247, { "webm", "video", "720p", "-", "vcodec:vp9", "DASH" }},
    {248, { "webm", "video", "1080p", "-", "vcodec:vp9", "DASH" }},
    {249, { "webm", "audio", "-", "50k", "acodec:opus", "DASH" }},
    {250, { "webm", "audio", "-", "70k", "acodec:opus", "DASH" }},
    {251, { "webm", "audio", "-", "160k", "acodec:opus", "DASH" }},
    {256, { "m4a", "audio", "-", "-", "acodec:aac", "DASH"}},
    {258, { "m4a", "audio", "-", "-", "acodec:aac", "DASH"}},
    {264, { "mp4", "video", "1440p", "-", "vcodec:h264", "DASH" }},
    {266, { "mp4", "video", "2160p60", "-", "vcodec:h264", "DASH" }},
    {271, { "webm", "video", "1440p", "-", "vcodec:vp9", "DASH" }},
    {272, { "webm", "video", "2160p", "-", "vcodec:vp9", "DASH" }},
    {278, { "webm", "video", "144p", "-", "-", "-" }},
    {298, { "mp4", "video", "720p60", "-", "vcodec:h264", "DASH" }},
    {299, { "mp4", "video", "1080p60", "-", "vcodec:h264", "DASH" }},
    {302, { "webm", "video", "720p60", "-", "vcodec:vp9", "DASH" }},
    {303, { "webm", "video", "1080p60", "-", "vcodec:vp9", "DASH" }},
    {308, { "webm", "video", "1440p60", "-", "vcodec:vp9", "DASH" }},
    {313, { "webm", "video", "2160p", "-", "vcodec:vp9", "DASH" }},
    {315, { "webm", "video", "2160p60", "-", "vcodec:vp9", "DASH" }},
    {325, { "m4a", "audio", "-", "-", "acodec:dtse", "DASH"}},
    {328, { "m4a", "audio", "-", "-", "acodec:ec-3", "DASH"}},
    {330, { "webm", "video", "144p60", "-", "hdr", "-" }},
    {331, { "webm", "video", "240p60", "-", "hdr", "-" }},
    {332, { "webm", "video", "360p60", "-", "hdr", "-" }},
    {333, { "webm", "video", "480p60", "-", "hdr", "-" }},
    {334, { "webm", "video", "720p60", "-", "hdr", "-" }},
    {335, { "webm", "video", "1080p60", "-", "hdr", "-" }},
    {336, { "webm", "video", "1440p60", "-", "hdr", "-" }},
    {337, { "webm", "video", "2160p60", "-", "hdr", "-" }},
    {394, { "mp4", "video", "144p", "-", "vcodec:av01.0.00M.08", "DASH"}},
    {395, { "mp4", "video", "240p", "-", "vcodec:av01.0.00M.08", "DASH"}},
    {396, { "mp4", "video", "360p", "-", "vcodec:av01.0.01M.08", "DASH"}},
    {397, { "mp4", "video", "480p", "-", "vcodec:av01.0.04M.08", "DASH"}},
    {398, { "mp4", "video", "720p", "-", "vcodec:av01.0.05M.08", "DASH"}},
    {399, { "mp4", "video", "1080p", "-", "vcodec:av01.0.08M.08", "DASH"}},
    {400, { "mp4", "video", "1440p", "-", "vcodec:av01.0.12M.08", "DASH"}},
    {401, { "mp4", "video", "2160p", "-", "vcodec:av01.0.12M.08", "DASH"}},
    {402, { "mp4", "video", "2880p", "-", "-", "-"}}
};

Itag::Itag(int itag) {
    this->itag = itag;
}

QString Itag::get_container(){
    if(videoInfoMap.contains(itag))
        return videoInfoMap[itag].container;
    return "";
}
QString Itag::get_content(){
    if(videoInfoMap.contains(itag))
        return videoInfoMap[itag].content;
    return "";
}
QString Itag::get_resolution(){
    if(videoInfoMap.contains(itag))
        return videoInfoMap[itag].resolution;
    return "";
}
QString Itag::get_bitrate(){
    if(videoInfoMap.contains(itag))
        return videoInfoMap[itag].bitrate;
    return "";
}
QString Itag::get_codec(){
    if(videoInfoMap.contains(itag))
        return videoInfoMap[itag].codec;
    return "";
}
QString Itag::get_protocol(){
    if(videoInfoMap.contains(itag))
        return videoInfoMap[itag].protocol;
    return "";
}
