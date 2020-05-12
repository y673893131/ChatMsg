#include "qdelegateemoji.h"
#include <QMovie>
#include <QPainter>
QDelegateEmoji::QDelegateEmoji(QObject *parent)
    :QStyledItemDelegate(parent)
{

}

void QDelegateEmoji::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    auto movie = reinterpret_cast<QMovie*>(index.data(Qt::DisplayRole).value<void*>());
    auto cur = movie->currentPixmap();
    auto rc = cur.rect();
    rc.moveCenter(option.rect.center());
    if (option.state & QStyle::State_MouseOver)
        painter->fillRect(option.rect, QColor(243,243,243));
    painter->drawPixmap(rc, movie->currentPixmap());
}
