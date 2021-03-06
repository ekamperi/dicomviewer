#ifndef MYGLWIDGET_H
#define MYGLWIDGET_H

#include <QOpenGLWidget>
#include <QGLShader>

#include "distline.h"
#include "denscircle.h"
#include "examdetails.h"
#include "include/widgets/examdetailswidget.h"
#include "geomtransformation.h"
#include "slice.h"
#include "topogram.h"

//#include "Magick++.h"

class Slice;

class MyGLWidget : public QOpenGLWidget
{
    Q_OBJECT

public:
    explicit MyGLWidget(QWidget *parent = 0);
    ~MyGLWidget();

    void resetView(void);

    void loadSlices(QVector<Slice *> vecSlices);
    void setSlice(Slice *pSlice);
    Slice *getSlice(void) const;
    unsigned int getSliceIndex(void) const;

    void genExamDetails(void);
    void genTopogram(float angle);

    /* Geometric transformations (e.g., flip, etc) */
    void setGeomTransformation(Geometry::Transformation geomTransformation);
    Geometry::Transformation getGeomTransformation(void) const;

    /* Pan mode */
    void setPanMode(bool enabled) { this->panMode = enabled; }
    bool isPanMode(void) const { return this->panMode; }

    /* Measure distance in mm */
    void setDistanceMeasure(bool enabled) { this->measureDistance = enabled; }
    bool isDistanceMeasureEnabled(void) const { return this->measureDistance; }

    /* Measure density in Hounsfield units */
    void setDensityMeasure(bool enabled) { this->measureDensity = enabled; }
    bool isDensityMeasureEnabled(void) const { return this->measureDensity; }

    /* Delete all/selected measures */
    void deleteAllMeasures(void);
    void deleteSelectedMeasures(void);

signals:
    void windowChanged(int x);
    void sliceChanged(int newSliceIndex);

public slots:
    void changeWindow(HUWindows::window newWindow);

private slots:
    void undockTopogram(void);
    void dockTopogram(void);
    void repaintSlice(float tmin, float tmax);

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
    void drawDensities(QPainter *pPainter);
    void drawDistance(QPainter *pPainter, const DistLine &line);
    void drawDensity(QPainter *painter, const DensCircle &circle);
    void drawDebug(QPainter *pPainter);

    void resetViewMatrix(void);

    int calcMeanDensity(QPoint centerPoint, float radius);
    unsigned int calcPhysicalDistance(const QLine &line);

    /* A vector with all the lines corresponding to measured distances */
    QVector<DistLine *> vecDists;

    /* A vector with all the circles corresponding to measured densities */
    QVector<DensCircle *> vecDensities;

    /* Whether we are measuring distance at the moment or not */
    bool measureDistance;
    bool measureDensity;
    QPoint startPoint;  /* Start point of measurement */
    QPoint endPoint;    /* End point of measurement */

    bool panMode;

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
    float oldScaleFactor;
    float offsetX, offsetY;
    float oldOffsetX, oldOffsetY;

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

    /*
     * Examination Details Widget
     * Topogram
     */
    ExamDetailsWidget *pExamDetailsWidget;
    Topogram *pTopogram;

    /* OpenGL matrices */
    QMatrix4x4 projectionMatrix;
    QMatrix4x4 viewMatrix;
    QMatrix4x4 modelMatrix;

    /* Whether debug mode is on or off */
    bool debugMode;
};

#endif // MYGLWIDGET_H
