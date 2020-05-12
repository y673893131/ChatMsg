#pragma once

#include <QListView>
#include <QWidget>
#include "./model/QModelChatMsg.h"
#include<QLabel>
class QMovie;
class QListViewMsg : public QListView
{
	Q_OBJECT
public:
    enum Msg_Flag
    {
        Msg_Normal,
        Msg_New,
        Msg_Html
    };
public:
	QListViewMsg(QWidget *parent = Q_NULLPTR, const QString& sObjName="");
    ~QListViewMsg();
	QString curCardNum();
    QSize fontSize(ChatMsg&, Msg_Flag nFlag = Msg_Normal);
    void setLeftTitle(const QString&);
    void sendMsg(const QString&, int, int sendType = Send_Successed, int flag = 0);
signals:
    void reset();
	void scrollTo(const QString&, int);
    void checkRunning(const QModelIndex&,const QModelIndex&);
    void stopView();
private:
    QSize getRealString(QString& src, int textWidth);
    QSize getRealStringNew(QString& src, int textWidth);
    QSize getRealStringHtml(QString& src, int textWidth);
    void resizeEvent(QResizeEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;

	virtual void keyPressEvent(QKeyEvent *event) override;
private slots:
    void onCheckRunning(const QModelIndex&);
    void onUpdate(const QModelIndex&);
private:
    void onSetIndexLoading(const QModelIndex&);
    void onStopIndexLoading(const QModelIndex&);
    QRect getLoadingRc(const QModelIndex&);
protected:
	QWidget* m_parent;
	QString m_sCurCardNum;
	QPoint m_p1, m_p2;
    QVector<QLabel*> m_loadingLabel;
    QVector<QMovie*> m_loadingMovie;
    QWidget* m_loading;
};
