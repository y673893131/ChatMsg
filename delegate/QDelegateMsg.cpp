#include "QDelegateMsg.h"
#include <QPainter>
#include <QDebug>
#include <QApplication>
#include "./model/QModelChatMsg.h"
#include <QTextDocument>
#define DIR(x) (QApplication::applicationDirPath() + x)

QDelegateMsg::QDelegateMsg(QObject *parent)
    : QStyledItemDelegate(parent), m_bBatchSelect(false), m_bRealName(false), m_lineHeight(20), m_width(0)
{
    m_leftPixmap.load(DIR("/Resources/left.png"));
    m_rightPixmap.load(DIR("/Resources/right.png"));
}

QDelegateMsg::~QDelegateMsg()
{
}

void QDelegateMsg::setLineHeight(int height)
{
    m_lineHeight = height;
}

int QDelegateMsg::lineHeight()
{
    return m_lineHeight;
}

void QDelegateMsg::setFont(QFont font)
{
    m_textFont = font;
}

void QDelegateMsg::setWidth(int width)
{
    m_width = width;
}

void QDelegateMsg::paint(QPainter *painter,
    const QStyleOptionViewItem &option, const QModelIndex &index) const
{
//    qDebug() << index;
    QStyleOptionViewItem viewOption(option);
    initStyleOption(&viewOption, index);
    QRect Rect = viewOption.rect;
    painter->setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);//������
    painter->setPen(Qt::NoPen);
    painter->setBrush(QBrush(Qt::gray));

    auto type = index.data(QModelChatMsg::Role_Type).toInt();
    auto msg = index.data(QModelChatMsg::Role_msg).toString();
    auto size = index.data(QModelChatMsg::Role_msgRc).toSize();
    auto isHtml = index.data(QModelChatMsg::Role_IsHtml).toBool();

    if (type == User_Type::User_Other) { // �û�
        auto m_iconLeftRect = QRect(20, 10 + Rect.y(), 40, 40);
        auto m_kuangLeftTitleRect = QRect(71, m_lineHeight / 4 * 3 + Rect.y(), Rect.width() - 71, 20);
        auto m_kuangLeftRect = QRect(71, m_kuangLeftTitleRect.bottom(), size.width() + 20, size.height() + 20);
        auto m_sanjiaoLeftRect = QRect(65, 9 + Rect.y(), 6, 36);
        auto m_textLeftRect = QRect(83, m_kuangLeftRect.y() + 10, size.width(), size.height());

        //
//        auto backColor = index.data(QModelChatMsg::Role_Icon_Back).value<QColor>();
        QColor backColor(124, 162, 115);
        QPainterPath path;
        path.addRoundedRect(m_iconLeftRect, (m_iconLeftRect.height()) / 2, (m_iconLeftRect.height()) / 2);
        painter->fillPath(path, QBrush(backColor));

        auto rc = m_iconLeftRect;
        rc.setWidth(m_iconLeftRect.width() * 3 / 4);
        rc.setHeight(m_iconLeftRect.height() * 3 / 4);
        rc.moveCenter(QPoint(m_iconLeftRect.x() + m_iconLeftRect.height() / 2 - 1, m_iconLeftRect.y() + m_iconLeftRect.height() / 2 - 1));
        painter->drawPixmap(rc, m_leftPixmap);
        //���ӱ�
        QColor col_Kuang(235, 245, 255);
        painter->setBrush(QBrush(col_Kuang));
        painter->drawRoundedRect(m_kuangLeftRect.x() - 1, m_kuangLeftRect.y() - 1, m_kuangLeftRect.width() + 2, m_kuangLeftRect.height() + 2, 4, 4);
        //��
        painter->drawRoundedRect(m_kuangLeftRect, 4, 4);

        //����
        QPointF points[3] = {
            QPointF(m_sanjiaoLeftRect.x(), 35 + Rect.y()),
            QPointF(m_sanjiaoLeftRect.x() + m_sanjiaoLeftRect.width(), 40 + Rect.y()),
            QPointF(m_sanjiaoLeftRect.x() + m_sanjiaoLeftRect.width(), 48 + Rect.y()),
        };
        QPen pen;
        pen.setColor(col_Kuang);
        painter->setPen(pen);
        painter->drawPolygon(points, 3);

        //ѡ������


        //����
        QPen penText;
        penText.setColor(Qt::gray);
        painter->setPen(penText);
        QTextOption textOption(Qt::AlignLeft | Qt::AlignTop);
        textOption.setWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);
        painter->setFont(m_textFont);
        QColor selColor(160, 160, 164);
        drawSelect(painter, m_textLeftRect, viewOption, selColor);

        auto title = index.data(QModelChatMsg::Role_Title_Text).toString();
        painter->drawText(m_kuangLeftTitleRect, title, textOption);
        penText.setColor(Qt::black);
        painter->setPen(penText);
        if(!isHtml)
            painter->drawText(m_textLeftRect, msg, textOption);
        else
        {
            painter->save();
            painter->translate(m_textLeftRect.topLeft());
            htmlSize(msg, m_textFont, m_textLeftRect.width(), painter);
            painter->restore();
        }
    }
    else if (type == User_Type::User_Me) { // �Լ�
        auto m_iconRightRect = QRect(Rect.width() - 60, 10 + Rect.y(), 40, 40);
        auto m_sanjiaoRightRect = QRect(m_iconRightRect.left() - 6, 9 + Rect.y(), 6, 36);
        auto m_kuangRightRect = QRect(m_sanjiaoRightRect.left() - size.width() - 25, 12 + Rect.y(), size.width() + 25, size.height() + 20);
        auto m_textRightRect = QRect(m_kuangRightRect.left() + 10, m_kuangRightRect.y() + 10, size.width(), size.height());
        auto m_textStateRightRect = QRect(m_kuangRightRect.left() - 25, m_textRightRect.y() + size.height() / 2 - 10, 20, 20);
        auto state = index.data(QModelChatMsg::Role_State).toInt();

        {
//            auto backColor = index.data(QModelChatMsg::Role_Icon_Back).value<QColor>();
            QColor backColor(121, 80, 197);
            QPainterPath path;
            path.addRoundedRect(m_iconRightRect, (m_iconRightRect.height()) / 2, (m_iconRightRect.height()) / 2);
            painter->fillPath(path, QBrush(backColor));
        }

        //icon
        painter->drawPixmap(m_iconRightRect, m_rightPixmap);

        //
        QColor col_Kuang(18, 183, 245);
        painter->setBrush(QBrush(col_Kuang));
        painter->drawRoundedRect(m_kuangRightRect, 4, 4);

        //
        QPointF points[3] = {
            QPointF(m_sanjiaoRightRect.x() + m_sanjiaoRightRect.width(), 30 + Rect.y()),
            QPointF(m_sanjiaoRightRect.x(), 25 + Rect.y()),
            QPointF(m_sanjiaoRightRect.x(), 35 + Rect.y()),
        };
        QPen pen;
        pen.setColor(col_Kuang);
        painter->setPen(pen);
        painter->drawPolygon(points, 3);

        //msg
        QPen penText;
        penText.setColor(Qt::white);
        painter->setPen(penText);
        QTextOption textOption(Qt::AlignLeft | Qt::AlignVCenter);
        textOption.setWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);
        painter->setFont(m_textFont);
        QColor selColor(51, 153, 255);
        drawSelect(painter, m_textRightRect, viewOption, selColor);
//        painter->drawText(m_textRightRect, msg, textOption);
        if(!isHtml)
            painter->drawText(m_textRightRect, msg, textOption);
        else
        {
            painter->save();
            painter->translate(m_textRightRect.topLeft());
            htmlSize(msg, m_textFont, m_textRightRect.width(), painter);
            painter->restore();
        }
        //send state
        if (state == Send_ing)
        {
            auto an = index.data(QModelChatMsg::Role_Running).toInt();
//            qDebug() << "m_textStateRightRect" << m_textStateRightRect;
            m_textStateRightRect.setWidth(20);
            int outerRadius = (m_textStateRightRect.width() - 1) >> 1;
            int innerRadius = ((m_textStateRightRect.width() - 1) >> 1)*0.38;
            int capsuleHeight = outerRadius - innerRadius;
            int capsuleWidth = (m_textStateRightRect.width() > 32) ? capsuleHeight * 0.23 : capsuleHeight * 0.35;
            int capsuleRadius = capsuleWidth >> 1;
            for (int i = 0; i < 12; ++i) {
                QColor color(/*87,189,164*/Qt::lightGray);
                color.setAlphaF(1.0f - (i / 12.0f));
                painter->setPen(Qt::NoPen);
                painter->setBrush(color);
                painter->save();
                painter->translate(m_textStateRightRect.center());
                painter->rotate(an - i*30.0f);
                painter->drawRoundedRect(((-capsuleWidth) >> 1), -(innerRadius + capsuleHeight), capsuleWidth, capsuleHeight, capsuleRadius, capsuleRadius);
                painter->restore();
            }
        }else if (state == Send_Failed)
        {
            QPainterPath path;
            path.addRoundedRect(m_textStateRightRect, 10, 10);
            painter->fillPath(path, QColor(215, 57, 40));

            QFont te_font = m_textFont;
            te_font.setFamily("MicrosoftYaHei");
            te_font.setPixelSize(19);
            te_font.setBold(true);
            painter->setFont(te_font);
            textOption.setAlignment(Qt::AlignCenter);
            painter->drawText(m_textStateRightRect, "!", textOption);
        }
//        QString sMsg;
//        QPainterPath path;
//        path.addRoundedRect(m_textStateRightRect, 5, 5);
//        if (state == Send_ing)
//        {
//            painter->fillPath(path, QColor(225, 194, 102));
//            sMsg = tr("sending");
//        }
//        else if (state == Send_Successed)
//        {
//            painter->fillPath(path, QColor(36, 219, 36));
//            sMsg = tr("send ok");
//        }
//        else
//        {
//            painter->fillPath(path, QColor(219, 36, 36));
//            sMsg = tr("send failed");
//        }

//        textoption.setAlignment(Qt::AlignCenter);
//        painter->drawText(m_textStateRightRect, sMsg, textoption);
    }
    else if (type == User_Type::User_Time) { // ʱ��
        QPen penText;
        penText.setColor(QColor(153, 153, 153));
        painter->setPen(penText);
        QTextOption viewOption(Qt::AlignCenter);
        viewOption.setWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);
        QFont te_font = m_textFont;
        te_font.setFamily("MicrosoftYaHei");
        te_font.setPixelSize(13);
        painter->setFont(te_font);
        auto time = index.data(QModelChatMsg::Role_Time).toString();
        painter->drawText(Rect, time, viewOption);
    }

    emit start(index);
    qDebug() << "start(" << index << ")";
}

int QDelegateMsg::textMargin()
{
    return m_textMargin;
}

QSize QDelegateMsg::htmlSize(const QString &src, QFont font, int nTextWidth,QPainter *painter) const
{
    QTextOption textOption(Qt::AlignLeft | Qt::AlignVCenter);
    textOption.setWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);
    QTextDocument text;
    text.setHtml(src);
    text.setDefaultTextOption(textOption);
    text.setDefaultFont(font);
    text.setIndentWidth(0);
    text.setTextWidth(nTextWidth);
    qDebug() << "text.size" << text.size() << text.textWidth() << text.idealWidth() << text.indentWidth() << text.documentMargin();
    auto size = QSize(text.idealWidth(), text.size().height());
    if(painter)
    {
        text.drawContents(painter);
//        if(src.indexOf(".gif") >= 0)

    }
    return size;
}

void QDelegateMsg::drawSelect(QPainter *painter, QRect& rc, const QStyleOptionViewItem &viewOption, QColor& color) const
{
    if (viewOption.state & QStyle::State_Selected)
        painter->fillRect(rc, color);
}
