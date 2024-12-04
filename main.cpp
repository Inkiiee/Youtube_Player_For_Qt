#include "webcomwidget.h"
#include "searchpage.h"

#include <QApplication>
#include <QPermission>
#include <QCoreApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    //startForegroundService();
    WebComWidget w;
    SearchPage s(&w);
    s.show();
    return a.exec();
}
