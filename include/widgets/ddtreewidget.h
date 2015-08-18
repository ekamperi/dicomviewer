#ifndef DDTREEWIDGET_H
#define DDTREEWIDGET_H

#include <QTreeWidgetItem>

class DDTreeWidget : public QTreeWidget
{
    Q_OBJECT
public:
    explicit DDTreeWidget(QWidget *parent = 0);
    QTreeWidgetItem * getDraggedItem(void);

protected:
    Qt::DropActions supportedDropActions() const;
    void mouseDoubleClickEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);

signals:

public slots:

private:
    QPoint dragStartPosition;
    QTreeWidgetItem *pDraggedItem;

    QPixmap generatePixmap(const QTreeWidgetItem *pItem);
};

Q_DECLARE_METATYPE(QTreeWidgetItem)

#endif // DDTREEWIDGET_H
