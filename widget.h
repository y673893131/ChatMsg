#ifndef WIDGET_H
#define WIDGET_H

#include "./base/FrameLesswidget.h"
#include <QTextEdit>

class QListViewMsg;
class QEmojiView;
class QPushButton;
class Widget : public QFrameLesswidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();
private slots:
    void onSend(int);
    void onSendState(int);
    void onSendFlag(int);
    void onSelectEmoji();
    void onInsertEmoji(const QModelIndex&);
    void onInputMovieChanged(int);
private:
    QListViewMsg* m_msgView;
    QTextEdit* m_inputMsg;
    QEmojiView* m_emojiView;
    QPushButton* m_btnEmoji;
    QMap<QMovie*, QString> m_mInputEmoji;
    // QWidget interface
protected:
    void mousePressEvent(QMouseEvent *event);
    void keyPressEvent(QKeyEvent *event);
};
#endif // WIDGET_H
