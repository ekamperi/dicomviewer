#ifndef GRIDWIDGET_H
#define GRIDWIDGET_H

#include <QEvent>
#include <QScrollArea>

#include "flowlayout.h"
#include "slice.h"

class GridWidget : public QWidget
{
    Q_OBJECT
public:
    explicit GridWidget(QWidget *parent = 0);
    void addSlices(const QVector<Slice *> &vecSlices);
    void selectAllSlices(void);

protected:
    bool event(QEvent *pEvent);

private:
    QVBoxLayout *pVLayout;
    QScrollArea *pScrollArea;
    QWidget *pContainerWidget;
    FlowLayout *pFlowLayout;

    const QVector<Slice *> *pVecSlices;

signals:
    void sliceDoubleClicked(const Slice *pSlice);

private slots:
    void changeWindow(HUWindows::window newWindow);
};

#endif // GRIDWIDGET_H
