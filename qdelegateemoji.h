#ifndef QDELEGATEEMOJI_H
#define QDELEGATEEMOJI_H

#include <QStyledItemDelegate>

class QDelegateEmoji : public QStyledItemDelegate
{
    Q_OBJECT
public:
    QDelegateEmoji(QObject *parent);
    void paint(QPainter *painter,
            const QStyleOptionViewItem &option, const QModelIndex &index) const;
};

#endif // QDELEGATEEMOJI_H
