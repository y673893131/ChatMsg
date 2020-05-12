#include "qemojiview.h"
#include <qemojimodel.h>
#include <qdelegateemoji.h>
#include <QHeaderView>
#include<QScrollBar>
#include <QDebug>
QEmojiView::QEmojiView(QWidget* parent)
    :QTableView(parent)
{
    auto model = new QEmojiModel(this);
    setModel(model);
    auto delegate = new QDelegateEmoji(this);
    setItemDelegate(delegate);
    resize(40*12, 200);

    for (auto col = 0; col < model->columnCount(); ++col) {
        setColumnWidth(col, 40);
    }

    for (auto row = 0; row < model->rowCount();++row) {
        setRowHeight(row, 40);
    }

    setHorizontalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAlwaysOff);
    horizontalHeader()->hide();
    verticalHeader()->hide();
    setShowGrid(false);
    setMouseTracking(true);
    connect(model, SIGNAL(update(const QModelIndex&)), this, SLOT(update(const QModelIndex&)));
}


void QEmojiView::focusOutEvent(QFocusEvent *event)
{
    Q_UNUSED(event)
    hide();
    qDebug() << "hide()";
}

void QEmojiView::setVisible(bool visible)
{
    QTableView::setVisible(visible);
    emit showSelf(visible);
}
