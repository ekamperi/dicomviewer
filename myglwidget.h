#ifndef MYGLWIDGET_H
#define MYGLWIDGET_H

#include <QtOpenGL>

#include "examdetails.h"
#include "geomtransformation.h"
#include "slice.h"
#include "topogram.h"

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
    Slice *getSlice(void) const;
    unsigned int getSliceIndex() const;

    void genTopogram(void);

    /* Geometric transformations (e.g., flip, etc) */
    void setGeomTransformation(Geometry::Transformation geomTransformation); 
    Geometry::Transformation getGeomTransformation(void) const;

    /* Measure distance in mm */
    void setDistanceMeasure(bool enabled) { this->measureDistance = enabled; }
    bool isDistanceMeasureEnabled(void) const { return this->measureDistance; }

    /* Measure density in Hounsfield units */
    void setDensityMeasure(bool enabled) { this->measureDensity = enabled; }
    bool isDensityMeasureEnabled(void) const { return this->measureDensity; }

signals:
    void windowChanged(int x);

private slots:
    void repaintSlice(float tmin, float tmax);
    void setTheTopogramFree(void);

protected:
    virtual void initializeGL();
    virtual void paintEvent(QPaintEvent *pEvent);
    virtual void resizeGL(int w, int h);

    void mouseMoveEvent(QMouseEvent *pEvent);
    void mousePressEvent (QMouseEvent *pEvent);
    void mouseReleaseEvent(QMouseEvent *pEvent);
    void wheelEvent(QWheelEvent *pEvent);

private:
    void drawDetails(QPainter *pPainter);
    void drawOutline(QPainter *pPainter);
    void drawCurrentDensity(QPainter *pPainter);
    void drawCurrentDistance(QPainter *pPainter);
    void drawDistances(QPainter *pPainter);

    void resetView(void);

    void getPointsInCircle(QVector<QPoint> *pVecPoints, QPoint centerPoint, float radius);

    int calcMeanDensity(int dist);
    unsigned int calcPhysicalDistance(QLine *pLine);

    /* A vector with all the lines corresponding to measured distances */
    QVector<QLine> vecDists;

    /* Whether we are measuring distance at the moment or not */
    bool measureDistance;
    bool measureDensity;
    QPoint startPoint;  /* Start point of measurement */
    QPoint endPoint;    /* End point of measurement */

    /* Shader related variables */
    QGLShader *pVertexShader;
    QGLShader *pFragmentShader;
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

    /* Scale factor used to zoom in/out */
    float scaleFactor;

    /* Current geometric transformation, such as as flip (if any) */
    Geometry::Transformation geomTransformation;

    //Magick::Image *pMagickImage;
    //Magick::Blob blob;

    /* Pointer to a dynamically allocated array with texture IDs (each slice
     * corresponds to one texture ID).
     */
    GLuint *pTexIDs;

    /* Currently displayed slice */
    Slice *pSlice;

    /*
     * A vector containing (pointers to) all slices managed by this widget.
     * Initially, 1 GL Widget would handle 1 slice, but this was super inefficient
     * due to many opengl contexts being created.
     */
    QVector<Slice *> vecSlices;

    Topogram *pTopogram;

    /* OpenGL matrices */
    QMatrix4x4 projectionMatrix;
    QMatrix4x4 viewMatrix;
    QMatrix4x4 modelMatrix;
};

#endif // MYGLWIDGET_H
