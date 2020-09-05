#ifndef VIEWER_CVGL_H
#define VIEWER_CVGL_H

#include <QGLWidget>
#include <QMouseEvent>
#include <opencv2/opencv.hpp>

#include "lib/edimageprocessor.h"

/**
 * @brief The Viewer class
 * Afficheur d'images utilisé par le lecteur
 * et les composants de l'application.
 *
 * L'afficheur permet de superposer des images
 * et des éléments graphiques simples comme
 * * Du texte
 * * Des pixels colorés (outil pinceaux rudimentaire)
 */
class ViewerCVGl : public QGLWidget {

    Q_OBJECT

    /* Constructeurs */
public:
    explicit ViewerCVGl(QWidget *parent = 0);
    ~ViewerCVGl();

    void setImgProc(EdImageProcessor* p);

    /* Signaux */
signals:
    void imageSizeChanged(int outW, int outH);
    void sceneChanged();
    void mouseClicked(int x, int y);      /**< Clique dans le cadre @see mouseClickedImage */
    void mouseClickedImage(int x, int y); /**< Coordonées rapportées à l'image d'origine @see mouseClicked */

    /* Accesseurs */
public:
    /**
     * @brief width
     * @return La largeur en pixels
     */
    const int& imageWidth() const;

    /**
     * @brief height
     * @return La hauteur en pixels
     */
    const int& imageHeight() const;

    /**
     * @brief Référence vers l'image affichée
     */
    const QImage& renderedImage() const;

    /**
     * @return Référence vers l'image d'origine (non redimentionnée)
     */
    const cv::Mat& originImage() const;

    /* Evénements */
public:
    void mousePressEvent(QMouseEvent* event);


    /* Public slots */
public slots:

    /**
     * @brief showImage Donne l'image à afficher, et lance le rendering
     * @return vrai si l'image est utilisable
     */
    bool showImage(cv::Mat image);

    /**
     * @brief refresh Met à jour l'affichage (appel de drawImage)
     * @return vrai si succès
     */
    bool refresh();

    /**
     * @brief addText Ajoute du texte à l'affichage
     * @param str   Le texte
     * @param x     x du coin sup. gauche
     * @param y     y du coin sup. gauche
     */
    void addText(const QString& str, const int& x, const int& y);

    /**
     * @brief drawPixel Ajoute un pixel de couleur à l'affichage
     * @param x     x
     * @param y     y
     * @param color Couleur du point
     */
    void drawPixel(const int& x, const int& y, const QColor& color);

    /**
     * @brief setThreshold Ajuste le niveau de seuillage sur l'image
     * @param lvl  niveau entre 0 et 255. À 0, le seuillage n'est pas calculé
     */
    void setThreshold(const int& lvl);

    void setSliceToolX(int x);
    void setSliceToolY(int y);

    void showTools();
    void hideTools();

    void showDrawings(); /**< Affiche la couche de dessins (et textes) */
    void hideDrawings(); /**< Cache la couche de dessins (et textes) */
    void resetDrawings(); /**< Efface les dessins */
    void reset();  /**< Efface l'affichage - dessins et image */

    /* Protected methods */
protected:
    void 	initializeGL(); /**< Initialisation d'openGL */
    void 	paintGL();      /**< OpenGL Rendering */
    void 	resizeGL(int width, int height); /**< Widget Resize Event */

    void    renderTools();  /**< dessine la vue des outils */

    bool    drawImage();   /**< Méthode intermédiaire pour l'affichage */

    void    updateScene();
    void    renderImage();


    /* Attributs */
protected:
    QImage _drawings;   /**< Calque de dessins (annotations) */
    bool _showDrawings; /**< Afficher / cacher les annotations */

    int _sliceToolx;    /**< Coordonnée de la ligne de coupe en x */
    int _sliceTooly;
    bool _showTools;

    bool _SceneChanged;  /**< Indicates when OpenGL view is to be redrawn */

    QImage _RenderQtImg;   /**< Qt image to be rendered */
    cv::Mat _OrigImage;    /**< original OpenCV image to be shown */

    QColor _BgColor;     /**< Background color */

    int _OutH;          /**< Resized Image height */
    int _OutW;          /**< Resized Image width */
    float _ImgRatio;    /**< height/width ratio */

    int _PosX; /**< Top left X position to render image in the center of widget */
    int _PosY; /**< Top left Y position to render image in the center of widget */

    EdImageProcessor* _imgProc; /**< Traitement des images */

};

#endif // VIEWER_CVGL_H
