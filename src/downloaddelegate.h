#ifndef DOWNLOADDELEGATE_H_
#define DOWNLOADDELEGATE_H_

#include <QItemDelegate>
#include <QtGui>
#include <QtDebug>

class DownloadViewDelegate : public QItemDelegate
{
	Q_OBJECT
protected:
	DownloadViewDelegate(QObject * parent);
	virtual void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index );
};

#endif /*DOWNLOADDELEGATE_H_*/
