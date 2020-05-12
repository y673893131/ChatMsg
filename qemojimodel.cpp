#include "qemojimodel.h"
#include <QSize>
#include <QFileInfo>
#include <QPixmap>

QEmojiModel::QEmojiModel(QObject *parent)
    : QAbstractTableModel(parent)
{
    for(int row = 1; row < 2; ++row)
    {
        QVector<QMovie*> list;
        for(int col = 1; col < 13; ++col)
        {
            QString s = QString("E:\\qt\\ChatMsg\\res\\%1_%2.gif").arg(row).arg(col);
            QFileInfo info(s);
            if(!info.exists()) break;
            QMovie* movie = new QMovie(s);
            Userdata* user = new Userdata(row - 1, col - 1);
            movie->setUserData(1, user);
            movie->setCacheMode(QMovie::CacheNone);
            movie->start();
//            movie->setPaused(true);
            list << movie;
            connect(movie, SIGNAL(frameChanged(int)), this, SLOT(updateIndex(int)));
        }

        if(list.isEmpty()) continue;
        m_movies.push_back(list);
    }
}

QVariant QEmojiModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    // FIXME: Implement me!
    return QVariant();
}

int QEmojiModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    // FIXME: Implement me!
    return m_movies.size();
}

int QEmojiModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    // FIXME: Implement me!
    return  12;
}

QModelIndex QEmojiModel::index(int row, int column, const QModelIndex &parent) const
{
    if(row < 0 || column < 0) return  QModelIndex();

    return createIndex(row, column);
}

QVariant QEmojiModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    // FIXME: Implement me!
    switch (role) {
    case Qt::SizeHintRole:
        return QSize(28,28);
    case Qt::DisplayRole:
        if(m_movies.size() > index.row() && m_movies[index.row()].size() > index.column())
            return QVariant::fromValue<void *>(m_movies[index.row()][index.column()]);
        break;
    }

    return QVariant();
}

void QEmojiModel::setPlay(bool bPlay)
{
    for(auto row = m_movies.begin(); row != m_movies.end(); ++row){
        for(auto it = row->begin(); it != row->end(); ++it){
            if((*it)->state() != QMovie::Running)
               (*it)->setPaused(!bPlay);
        }
    }
}

void QEmojiModel::updateIndex(int n)
{
    QMovie* movie = qobject_cast<QMovie*>(sender());
    auto data = reinterpret_cast<Userdata*>(movie->userData(1));
    emit update(index(data->row, data->col));
}
