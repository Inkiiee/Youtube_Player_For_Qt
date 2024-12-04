import QtQuick
import QtQuick.Controls

Rectangle {
    visible: true
    width: parent.width
    height: parent.height

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
        z: 4

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
        icon.source: "qrc:/search_icon.png"
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

        Button{
            id: playButton
            objectName: "sub_player_play_btn"
            icon.source: iconSource
            anchors.bottom: parent.bottom
            x: parent.width/2
            z: 3
            background: null

            property int status: 0
            property string iconSource: "qrc:/pause_icon.png"

            onClicked:{
                if(playButton.status === 0) {text_inputer.pause();}
                else if(playButton.status === 1) {text_inputer.play();}
                else if(playButton.status === 2) {text_inputer.refeat();}
            }
        }
        Button{
            id: preButton
            icon.source: "qrc:/pre_video_icon.png"
            anchors.right: playButton.left
            y: playButton.y
            z: 3
            background: null

            onClicked:{text_inputer.pre_video();}
        }
        Button{
            id: nextButton
            icon.source: "qrc:/next_video_icon.png"
            anchors.left: playButton.right
            y: playButton.y
            z:3
            background: null

            onClicked:{text_inputer.next_video();}
        }

        Button{
            id: replayModeButton
            objectName: "replay_mode_button"
            //mode 0: auto play, 1: repeat again, 2: play once the video
            property int mode: 0
            anchors{top: parent.top; right: parent.right}
            icon.source: "qrc:/auto_replay_mode.png"
            background: null
            z: 3

            onClicked:{
                mode += 1;
                if(mode >= 3){mode=0;}

                if(mode === 0) {replayModeButton.icon.source = "qrc:/auto_replay_mode.png"}
                else if(mode === 1) {replayModeButton.icon.source = "qrc:/again_replay_mode.png"}
                else {replayModeButton.icon.source = "qrc:/x_icon.png"}
            }
        }
        Button{
            id: downloadButton
            anchors{top: parent.top; right: replayModeButton.left}
            icon.source: "qrc:/download_icon.png"
            background: null
            z: 3

            onClicked: text_inputer.download();
        }

        MouseArea {
            anchors.fill: parent
            onDoubleClicked: {text_inputer.show_video(); parent.visible = false;}
        }

    }

    Button{
        id: playlistPageBtn
        x: parent.width - width
        y: parent.height - height
        z: 2
        icon.source: "qrc:/list_check_icon.png"

        onClicked: text_inputer.show_playlist();
    }
}
