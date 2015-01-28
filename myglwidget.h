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

    void loadSlices(QVector<Slice *> vecSlices);
    void setSlice(Slice *pSlice);
    const Slice *getSlice() const;
    unsigned int getSliceIndex() const;

    void setDistanceMeasure(bool enabled) { this->measureDistance = enabled; }
    bool isDistanceMeasureEnabled(void) const { return this->measureDistance; }

signals:
    void windowChanged(int x);

protected:
    virtual void initializeGL();
    virtual void paintEvent(QPaintEvent *pEvent);
    virtual void resizeGL(int w, int h);

    void mouseMoveEvent(QMouseEvent *pEvent);
    void mousePressEvent (QMouseEvent *pEvent);
    void mouseReleaseEvent(QMouseEvent *pEvent);

private:
    void drawDetails(QPainter *pPainter);
    void drawOutline(QPainter *pPainter);
    void drawCurrentDistance(QPainter *pPainter);
    void drawDistances(QPainter *pPainter);
    unsigned int calcPhysicalDistance(QLine *pLine);

    /* A vector with all the lines corresponding to measured distances */
    QVector<QLine> vecDists;

    /* Whether we are measuring distance at the moment or not */
    bool measureDistance;
    QPoint startPoint;  /* Start point of measurement */
    QPoint endPoint;    /* End point of measurement */

    QGLShaderProgram *pProgram;

    /* The min and max values in our tranfer function, corresponding to min and
     * max pixel luminance (range: 0.0 - 1.0). These values will be passed to
     * the fragment shader.
     */
    float tmin;
    float tmax;

    /* Custom stuff */
    void png2raw(QString string);
    int texWidth;
    int texHeight;

    //Magick::Image *pMagickImage;
    //Magick::Blob blob;

    /* Pointer to a dynamically allocated array with texture IDs (each slice
     * corresponds to one texture ID).
     */
    GLuint *pTexIDs;

    /* Currently displayed slice */
    Slice *pSlice;

    /* A vector containing (pointers to) all slices managed by this widget */
    QVector<Slice *> vecSlices;
};

#endif // MYGLWIDGET_H
