#include <QApplication>
#include <QCoreApplication>
#include <QJniObject>

#include "player.h"
#include "searchpage.h"

void enterPip(){
    auto activity = QJniObject(QNativeInterface::QAndroidApplication::context());
    if(activity.isValid()){
        activity.callObjectMethod("enterPictureInPictureMode", "()V");
    }
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Player player;

    SearchPage search_page(&player);

    QObject::connect(qApp, &QGuiApplication::applicationStateChanged, [](Qt::ApplicationState state) {
        if (state == Qt::ApplicationSuspended) {
            qDebug() << "홈 버튼이 눌려서 앱이 백그라운드로 전환되었습니다.";
            enterPip();
        } else if (state == Qt::ApplicationActive) {
            qDebug() << "앱이 포그라운드로 돌아왔습니다.";
        }
    });

    search_page.show();
    return a.exec();
}
