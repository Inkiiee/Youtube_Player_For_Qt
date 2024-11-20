#include "webcomwidget.h"

#include <QApplication>
#include <QPermission>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    WebComWidget w;
    w.show();
    return a.exec();
}
