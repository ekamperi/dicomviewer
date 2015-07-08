#ifndef GRIDWIDGET_H
#define GRIDWIDGET_H

#include <QEvent>
#include <QScrollArea>

#include "flowlayout.h"
#include "slice.h"

class GridWidget : public QScrollArea
{
    Q_OBJECT
public:
    explicit GridWidget(QWidget *parent = 0);
    void addSlices(const QVector<Slice *> &vecSlices);
    void selectAllSlices(void);

protected:
    bool event(QEvent *pEvent);

private:
    QWidget *pContainerWidget;
    FlowLayout *flowLayout;

    const QVector<Slice *> *pVecSlices;

signals:
    void sliceDoubleClicked(const Slice *pSlice);
};

#endif // GRIDWIDGET_H
