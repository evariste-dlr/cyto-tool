#ifndef POPULATION
#define POPULATION

#include <QGridLayout>
#include <opencv2/opencv.hpp>

#include "viewercvgl.h"
#include "player.h"
#include "component.h"

/**
 * @brief Gestion du composant Population, permettant de
 * décompter les cellules grâce à un ensemble d'outils de
 * traitement d'image
 *
 * @see Fourier
 * @see Component
 */
class Population : public Component {

    Q_OBJECT

public:
    Population(MainWindow* w,        /**< Fenêtre parente */
               QWidget* ui,          /**< Layout utilisé */
               ViewerCVGl* v,        /**< Widget d'affichage */
               Player* p             /**< Lecteur d'images */
           );
    ~Population();

public slots:
    //virtual void setEnabled(bool e); /**< Activer / désactiver le module */
    void setThresh(int t);
    void setContrast(int c);
    void setLumin(int l);
    void setEltSize(int s);   /**< Taille de l'élement structurant */
    void setEltShape(int s);  /**< Forme de l'élément structurant */

    void invThresh(bool i);  /**< Seuillage inverse */
    void enThresh(bool e);   /**< Activer le seuillage */

    void equalize();      /**< Égaliser l'histogramme */
    void resetLinear();   /**< Remise à zéro des transformations linéaires */

    void dilate();        /**< Dilater */
    void erode();         /**< Éroder */

    void count();         /**< Compter les cellules */
    void resetDisplay();  /**< Afficher l'image d'origine */

    void render();

    void copyOrigImage();
    void enable();
    void disable();
    void updateTableSize(int columns);

protected:
    void init();

    void linearTransform(const cv::Mat& src);

    virtual void updateXml();

protected:
    ViewerCVGl* _viewer;
    Player* _player;

    cv::Mat _origin;    /**< Image d'origine */
    cv::Mat _rendered;  /**< Image à afficher */
    cv::Mat _renderedBin; /**< Image binaire à afficher */

    cv::Mat _tmpImage;

    bool _invThresh;
    int _thresh;
    double _contrast;
    int _lumin;

    int _eltSize;     /**< Taille de l'élément structurant */
    int _eltShape;    /**< Forme de l'élément structurant @see cv::MORPH_* */

    bool _threshEn;   /**< Seuillage activé */
    bool _init;

};

#endif // POPULATION

