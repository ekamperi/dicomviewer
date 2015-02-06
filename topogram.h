#ifndef TOPOGRAM_H
#define TOPOGRAM_H

#include <QLabel>
#include <QWidget>

class Topogram : public QWidget
{
    Q_OBJECT
public:
    explicit Topogram(QWidget *parent = 0);
    explicit Topogram(float *pData, int width, int height, QWidget *parent = 0);
    ~Topogram();

protected:
    virtual void resizeEvent(QResizeEvent *pEvent);

signals:

public slots:

private:
    QLabel *pLabel;
};

#endif // TOPOGRAM_H
