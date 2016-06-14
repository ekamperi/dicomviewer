#ifndef EXAMDETAILSWIDGET_H
#define EXAMDETAILSWIDGET_H

#include <QLabel>
#include <QVBoxLayout>
#include <QWidget>

#include "examdetails.h"
#include "selectable.h"

class ExamDetailsWidget : public QWidget, public Selectable
{
    Q_OBJECT
public:
    explicit ExamDetailsWidget(const ExamDetails *e, QWidget *parent = 0);
    ~ExamDetailsWidget();

    void setExamDetails(const ExamDetails *pExamDetails);

protected:
    virtual void mousePressEvent(QMouseEvent *pEvent);
    virtual void mouseReleaseEvent(QMouseEvent *pEvent);
    virtual void paintEvent(QPaintEvent *pEvent);

signals:
    void iWantToHide();

private slots:
    void showContextMenu(const QPoint &pos);

private:
    const ExamDetails *pExamDetails;
};

#endif // EXAMDETAILSWIDGET_H
