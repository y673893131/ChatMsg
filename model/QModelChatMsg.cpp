#include "QModelChatMsg.h"
#include <qapplication.h>
#include <QDateTime>
#include <QDebug>
QModelChatMsg::QModelChatMsg(QObject *parent)
	: QAbstractListModel(parent)
{
    m_timerRun = new QTimer(this);
    m_timerRun->setInterval(100);
    connect(m_timerRun, SIGNAL(timeout()), this ,SLOT(onRunning()));
}

QModelChatMsg::~QModelChatMsg()
{
}

int QModelChatMsg::rowCount(const QModelIndex &parent /*= QModelIndex()*/) const
{
    Q_UNUSED(parent)
	return m_data.msgs.size();
}

int QModelChatMsg::columnCount(const QModelIndex &parent /*= QModelIndex()*/) const
{
    Q_UNUSED(parent)
	return 1;
}

QVariant QModelChatMsg::data(const QModelIndex &index, int role /*= Qt::DisplayRole*/) const
{
	if (!index.isValid()) return QVariant();
	auto row = index.row();
	switch (role)
	{
	case Role_Icon_Back:
		return m_data.color;
	case Role_Type:
		if (m_data.msgs.size() > row) return m_data.msgs[row].type;
		break;
	case Role_Title_Text:
        if (m_data.msgs.size() > row) return m_data.sTitle;
		break;
	case Role_Time:
        if (m_data.msgs.size() > row) return QDateTime::fromTime_t(m_data.msgs[row].nTm).toString("HH:mm");
		break;
	case Qt::SizeHintRole:
		if (m_data.msgs.size() > row) return m_data.msgs[row].rcSize;
		break;
	case Role_msgRc:
		if (m_data.msgs.size() > row) return m_data.msgs[row].textSize;
		break;
	case Role_msg:
		if (m_data.msgs.size() > row) return m_data.msgs[row].msg;
		break;
	case Role_State:
		if (m_data.msgs.size() > row)
		{
			if (m_data.msgs[row].nIndex <= 0)
				return m_data.msgs[row].state;
			else
				return Send_Successed;
		}
		break;
    case Role_Running:
        if (m_data.msgs.size() > row) return m_data.msgs[row].nRunning;
        break;
    case Role_IsHtml:
        if (m_data.msgs.size() > row) return m_data.msgs[row].bIsHtml;
        break;
	default:
		break;
	}

	return QVariant();
}

QModelIndex QModelChatMsg::index(int row, int column /*= 0*/, const QModelIndex &parent /*= QModelIndex()*/) const
{
	if (parent.isValid() || row >= m_data.msgs.size())
		return QModelIndex();

	return createIndex(row, column);
}

QModelIndex QModelChatMsg::sibling(int row, int column, const QModelIndex &idx) const
{
	if (!idx.isValid() || row >= m_data.msgs.size())
		return QModelIndex();

    return createIndex(row, column);
}

User_Info &QModelChatMsg::srcData()
{
    return m_data;
}

void QModelChatMsg::onCheckRunning(const QModelIndex &top, const QModelIndex &bottom)
{
    m_top = top;
    m_bottom = bottom;
    if(!bottom.isValid())
    {
        for(auto it : m_data.msgs)
        {
            if(it.type == User_Me && it.state == Send_ing)
            {
                if(!m_timerRun->isActive()) m_timerRun->start();
                return;
            }
        }

        if(m_timerRun->isActive()) m_timerRun->stop();
        return;
    }

    int start = top.row();
    int end = qMin(bottom.row(),(m_data.msgs.size() - 1));
    for(auto row = start; row <= end; ++row)
    {
        if(m_data.msgs[row].type == User_Me && m_data.msgs[row].state == Send_ing)
        {
            if(!m_timerRun->isActive()) m_timerRun->start();
            return;
        }
    }

    if(m_timerRun->isActive()) m_timerRun->stop();
}

void QModelChatMsg::onStopView()
{
    if(m_timerRun->isActive())
        m_timerRun->stop();
}

void QModelChatMsg::sendMsg(const ChatMsg& msg)
{
    checkTime(msg.nTm);
    m_data.msgs.push_back(msg);
    emit layoutChanged();
}

void QModelChatMsg::onRunning()
{
    bool bRunning = false;
    int start = qMax(m_top.row(), 0);
    int end = 0;
    if(!m_bottom.isValid())
        end = m_data.msgs.size() - 1;
    else
        end = qMin(m_bottom.row(),(m_data.msgs.size() - 1));

    for(auto row = end; row >= start; --row)
    {
        if(m_data.msgs[row].type == User_Me && m_data.msgs[row].state == Send_ing)
        {
            m_data.msgs[row].nRunning = (m_data.msgs[row].nRunning + 30) % 360;
            bRunning = true;
//            qDebug() << "index(row)" << index(row);
            emit update(index(row));
        }
    }

    if(!bRunning)
    {
        if(m_timerRun->isActive()) m_timerRun->stop();
    }
}

bool QModelChatMsg::checkTime(uint tm)
{
    bool isShowTime = false;
    if (m_data.msgs.size() > 0)
        isShowTime = ((tm - m_data.msgs.rbegin()->nTm) > 60); // 两个消息相差一分钟
    else
        isShowTime = true;

    if (isShowTime) {
        ChatMsg msg;
        msg.nTm = tm; //时间戳
        msg.type = User_Time;
        msg.rcSize = QSize(200, 40);
        m_data.msgs.push_back(msg);
        return true;
    }

    return false;
}

void QModelChatMsg::onReset()
{
    m_data = User_Info();
    emit layoutChanged();
}
