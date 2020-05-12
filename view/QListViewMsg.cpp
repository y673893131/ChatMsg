#include "QListViewMsg.h"
#include "./model/QModelChatMsg.h"
#include "./delegate/QDelegateMsg.h"
#include <QDebug>
#include <QClipboard>
#include <QApplication>
#include <QScrollBar>
#include <QTimer>
#include <QDateTime>
#include <QMouseEvent>
#include <time.h>
#include <QTextDocument>
#include <qmath.h>
#include <QTextLayout>
#include <QMovie>
#include <QMessageBox>
QListViewMsg::QListViewMsg(QWidget *parent, const QString& sObjName)
	: QListView(parent), m_parent(parent)
{
	if (!sObjName.isEmpty())
		setObjectName(sObjName);
    auto pModel = new QModelChatMsg(this);
	setModel(pModel);
    auto pDelegate = new QDelegateMsg(this);
    QFont font;
    font.setFamily("Microsoft YaHei");
    font.setPixelSize(13);
    setFont(font);
	pDelegate->setFont(this->font());
    qDebug() << "this->font()" <<this->font().pixelSize() << sObjName;
	setItemDelegate(pDelegate);
	verticalScrollBar()->setContextMenuPolicy(Qt::NoContextMenu);
	horizontalScrollBar()->setContextMenuPolicy(Qt::NoContextMenu);
	setEditTriggers(QAbstractItemView::NoEditTriggers);
	setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
	setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	setSelectionMode(QAbstractItemView::MultiSelection);
    setMouseTracking(true);
    connect(this, SIGNAL(reset()), pModel, SLOT(onReset()));
//    connect(pDelegate, SIGNAL(start(const QModelIndex&)), pModel, SLOT(onStart(const QModelIndex&)));
    connect(pDelegate, SIGNAL(start(const QModelIndex&)), this, SLOT(onCheckRunning(const QModelIndex&)));
    connect(this, SIGNAL(checkRunning(const QModelIndex&,const QModelIndex&)), pModel, SLOT(onCheckRunning(const QModelIndex&,const QModelIndex&)));
    connect(this, SIGNAL(stopView()), pModel, SLOT(onStopView()));
    connect(pModel, SIGNAL(update(const QModelIndex&)), this, SLOT(onUpdate(const QModelIndex&)));

    //m_loading = new QWidget (parent);
    for(int n = 0; n < 20; ++n)
    {
        QLabel* loadinglabel = new QLabel(this);
        loadinglabel->setObjectName("loading");
        loadinglabel->setStyleSheet("QLabel#loading{background: transparent;}");
        loadinglabel->setFixedSize(20,20);
        auto movie = new QMovie(this);
        movie->setFileName("E:\\qt\\ChatMsg\\res\\loading.gif");
        loadinglabel->setMovie(movie);
        m_loadingLabel.push_back(loadinglabel);
    }
//    auto& data = pModel->srcData();
//    data.msgs.resize(10000);
//    qDebug() << clock();
//    for(auto& it : data.msgs)
//    {
//        it.nTm = QDateTime::currentDateTime().toTime_t();
//        it.msg = "123123123";
//        it.msgCt = clock();
//        it.type = User_Me;
//        it.state = Send_Failed;
//        fontSize(it);
//    }
//    qDebug() << clock();
}

QListViewMsg::~QListViewMsg()
{
}

QString QListViewMsg::curCardNum()
{
	return m_sCurCardNum;
}

QSize QListViewMsg::fontSize(ChatMsg& chatMsg, Msg_Flag flag)
{
//    qDebug() << "fontSize" << this->width();
	if (chatMsg.type == User_Time)
	{
		chatMsg.rcSize = QSize(this->width(), 40);
		return chatMsg.rcSize;
	}
	int titleHeight = 0;
	if (chatMsg.type != User_Me) titleHeight = 20;
	int minHei = 30 + titleHeight;

	int textWidth = this->width() - 174;
	int spaceWid = this->width() - textWidth;

	auto msg = chatMsg.msg;
    if(flag == Msg_Normal) {
        chatMsg.rcSize = getRealString(msg, textWidth);
        chatMsg.textSize = chatMsg.rcSize;
        auto lineheight = ((QDelegateMsg*)this->itemDelegate())->lineHeight();
        chatMsg.textSize.setHeight(chatMsg.rcSize.height() - 2 * lineheight);
        int hei = chatMsg.rcSize.height() < minHei ? minHei : (chatMsg.rcSize.height() + titleHeight);
        chatMsg.rcSize.setHeight(hei);
    }
    else
    {
        if(flag == Msg_New)
            chatMsg.textSize = getRealStringNew(msg, textWidth);
        else
            chatMsg.textSize = getRealStringHtml(chatMsg.msg, textWidth);

        int hei = chatMsg.textSize.height() + titleHeight + 34;
        hei = hei < minHei ? minHei : hei;
        chatMsg.rcSize.setHeight(hei);
        chatMsg.rcSize.setWidth(chatMsg.textSize.width());
    }

//    qDebug() << "chatMsg.textSize:" << chatMsg.textSize <<"chatMsg.rcSize:" << chatMsg.rcSize;
    return chatMsg.rcSize;
}

void QListViewMsg::setLeftTitle(const QString & sTitle)
{
    auto model = reinterpret_cast<QModelChatMsg*>(this->model());
    model->srcData().sTitle = sTitle;
}

void QListViewMsg::sendMsg(const QString& msgText, int type, int sendType, int flag)
{
    qDebug() << msgText;
    auto model = reinterpret_cast<QModelChatMsg*>(this->model());
	ChatMsg msg;
    msg.nTm = QDateTime::currentDateTime().toTime_t(); //时间戳
	msg.msg = msgText;
    msg.type = static_cast<User_Type>(type);
    msg.state = (Msg_Send_State)sendType;
    msg.bIsHtml = flag == Msg_Html;
    if(msg.bIsHtml)
    {
        QTextDocument text;
        text.setHtml(msg.msg);
        auto textcus = text.find("123");
        if(!textcus.isNull())
        {
            textcus.movePosition(QTextCursor::StartOfLine,QTextCursor::KeepAnchor,0);
            textcus.insertImage("C:\\Users\\Administrator\\Desktop\\test\\png\\1_1.png");
        }

        msg.msg = text.toHtml();
    }
    fontSize(msg, (Msg_Flag)flag);
	msg.msgCt = clock();
    emit model->sendMsg(msg);
	QTimer::singleShot(0, this, SLOT(scrollToBottom()));
}

QSize QListViewMsg::getRealString(QString& src, int textWidth)
{
	src.replace("\t", "    ");

	QFontMetricsF fm(this->font());
	auto lineHeight = fm.lineSpacing();
    ((QDelegateMsg*)itemDelegate())->setLineHeight(lineHeight);
	int nCount = src.count("\n");
	int nMaxWidth = 0;
	if (nCount == 0) {
		nMaxWidth = fm.width(src);
		QString value = src;
		if (nMaxWidth > textWidth) {
			nMaxWidth = textWidth;
			int size = textWidth / fm.width(" ");
			int num = fm.width(value) / textWidth;
			int ttmp = num*fm.width(" ");
			num = (fm.width(value)) / textWidth;
			nCount += num;
			QString temp = "";
			for (int i = 0; i < num; i++) {
				temp += value.mid(i*size, (i + 1)*size) + "\n";
			}
			src.replace(value, temp);
		}
	}
	else {
		auto valueList = src.split("\n");
		int i = 0;
		for (auto value : valueList) {
			//QString value = valueList.at(i);
			nMaxWidth = fm.width(value) > nMaxWidth ? fm.width(value) : nMaxWidth;
			if (fm.width(value) > textWidth) {
				nMaxWidth = textWidth;
				int size = textWidth / fm.width(" ");
				int num = fm.width(value) / textWidth;
				num = ((i + num)*fm.width(" ") + fm.width(value)) / textWidth;
				nCount += num;
				QString temp = "";
				for (int i = 0; i < num; i++) {
					temp += value.mid(i*size, (i + 1)*size) + "\n";
				}
				src.replace(value, temp);
			}
			++i;
		}
	}
    return QSize(nMaxWidth, (nCount + 3) * lineHeight);
}

QSize QListViewMsg::getRealStringNew(QString &src, int textWidth)
{
    auto msg = src;
    msg.replace('\n', QChar::LineSeparator);
    QTextLayout m_textlayout;
    m_textlayout.setText(msg);
    m_textlayout.setFont(this->font());
    qDebug() << "this->font()" <<this->font().pixelSize();

    QTextOption option(Qt::AlignLeft | Qt::AlignTop);
    option.setWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);
    m_textlayout.setTextOption(option);
    m_textlayout.beginLayout();

    qreal height = 0.0;
    qreal width = 0.0;
    QStringList m_strlist;
    m_strlist.clear();
    QFontMetricsF fm(this->font());
    auto lineHeight = fm.lineSpacing();
    reinterpret_cast<QDelegateMsg*>(itemDelegate())->setLineHeight(lineHeight);
    while (1) {
        QTextLine l = m_textlayout.createLine();
        if (!l.isValid())
            break;

        l.setLineWidth(textWidth);
        m_strlist.push_back(src.mid(l.textStart(), l.textLength()));
        // Make sure lines are positioned on whole pixels
        height = qCeil(height);
        l.setPosition(QPointF(0., height));
        height += lineHeight;
        width = qMax(width, l.naturalTextWidth());
    }
    m_textlayout.endLayout();
    return QSize(width, height);
}

QSize QListViewMsg::getRealStringHtml(QString &src, int textWidth)
{
    return reinterpret_cast<QDelegateMsg*>(itemDelegate())->htmlSize(src, this->font(), textWidth);
}

void QListViewMsg::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event)
//    qDebug() << "resize" << this->width();
    reinterpret_cast<QDelegateMsg*>(itemDelegate())->setWidth(this->width());
    auto model = reinterpret_cast<QModelChatMsg*>(this->model());
	auto& data = model->srcData();
	for (auto& it : data.msgs)
        fontSize(it, it.bIsHtml ? Msg_Html : Msg_New);
	if (verticalScrollBar()->maximum() == verticalScrollBar()->value())
		QTimer::singleShot(0, this, SLOT(scrollToBottom()));

//    m_loading->setGeometry(this->geometry());
//    for(auto& it : m_loadingLabel){
//        if(it->movie()->state() != QMovie::NotRunning)
//        {
//            qDebug() << it << n;
//            it->move(loadingRc.topLeft());
//            it->movie()->start();
//            return;
//        }
//    }
	emit model->layoutChanged();
}

void QListViewMsg::mousePressEvent(QMouseEvent *event)
{
	if (!(event->buttons() & Qt::LeftButton))
	{
        QListView::mousePressEvent(event);
		return;
	}

	m_p1 = event->pos();
	m_p2 = m_p1;
	auto index = indexAt(m_p1);
	clearSelection();
	if (index.isValid())
	{
		auto rc = visualRect(index);
        auto type = model()->data(index, QModelChatMsg::Role_Type).toInt();
        auto size = model()->data(index, QModelChatMsg::Role_msgRc).toSize();

        QRect textRc,stateRc;
		if (type == User_Me)
		{
			textRc.setLeft(rc.x() + rc.width() - size.width() - 81);
			textRc.setTop(rc.y() + 22);
            auto sendType = model()->data(index, QModelChatMsg::Role_State).toInt();
            if(sendType == Send_Failed)
            {
                stateRc.setLeft(rc.x() + rc.width() - size.width() - 81 - 35);
                stateRc.setTop(rc.y() + 22 + size.height() / 2 - 10);
                stateRc.setSize(QSize(20, 20));
                if (stateRc.contains(m_p2))
                {
                    //resend failed msg
                    qDebug() << "resend:"<<index;
                    QMessageBox::information(this, "resend", QString("resend line number %1: %2 ").arg(index.row()).arg(model()->data(index, QModelChatMsg::Role_msg).toString()));
                }
            }
		}
		else
		{
			textRc.setLeft(rc.x() + 83);
            auto lineheight = reinterpret_cast<QDelegateMsg*>(itemDelegate())->lineHeight();
			textRc.setTop(rc.y() + lineheight / 4 * 3 + 30);
		}

		textRc.setSize(size);
		if (!textRc.contains(m_p1))
			QTimer::singleShot(0, this, SLOT(clearSelection()));
		update(index);
	}
    QListView::mousePressEvent(event);
}

void QListViewMsg::mouseReleaseEvent(QMouseEvent *event)
{
	if (!(event->buttons() & Qt::LeftButton))
	{
        QListView::mouseReleaseEvent(event);
		return;
	}

	m_p2 = event->pos();
	auto index = indexAt(m_p2);
    QListView::mouseReleaseEvent(event);
}

void QListViewMsg::mouseMoveEvent(QMouseEvent *event)
{
//	if (!(event->buttons() & Qt::LeftButton))
//	{
//		__super::mouseMoveEvent(event);
//		return;
//	}

	m_p2 = event->pos();
	auto index = indexAt(m_p2);
    if(index.isValid())
    {
        auto sendType = model()->data(index, QModelChatMsg::Role_State).toInt();
        if(sendType == Send_Failed)
        {
            auto rc = visualRect(index);
            auto type = model()->data(index, QModelChatMsg::Role_Type).toInt();
            auto size = model()->data(index, QModelChatMsg::Role_msgRc).toSize();
            //auto m_textStateRightRect = QRect(m_kuangRightRect.left() - 25, m_textRightRect.y() + size.height() / 2 - 10, 20, 20);

            QRect stateRc;
            stateRc.setLeft(rc.x() + rc.width() - size.width() - 81 - 35);
            stateRc.setTop(rc.y() + 22 + size.height() / 2 - 10);
            stateRc.setSize(QSize(20, 20));
            if (stateRc.contains(m_p2))
                setCursor(Qt::PointingHandCursor);
            else
                setCursor(Qt::ArrowCursor);
        }
        else
            setCursor(Qt::ArrowCursor);
    }else
        setCursor(Qt::ArrowCursor);
	//qDebug() << "move" << index;
	//update(QRect(m_p1, m_p2));
    QListView::mouseMoveEvent(event);
}
#include <QMimeData>
void QListViewMsg::keyPressEvent(QKeyEvent *event)
{
	if ((event->modifiers() == Qt::ControlModifier) && (event->key() == Qt::Key_C))
	{
		auto selected = selectedIndexes();
		bool bSignal = selected.size() == 1 ? true : false;
		QString sCopy;
		for (auto it : selected)
		{

            if(!it.data(QModelChatMsg::Role_IsHtml).toBool())
                sCopy += it.data(QModelChatMsg::Role_msg).toString();
            else
            {
                auto sTemp = it.data(QModelChatMsg::Role_msg).toString();
                QTextDocument text;
                text.setHtml(sTemp);

                sCopy += text.toPlainText();
            }
			if (!bSignal) sCopy += "\n";
		}

		QClipboard *board = QApplication::clipboard();
        qDebug() <<sCopy;
        board->setText(sCopy);
    }
}

void QListViewMsg::onCheckRunning(const QModelIndex& index)
{
    if(isHidden())
    {
        emit stopView();
        return;
    }
    auto topIndex = indexAt(QPoint(0,0));
    auto bottomIndex = indexAt(QPoint(0,0 + height() - 5));
//    qDebug() << topIndex << bottomIndex;
    emit checkRunning(topIndex, bottomIndex);

    //    onSetIndexLoading(index);
}
#include<QTimer>
void QListViewMsg::onUpdate(const QModelIndex &index)
{
    if(isHidden())
        return;
//    update(index);
    auto rc = getLoadingRc(index);
//    qDebug() << index << rc;
//    repaint(rc);
    viewport()->repaint(rc);
}

void QListViewMsg::onSetIndexLoading(const QModelIndex & index)
{
    auto rc = visualRect(index);
    auto type = model()->data(index, QModelChatMsg::Role_Type).toInt();
    auto size = model()->data(index, QModelChatMsg::Role_msgRc).toSize();
    QRect loadingRc;
    //auto m_textStateRightRect = QRect(m_kuangRightRect.left() - 25, m_textRightRect.y() + size.height() / 2 - 10, 20, 20);

    if (type == User_Me)
    {
        loadingRc.setLeft(rc.x() + rc.width() - size.width() - 81 - 35);
        loadingRc.setTop(rc.y() + 22 + size.height() / 2 - 10);
    }
    else
    {
        return;
    }

    int row = index.row();
    row = row % 20;
    auto label = m_loadingLabel[row];
    auto movie = label->movie();
    if(label->pos() != loadingRc.topLeft())
        label->move(loadingRc.topLeft());
    if(movie->state() == QMovie::NotRunning)
        movie->start();
    auto topIndex = indexAt(QPoint(0,0));
    auto bottomIndex = indexAt(QPoint(0,0 + height() - 5));
    int startRow = 0,endRow = 0;
    if(topIndex.isValid())
        startRow = topIndex.row();
    if(bottomIndex.isValid())
        endRow = bottomIndex.row() % 20;
    else
    {
        auto model = (QModelChatMsg*)this->model();
        endRow =( model->srcData().msgs.size() -1) % 20;
    }

//    qDebug() << startRow << endRow << index;
//    for (int n = startRow; n <= endRow; ++n) {
//        m_loadingLabel[n]->movie()->start();
//    }
//    for (int n = 0; n < startRow; ++n) {
//        m_loadingLabel[n]->movie()->stop();
//    }
//    for (int n = endRow; n < 20; ++n) {
//        m_loadingLabel[n]->movie()->stop();
//    }
}

void QListViewMsg::onStopIndexLoading(const QModelIndex &)
{

}

QRect QListViewMsg::getLoadingRc(const QModelIndex & index)
{
    auto rc = visualRect(index);
    auto type = model()->data(index, QModelChatMsg::Role_Type).toInt();
    auto size = model()->data(index, QModelChatMsg::Role_msgRc).toSize();
    QRect loadingRc;
    //auto m_textStateRightRect = QRect(m_kuangRightRect.left() - 25, m_textRightRect.y() + size.height() / 2 - 10, 20, 20);

    if (type == User_Me)
    {
        loadingRc.setLeft(rc.x() + rc.width() - size.width() - 81 - 35);
        loadingRc.setTop(rc.y() + 22 + size.height() / 2 - 10);
    }

    loadingRc.setSize(QSize(20,20));
    return loadingRc;
}
