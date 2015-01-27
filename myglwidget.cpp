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
    Q_ASSERT(this->pProgram);
    delete this->pProgram;
}

void MyGLWidget::setSlice(Slice *pSlice)
{
    Q_ASSERT(pSlice);

    this->pSlice = pSlice;
    this->loadTexture(
                pSlice->getRawPixelData(),
                pSlice->getWidth(),
                pSlice->getHeight(),
                pSlice->getFormat());

    this->update();
}

void MyGLWidget::loadTexture(float *pRawPixel,
                             unsigned int width,
                             unsigned int height,
                             GLint format)
{
    qDebug() << Q_FUNC_INFO;
    this->pRawPixel = pRawPixel;

    /* From the QGLWidget official documentation:
     *
     * If you need to call the standard OpenGL API functions from other places
     * (e.g. in your widget's constructor or in your own paint functions), you
     * must call makeCurrent() first.
     *
     * makeCurrent() makes this widget the current widget for OpenGL operations,
     * i.e. makes the widget's rendering context the current OpenGL rendering
     * context.
     */
    this->makeCurrent();

    glEnable(GL_TEXTURE_2D);
    Q_ASSERT(glGetError() == GL_NO_ERROR);

    /*
     * glDeleteTextures() silently ignores names that do not correspond
     * to existing textures.
     */
    glDeleteTextures(1, &this->textureID);
    Q_ASSERT(glGetError() == GL_NO_ERROR);

    /* Obtain an id for the texture */
    glGenTextures(1, &this->textureID);
    Q_ASSERT(glGetError() == GL_NO_ERROR);

    /* Select texture */
    glBindTexture(GL_TEXTURE_2D, this->textureID);
    Q_ASSERT(glGetError() == GL_NO_ERROR);

    /* Just a sanity check */
    Q_ASSERT((format == GL_LUMINANCE) || (format == GL_RGB));
    Q_ASSERT(this->pRawPixel);

    glTexImage2D(GL_TEXTURE_2D, 0, format,
                 width,
                 height, 0, GL_LUMINANCE, GL_FLOAT,
                 this->pRawPixel);
    Q_ASSERT(glGetError() == GL_NO_ERROR);

    /* Set texture stretching parameters */
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    Q_ASSERT(glGetError() == GL_NO_ERROR);

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
    qDebug() << Q_FUNC_INFO;
    qDebug() << "tmin=" << this->tmin << "tmax=" << this->tmax;
    qDebug() << event->rect();

    QPainter painter;
    painter.begin(this);

    /* This is equivalent to calling glUseProgram() */
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

    glEnable(GL_TEXTURE_2D);
    glBegin(GL_QUADS);
        glTexCoord2d(0.0, 0.0); glVertex2d(0.0, 0.0);
        glTexCoord2d(1.0, 0.0); glVertex2d(1.0, 0.0);
        glTexCoord2d(1.0, 1.0); glVertex2d(1.0, 1.0);
        glTexCoord2d(0.0, 1.0); glVertex2d(0.0, 1.0);
    glEnd();
    glDisable(GL_TEXTURE_2D);
    Q_ASSERT(glGetError() == GL_NO_ERROR);

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
        this->update();
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

void MyGLWidget::drawDistances(QPainter *painter)
{
    Q_ASSERT(painter);

    QPen oldPen, linePen, textPen;
    oldPen = painter->pen();
    linePen.setWidth(3);
    linePen.setColor(Qt::red);
    textPen.setColor(Qt::yellow);

    for (unsigned int i = 0; i < this->vecDists.size(); i++) {
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

const Slice * MyGLWidget::getSlice() const
{
    Q_ASSERT(this->pSlice);
    return this->pSlice;
}

unsigned int MyGLWidget::getSliceIndex() const
{
    Q_ASSERT(this->pSlice);
    return this->pSlice->getIndex();
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

    return (unsigned int)len;
}
