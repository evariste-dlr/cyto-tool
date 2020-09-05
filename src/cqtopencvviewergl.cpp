#include "include/cqtopencvviewergl.h"

#include <QDebug>

CQtOpenCVViewerGl::CQtOpenCVViewerGl(QWidget *parent) :
    QGLWidget(parent)
{
    _SceneChanged = false;
    _BgColor = QColor::fromRgb(150, 150, 150);

    _OutH = 0;
    _OutW = 0;
    _ImgRatio = 4.0f/3.0f;

    _PosX = 0;
    _PosY = 0;
}

void CQtOpenCVViewerGl::initializeGL()
{
    makeCurrent();
    qglClearColor(_BgColor.darker());
}

void CQtOpenCVViewerGl::resizeGL(int width, int height)
{
    makeCurrent();
    glViewport(0, 0, (GLint)width, (GLint)height);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    glOrtho(0, width, 0, height, 0, 1);	// To Draw image in the center of the area

    glMatrixMode(GL_MODELVIEW);

    // ---> Scaled Image Sizes
    _OutH = width/_ImgRatio;
    _OutW = width;

    if(_OutH>height)
    {
        _OutW = height*_ImgRatio;
        _OutH = height;
    }

    emit imageSizeChanged( _OutW, _OutH );
    // <--- Scaled Image Sizes

    _PosX = (width-_OutW)/2;
    _PosY = (height-_OutH)/2;

    _SceneChanged = true;

    updateScene();
}

void CQtOpenCVViewerGl::updateScene()
{
    if( _SceneChanged && this->isVisible() )
        updateGL();
}

void CQtOpenCVViewerGl::paintGL()
{
    makeCurrent();

    if( !_SceneChanged )
        return;

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    renderImage();

    _SceneChanged = false;
}

void CQtOpenCVViewerGl::renderImage()
{
    makeCurrent();

    glClear(GL_COLOR_BUFFER_BIT);

    if (!_RenderQtImg.isNull())
    {
        glLoadIdentity();

        QImage image; // the image rendered

        glPushMatrix();
        {
            int imW = _RenderQtImg.width();
            int imH = _RenderQtImg.height();

            // The image is to be resized to fit the widget?
            if( imW != this->size().width() &&
                    imH != this->size().height() )
            {

                image = _RenderQtImg.scaled( //this->size(),
                                             QSize(_OutW,_OutH),
                                             Qt::IgnoreAspectRatio,
                                             Qt::SmoothTransformation
                                             );

                qDebug() << tr( "Image size: (%1x%2)").arg(imW).arg(imH);
            }
            else
                image = _RenderQtImg;

            // ---> Centering image in draw area
            glRasterPos2i( _PosX, _PosY );
            // <--- Centering image in draw area

            imW = image.width();
            imH = image.height();

            glDrawPixels( imW, imH, GL_RGBA, GL_UNSIGNED_BYTE, image.bits());
        }
        glPopMatrix();

        // end
        glFlush();
    }
}

bool CQtOpenCVViewerGl::showImage( cv::Mat image )
{
    image.copyTo(_OrigImage);

    _ImgRatio = (float)image.cols/(float)image.rows;

    if( _OrigImage.channels() == 3)
        _RenderQtImg = QImage((const unsigned char*)(_OrigImage.data),
                              _OrigImage.cols, _OrigImage.rows,
                              _OrigImage.step, QImage::Format_RGB888).rgbSwapped();
    else if( _OrigImage.channels() == 1)
        _RenderQtImg = QImage((const unsigned char*)(_OrigImage.data),
                              _OrigImage.cols, _OrigImage.rows,
                              _OrigImage.step, QImage::Format_Indexed8);
    else
        return false;

    _RenderQtImg = QGLWidget::convertToGLFormat(_RenderQtImg);

    _SceneChanged = true;

    updateScene();

    return true;
}
