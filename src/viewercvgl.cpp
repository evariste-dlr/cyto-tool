#include "include/viewercvgl.h"

#include <QDebug>
#include <QErrorMessage>

/************************
 * Constructeurs
 ************************/

ViewerCVGl::ViewerCVGl(QWidget *parent) :
    QGLWidget(parent),
    _showDrawings(false), _showTools(false),
    _sliceToolx(0), _sliceTooly(0),
    _SceneChanged(false),
    _BgColor(QColor::fromRgb(150,150,150)),
    _OutH(0), _OutW(0),
    _ImgRatio(4.0f/3.0f),
    _PosX(0), _PosY(0)
{
    _drawings = QImage(0,0, QImage::Format_Mono);
}

ViewerCVGl::~ViewerCVGl(){}


void ViewerCVGl::setImgProc(EdImageProcessor *p){
    _imgProc = p;
}


/************************
 * Accesseurs
 ************************/

const int&
ViewerCVGl::imageWidth() const {
    return _OutW;
}

const int&
ViewerCVGl::imageHeight() const {
    return _OutH;
}

const QImage&
ViewerCVGl::renderedImage() const {
    return _RenderQtImg;
}

const cv::Mat&
ViewerCVGl::originImage() const {
    return _OrigImage;
}


/************************
 * Événements
 ************************/
void
ViewerCVGl::mousePressEvent(QMouseEvent *event){
    int imgX, imgY;

    imgX = event->x() - _PosX;
    imgY = event->y() - _PosY;
    imgX *= ((double)(_OrigImage.cols) / _OutW);
    imgY *= ((double)(_OrigImage.rows) / _OutH);

    emit mouseClicked(event->x(), event->y());
    emit mouseClickedImage(imgX, imgY);
}

/************************
 * Public slots
 ************************/

bool
ViewerCVGl::refresh(){
    return drawImage();
}

void
ViewerCVGl::setThreshold(const int &lvl){
    if (lvl <= 255 && lvl >= 0){
        _imgProc->setThresh(lvl);
        drawImage();
    }
}

void
ViewerCVGl::setSliceToolX(int x){
    if (x >=0 && x < size().width()){
        _sliceToolx = x;
        _SceneChanged = true;
        updateScene();
    }
}

void
ViewerCVGl::setSliceToolY(int y){
    if (y >=0 && y < size().height()){
        _sliceTooly = y;
        _SceneChanged = true;
        updateScene();
    }
}

void ViewerCVGl::showTools(){ _showTools = true; }
void ViewerCVGl::hideTools(){ _showTools = false; }


void
ViewerCVGl::showDrawings() {
    _showDrawings = true;
    _SceneChanged = true;
    updateScene();
}

void
ViewerCVGl::hideDrawings() {
    _showDrawings = false;
    _SceneChanged = true;
    updateScene();
}

void
ViewerCVGl::resetDrawings(){
    if (_drawings.isNull()){
        _drawings = QImage(size(), QImage::Format_Mono);
    }
}

void
ViewerCVGl::reset(){
    resetDrawings();
    showImage(cv::Mat::zeros(_OutW,_OutH,CV_8UC1));
}


/*** Graphic edition methods ***/

void
ViewerCVGl::addText(const QString &str, const int &x, const int &y){

}

void
ViewerCVGl::drawPixel(const int &x, const int &y, const QColor &color){

}

/************************
 * Protected methods
 ************************/



        /***********************************************************
         *     Traitements d'affichage (OpenGL) - (OpenCV)
         ***********************************************************/

void
ViewerCVGl::renderTools(){
    if (_showDrawings){


    }

    if (_showTools){
        // Affichage de la ligne de coupe horizonale
        int gly = size().height() - _sliceTooly;
        glBegin(GL_LINES);
            glColor3f(1.0,0.0,0.0);
            glVertex2i(0, gly);
            glVertex2i(size().width(), gly);
        glEnd();

        // Affichage de la ligne de coupe verticale
        glBegin(GL_LINES);
            glVertex2i(_sliceToolx, 0);
            glVertex2i(_sliceToolx, size().height());
        glEnd();
    }
}


void ViewerCVGl::initializeGL()
{
    makeCurrent();
    qglClearColor(_BgColor.darker());
}

void ViewerCVGl::resizeGL(int width, int height)
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

void ViewerCVGl::updateScene()
{
    if( _SceneChanged && this->isVisible() )
        updateGL();
}

void ViewerCVGl::paintGL()
{
    makeCurrent();

    if( !_SceneChanged )
        return;

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    renderImage();

    _SceneChanged = false;
}

void ViewerCVGl::renderImage()
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

                //qDebug() << tr( "Image size: (%1x%2)").arg(imW).arg(imH);
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

        renderTools();

        // end
        glFlush();


        // Tell everyone the scene changed
        emit sceneChanged();
    }
}

bool ViewerCVGl::showImage( cv::Mat image )
{
    if (image.data == NULL){
        QErrorMessage dial;
        dial.showMessage("Format d'image non pris en charge.");
        dial.exec();
    }

    image.copyTo(_OrigImage);
    _ImgRatio = (float)image.cols/(float)image.rows;

    bool res = drawImage();
    resizeGL(width(), height());
    return res;
}


bool ViewerCVGl::drawImage(){
    // On applique les traitements éventuels à l'image d'origine
    cv::Mat img = _imgProc->process(_OrigImage);

    if( img.channels() == 3)
        _RenderQtImg = QImage((const unsigned char*)(img.data),
                              img.cols, img.rows,
                              img.step, QImage::Format_RGB888).rgbSwapped();
    else if( img.channels() == 1)
        _RenderQtImg = QImage((const unsigned char*)(img.data),
                              img.cols, img.rows,
                              img.step, QImage::Format_Indexed8);
    else
        return false;

    _RenderQtImg = QGLWidget::convertToGLFormat(_RenderQtImg);

    _SceneChanged = true;

    updateScene();

    return true;
}


