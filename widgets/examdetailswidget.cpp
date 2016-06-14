#include "include/helpers/mymath.h"
#include "include/widgets/examdetailswidget.h"

#include <math.h>
#include <QtGlobal>
#include <QDebug>
#include <QMenu>
#include <QMouseEvent>
#include <QPainter>
#include <QVBoxLayout>

ExamDetailsWidget::ExamDetailsWidget(const ExamDetails *e, QWidget *parent)
    : QWidget(parent)
{
    qDebug() << Q_FUNC_INFO;

    Q_ASSERT(e);
    this->pExamDetails = e;
}

ExamDetailsWidget::~ExamDetailsWidget()
{
}

void ExamDetailsWidget::setExamDetails(const ExamDetails *e)
{
    Q_ASSERT(e);
    this->pExamDetails = e;
    this->update();
}

void ExamDetailsWidget::mousePressEvent(QMouseEvent *pEvent)
{
    if (pEvent->buttons() & Qt::LeftButton) {
        pEvent->accept();
        this->setSelection(!this->isSelected());
        this->update();
    }
    if (pEvent->buttons() & Qt::RightButton) {
        pEvent->accept();
    }
}

void ExamDetailsWidget::mouseReleaseEvent(QMouseEvent *pEvent)
{
    /*
     * Change slice if and only if user clicked with *left* button.
     * Because s/he may be releasing the right button while changing
     * window/width.
     *
     * From the Qt docs regarding the QMouseEvent::buttons() function:
     * "... For mouse release events this *excludes* the button that caused
     * the event."
     *
     * Whereas for QMouseEvent::button():
     * "... Returns the button that caused the event."
     *
     * For the above reason, use ::button() instead of ::buttons() here.
     */
    if (pEvent->button() == Qt::LeftButton) {
    } else if (pEvent->button() == Qt::RightButton) {
        this->showContextMenu(pEvent->pos());
    }
}

void ExamDetailsWidget::showContextMenu(const QPoint &pos)
{
    QPoint globalPos = this->mapToGlobal(pos);
    QMenu ctxMenu;

    QAction *pHideAction;
    pHideAction = ctxMenu.addAction("Hide examination details");

    QAction *pSelectedItem = ctxMenu.exec(globalPos);
    if (pSelectedItem == pHideAction) {
        emit iWantToHide();
    }
}

void ExamDetailsWidget::paintEvent(QPaintEvent *pEvent)
{
    QPainter painter;

    painter.begin(this);

    /* Setup pen. Yellow on black background. */
    QFont font = painter.font();
    font.setPointSize(10);
    painter.setFont(font);
    painter.setPen(Qt::yellow);

    /* Draw generic examination details (patient name, age, sex, etc) */
    painter.drawText(
                QRect(5, 5, this->width(), this->height()),
                Qt::TextWordWrap,
                this->pExamDetails->getGenericDetails());

    /* Draw specific examination details (e.g. KVp, mAs in CT) */
    painter.drawText(
                QRect(5, 5 + this->height() - 100,
                      this->width(), this->height()),
                Qt::TextWordWrap,
                this->pExamDetails->getSpecificDetails(Exam::CT));

    if (this->isSelected()) {
        QPen oldPen, myPen;
        oldPen = painter.pen();
        myPen.setWidth(2);
        myPen.setStyle(Qt::DashLine);
        myPen.setColor(Qt::red);
        painter.setPen(myPen);
        painter.drawRect(QRect(0, 0, this->width() - 2, this->height() - 2));
        painter.setPen(oldPen);
    }
    painter.end();
}
