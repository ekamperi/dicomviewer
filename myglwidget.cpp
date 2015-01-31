#include "examdetails.h"
#include "slice.h"
#include "myglwidget.h"

#include <iostream>
#include <QDebug>
#include <QMouseEvent>
#include <QtGlobal>
#include <QtOpenGL/QGLWidget>

MyGLWidget::MyGLWidget(QWidget *parent) :
    QGLWidget(QGLFormat(QGL::SampleBuffers), parent)
{
    qDebug() << Q_FUNC_INFO;

    //pMagickImage = NULL;
    setMouseTracking(true);

    this->measureDistance = false;

    this->tmin = 0.0;
    this->tmax = 0.4;
}

MyGLWidget::~MyGLWidget()
{
    qDebug() << Q_FUNC_INFO;
//    if (this->pMagickImage) {
//        delete this->pMagickImage;
//        this->pMagickImage = NULL;
//    }
//    Q_ASSERT(this->pProgram);
//    delete this->pProgram;
    /* Don't forget to delete the textures */
    this->makeCurrent();
    glDeleteTextures(this->vecSlices.size(), this->pTexIDs);
    Q_ASSERT(glGetError() == GL_NO_ERROR);
}

void MyGLWidget::setSlice(Slice *pSlice)
{
    Q_ASSERT(pSlice);
    this->pSlice = pSlice;

    /* Force a redraw */
    this->update();
}

void MyGLWidget::loadSlices(QVector<Slice *> vecSlices)
{
    qDebug() << Q_FUNC_INFO;

    /* From the QGLWidget official documentation:
     *
     * If you need to call the standard OpenGL API functions from other places
     * (e.g. in your widget's constructor or in your own paint functions), you
     * must call makeCurrent() first.
     *
     * makeCurrent() makes this widget the current widget for OpenGL operations,
     * i.e. makes the widget's rendering context the current OpenGL rendering
     * context.
     * DO NOT REMOVE THIS CALL.
     */
    this->makeCurrent();

    glEnable(GL_TEXTURE_2D);
    Q_ASSERT(glGetError() == GL_NO_ERROR);

    /* Obtain ids for the textures */
    int nTextures = vecSlices.size();
    if (nTextures < 1) {
        qDebug() << "nTextures < 1";
        return;
    }
    this->pTexIDs = (GLuint *) malloc(nTextures * sizeof(GLuint));
    Q_ASSERT(this->pTexIDs);

    glGenTextures(nTextures, this->pTexIDs);
    Q_ASSERT(glGetError() == GL_NO_ERROR);

    for (int i = 0; i < vecSlices.size(); i++) {
        /* Select texture */
        glBindTexture(GL_TEXTURE_2D, this->pTexIDs[i]);
        Q_ASSERT(glGetError() == GL_NO_ERROR);

        /* Select slice */
        Slice *pSlice = vecSlices.at(i);
        Q_ASSERT(pSlice);

        glTexImage2D(GL_TEXTURE_2D, 0,
                     pSlice->getFormat(),
                     pSlice->getWidth(),
                     pSlice->getHeight(), 0, GL_LUMINANCE, GL_FLOAT,
                     pSlice->getRawPixelData());
        Q_ASSERT(glGetError() == GL_NO_ERROR);

        /* Set texture stretching parameters */
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        Q_ASSERT(glGetError() == GL_NO_ERROR);
    }

    glDisable(GL_TEXTURE_2D);
    Q_ASSERT(glGetError() == GL_NO_ERROR);
}

void MyGLWidget::png2raw(QString filename)
{/*
    try {
        pMagickImage = new Magick::Image(filename.toStdString().c_str());
        pMagickImage->write(&this->blob, "RGB");

        this->pRawPixel = (float*)this->blob.data();
        this->texWidth  = this->pMagickImage->columns();
        this->texHeight = this->pMagickImage->rows();
    } catch (Magick::Exception &error) {
        qDebug() << "Caught exception: " << error.what();
        this->pMagickImage = NULL;
    }*/
}

void MyGLWidget::initializeGL()
{
    glEnable(GL_TEXTURE_2D);
    Q_ASSERT(glGetError() == GL_NO_ERROR);

    /* Construct the shaders */
    this->pProgram = new QGLShaderProgram(this);
    Q_ASSERT(this->pProgram);

    bool rv;
    rv = this->pProgram->addShaderFromSourceFile(QGLShader::Vertex, "./vertex.sh");
    Q_ASSERT(rv);
    rv = this->pProgram->addShaderFromSourceFile(QGLShader::Fragment, "./fragment.sh");
    Q_ASSERT(rv);

    rv = this->pProgram->link();
    Q_ASSERT(rv);
}

void MyGLWidget::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, 1, 1, 0, 0, 1);
    glMatrixMode(GL_MODELVIEW);
}

void MyGLWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter;
    painter.begin(this);

    /* This is equivalent to calling glUseProgram() */
    Q_ASSERT(this->pProgram);
    bool rv = this->pProgram->bind();
    Q_ASSERT(rv);

    /* Setup the min, max values for transfer function */
    int tmin_loc = this->pProgram->uniformLocation("tmin");
    int tmax_loc = this->pProgram->uniformLocation("tmax");
    Q_ASSERT(tmin != -1);
    Q_ASSERT(tmax != -1);
    this->pProgram->setUniformValue(tmin_loc, this->tmin);
    this->pProgram->setUniformValue(tmax_loc, this->tmax);

    /* Ready to do the drawing */
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.0, 0.0, 0.0, 0.0);

    /* Bind again (besides setSlice() */
    glBindTexture(GL_TEXTURE_2D, this->pTexIDs[this->pSlice->getIndex()]);
    Q_ASSERT(glGetError() == GL_NO_ERROR);

    glBegin(GL_QUADS);
        glTexCoord2d(0.0, 0.0); glVertex2d(0.0, 0.0);
        glTexCoord2d(1.0, 0.0); glVertex2d(1.0, 0.0);
        glTexCoord2d(1.0, 1.0); glVertex2d(1.0, 1.0);
        glTexCoord2d(0.0, 1.0); glVertex2d(0.0, 1.0);
    glEnd();

    this->pProgram->release();

    /* Proceed with native drawing */
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::TextAntialiasing);
    painter.setRenderHint(QPainter::HighQualityAntialiasing);

    /* Draw examination details (patient name, ID, age, sex) */
    this->drawDetails(&painter);

    /* Draw any measuring distances */
    this->drawDistances(&painter);
    if (this->measureDistance) {
        this->drawCurrentDistance(&painter);
    }

    /* Draw any measuring densities */
    if (this->measureDensity) {
        this->drawCurrentDensity(&painter);
    }

    painter.end();
}

void MyGLWidget::drawDetails(QPainter *pPainter)
{
    Q_ASSERT(pPainter);

    QFont font = pPainter->font();
    font.setPointSize(12);
    pPainter->setFont(font);
    pPainter->setPen(Qt::yellow);
    ExamDetails examDetails = this->pSlice->getExamDetails();

    pPainter->drawText(
                QRect(5, 5, this->width(), this->height()),
                Qt::TextWordWrap,
                  "Name: " + examDetails.getPatientName()
                + ", ID: " + examDetails.getPatientID()  + "\n"
                +  "Age: " + examDetails.getPatientAge() + "\n"
                +  "Sex: " + examDetails.getPatientSex() + "\n"
                + "Date: " + examDetails.getStudyDate());
}

void MyGLWidget::drawCurrentDistance(QPainter *painter)
{
    QPen oldPen, myPen;
    oldPen = painter->pen();
    myPen.setWidth(3);
    myPen.setColor(QColor(255, 0, 0, 128));
    painter->setPen(myPen);

    QLine line(this->startPoint, this->endPoint);
    painter->drawLine(line);

    /* Calculate distance in physical units */
    unsigned int physicalDist;
    physicalDist = this->calcPhysicalDistance(&line);

    /* Calculate the point coordinates where the text will be drawn */
    myPen.setColor(Qt::yellow);
    painter->setPen(myPen);
    QPoint textPoint(
                (this->startPoint.x()+this->endPoint.x())/2 - 15,
                (this->startPoint.y()+this->endPoint.y())/2 - 15);
    painter->drawText(textPoint, QString::number(physicalDist) + " mm");

    painter->setPen(oldPen);
}

void MyGLWidget::drawCurrentDensity(QPainter *painter)
{
    Q_ASSERT(painter);

    QPen oldPen, myPen;
    oldPen = painter->pen();
    myPen.setWidth(3);
    myPen.setColor(QColor(255, 0, 0, 200));
    painter->setPen(myPen);

    /* Draw a circle with center at mouse cursor click and a varying radius */
    float dx = this->endPoint.x() - this->startPoint.x();
    float dy = this->endPoint.y() - this->startPoint.y();
    int dist = sqrt(dx*dx + dy*dy);
    painter->drawEllipse(this->startPoint, dist, dist);

    /* Calculate mean density over the above region */
    int meanDensity = this->calcMeanDensity(painter, dist);

    /* Display result */
    painter->drawText(this->startPoint, QString::number(meanDensity) + " HUs");

    painter->setPen(oldPen);
}

void MyGLWidget::drawDistances(QPainter *painter)
{
    Q_ASSERT(painter);

    QPen oldPen, linePen, textPen;
    oldPen = painter->pen();
    linePen.setWidth(3);
    linePen.setColor(Qt::red);
    textPen.setColor(Qt::yellow);

    for (int i = 0; i < this->vecDists.size(); i++) {
       QLine line = this->vecDists.at(i);
       painter->setPen(linePen);
       painter->drawLine(line);

       unsigned int physicalDist = this->calcPhysicalDistance(&line);
       painter->setPen(textPen);
       QPoint textPoint(
                   (line.p1().x()+line.p2().x())/2 - 15,
                   (line.p1().y()+line.p2().y())/2 - 15);
       painter->drawText(textPoint, QString::number(physicalDist) + " mm");
    }

    painter->setPen(oldPen);
}

void MyGLWidget::mouseMoveEvent(QMouseEvent *pEvent)
{
    if (pEvent->buttons() & Qt::RightButton) {
        float x = pEvent->localPos().x();
        float y = pEvent->localPos().y();

        float width = y / ((float) this->height());
        float center = x / ((float) this->width());

        if (width < 0.0) width = 0.0;
        if (width > 1.0) width = 1.0;
        if (center < 0.0) center = 0.0;
        if (center > 1.0) center = 1.0;

        this->tmin = center - width/2;
        this->tmax = center + width/2;

        if (this->tmin < 0.0) this->tmin = 0.0;
        if (this->tmax > 1.0) this->tmax = 1.0;

        qDebug() << "tmin =" << this->tmin << "tmax =" << this->tmax;
        this->update();
    } else if (pEvent->buttons() & Qt::LeftButton) {
        if (this->measureDistance) {
            this->endPoint = pEvent->pos();
            this->update();
       } else if (this->measureDensity) {
            this->endPoint = pEvent->pos();
            this->update();
        }
    }
    pEvent->ignore();
}

void MyGLWidget::mousePressEvent(QMouseEvent *pEvent)
{
    this->startPoint = pEvent->pos();
    this->endPoint = this->startPoint;
}

void MyGLWidget::mouseReleaseEvent(QMouseEvent *pEvent)
{
    if (this->measureDistance) {
        this->endPoint = pEvent->pos();
        /* Skip single points */
        if (this->startPoint != this->endPoint) {
            this->vecDists.push_back(QLine(this->startPoint, this->endPoint));
            this->update();
        }
    }
}

const Slice *MyGLWidget::getSlice() const
{
    Q_ASSERT(this->pSlice);
    return this->pSlice;
}

unsigned int MyGLWidget::getSliceIndex() const
{
    Q_ASSERT(this->pSlice);
    return this->pSlice->getIndex();
}

int MyGLWidget::calcMeanDensity(QPainter *painter, int dist)
{
    /* Get the points that are enclosed by the user-defined circle
     * with center 'this->startPoint' and radius 'dist' */
    QVector<QPoint> vecPoints;
    this->getPointsInCircle(&vecPoints, this->startPoint, dist);

    /* Calculate average luminance over these points */
    float *pPixelData = this->pSlice->getRawPixelData();
    Q_ASSERT(pPixelData);

    float totalLuminance = 0.0;
    QPoint newPoint;
    for (int i = 0; i < vecPoints.size(); i++) {
        QPoint oldPoint = vecPoints.at(i);
            float w1 = this->pSlice->getWidth();
            float w2 = this->width();
            float h1 = this->pSlice->getHeight();
            float h2 = this->height();
            newPoint.setX((w1/w2) * oldPoint.x());
            newPoint.setY((h1/h2) * oldPoint.y());
            totalLuminance += pPixelData[newPoint.y()*this->pSlice->getWidth() + newPoint.x()];
    }

    /* We need to convert luminance to HUs via the linear transformation:
       HU = value * slope + intercept -> <HU> = <value> * slope + intercept
       Where <x> denotes mean value of 'x'
    */
    float meanHUs =
         (this->pSlice->getGlobalMaxPixel() * totalLuminance / vecPoints.size()) - 1024;

    return (int) (meanHUs);
}

unsigned int MyGLWidget::calcPhysicalDistance(QLine *pLine)
{
    Q_ASSERT(pLine);

    float hs = this->pSlice->pDicomFile->getHorizontalSpacing();
    float vs = this->pSlice->pDicomFile->getVerticalSpacing();
    float sx = ((float)this->pSlice->getWidth()) / ((float)this->width());
    float sy = ((float)this->pSlice->getHeight()) / ((float)this->height());
    float dx = sx * (pLine->p2().x() - pLine->p1().x());
    float dy = sy * (pLine->p2().y() - pLine->p1().y());
    float len = sqrt((dx*dx*hs*hs) + (dy*dy*vs*vs));

//    qDebug() << "sx =" << sx << " sy =" << sy << " dx =" << dx << " dy =" << dy
//                << " hs =" << hs << " vs =" << vs;

    /* Physical distance unit is millimeter.
     * No need to have a resolution less than 1mm */
    return (unsigned int)len;
}

void MyGLWidget::getPointsInCircle(QVector<QPoint> *pVecPoints,
                                   QPoint centerPoint, float radius)
{
    Q_ASSERT(pVecPoints);

    /* Calculate bounding box coordinates */
    int left   = qMax(0.0f, centerPoint.x() - radius);
    int top    = qMax(0.0f, centerPoint.y() - radius);
    int right  = centerPoint.x() + radius;
    int bottom = centerPoint.y() + radius;

    for (int x = left; x < right; x++) {
        for (int y = top; y < bottom; y++) {
            float dx = x - centerPoint.x();
            float dy = y - centerPoint.y();
            float distSquared = dx*dx + dy*dy;
            if (distSquared < radius*radius) {
                pVecPoints->push_back(QPoint(x, y));
            }
        }
    }
}
