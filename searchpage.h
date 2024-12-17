#ifndef SEARCHPAGE_H
#define SEARCHPAGE_H

#include <QObject>
#include <QAbstractListModel>
#include <QString>
#include <QQuickWidget>

#include "player.h"

struct ListItem {
    QString name;
    QString image;
    QString videoId;
};

class SearchListModel : public QAbstractListModel {
    Q_OBJECT
public:
    enum Roles {
        NameRole = Qt::UserRole + 1,
        ImageRole,
        IdRole
    };

    explicit SearchListModel(QObject *parent = nullptr) : QAbstractListModel(parent) {}

    void addItem(const QString &name, const QString &image, const QString &video_id) {
        beginInsertRows(QModelIndex(), items.size(), items.size());
        items.append({name, image, video_id});
        endInsertRows();
    }

    void clearItems(){
        beginResetModel();
        items.clear();
        endResetModel();
    }

    int rowCount(const QModelIndex &parent = QModelIndex()) const override {
        Q_UNUSED(parent)
        return items.size();
    }

    QVariant data(const QModelIndex &index, int role) const override {
        if (!index.isValid() || index.row() >= items.size())
            return QVariant();

        const ListItem &item = items[index.row()];
        if (role == NameRole)
            return item.name;
        if (role == ImageRole)
            return item.image;
        if (role == IdRole)
            return item.videoId;
        return QVariant();
    }

    QHash<int, QByteArray> roleNames() const override {
        QHash<int, QByteArray> roles;
        roles[NameRole] = "name";
        roles[ImageRole] = "image";
        roles[IdRole] = "videoId";
        return roles;
    }

private:
    QList<ListItem> items;
};

class PlaylistModel : public QAbstractListModel {
    Q_OBJECT
public:
    enum Roles {
        NameRole = Qt::UserRole + 1,
        ImageRole,
        IdRole
    };

    explicit PlaylistModel(QObject *parent = nullptr) : QAbstractListModel(parent) {}

    void addItem(const QString &name, const QString &image, const QString &video_id) {
        beginInsertRows(QModelIndex(), items.size(), items.size());
        items.append({name, image, video_id});
        endInsertRows();
    }

    void clearItems(){
        beginResetModel();
        items.clear();
        endResetModel();
    }

    void removeItem(int row, const QModelIndex &parent = QModelIndex()){
        beginRemoveRows(parent, row, row);
        items.remove(row);
        endRemoveRows();
    }

    int rowCount(const QModelIndex &parent = QModelIndex()) const override {
        Q_UNUSED(parent)
        return items.size();
    }

    QVariant data(const QModelIndex &index, int role) const override {
        if (!index.isValid() || index.row() >= items.size())
            return QVariant();

        const ListItem &item = items[index.row()];
        if (role == NameRole)
            return item.name;
        if (role == ImageRole)
            return item.image;
        if (role == IdRole)
            return item.videoId;
        return QVariant();
    }

    QHash<int, QByteArray> roleNames() const override {
        QHash<int, QByteArray> roles;
        roles[NameRole] = "name";
        roles[ImageRole] = "image";
        roles[IdRole] = "videoId";
        return roles;
    }

    ListItem get_play_video(int idx){
        return items[idx];
    }

private:
    QList<ListItem> items;
};


class SearchPage : public QWidget
{
    Q_OBJECT

public:
    explicit SearchPage(Player * p, QWidget* parent = nullptr);
    ~SearchPage();

    Q_INVOKABLE void set_text(const QString& str);
    Q_INVOKABLE void request_play_video(const QString& name, const QString& image, const QString& video_id, int idx);
    Q_INVOKABLE void show_search_page();
    Q_INVOKABLE void show_playlist();
    Q_INVOKABLE void add_video(const QString& name, const QString& image, const QString& video_id);
    Q_INVOKABLE void show_video();
    Q_INVOKABLE void remove_video(int idx);

    Q_INVOKABLE void pause();
    Q_INVOKABLE void play();
    Q_INVOKABLE void refeat();
    Q_INVOKABLE void next_video();
    Q_INVOKABLE void pre_video();
    Q_INVOKABLE void download();
protected:
    void keyPressEvent(QKeyEvent *event) override {
        if (event->key() == Qt::Key_Back) {
            if(playlist_quick->isHidden() && player->isHidden())
                QCoreApplication::quit(); // 앱 종료
            else
                show_search_page();
            event->accept();
        } else {
            QWidget::keyPressEvent(event);
        }
    }

private:
    Player * player;
    SearchListModel slm;
    PlaylistModel plm;
    QString text;
    QQuickWidget * quickWidget;
    QQuickWidget * playlist_quick;

    QString title;
    QString image;
    QString video;
    int play_idx = -1;

    void update_subplayer(bool video_enable);
};
#endif // SEARCHPAGE_H
