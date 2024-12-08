import QtQuick
import QtQuick.Controls

Rectangle {
    visible: true
    width: 400
    height: 300

    ListView {
        id: listView
        anchors.fill: parent
        z: 0
        signal removeItem(int idx)

        model: PlaylistModel

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

                Button{
                    id: removeBtn
                    anchors.right : parent.right
                    anchors.bottom: parent.bottom
                    icon.source: "qrc:/icons/x_icon.png"

                    onClicked: {
                        var cindex = listView.currentIndex;
                        listView.currentIndex = -1;

                        if(cindex > index) cindex = cindex - 1;
                        else if(cindex == index){
                            if(cindex < (listView.count - 1)) cindex = cindex;
                            else if(listView.count > 1) cindex = 0;
                            else cindex = -1;
                        }

                        listView.currentIndex = cindex;
                        listView.removeItem(index);
                    }
                }

                MouseArea {
                    width: parent.width - removeBtn.width
                    height: parent.height
                    onClicked: {
                        listView.currentIndex = index;// 클릭 시 선택된 아이템 업데이트
                        console.log("request video: " + model.videoId);
                        text_inputer.request_play_video(model.name, model.image, model.videoId, index);
                    }
                }
            }
        }
    }

    Button{
        anchors.bottom: parent.bottom
        x: parent.width - width
        y: parent.height - height
        icon.source: "qrc:/icons/back_icon.png"
        z:2

        onClicked:{
            text_inputer.show_search_page();
        }
    }

    Connections{
        target: listView
        function onRemoveItem(idx){
            text_inputer.remove_video(idx)
        }
    }
}
