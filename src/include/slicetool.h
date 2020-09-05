#ifndef SLICETOOL_H
#define SLICETOOL_H

/******************************************
 * slicetool.h
 * ****************************************
 *
 * Gestion des outils graphiques de coupe :
 * * viewerSliceToolH
 * * viewerSliceToolV
 */

#include <QObject>
#include <QVector>

#include "lib/qcustomplot.h"
#include "viewercvgl.h"

/**
 * @brief Gestion des outils graphiques de coupe.
 */
class SliceTool : public QObject{

    Q_OBJECT

public:
    SliceTool(QCustomPlot* h, QCustomPlot* v, ViewerCVGl* view);

public slots:
    bool placeSlice(int x, int y); /**< Place les axes et désactive l'édition */

    void editH();        /**< Active l'édition des axes de coupe */
    void editV();

    /**
     * Met à jour l'affichage sans changer les axes
     */
    void update(bool h, bool v);
    void update();

protected:
    QCustomPlot* _hTool;    /**< Pointeur vers le rendu horizontal */
    QCustomPlot* _vTool;    /**< Pointeur vers le rendu vertical */
    ViewerCVGl* _viewer;    /**< Pointeur vers le viewer */

    QVector<int> _hData;    /**< Données horizontales */
    QVector<int> _vData;    /**< Données verticales */

    int _x;
    int _y;

    bool _editingH;         /**< Vrai si on est en train d'éditer les axes */
    bool _editingV;
};

#endif
