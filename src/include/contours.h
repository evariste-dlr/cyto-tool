#ifndef CONTOURS
#define CONTOURS

#include "lib/qcustomplot.h"

#include "viewercvgl.h"
#include "player.h"
#include "component.h"

/**
 * @brief Prédicat "Moyenne" de contrainte d'homogénéïté.
 */
class _MeanPredicate {
public:
    _MeanPredicate(int t);

    /**
     * @brief Opérateur d'appel
     * @param ims   image source
     * @param p     point de l'image
     * @return  `true` si `ims[p]` est à une distance de la moyenne des valeurs
     * des points déjà traités inférieure à un seuil
     * @see _thresh
     */
    bool operator()(const cv::Mat& ims, const cv::Point2i& p);

private:
    int _thresh;
    int _sum; /**< Somme des valeurs traitées */
    int _n;   /**< Nombre de valeurs traitées */
};


/**
 * @brief Prédicat "Valeur" de contrainte d'homogénéïté.
 */
class _ValuePredicate {
public:
    _ValuePredicate(int v, int t);

    /**
     * @brief Opérateur d'appel
     * @param ims   image source
     * @param p     point de l'image
     * @return  `true` si `ims[p]` est à une distance de la valeur du pixel germe
     * inférieure à un seuil
     * @see _thresh
     */
    bool operator()(const cv::Mat& ims, const cv::Point2i& p);

private:
    int _val;
    int _thresh;
};


/**
 * @brief Composant "Contours".
 * Permet d'extraire des descripteurs de contours d'une cellule
 * sélectionnée.
 * * Descripteurs de Fourier
 * * Signature polaire
 *
 * La sélection se fait par croissance de région sur un germe
 * défini à la souris.
 */
class Contours : public Component {

    Q_OBJECT

public:
    /**
     * @brief Nombre de points d'intervalle pour calculer la
     * tangente au contour
     */
    static const int D_FOURIER = 3;

public:
    Contours(MainWindow* w,     /**< Fenêtre */
             QWidget* ui,       /**< UI */
             ViewerCVGl* v,     /**< Widget d'affichage des images */
             Player* p          /**< Lecteur d'images */
            );

    ~Contours();

public slots:
    void enable();
    void disable();
    void copyOrigImage();

    void editSeed();                /**< Set the seed to be edited */
    void placeSeed(int x, int y);   /**< Place le germe en `(x,y)` si `_editSeed` */
    void setThresh(int t);          /**< Seuil pour le critère d'homogénéïté */
    void setHomoType(int h);        /**< Critère d'homogénéïté */
    void setHarmNb(int n);          /**< Nombre d'harmoniques à prendre en compte **/

    void displayContours(bool d);

    void render();

protected:
    void init();
    void initPlots();
    virtual void updateXml();

    void updatePolarDesc(const QVector<double>& a, const QVector<double>& m);

    /**
     * @brief Dessiner les QCustomPlot consernant la forme sélectionnée :
     * @see _shape
     * @see _flat
     */
    void drawShapePlots();

    /**
     * @brief Met à jour `_render` en ajoutant le contour de la région
     * sélectionnée à partir du germe, en appliquant une croissance
     * de région. Met aussi à jour `_contour` avec ce même contour.
     *
     * Si `_seedPlaced` est faux, regGrow se contente de copier l'image
     * d'origine.
     *
     * @see setThresh
     * @see setHomoType
     * @see placeSeed
     */
    void regGrow();


    /**
     *  Segmentation par croissance de région
     */
    template<class BinaryPredicate>
    static void
    segmReg(const cv::Mat& ims,        /**< Image source */
                 cv::Mat& imd,        /**< Image de destination */
                 BinaryPredicate hmg, /**< Critère d'homogénéïté (implémente operator(cv::Mat, cv::Point)) */
                 const cv::Point& seed);    /**< Germe */

protected:
    /**
     * @return le voisin `n` en connectivité v4 du point `p` si `n` est
     * compris entre 0 et 3. Sinon, retourne `p`
     * @see v8
     */
    static cv::Point2i
    v4(cv::Point2i p, int n);

    /**
     * @return le voisin `n` en connectivité v8 du point `p` si `n` est
     * compris entre 0 et 3. Sinon, retourne `p`
     * @see v4
     */
    static cv::Point2i
    v8(cv::Point2i p, int n);

protected:
    /**
     * @brief Liste des prédicats d'homogénéïté pour la croissance de région
     */
    enum HomoPredicateType{
        VAL, MEAN
    };

protected:
    ViewerCVGl* _viewer;        /**< Widget d'affichage des images */
    Player* _player;            /**< Lecteur d'images */

    QCustomPlot* _shape;        /**< Graphique représentant la forme en polaire */
    QCustomPlot* _flat;         /**< Graphique représentant la signature polaire */
    QCustomPlot* _var;          /**< Graphique des variations de la tangente */
    QCustomPlot* _fourier;      /**< Diagramme des harmoniques */

    QCPCurve* _shapeCurve;
    QCPCurve* _flatCurve;
    QCPCurve* _varCurve;
    QCPBars* _fourierCurve;
    QCPCurve* _firstB; // Première biscectrice pour l'affichage

    cv::Mat _origin;            /**< Image d'origine en niveaux de gris */
    cv::Mat _originColor;       /**< Image d'origine en couleur */
    cv::Mat _rendered;          /**< Image à afficher après traitements */
    cv::Mat _mask;              /**< Masque de la forme à analyser */

    std::vector<double> _fourierDesc; /**< Descripteur de fourier du contour */

    /**
     * Descripteur basé sur la signature polaire.
     * Descripteur à trois dimensions :
     * * variance de la signature
     * * nombre de groupes de points au dessus de la médiane (nombre de "pics")
     * * nombre moyen de points par angle dans la signature
     */
    std::vector<double> _polarDesc;

    cv::Point2i _seed;          /**< Germe pour la croissance de région */
    int _thresh;                /**< Seuil du critère pour la croissance de région */
    bool _editSeed;             /**< Édite-t-on le germe ? */
    bool _seedPlaced;           /**< Le germe a été placé par l'utilisateur */

    bool _displayContours;      /**< Afficher uniquement les contours */
    cv::vector<cv::Point> _contour; /**< Contour de la forme à analyser */
    int _harmNb;                /**< Nombre d'harmoniques */

    HomoPredicateType _homPred; /**< Prédicat d'homogénéïté pour la croissance de région */

    bool _init;                 /**< Le composant a été initialisé */
};

#endif // CONTOURS

