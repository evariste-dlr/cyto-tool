#ifndef CQTOPENCVVIEWERGL_H
#define CQTOPENCVVIEWERGL_H

#include <QGLWidget>
#include <opencv2/core/core.hpp>


class CQtOpenCVViewerGl : public QGLWidget
{
    Q_OBJECT
public:
    explicit CQtOpenCVViewerGl(QWidget *parent = 0);

signals:
    /**
     * Used to resize the image outside the widget
     */
    void    imageSizeChanged( int outW, int outH ); 

public slots:
    /**
     * Used to set the image to be viewed
     */
    bool    showImage( cv::Mat image ); 

protected:
    void 	initializeGL(); /// OpenGL initialization
    void 	paintGL(); /// OpenGL Rendering
    void 	resizeGL(int width, int height);        /// Widget Resize Event

    void        updateScene();
    void        renderImage();

protected:
    bool        _SceneChanged;          /// Indicates when OpenGL view is to be redrawn

    QImage      _RenderQtImg;           /// Qt image to be rendered
    cv::Mat     _OrigImage;             /// original OpenCV image to be shown

    QColor      _BgColor;		/// Background color

    int         _OutH;                  /// Resized Image height
    int         _OutW;                  /// Resized Image width
    float       _ImgRatio;             /// height/width ratio

    int         _PosX;                  /// Top left X position to render image in the center of widget
    int         _PosY;                  /// Top left Y position to render image in the center of widget

};

#endif // CQTOPENCVVIEWERGL_H
