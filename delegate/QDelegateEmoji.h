#pragma once

#include <QStyledItemDelegate>
#include <QFont>
class QDelegateMsg : public QStyledItemDelegate
{
	Q_OBJECT
public:
    QDelegateMsg(QObject *parent);
    ~QDelegateMsg();
	void setLineHeight(int);
	int lineHeight();
	void setFont(QFont);
	void setWidth(int);
	void paint(QPainter *painter,
		const QStyleOptionViewItem &option, const QModelIndex &index) const;

	int textMargin();
signals:
    void start(const QModelIndex&) const;

private:
	void drawSelect(QPainter *painter, QRect& rc, const QStyleOptionViewItem &option, QColor& color) const;
private:
	bool m_bBatchSelect, m_bRealName;
	int m_lineHeight, m_width, m_textMargin;
	QFont m_textFont;
    QPixmap m_leftPixmap, m_rightPixmap;
};
