#ifndef QEMOJIMODEL_H
#define QEMOJIMODEL_H

#include <QAbstractTableModel>
#include <QMovie>

struct Userdata : QObjectUserData
{
    Userdata(int row, int col)
        :row(row),col(col)
    {
    }
    int row;
    int col;
};

class QEmojiModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    explicit QEmojiModel(QObject *parent = nullptr);

    // Header:
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    void setPlay(bool bPlay);
signals:
    void update(const QModelIndex&);
private slots:
    void updateIndex(int);
private:
    QVector<QVector<QMovie*>> m_movies;
};

#endif // QEMOJIMODEL_H
