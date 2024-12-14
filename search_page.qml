import QtQuick
import QtQuick.Controls

Rectangle {
    visible: true
    width: 400
    height: 300

    Timer{
        id: informTimer
        interval: 1000
        repeat: false
        onTriggered: information.visible = false;
    }

    Rectangle{
        id: information
        visible: false
        anchors{
            horizontalCenter: parent.horizontalCenter
            verticalCenter: parent.verticalCenter
        }
        color: "black"
        radius: 10
        z: 5

        Text{
            id: informationText
            anchors{
                horizontalCenter: parent.horizontalCenter
                bottom: parent.bottom
                bottomMargin: 20
            }
            z:5
        }
    }
    TextField {
        id: textField
        anchors.top: parent.top
        width: parent.width * 0.8
        placeholderText: "youtube search"
        z: 1

        Keys.onReturnPressed: {button.clicked();}
    }

    Button{
        id: button
        anchors.left: textField.right
        width: parent.width - textField.width
        height: textField.height
        icon.source: "qrc:/icons/search_icon.png"
        z: 1

        onPressed: {
            button.scale = 0.9;
        }

        onReleased: {
            button.scale = 1;
        }

        onClicked:{
            text_inputer.set_text(textField.text);
        }
    }

    ListView {
        id: listView
        anchors.top: textField.bottom
        width: parent.width
        height: parent.height - textField.height -button.height
        z: 0

        model: SearchListModel

        delegate: Item {
            width: listView.width
            height: listView.height/5

            Rectangle {
                anchors.fill: parent
                anchors.margins: 10
                color: listView.currentIndex === index ? "lightblue" : "white"
                // 색상 변경 시 애니메이션 추가
                Behavior on color {
                    ColorAnimation {
                        duration: 200
                    }
                }

                Image {
                    id: thumbnail
                    source: model.image
                    width: parent.width /3
                    height: parent.height
                    fillMode: Image.PreserveAspectFit
                }

                Text {
                    x: thumbnail.width + 10
                    text: model.name
                    anchors.verticalCenter: parent.verticalCenter
                }

                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        listView.currentIndex = index;// 클릭 시 선택된 아이템 업데이트
                        informationText.text = model.name + " 추가됨!!";
                        information.visible = true;
                        informTimer.start();
                        text_inputer.add_video(model.name, model.image, model.videoId);
                    }
                }
            }
        }
    }

    Rectangle{
        id: subPlayer
        objectName: "sub_player"
        anchors.bottom: parent.bottom
        width: parent.width
        height: parent.height / 6
        z: 1
        visible: false
        color: "white"
        border.color: "gray"

        Image {
            id: subPlayerImage
            objectName: "sub_player_image"
            width: parent.width /3
            height: parent.height
            z: 2
            fillMode: Image.PreserveAspectFit
        }

        Text {
            id: subPlayerText
            objectName: "sub_player_text"
            x: subPlayerImage.width + 10
            z: 1
            anchors.verticalCenter: parent.verticalCenter
            SequentialAnimation {
                running: true
                loops: Animation.Infinite

                NumberAnimation {
                    target: subPlayerText
                    property: "x"
                    from: subPlayer.width
                    to: 0
                    duration: 5000
                    easing.type: Easing.InOutQuad
                }
            }
        }

        Image{
            id: playButton
            objectName: "sub_player_play_btn"
            anchors.bottom: parent.bottom
            width: 30
            height: 30
            x: parent.width/2
            z: 3
            property int status: 0
            property string iconSource: "qrc:/icons/pause_icon.png"
            source: iconSource
            MouseArea{
                anchors.fill: parent
                onClicked:{
                    if(playButton.status === 0) {text_inputer.pause();}
                    else if(playButton.status === 1) {text_inputer.play();}
                    else if(playButton.status === 2) {text_inputer.refeat();}
                }
            }
        }
        Image{
            id: preButton
            source: "qrc:/icons/pre_video_icon.png"
            anchors.right: playButton.left
            width: 30
            height: 30
            y: playButton.y
            z: 3
            MouseArea{
                anchors.fill: parent
                onClicked:{text_inputer.pre_video();}
            }
        }
        Image{
            id: nextButton
            source: "qrc:/icons/next_video_icon.png"
            anchors.left: playButton.right
            width: 30
            height: 30
            y: playButton.y
            z:3
            MouseArea{
                anchors.fill: parent
                onClicked:{text_inputer.next_video();}
            }
        }

        Image{
            id: replayModeButton
            objectName: "replay_mode_button"
            //mode 0: auto play, 1: repeat again, 2: play once the video
            property int mode: 0
            anchors{top: parent.top; right: parent.right}
            source: "qrc:/icons/auto_replay_mode.png"
            width: 30
            height: 30
            z: 3
            MouseArea{
                anchors.fill: parent
                onClicked:{
                    parent.mode += 1;
                    if(parent.mode >= 3){parent.mode=0;}

                    if(parent.mode === 0) {replayModeButton.source = "qrc:/icons/auto_replay_mode.png"}
                    else if(parent.mode === 1) {replayModeButton.source = "qrc:/icons/again_replay_mode.png"}
                    else {replayModeButton.source = "qrc:/icons/x_icon.png"}
                }
            }
        }
        Image{
            id: downloadButton
            anchors{top: parent.top; right: replayModeButton.left}
            source: "qrc:/icons/download_icon.png"
            width: 30
            height: 30
            z: 3
            MouseArea{
                anchors.fill: parent
                onClicked: text_inputer.download();
            }
        }

        MouseArea {
            anchors.fill: parent
            onDoubleClicked: {text_inputer.show_video(); parent.visible = false;}
        }

    }

    Image{
        id: playlistPageBtn
        anchors{bottom: parent.bottom; right: parent.right}
        width: 30
        height: 30
        z: 2
        source: "qrc:/icons/list_check_icon.png"

        MouseArea{
            anchors.fill: parent
            onClicked: text_inputer.show_playlist();
        }
    }
}
