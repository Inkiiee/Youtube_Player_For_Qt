import QtQuick
import QtQuick.Controls
import QtMultimedia

Rectangle {
    visible: true
    width: 400
    height: 300
    color: "black"

    VideoOutput{
        anchors.fill: parent
        id: videoOutput
        objectName: "video_output"
    }

    AudioOutput{
        id: audioOutput
        objectName: "audio_output"
        volume: 1
    }

    MouseArea{
        id: mouseArea
        anchors.fill: parent
        property bool visibleOption: false

        Timer{
            id: timer
            interval: 3000
            repeat: false
            onTriggered:{ mouseArea.visibleOption = false; }
        }
        onClicked:{
            visibleOption = !visibleOption;
            if(visibleOption){timer.running = true;}
        }
    }


    Rectangle{
        id: optionsRect
        anchors.fill: parent
        width: parent.width
        height: parent.height
        visible: mouseArea.visibleOption
        color: "#00000000"

        Slider{
            id: videoSlider
            objectName: "video_slider"
            anchors{
                bottom: parent.bottom
                bottomMargin: 10
                left: parent.left
                leftMargin: 10
            }
            width: parent.width - 20
            height: 5
            Rectangle {
                implicitWidth: videoSlider.width
                implicitHeight: 4
                color: "gray"
                radius: 5
                opacity: 0.5

                // 트랙의 채워진 부분
                Rectangle {
                    width: videoSlider.visualPosition * parent.width
                    height: parent.height
                    color: "darkblue"
                    radius: 2
                }
            }
            handle: Item{}
            onMoved: {
                if(player.canChange()){player.set_position(value);}
            }
        }
        Text{
            id: playTimeText
            objectName: "play_time_text"
            anchors{bottom: videoSlider.top; left: videoSlider.left}
            width: 50
            height: 20
            property string playTime: "00:00"
            property string endTime: "00:00"
            text: playTime + "/" + endTime
            verticalAlignment: Text.AlignVCenter
            font.bold: true
            color: "white"
        }
        Image{
            id: videoVolume
            anchors{
                bottom: videoSlider.top
                bottomMargin: 5
                right: parent.right
                rightMargin: 10
            }
            width: 20
            height: 20
            source: audioOutput.volume != 0 ? "qrc:/icons/volume_icon.png" : "qrc:/icons/volume_off_icon"
            fillMode: Image.PreserveAspectFit
            opacity: 0.5

            MouseArea{
                anchors.fill: videoVolume
                onClicked:{
                    var state = volumeSlider.visible;
                    volumeSlider.visible = !state;
                }
            }
        }
        Slider{
            id: volumeSlider
            anchors{
                bottom: videoVolume.top
                bottomMargin: 5
                horizontalCenter: videoVolume.horizontalCenter
            }
            width: 4
            height: 80
            visible: false

            from: 0
            value: audioOutput.volume
            to: 1
            orientation: Qt.Vertical
            Rectangle {
                implicitWidth: 4
                implicitHeight: volumeSlider.height
                color: "gray"
                radius: 5
                opacity: 0.5

                // 트랙의 채워진 부분
                Rectangle {
                    anchors.bottom: parent.bottom
                    width: 4
                    height: parent.height - volumeSlider.visualPosition * parent.height
                    color: "darkblue"
                    radius: 2
                }
            }
            handle: Item{}

            onMoved:{audioOutput.setVolume(value);}
        }

        Rectangle{
            id: playButton
            anchors.centerIn: parent
            width: parent.height/8
            height: parent.height/8
            color: "gray"
            opacity: 0.5
            radius: width

            Image{
                id: playIcon
                objectName: "play_icon"
                anchors.centerIn: parent
                width: parent.width - 10
                height: parent.width - 10
                property string imageSource: "qrc:/icons/video_pause_icon.png"
                property int state: 0
                source: imageSource
            }

            MouseArea{
                anchors.fill: parent
                onClicked:{
                    if(playIcon.state === 0){player.request_pause_video();}
                    else if(playIcon.state === 1){player.request_play_video();}
                    else if(playIcon.state === 2){player.request_repeat_video();}
                }
            }
        }
        Rectangle{
            anchors{
                right:playButton.left
                rightMargin: 10
                verticalCenter: playButton.verticalCenter
            }
            width: parent.height/10
            height: parent.height/10
            color: "gray"
            opacity: 0.5
            radius: width

            Image{
                id: skipBackIcon
                anchors.centerIn: parent
                width: parent.width - 10
                height: parent.width - 10
                source: "qrc:/icons/skip_back_icon.png"
            }

            MouseArea{
                anchors.fill: parent
                onClicked:{player.request_pre_video();}
            }
        }
        Rectangle{
            anchors{
                left:playButton.right
                leftMargin: 10
                verticalCenter: playButton.verticalCenter
            }
            width: parent.height/10
            height: parent.height/10
            color: "gray"
            opacity: 0.5
            radius: width

            Image{
                id: skipFowardIcon
                anchors.centerIn: parent
                width: parent.width - 10
                height: parent.width - 10
                source: "qrc:/icons/skip_foward_icon.png"
            }

            MouseArea{
                anchors.fill: parent
                onClicked:{player.request_next_video();}
            }
        }
        Text{
            id: videoTitle
            objectName: "video_title"
            anchors{
                left: parent.left
                leftMargin: 5
                top: parent.top
                topMargin: 10
            }
            text: "title"
            color: "white"

            font.bold: true
            font.pixelSize: 15
        }
        Image{
            id: playListIcon
            anchors{
                right: parent.right
                rightMargin: 5
                top: parent.top
                topMargin: 10
            }
            width: 20
            height: 20
            source: "qrc:/icons/playlist_icon.png"

            MouseArea{
                anchors.fill: parent
                onClicked:{player.request_show_playlist();}
            }
        }
        Image{
            id: optionIcon
            anchors{
                right: playListIcon.left
                rightMargin: 10
                top: parent.top
                topMargin: 10
            }
            width: 20
            height: 20
            source: "qrc:/icons/option_icon.png"

            MouseArea{
                anchors.fill: parent
                onClicked: {popup.open();}
            }
        }
        Image{
            id: downloadIcon
            anchors{
                right: optionIcon.left
                rightMargin: 10
                top: parent.top
                topMargin: 10
            }
            width: 20
            height: 20
            source: "qrc:/icons/video_download_icon.png"

            MouseArea{
                anchors.fill: parent
                onClicked:{player.request_download_video();}
            }
        }
    }
}
