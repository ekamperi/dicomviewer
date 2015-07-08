#ifndef SLICEWIDGET_H
#define SLICEWIDGET_H

#include <QWidget>
#include "myglwidget.h"
class SliceWidget : public QWidget
{
    Q_OBJECT
public:
    explicit SliceWidget(QVector<Slice *> *pVecSlices, QWidget *parent = 0);
    ~SliceWidget();

    void loadSlices(QVector<Slice *> vecSlices);
    MyGLWidget *pGLWidget;

protected:
    bool event(QEvent *pEvent);
    void wheelEvent(QWheelEvent *event);

private:
    QVector<Slice *> *vecSlices;
    QWidget *pContainerWidget;

    struct SliceDirection { enum is { Prev, Next }; };
    void gotoSlice(SliceDirection::is dir);
    void gotoSlice(const Slice *pSlice);
    void gotoNextSlice(void);
    void gotoPrevSlice(void);

private slots:
    void gotoSlice(int idx);
};

#endif // SLICEWIDGET_H
