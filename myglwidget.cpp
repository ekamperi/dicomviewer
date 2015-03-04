#include "examdetails.h"
#include "geomtransformation.h"
#include "slice.h"
#include "myglwidget.h"
#include "mymath.h"

#include <iostream>
#include <QDebug>
#include <QMouseEvent>
#include <QtGlobal>
#include <QtOpenGL/QGLWidget>
#include <QGLShader>

MyGLWidget::MyGLWidget(QWidget *parent) :
    QGLWidget(QGLFormat(QGL::SampleBuffers), parent)
{
    qDebug() << Q_FUNC_INFO;

    /* Make the background black */
    this->setStyleSheet("background-color: black;");

    //pMagickImage = NULL;

    /* This is necessary in order to track current mouse wheel position */
    setMouseTracking(true);

    /* By default we don't measure anything */
    this->measureDistance = false;
    this->measureDensity = false;
    this->panMode = false;

    /* Reset view */
    this->viewMatrix.setToIdentity();
    this->resetView();

    /* Default window/width */
    this->tmin = 0.0;
    this->tmax = 0.4;

    /* By default we don't show any topogram */
    this->pTopogram = NULL;

    /* By default we don't debug */
    this->debugMode = false;
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

    /* Get rid of the topogram, if any */
    if (this->pTopogram) {
        delete this->pTopogram;
    }

    /* Don't forget to delete the textures */
    this->makeCurrent();
    glDeleteTextures(this->vecSlices.size(), this->pTexIDs);
    Q_ASSERT(glGetError() == GL_NO_ERROR);
}

void MyGLWidget::setSlice(Slice *pSlice)
{
    Q_ASSERT(pSlice);
    this->pSlice = pSlice;

    /* When a slice, that we are connected with, needs a repaint
     * e.g. when its window/width have changed, repaint!
     */
    connect(this->pSlice, SIGNAL(iNeedRepaint(float, float)),
            this, SLOT(repaintSlice(float, float)));

    /* Also notify our associated topogram, if any */
    if (pTopogram) {
        this->pTopogram->setNewSliceIndex(pSlice->getIndex());
    }

    /* Force a redraw */
    this->update();
}

void MyGLWidget::loadSlices(QVector<Slice *> vecSlices)
{
    qDebug() << Q_FUNC_INFO;

    this->vecSlices = vecSlices;

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
    bool rv;

    /* Setup the OpenGL matrices */
    this->projectionMatrix.ortho(0, 1, 1, 0, 0, 1);
    resetViewMatrix();

    glEnable(GL_TEXTURE_2D);
    Q_ASSERT(glGetError() == GL_NO_ERROR);

    /* Construct the default vertex and fragment shaders */
    this->pProgram = new QGLShaderProgram(this);
    Q_ASSERT(this->pProgram);

    /* Default vertex shader */
    QGLShader *pDefVertexShader = new QGLShader(QGLShader::Vertex);
    Q_ASSERT(pDefVertexShader);
    rv = pDefVertexShader->compileSourceFile("./vertex.sh");
    Q_ASSERT(rv);
    rv = this->pProgram->addShader(pDefVertexShader);
    Q_ASSERT(rv);
    this->pVertexShader = pDefVertexShader;

    /* Default fragment shader */
    QGLShader *pDefFragmentShader = new QGLShader(QGLShader::Fragment);
    Q_ASSERT(pDefFragmentShader);
    rv = pDefFragmentShader->compileSourceFile("./fragment.sh");
    Q_ASSERT(rv);
    rv = this->pProgram->addShader(pDefFragmentShader);
    Q_ASSERT(rv);
    this->pFragmentShader = pDefFragmentShader;

    rv = this->pProgram->link();
    Q_ASSERT(rv);
}

void MyGLWidget::resizeGL(int w, int h)
{
    this->resetViewMatrix();

    /* If there is a topogram and only if it's embedded into us move it */
    if (this->pTopogram && this->pTopogram->isEmbedded()) {
        this->pTopogram->move(this->width()-pTopogram->width(), 0);
    }
}

void MyGLWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter;
    painter.begin(this);

    /* Bind this program shader to current opengl context.
     * This is equivalent to calling glUseProgram(). */
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

    //
    int pm_loc = this->pProgram->uniformLocation("projectionMatrix");
    int vm_loc = this->pProgram->uniformLocation("viewMatrix");
    int mm_loc = this->pProgram->uniformLocation("modelMatrix");
    Q_ASSERT(pm_loc != -1);
    Q_ASSERT(vm_loc != -1);
    Q_ASSERT(mm_loc != -1);
    this->pProgram->setUniformValue(pm_loc, this->projectionMatrix);
    this->pProgram->setUniformValue(vm_loc, this->viewMatrix);
    this->pProgram->setUniformValue(mm_loc, this->modelMatrix);

    /* Ready to do the drawing */
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.0, 0.0, 0.0, 0.0);

    /* Bind again (besides setSlice()) */
    glBindTexture(GL_TEXTURE_2D, this->pTexIDs[this->pSlice->getIndex()]);
    Q_ASSERT(glGetError() == GL_NO_ERROR);

    glBegin(GL_QUADS);
        glTexCoord2d(0.0, 0.0); glVertex2d(0.0, 0.0);
        glTexCoord2d(1.0, 0.0); glVertex2d(1.0, 0.0);
        glTexCoord2d(1.0, 1.0); glVertex2d(1.0, 1.0);
        glTexCoord2d(0.0, 1.0); glVertex2d(0.0, 1.0);
    glEnd();

    /* Should we call this at all ? */
    this->pProgram->release();

    /* Proceed with native drawing */
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setRenderHint(QPainter::TextAntialiasing, true);
    painter.setRenderHint(QPainter::HighQualityAntialiasing, true);

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

    /* Draw debug information */
    if (this->debugMode) {
        this->drawDebug(&painter);
    }

    painter.end();
}

void MyGLWidget::drawDetails(QPainter *pPainter)
{
    Q_ASSERT(pPainter);

    QFont font = pPainter->font();
    font.setPointSize(10);
    pPainter->setFont(font);
    pPainter->setPen(Qt::gray);
    ExamDetails examDetails = this->pSlice->getExamDetails();

    /* Draw generic examination details (patient name, age, sex, etc) */
    pPainter->drawText(
                QRect(5, 5, this->width(), this->height()),
                Qt::TextWordWrap,
                examDetails.getGenericDetails());

    /* Draw specific examination details (e.g. KVp, mAs in CT) */
    pPainter->drawText(
                QRect(5, 5 + this->height() - 100,
                      this->width(), this->height()),
                      Qt::TextWordWrap,
                      examDetails.getSpecificDetails(Exam::CT));
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

    /* Save old pen before replacing it */
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
    int meanDensity = this->calcMeanDensity(dist);

    /* Display result */
    painter->drawText(this->startPoint, QString::number(meanDensity) + " HUs");

    /* Restore old pen */
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

void MyGLWidget::drawDebug(QPainter *painter)
{
    Q_ASSERT(painter);

    QPen oldPen, textPen;
    oldPen = painter->pen();
    textPen.setColor(Qt::red);

    /* Get current cursor position */
    QPointF mousePos = this->mapFromGlobal(QCursor::pos());

    /* Convert to [0,1] */
    QPointF m = QPointF(mousePos.x() / this->width(),
                        mousePos.y() / this->height());
    QPointF p = this->viewMatrix.inverted().map(m);

    /* Construct the debug message (mouse pos) -> (original pos) */
    QString dm = "(" +
            QString::number(m.x(), 'g', 2) + ", " +
            QString::number(m.y(), 'g', 2) + ") -> (" +
            QString::number(p.x(), 'g', 2) + ", " +
            QString::number(p.y(), 'g', 2) + ")";

    /* Write the debug message in the current cursor position */
    painter->setPen(textPen);
    painter->drawText(mousePos, dm);

    /* Restore old pen */
    painter->setPen(oldPen);
}

void MyGLWidget::mouseMoveEvent(QMouseEvent *pEvent)
{
    if (pEvent->buttons() & Qt::RightButton) {
        float x = pEvent->localPos().x();
        float y = pEvent->localPos().y();

        float width = y / ((float) this->height());
        float center = x / ((float) this->width());

        this->tmin = center - width/2;
        this->tmax = center + width/2;

        if (this->tmin < 0.0) this->tmin = 0.0;
        if (this->tmax > 1.0) this->tmax = 1.0;

//        qDebug() << "tmin =" << this->tmin << "tmax =" << this->tmax;
        this->update();
    } else if (pEvent->buttons() & Qt::LeftButton) {
        if (this->measureDistance) {
            this->endPoint = pEvent->pos();
            this->update();
       } else if (this->measureDensity) {
            this->endPoint = pEvent->pos();
            this->update();
        } else if (this->isPanMode()) {
            float dx = pEvent->pos().x() - this->startPoint.x();
            float dy = pEvent->pos().y() - this->startPoint.y();
            this->offsetX = dx / this->width();
            this->offsetY = dy / this->height();
            this->resetViewMatrix();
        }
    } else if (this->debugMode) {
        this->update();
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
    } else if (this->panMode) {
        this->oldOffsetX += this->offsetX;
        this->oldOffsetY += this->offsetY;
        this->offsetX = this->offsetY = 0;
    }
}

Slice *MyGLWidget::getSlice(void) const
{
    Q_ASSERT(this->pSlice);
    return this->pSlice;
}

unsigned int MyGLWidget::getSliceIndex() const
{
    Q_ASSERT(this->pSlice);
    return this->pSlice->getIndex();
}

int MyGLWidget::calcMeanDensity(int dist)
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
            totalLuminance +=
                    pPixelData[
                        newPoint.y()*this->pSlice->getWidth() + newPoint.x()];
    }

    /* We need to convert luminance to HUs via the linear transformation:
       HU = value * slope + intercept -> <HU> = <value> * slope + intercept
       Where <x> denotes mean value of 'x'
    */
    const HUConverter *pHUConverter = this->pSlice->pDicomFile->getHUConverter();
    Q_ASSERT(pHUConverter);

    float slope = pHUConverter->getSlope();
    float intercept = pHUConverter->getIntercept();
    float meanHUs =
         (this->pSlice->getGlobalMaxPixel() * slope * totalLuminance / vecPoints.size())
            + intercept;

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
    /* Discard points with negative (offscreen) coordinates */
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

void MyGLWidget::repaintSlice(float tmin, float tmax)
{
    qDebug() << Q_FUNC_INFO;
    qDebug() << "tmin =" << tmin << "tmax =" << tmax;

    this->tmin = tmin;
    this->tmax = tmax;

    /* Force a redraw */
    this->update();
}

void MyGLWidget::wheelEvent(QWheelEvent *pEvent)
{
    if (QApplication::keyboardModifiers() & Qt::ControlModifier) {
        pEvent->accept();
        int delta = pEvent->delta();
        this->oldScaleFactor = this->scaleFactor;
        if (delta > 0) {
            if (this->scaleFactor > 0.1) {
                this->scaleFactor -= 0.15;
            }
        } else {
            if (this->scaleFactor < 10.0) {
                this->scaleFactor += 0.15;
            }
        }
        this->resetViewMatrix();
        this->update();
    }

    /* Let others be notified as well (e.g., the Main Window) */
    pEvent->ignore();
}

void MyGLWidget::setGeomTransformation(Geometry::Transformation geomTransformation)
{
    qDebug() << Q_FUNC_INFO;

    this->geomTransformation = geomTransformation;

    QGLShader *pNewVertexShader = new QGLShader(QGLShader::Vertex);
    Q_ASSERT(pNewVertexShader);

    bool rv = false;
    switch (this->geomTransformation) {
    case Geometry::FLIP_HORIZONTALLY:
        rv = pNewVertexShader->compileSourceFile("./flipHorVertex.sh");
        break;
    case Geometry::FLIP_VERTICALLY:
        rv = pNewVertexShader->compileSourceFile("./flipVertVertex.sh");
        break;
    case Geometry::NO_TRANSFORMATION:
        rv = pNewVertexShader->compileSourceFile("./vertex.sh");
        break;
    }
    Q_ASSERT(rv);

    /* Remove the old vertex shader (the object is not deleted by 'removeShader()') */
    Q_ASSERT(this->pVertexShader);
    this->pProgram->removeShader(this->pVertexShader);
    delete this->pVertexShader;

    /* Add the new vertex shader */
    rv = this->pProgram->addShader(pNewVertexShader);
    Q_ASSERT(rv);
    this->pVertexShader = pNewVertexShader;

    /* Force a redraw */
    this->update();
}

Geometry::Transformation
MyGLWidget::getGeomTransformation(void) const
{
    return this->geomTransformation;
}

void MyGLWidget::genTopogram(void)
{
    qDebug() << Q_FUNC_INFO;

    float *pResult;
    if (!this->pTopogram) {
        pResult = (float *)malloc(sizeof(float) * 512*512);
        Q_ASSERT(pResult);

        /* For every (x, z) calculate the average luminance along the y axis */
        for (int z = 0; z < this->vecSlices.size(); z++) {
            Slice *pzSlice = this->vecSlices.at(z);
            Q_ASSERT(pzSlice);

            unsigned int w = pzSlice->getWidth();
            unsigned int h = pzSlice->getHeight();

            for (unsigned int x = 0; x < w; x++) {
                float luminance = 0.0;
                for (unsigned int y = 0; y < h; y++) {
                    luminance += pzSlice->getRawPixelData()[(int)(y*w + x)];
                }
                pResult[z*w+x] = MyMath::sstep(0.0, 0.3, luminance / h);
            }
        }

        /* Construct a new topogram */
        this->pTopogram = new Topogram(
                    pResult, 512, this->vecSlices.size(), this->pSlice->getIndex());

        /* By default, the topogram is embedded in the GL widget */
        this->pTopogram->setParent(this);

        /* But also let the user un-embed it, should s/he want */
        connect(this->pTopogram, SIGNAL(iWantToBreakFree(void)),
                this, SLOT(setTheTopogramFree(void)));

        /* Position the topogram at the top right corner */
        this->pTopogram->show();
        this->pTopogram->move(this->width()-pTopogram->width(), 0);
    } else {
        this->pTopogram->hide();
        this->pTopogram->deleteLater();
        this->pTopogram = NULL;
    }
}

void MyGLWidget::setTheTopogramFree(void)
{
    qDebug() << Q_FUNC_INFO;

    this->pTopogram->setParent(NULL);
    this->pTopogram->move(
                this->mapToGlobal(
                    QPoint(this->width()-pTopogram->width(), 0)));
    this->pTopogram->show();
    this->pTopogram->setEmbedded(false);
}

void MyGLWidget::resetViewMatrix(void)
{
    qDebug();
    qDebug() << Q_FUNC_INFO << "sf =" << this->scaleFactor;

    /* Get mouse position in local (to GL widget) coordinates */
    QPointF mousePos = this->mapFromGlobal(QCursor::pos());
    QPointF m = QPointF(((float)mousePos.x()) / this->width(),
                        ((float)mousePos.y()) / this->height());

    /* Get position in the original slice data */
    QPointF p = this->viewMatrix.inverted().map(m);
    qDebug() << "m =" << m << " p =" << p;

    /* Calculate offsets from paning (if any) */
    float ofsX = this->oldOffsetX + this->offsetX;
    float ofsY = this->oldOffsetY + this->offsetY;
    qDebug() << "ofsX =" << ofsX << " ofsY =" << ofsY;

    float dx, dy;
    if (this->offsetX == 0 && this->offsetY == 0) {
        qDebug() << "ZOOMING!";
        dx = this->scaleFactor * p.x() - m.x();
        dy = this->scaleFactor * p.y() - m.y();
        this->oldOffsetX = -dx;
        this->oldOffsetY = -dy;
    } else {
        qDebug() << "PANNING!";
        dx = -ofsX;
        dy = -ofsY;
    }

    this->viewMatrix.setToIdentity();
    this->viewMatrix.translate(-dx, -dy);
    this->viewMatrix.scale(this->scaleFactor);

    qDebug() << this->viewMatrix;

    /* Update the view */
    this->update();
}

void MyGLWidget::resetView(void)
{
    this->scaleFactor = 1.0;
    this->oldScaleFactor = 1.0;
    this->oldOffsetX = 0.0;
    this->oldOffsetY = 0.0;
    this->offsetX = 0.0;
    this->offsetY = 0.0;
    this->viewMatrix.setToIdentity();
    this->resetViewMatrix();
}
