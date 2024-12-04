#include "videoitem.h"
#include "ui_videoitem.h"

VideoItem::VideoItem(const QByteArray& image_data, const QString& text, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::VideoItem)
{
    ui->setupUi(this);
    image_label = ui->image_label;
    label = ui->label;

    QHBoxLayout * hbox = new QHBoxLayout(this);
    hbox->addWidget(image_label);
    hbox->addWidget(label);

    QPixmap pixmap;
    pixmap.loadFromData(image_data);
    auto scaled_pixmap = pixmap.scaled(image_label->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
    image_label->setPixmap(scaled_pixmap);
    label->setText(text);
}

VideoItem::~VideoItem()
{
    delete ui;
}
