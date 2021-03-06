#include "include/widgets/ddtreewidget.h"

#include <QApplication>
#include <QDebug>
#include <QDrag>
#include <QMimeData>
#include <QMouseEvent>
#include <QPainter>

#include "patient.h"
#include "series.h"
#include "study.h"

DDTreeWidget::DDTreeWidget(QWidget *parent) :
    QTreeWidget(parent)
{
    qDebug() << Q_FUNC_INFO;
}

Qt::DropActions DDTreeWidget::supportedDropActions() const
{
    qDebug() << Q_FUNC_INFO;
    return Qt::MoveAction;
}

void DDTreeWidget::mouseDoubleClickEvent(QMouseEvent *event)
{
    qDebug() << Q_FUNC_INFO;
    QTreeWidget::mouseDoubleClickEvent(event);
}

void DDTreeWidget::mousePressEvent(QMouseEvent *event)
{
    qDebug() << Q_FUNC_INFO;

    if (event->button() == Qt::LeftButton) {
        this->dragStartPosition = event->pos();
    }
    QTreeWidget::mousePressEvent(event);
}

void DDTreeWidget::mouseMoveEvent(QMouseEvent *event)
{
    /* Drag only with the left mouse button */
    if (!(event->buttons() & Qt::LeftButton)) {
        return;
    }

    /* Initiate drag only when the mouse has covered some minimum distance */
    if ((event->pos() - dragStartPosition).manhattanLength()
         < QApplication::startDragDistance()) {
        return;
    }

    /* Drag only if there's actually an item selected */
    this->pDraggedItem = this->selectedItems().at(0);
    if (!this->pDraggedItem) {
        return;
    }

    QMimeData *mimeData = new QMimeData;
    mimeData->setText("start");

    QDrag *drag = new QDrag(this);
    drag->setMimeData(mimeData);

    /* Generate a pixmap */
    QPixmap pixmap = this->generatePixmap(this->pDraggedItem);
    drag->setPixmap(pixmap);

    drag->setHotSpot(QPoint(drag->pixmap().width()/2,
                            drag->pixmap().height()));

    Qt::DropAction dropAction;
    dropAction = drag->exec(Qt::MoveAction);
}

QTreeWidgetItem * DDTreeWidget::getDraggedItem(void)
{
    qDebug() << Q_FUNC_INFO;
    QTreeWidgetItem *pItem = this->pDraggedItem;
    this->pDraggedItem = NULL;
    return pItem;
}

QPixmap DDTreeWidget::generatePixmap(const QTreeWidgetItem *pItem)
{
    qDebug() << Q_FUNC_INFO;
    Q_ASSERT(pItem);

    /* Get patient name, study and series to construct a useful
     * thumbnail while dragging
     */
    Patient patient;
    Study study;
    Series series;
    series  = pItem->data(0, Qt::UserRole).value<Series>();
    study   = pItem->parent()->data(0, Qt::UserRole).value<Study>();
    patient = pItem->parent()->parent()->data(0, Qt::UserRole).value<Patient>();

    /* Calculate the bounding rectangle needed to display the text */
    QFont font("Arial", 11);
    QFontMetrics fontMetrics =
            QFontMetrics(font);
    QString text =
            patient.getName() + "\n" +
            study.getDesc() + "\n" +
            series.getDesc();
    QRect boundingRect =
            fontMetrics.boundingRect(
                0,0,350,0,Qt::AlignLeft | Qt::TextWordWrap,
                text);

    /* Add some margins around the bounding rect */
    QRect outerRect = boundingRect.marginsAdded(QMargins(10,10,10,10));
    outerRect.moveTopLeft(QPoint(0,0));
    QRect redRect = boundingRect.marginsAdded(QMargins(5,5,5,5));
    redRect.moveTopLeft(QPoint(5,5));
    boundingRect.moveTopLeft(QPoint(10,10));

    /* Generate the actual pixmap */
    QPixmap pixmap(outerRect.size());
    pixmap.fill(QColor("#DEDEDE"));
    QPainter painter(&pixmap);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::HighQualityAntialiasing);
    painter.setFont(font);
    painter.drawText(boundingRect, Qt::AlignLeft | Qt::TextWordWrap, text, &boundingRect);

    /* Draw a red border around the pixmap */
    QPen p;
    p.setWidth(2);
    p.setColor(Qt::red);
    painter.setPen(p);
    painter.drawRoundedRect(redRect,5,5);

    return pixmap;
}
