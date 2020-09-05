#ifndef EDIMAGEPROCESSOR_H
#define EDIMAGEPROCESSOR_H

#include <QObject>
#include <opencv2/opencv.hpp>
#include <deque>


/**
 * @brief Classe regroupant divers appels à des traitements
 * d'image via openCV
 *
 * EdImageProcessor permet d'ordonnancer quelques traitements
 * basiques sur des images à l'aide d'openCV. Il suffit
 * de configurer l'instance et de passer une fonction
 * en paramètre de process() pour obtenir le résultat de la
 * chaine de traitements.
 *
 * @author Évariste DALLER
 * @date Avril 2016
 */
class EdImageProcessor : public QObject{

    Q_OBJECT

    /* Constructeurs */
public:
    explicit EdImageProcessor(QObject *parent=0);
    ~EdImageProcessor();

public:
    /**
     * @brief Code de priorité associé à chaque traitement
     * @see process
     */
    enum PCode{
        CTRST, THRESH, ERODE, DILATE
    };

    /* Constantes */

    /**
     * @brief Taille maximum de l'élément structurant uilisé pour les opérations
     * morphologiques
     */
    static const int MAX_ELT_SIZE = 50;



    /* Utilisation */
public:
    /**
     * @brief Applique les traitements prévus dans l'ordre de la liste de
     * priorité, avec les paramètres précédemment réglés à l'aide des fonctions
     * adaptées.
     * @param src  Image source
     * @return Image après traitements
     *
     * @see setContrast
     * @see setThresh
     * @see setEltSize
     * @see setThreshBin
     * @see addPriorList
     */
    cv::Mat process(cv::Mat src);


public slots:
    /**
     * @brief Ajuste le contraste de l'image
     * @param lvl  Niveau entre 0 et 255 (utile généralement entre 0 et 5)
     */
    void setContrast(double lvl);

    /**
     * @brief Applique un seuillage. Si _threshBin est vrai, il s'agit d'une
     * binarisation.
     * @param lvl  Niveau du seuil entre 0 et 255
     */
    void setThresh(int lvl);

    /**
     * @brief Ajuste la taille de l'élément structurant utilisé pour les
     * opérations morphologiques (la forme est un disque).
     * @param size  Taille en pixels entre 1 et MAX_ELT_SIZE
     */
    void setEltSize(int size);

    /**
     * @brief Décide le type de seuillage
     * @see cv::threshold()
     */
    void setThreshType(int type);

    /**
     * @brief Vide la liste de priorité.
     */
    void resetPriority();

    /**
     * @brief Ajoute le code de priorité donné à la liste de priorité des
     * traitements.
     *
     * Lors de l'appel à process(), la liste est parcourue dans l'ordre, et
     * les actions associées à chaque code sont exécutées les unes après les
     * autres
     */
    void addPriorList(PCode p);

    /* Private Methods */
protected:

    /**
     * @brief Helper function
     *
     * Choisi quelle est l'image source et la destination pour les
     * traitements entre `this->_img1` et `this->_img2`
     */
    void edImageProcessor::chooseSourceImage(cv::Mat **src, cv::Mat **dest){

    void contrast();
    void threshold();
    void erode();
    void dilate();

    /* Membres */
protected:
    std::deque<PCode> _priority;
    double  _contrastLvl;
    int  _threshLvl;
    int _threshType;
    int  _eltSize;

    bool _evenImg;

    cv::Mat _img1;
    cv::Mat _img2;
};

#endif // EDIMAGEPROCESSOR_H

