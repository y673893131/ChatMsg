#ifndef QEMOJIVIEW_H
#define QEMOJIVIEW_H

#include <QTableView>

class QEmojiView : public QTableView
{
    Q_OBJECT
public:
    QEmojiView(QWidget* parent);
signals:
    void showSelf(bool bShow);
    // QWidget interface
protected:
    void focusOutEvent(QFocusEvent *event);

    // QWidget interface
public slots:
    void setVisible(bool visible);
};

#endif // QEMOJIVIEW_H
