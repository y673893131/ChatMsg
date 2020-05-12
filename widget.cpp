#include "widget.h"
#include "./view/QListViewMsg.h"
#include <QPushButton>
#include <QBoxLayout>
#include <QButtonGroup>
#include <qemojiview.h>
#include "qemojimodel.h"
#include <QTextDocument>
#include <QMouseEvent>
#include <qdebug.h>
#include <QPainter>
#include<Windows.h>
#include <QBitmap>
#include <QtMath>
#define STR(x) QString::fromUtf8(x)

Widget::Widget(QWidget *parent)
    : QFrameLesswidget(parent)
{
    setWindowFlag(Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);

    auto widget = this;
    m_msgView = new QListViewMsg(widget);
    m_msgView->setLeftTitle(STR("Tom"));
    m_inputMsg = new QTextEdit(widget);
//    m_inputMsg->setAcceptRichText(false);

    QFont te_font;
    te_font.setFamily("Microsoft YaHei");
    te_font.setPixelSize(13);
    m_inputMsg->setFont(te_font);
    m_emojiView = new QEmojiView(widget);
    m_emojiView->hide();
    auto btnSendLeft = new QPushButton(STR("Left"), widget);
    auto btnSendRight = new QPushButton(STR("Right"), widget);
    auto btnSending = new QPushButton(STR("Sending"), widget);
    auto btnSendOK = new QPushButton(STR("Send successed"), widget);
    auto btnSendFailed = new QPushButton(STR("Send failed"), widget);
    auto btnSendOld = new QPushButton(STR("Send Old(Manual calculation)"), widget);
    auto btnSendNew = new QPushButton(STR("Send New(textLayout calculation)"), widget);
    auto btnSendHtml = new QPushButton(STR("Send Html(like emoji)"), widget);
    m_btnEmoji = new QPushButton(widget);
    m_btnEmoji->setCheckable(true);
    m_btnEmoji->setObjectName("emoji_btn");
    m_btnEmoji->setStyleSheet("QPushButton#emoji_btn{min-width:20px;min-height:20px;max-width:20px;max-height:20px;border-image:url(../ChatMsg/res/emj.png);}QPushButton#emoji_btn:hover,QPushButton#emoji_btn:checked{border-image:url(../ChatMsg/res/emj_hover.png);}");
    QButtonGroup* group = new QButtonGroup(widget);
    QButtonGroup* groupState = new QButtonGroup(widget);
    QButtonGroup* groupFlag = new QButtonGroup(widget);
    group->addButton(btnSendLeft, User_Other);
    group->addButton(btnSendRight, User_Me);
    groupState->addButton(btnSending, Send_ing);
    groupState->addButton(btnSendOK, Send_Successed);
    groupState->addButton(btnSendFailed, Send_Failed);
    groupFlag->addButton(btnSendOld, QListViewMsg::Msg_Normal);
    groupFlag->addButton(btnSendNew, QListViewMsg::Msg_New);
    groupFlag->addButton(btnSendHtml, QListViewMsg::Msg_Html);

    QVBoxLayout* layout = new QVBoxLayout(widget);
    widget->setLayout(layout);
    QHBoxLayout* btnLayout = new QHBoxLayout(widget);
    QHBoxLayout* btnLayoutState = new QHBoxLayout(widget);
    QHBoxLayout* btnLayoutFlag = new QHBoxLayout(widget);

    layout->setMargin(10);
    layout->addWidget(m_msgView, 7);
    layout->addWidget(m_btnEmoji, 1);
    layout->addWidget(m_inputMsg, 2);
    layout->addLayout(btnLayout, 1);
    layout->addLayout(btnLayoutState, 1);
    layout->addLayout(btnLayoutFlag, 1);

    btnLayout->addWidget(btnSendLeft);
    btnLayout->addWidget(btnSendRight);
    btnLayoutState->addWidget(btnSending);
    btnLayoutState->addWidget(btnSendOK);
    btnLayoutState->addWidget(btnSendFailed);
    btnLayoutFlag->addWidget(btnSendOld);
    btnLayoutFlag->addWidget(btnSendNew);
    btnLayoutFlag->addWidget(btnSendHtml);

    connect(group, SIGNAL(buttonClicked(int)), this, SLOT(onSend(int)));
    connect(groupState, SIGNAL(buttonClicked(int)), this, SLOT(onSendState(int)));
    connect(groupFlag, SIGNAL(buttonClicked(int)), this, SLOT(onSendFlag(int)));
    connect(m_btnEmoji, SIGNAL(pressed()), this, SLOT(onSelectEmoji()));
    connect(m_emojiView, SIGNAL(clicked(const QModelIndex&)), this, SLOT(onInsertEmoji(const QModelIndex&)));
    connect(m_emojiView, SIGNAL(showSelf(bool)), m_btnEmoji, SLOT(setChecked(bool)));
    connect(m_inputMsg->document(), &QTextDocument::contentsChange, [this](int from, int charsRemoved, int charsAdded)
    {
        if(charsRemoved)
        {
            auto html = m_inputMsg->document()->toHtml();
            for (QMap<QMovie*, QString>::iterator it = m_mInputEmoji.begin(); it != m_mInputEmoji.end();)
            {
                if(!html.contains(it.value()))
                {
                    disconnect(it.key(), SIGNAL(frameChanged(int)), this, SLOT(onInputMovieChanged(int)));
                    qDebug() << "disconnect" << it.value();
                    it = m_mInputEmoji.erase(it);
                }
                else
                    ++it;
            }
        }
    });
}

Widget::~Widget()
{
}

void Widget::onSend(int type)
{
    auto sText = m_inputMsg->toPlainText();
    if(sText.isEmpty()) return;
    m_msgView->sendMsg(sText, type);
    m_inputMsg->clear();
}

void Widget::onSendState(int sendType)
{
    auto sText = m_inputMsg->toPlainText();
    if(sText.isEmpty()) return;
    m_msgView->sendMsg(sText, User_Me, sendType);
    m_inputMsg->clear();
}

void Widget::onSendFlag(int flag)
{
    QString sText;
    if(flag == QListViewMsg::Msg_Html)
        sText = m_inputMsg->toHtml();
    else
        sText = m_inputMsg->toPlainText();
    if(m_inputMsg->toPlainText().isEmpty()) return;
    m_inputMsg->clear();
    m_inputMsg->clearMask();
    if(sText.isEmpty()) return;
    m_msgView->sendMsg(sText, User_Other, Send_Successed, flag);
}

void Widget::onSelectEmoji()
{
//    auto pos = m_inputMsg->pos();
    auto pos = m_btnEmoji->pos();
//    pos.setX(pos.x() + m_btnEmoji->width() / 2);
    pos.setY(pos.y() - m_emojiView->height());
    m_emojiView->move(pos);
    qDebug() << "m_emojiView->isVisible()" << m_emojiView->isVisible() << m_btnEmoji->isChecked();

    if(m_emojiView->isHidden())
    {
        m_emojiView->show();
        m_emojiView->raise();
        m_emojiView->setFocus();
    }
}

void Widget::onInsertEmoji(const QModelIndex &index)
{
    QMovie* movie = reinterpret_cast<QMovie*>(index.data(Qt::DisplayRole).value<void*>());
    if(movie)
    {
        static int emojiIndex = 0;
        auto url = movie->fileName();
        auto html = QString("<img src='%1'/>").arg(url);
        //m_inputMsg->insertHtml(html);
        m_inputMsg->textCursor().insertImage(url);
        m_emojiView->hide();
        m_inputMsg->setFocus();

        if(m_mInputEmoji.contains(movie)) return;
        m_mInputEmoji.insert(movie, url);
        connect(movie, SIGNAL(frameChanged(int)), this, SLOT(onInputMovieChanged(int)));
    }
}

void Widget::onInputMovieChanged(int)
{
    QMovie* movie = qobject_cast<QMovie*>(sender());
    m_inputMsg->document()->addResource(QTextDocument::ImageResource,
            m_mInputEmoji.value(movie), movie->currentPixmap());
    m_inputMsg->setLineWrapColumnOrWidth(m_inputMsg->lineWrapColumnOrWidth());
}

void Widget::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton)
    {
        m_emojiView->hide();
    }

    QFrameLesswidget::mousePressEvent(event);
}

void Widget::keyPressEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_Escape)
        close();
}
