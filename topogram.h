#ifndef TOPOGRAM_H
#define TOPOGRAM_H

#include <QLabel>
#include <QVBoxLayout>
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
    QVBoxLayout *pLayout;
    QLabel *pLabel;
    QImage* pImage;
    QPixmap *pPixmap;
};

#endif // TOPOGRAM_H
