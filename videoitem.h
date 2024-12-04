#ifndef VIDEOITEM_H
#define VIDEOITEM_H

#include <QWidget>
#include <QLabel>
#include <QHBoxLayout>

namespace Ui {
class VideoItem;
}

class VideoItem : public QWidget
{
    Q_OBJECT

public:
    explicit VideoItem(const QByteArray& image_data, const QString& text, QWidget *parent = nullptr);
    ~VideoItem();

private:
    Ui::VideoItem *ui;
    QLabel * image_label;
    QLabel * label;
};

#endif // VIDEOITEM_H
