#ifndef MYGLWIDGET_H
#define MYGLWIDGET_H

#include <QtOpenGL>

#include "examdetails.h"
#include "slice.h"

//#include "Magick++.h"

class Slice;

class MyGLWidget : public QGLWidget
{
    Q_OBJECT

public:
    explicit MyGLWidget(QWidget *parent = 0);
     ~MyGLWidget();

     void setSlice(Slice *pSlice);
     const Slice * getSlice() const;
     unsigned int getSliceIndex() const;

     //
     void setDistanceMeasure(bool enabled) { this->measureDistance = enabled; }
     bool isDistanceMeasureEnabled(void) const { return this->measureDistance; }

signals:
    void windowChanged(int x);

protected:
     virtual void initializeGL();
     virtual void resizeGL(int w, int h);
     //virtual void paintGL();

    void mousePressEvent (QMouseEvent * event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void paintEvent(QPaintEvent *event);

private:
     void loadTexture(float *pRawData, unsigned int width, unsigned int height, GLint format);
     void drawDetails(QPainter *pPainter);
     void drawOutline(QPainter *pPainter);
     void drawCurrentDistance(QPainter *painter);
     void drawDistances(QPainter *painter);
     unsigned int calcPhysicalDistance(QLine *pLine);

     //
     QVector<QLine> vecDists;

     //
     bool measureDistance;
    QPoint startPoint;
    QPoint endPoint;

     //
     QGLShaderProgram *pProgram;

     // XXX
     float tmin;
     float tmax;

     /* Custom stuff */
     void png2raw(QString string);
     int texWidth;
     int texHeight;

     //Magick::Image *pMagickImage;
     //Magick::Blob blob;

     GLuint textureID;
     float *pRawPixel;

     Slice *pSlice;
};

#endif // MYGLWIDGET_H
