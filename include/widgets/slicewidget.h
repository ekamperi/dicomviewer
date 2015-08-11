#ifndef SLICEWIDGET_H
#define SLICEWIDGET_H

#include <QWidget>
#include <QVBoxLayout>

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
    QHBoxLayout *pHLayout;
    QScrollBar *pScrollBar;

    HUWindows::window huWindow;

    struct SliceDirection { enum is { Prev, Next }; };
    void gotoSlice(SliceDirection::is dir);
    void gotoSlice(const Slice *pSlice);
    void gotoNextSlice(void);
    void gotoPrevSlice(void);

signals:
    void sliceChanged(int idx) const;
    void backToGridWidget(void) const;

private slots:
    void gotoSlice(int idx);
    void scrollBarValueChanged(int value);
    void changeWindow(HUWindows::window newWindow);
    void showContextMenu(const QPoint &pos);
};

#endif // SLICEWIDGET_H
