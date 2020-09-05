
#include "include/slicetool.h"
#include <iostream>

/***************
 * Constructeurs
 ***************/

SliceTool::SliceTool(QCustomPlot *h, QCustomPlot *v, ViewerCVGl* view) :
    _hTool(h),
    _vTool(v),
    _viewer(view),
    _editingH(false),
    _editingV(false),
    _x(0), _y(0)
{
    /* Connexion des signaux */
    QObject::connect(h, SIGNAL(mousePress(QMouseEvent*)), this, SLOT(editH()));
    QObject::connect(v, SIGNAL(mousePress(QMouseEvent*)), this, SLOT(editV()));

    h->addGraph();
    v->addGraph();

    /* Réglage de l'affichage */
    h->axisRect()->setAutoMargins(QCP::msNone);
    v->axisRect()->setAutoMargins(QCP::msNone);
    h->axisRect()->setMargins(QMargins(0,0,0,0));
    v->axisRect()->setMargins(QMargins(0,0,0,0));

    h->yAxis->setRange(0.0, 255.0);
    v->xAxis->setRange(0.0, 255.0);

    h->xAxis->setVisible(false);
    h->yAxis->setVisible(false);
    v->xAxis->setVisible(false);
    v->yAxis->setVisible(false);

    v->yAxis->setRangeReversed(true);
    v->graph(0)->setKeyAxis(v->yAxis);
    v->graph(0)->setValueAxis(v->xAxis);
}


/****************
 * Slots
 ****************/

bool
SliceTool::placeSlice(int x, int y){
    bool res = _editingH | _editingV;

    if (_editingH){
        res = true;
        _y = y;
        _viewer->setSliceToolY(y);
    }

    if (_editingV){
        res = true;
        _x = x;
        _viewer->setSliceToolX(x);
    }

    update(_editingH, _editingV);
    _editingH = false;
    _editingV = false;

    return res;
}


void
SliceTool::editH(){
    _editingH = true;
}

void
SliceTool::editV(){
    _editingV = true;
}

void
SliceTool::update(bool h, bool v){
    if (h || v){
        QImage img = _viewer->grabFrameBuffer(false);
        if (h){
            if (img.size().width() != _hData.size()){
                _hData.resize(img.size().width());
                _hTool->xAxis->setRange(0,_hData.size());
            }

            _hTool->graph(0)->clearData();
            for(int i=0; i<_hData.size(); i++){
                _hData[i] = qGray(img.pixel(i, _y)); // en niveaux de gris
                _hTool->graph(0)->addData(i,_hData[i]);
            }
            _hTool->replot();
        }
        if (v){
            if (img.size().height() != _vData.size()){
                _vData.resize(img.size().height());
                _vTool->yAxis->setRange(0,_vData.size());
            }

            _vTool->graph(0)->clearData();
            for(int i=0; i<_vData.size(); i++){
                _vData[i] = qGray(img.pixel(_x, i));
                _vTool->graph(0)->addData(i, _vData[i]);
            }
            _vTool->replot();
        }
    }
}

void
SliceTool::update(){
    update(true, true);
}
