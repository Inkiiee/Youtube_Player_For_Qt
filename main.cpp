#include <QApplication>

#include "player.h"
#include "searchpage.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Player player;
    //player.setWindowFlags(Qt::FramelessWindowHint);

    SearchPage search_page(&player);
    search_page.show();
    return a.exec();
}
