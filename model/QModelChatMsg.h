#pragma once

#include <QColor>
#include <QIcon>
#include <qset.h>
#include <QTimer>
#include <QAbstractListModel>

enum User_Type{
    User_System = 0,//系统
    User_Me,        //自己
    User_Other,     //其他
    User_Time,      //时间
};

enum Msg_Send_State
{
    Send_ing,
    Send_Successed,
    Send_Failed
};

typedef struct _ChatMsg_
{
    _ChatMsg_()
        :nTm(0), nCashierId(0), nIndex(0), nRunning(0),bIsHtml(false)
        , msgCt(0), state(Send_ing),  nSelectBegPos(0), nSelectEndPos(0)
    {
    }

    uint nTm;
    User_Type type;
    int nCashierId;
    int nIndex;
    int nRunning;
    long msgCt;
    Msg_Send_State state;
    bool bIsHtml;
    QSize textSize;
    QSize rcSize;
    QString sTitle;
    QString msg;
    int nSelectBegPos;
    int nSelectEndPos;
    QVector<QRect> selected;
}ChatMsg;

typedef struct _User_Info_
{
    _User_Info_()
        :nProcess(0)
    {
    }

    QString sTitle;
    QString sUserName;
    QColor color;
    int nProcess;
    QVector<ChatMsg> msgs;
}User_Info;

class QListViewMsg;

class QModelChatMsg : public QAbstractListModel
{
	Q_OBJECT
public:
	enum Data_role
	{
		Role_Icon_Back = Qt::UserRole + 1,
		Role_Type,
		Role_Title_Text,
		Role_Time,
		Role_msg,
		Role_msgRc,
        Role_State,
        Role_Running,
        Role_IsHtml
	};
public:
    QModelChatMsg(QObject *parent);
    ~QModelChatMsg();

	int rowCount(const QModelIndex &parent = QModelIndex()) const;
	int columnCount(const QModelIndex &parent = QModelIndex()) const;
	QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

	QModelIndex index(int row, int column = 0, const QModelIndex &parent = QModelIndex()) const;
	QModelIndex sibling(int row, int column, const QModelIndex &idx) const;

    User_Info& srcData();
signals:
    void update(const QModelIndex&);
public slots:
    void onReset();
    void sendMsg(const ChatMsg& msg);
    void onCheckRunning(const QModelIndex&, const QModelIndex&);
    void onStopView();
private slots:
    void onRunning();
private:
    bool checkTime(uint tm);
    bool checkHtml(const ChatMsg& msg);
private:
    User_Info m_data;
    QTimer* m_timerRun;
    QModelIndex m_top,m_bottom;
};
