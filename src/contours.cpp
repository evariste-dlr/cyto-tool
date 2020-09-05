#include <QObject>
#include <QSlider>
#include <QComboBox>
#include <QCheckBox>
#include <QSpinBox>
#include <QPushButton>

#include <math.h>
#include <stack>

#include "lib/qmathstools.h"
#include "include/contours.h"


Contours::Contours(MainWindow* w, QWidget* ui, ViewerCVGl *v, Player *p) :
    Component(w,ui),
    _viewer(v), _player(p),
    _seed(cv::Point2i(0,0)),
    _thresh(0), _editSeed(false), _seedPlaced(false),
    _homPred(HomoPredicateType::MEAN), _harmNb(10),
    _init(false)
{
    initPlots();
}

Contours::~Contours(){}


/**********************************
 *           SLOTS
 **********************************/

void
Contours::enable(){
    copyOrigImage();

    // Connect particulier (disconnected si disabled)
    QObject::connect(_player, SIGNAL(fileListIdChanged(int)),
                     this, SLOT(copyOrigImage())
                    );
    if (!_init)  init();
}

void
Contours::copyOrigImage(){
    _editSeed = false;
    _seedPlaced = false;

    _viewer->originImage().copyTo(_originColor);
    if (_originColor.channels() == 3 || _originColor.channels() == 4)
        cv::cvtColor(_originColor, _origin, cv::COLOR_RGB2GRAY);
    else
        _originColor.copyTo(_origin);

    _originColor.copyTo(_rendered);
    _mask.create(_origin.size(), CV_8UC1);
    render();
}

void
Contours::disable(){
    QObject::disconnect(_player, SIGNAL(fileListIdChanged(int)),
                     this, SLOT(copyOrigImage())
                    );
    _viewer->showImage(_originColor);

    _editSeed = false; // au cas où
    _seedPlaced = false;
}


void
Contours::editSeed(){
    _editSeed = true;
    _seedPlaced = false;
}

void
Contours::placeSeed(int x, int y){
    if (_editSeed){
        x = (x > _rendered.cols) ? _rendered.cols : x;
        y = (y > _rendered.rows) ? _rendered.rows : y;
        _seed.x = (x < 0) ? 0 : x;
        _seed.y = (y < 0) ? 0 : y;
        _seedPlaced = true;
        render();
    }
    _editSeed = false;
}

void
Contours::setThresh(int t){
    _thresh = t;
    if (_seedPlaced)   render();
}

void
Contours::setHomoType(int h){
    switch(h){
    case 0:
        _homPred = HomoPredicateType::MEAN;
        break;
    case 1:
        _homPred = HomoPredicateType::VAL;
        break;
    }
    render();
}


void
Contours::displayContours(bool d){
    _displayContours = d;
    render();
}


void
Contours::setHarmNb(int n){
    if (n > 0) _harmNb = n;
    render();
}

void
Contours::render(){
    regGrow();
    _viewer->showImage(_rendered);
}


/********************************
 *          PROTECTED
 ********************************/

void
Contours::regGrow(){
    if (_seedPlaced){
        cv::Mat tmp;
        cv::Mat mask;
        //cv::cvtColor(_origin, tmp, cv::COLOR_RGB2GRAY);

        uchar val = _origin.at<uchar>(_seed);

        switch(_homPred){
        case HomoPredicateType::MEAN:
            segmReg(_origin, mask, _MeanPredicate(_thresh), _seed);
            break;
        case HomoPredicateType::VAL:
            segmReg(_origin, mask, _ValuePredicate((int)(val), _thresh), _seed);
        }

        // Sélection de l'image à afficher : l'originale ou le masque (binaire)
        if (_displayContours){
            _originColor.copyTo(_rendered);
        }else{
            mask.copyTo(_rendered);
        }

        cv::vector<cv::vector<cv::Point> > ct;
        cv::findContours(mask, ct, cv::RETR_TREE, cv::CHAIN_APPROX_NONE);
        if (ct.size() > 0){
            _contour = ct[0];

            cv::drawContours(_rendered, ct, 0, cv::Scalar(0,0,255), 1);
            _mask = cv::Mat::zeros(mask.size(), CV_8UC1);
            cv::drawContours(_mask, ct, 0, 255, 1);

            _mask.copyTo(tmp);
            segmReg(tmp, _mask, _ValuePredicate(0,1), _seed);
            drawShapePlots();
        }
    }
    else{
        _origin.copyTo(_rendered);
    }
}


template<class BinaryPredicate>
void Contours::segmReg(const cv::Mat& ims, cv::Mat& imd,
                       BinaryPredicate hmg, const cv::Point& seed){

    // On n'accepte que des matrice de type uchar
    CV_Assert(ims.depth() == CV_8U);

    std::stack<cv::Point2i> pile;  // La pile
    cv::Mat visit = cv::Mat::zeros(ims.size(), CV_8UC1); // Carte des visites

    /* initialisation */
    imd = cv::Mat::zeros(ims.size(), CV_8UC1);

    visit.at<uchar>(seed) = 1;
    pile.push(seed);

     while (!pile.empty()){
        cv::Point2i p = pile.top();
        pile.pop();
        imd.at<uchar>(p) = 255; // Label = 1 (255)

        /* Pour chaque voisin */
        for(int i=0; i<4; i++){
            cv::Point2i q = v4(p, i);
            if ((q.x > 0) && (q.y > 0) &&
                (q.x < ims.cols) && (q.y < imd.rows)){
                if (visit.at<uchar>(q) == 0 && hmg(ims, q)){
                    pile.push(q);
                    visit.at<uchar>(q) = 1;
                }
            }
        }
    }
}


void
Contours::updateXml(){
    _data = QDomDocument("Contours");

    QDomElement root = _data.createElement("descripteurs");
    _data.appendChild(root);

    QDomElement signNode = _data.createElement("signature");
    QDomElement fourierNode = _data.createElement("fourier");
    QDomElement node;
    QDomText value;

    node = _data.createElement("variance");
    value = _data.createTextNode(QString::number(_polarDesc[0]));
    node.appendChild(value);
    signNode.appendChild(node);

    node = _data.createElement("pics");
    value = _data.createTextNode(QString::number(_polarDesc[1]));
    node.appendChild(value);
    signNode.appendChild(node);

    for (int i=0; i<_fourierDesc.size(); i++){
        node = _data.createElement("harm");
        node.setAttribute("n", i);
        value = _data.createTextNode(QString::number(_fourierDesc[i]));
        node.appendChild(value);
        fourierNode.appendChild(node);
    }

    root.appendChild(signNode);
    root.appendChild(fourierNode);
}


void
Contours::drawShapePlots(){
    // Trouver le centre de gravité
    cv::Point2i p;
    uint sumX, sumY, coefs; // < pour le calcul de la moyenne

    sumX = 0;
    sumY = 0;
    coefs = 0;
    for (p.y=0; p.y<_origin.rows; (p.y)++){
        for (p.x=0; p.x<_origin.cols; (p.x)++){
            if (_mask.at<uchar>(p) != 0){
                sumX += (255 - (int)(_origin.at<uchar>(p))) * p.x;
                sumY += (255 - (int)(_origin.at<uchar>(p))) * p.y;
                coefs += 255 - (int)(_origin.at<uchar>(p));
            }
        }
    }

    p.x = (coefs == 0) ? 0 : sumX / coefs;
    p.y = (coefs == 0) ? 0 : sumY / coefs;
    cv::circle(_rendered, p, 2, cv::Scalar(255,0,0),-1); // Affichage de G

    // Convertir les contours (et translater pour centrer en G)
    QVector<double> x(_contour.size()+1);
    QVector<double> y(_contour.size()+1);
    double xmax = .0;
    double ymax = .0;
    for (int i=0; i<_contour.size(); i++){
        x[i] = _contour[i].x - p.x;
        y[i] = - _contour[i].y + p.y;
        if (x[i] > xmax) xmax = x[i];
        if (y[i] > ymax) ymax = y[i];
    }
    x[x.size()-1] = x[0];
    y[y.size()-1] = y[0];
    double margin = 3.0;
    double max = (xmax > ymax) ? xmax : ymax;
    max += margin;

    // Calcul de la taille de la fenêtre d'affichage
    _shapeCurve->setData(x,y);
    _shape->xAxis->setRange(-max, max);
    _shape->yAxis->setRange(-max, max);
    _shape->replot();


    // Calcul de la signature polaire = contour exprimé en coordonnées polaires
    QVector<double> m(_contour.size()); // magnitude
    QVector<double> a(_contour.size()); // angle
    for (uint i=0; i<_contour.size(); i++){
        a[i] = atan2(y[i], x[i]);
        m[i] = sqrt((x[i]*x[i]) + (y[i]*y[i]));
    }
    QMathsTools::normalize(m, 1.0); // On normalise la magnitude

    _flatCurve->setData(a,m);
    _flat->rescaleAxes();
    _flat->yAxis->setRangeLower(0.0);
    _flat->replot();

    // Mise à jour des descripteurs
    updatePolarDesc(a,m);


    // Calcul du différentiel d'angle de tangente pour fourier
    if (x.size() > D_FOURIER){
        int size = x.size() / D_FOURIER;
        QVector<double> d(size);
        QVector<double> e(size);
        QVector<double> diff(size);
        double an = .0;
        double anprev = atan2(y[D_FOURIER]-y[0], x[D_FOURIER]-x[0]);
        for (uint i=0; i<size-1; i++){
            e[i] = double(i) * ((2*M_PI) / size); // <- normalisation dans [0;2PI]
            an = atan2(y[i*D_FOURIER + D_FOURIER]-y[i*D_FOURIER],
                       x[i*D_FOURIER + D_FOURIER]-x[i*D_FOURIER]); // angle
            d[i] = an;
            diff[i] = an - anprev;
            anprev = an;
        }

        _varCurve->setData(e, d);
        _var->rescaleAxes();
        _var->replot();


        // Fourier
        int nh = (diff.size() > _harmNb) ? _harmNb : diff.size();

        _fourierDesc.clear();
        cv::dft(diff.toStdVector(), _fourierDesc, cv::DFT_REAL_OUTPUT);
        _fourierDesc.resize(nh);

        _fourierCurve->clearData();
        double min = .0;
        max = .0;
        for (int i=0; i<_fourierDesc.size(); i++){
            _fourierCurve->addData((double)(i), _fourierDesc[i]);
            min = (min < _fourierDesc[i]) ? min : _fourierDesc[i];
            max = (max > _fourierDesc[i]) ? max : _fourierDesc[i];
        }

        _fourier->xAxis->setRange(.0, (double)(_fourierDesc.size()));
        _fourier->yAxis->setRange(min, max);
        _fourier->replot();
    } //< si x.size > D_FOURIER
}


void
Contours::updatePolarDesc(const QVector<double>& a, const QVector<double>& m){
    _polarDesc.clear();
    _polarDesc.resize(3);

    // Première dimension : la variance
    _polarDesc[0] = QMathsTools::variance(m);

    // Deuxième dimension : le nombre de groupes au dessus de la médiane
    double med = QMathsTools::median(m);
    int n = 0; bool in = false;
    for (int i=0; i<m.size(); i++){
        if (!in && m[i] > med){
            n++; // on a trouvé un pic
            in = true;
        }
        if (in && m[i] <= med){
            in = false; // On sors du pic
        }
    }
    // On traite la périodicité : que ce passe-t-il pour le dernier point ?
    //  -> On supprime un groupe compté double si le premier et le dernier point sont dans un pic
    if (in && m[0] > med) n--;
    _polarDesc[1] = n;

    // Troisième dimension : nombre moyen de points du contour pour un angle donné
    //...
    _polarDesc[2] = 1.0;
}

/****************** Utils *****************/
cv::Point2i
Contours::v4(cv::Point2i p, int n){
    cv::Point2i r;
    switch(n){
    case 0:
        r.x = p.x+1;
        r.y = p.y;
        break;
    case 1:
        r.x = p.x;
        r.y = p.y+1;
        break;
    case 2:
        r.x = p.x-1;
        r.y = p.y;
        break;
    case 3:
        r.x = p.x;
        r.y = p.y-1;
        break;
    default:
        r.x = p.x;
        r.y = p.y;
    }
    return r;
}

cv::Point2i
Contours::v8(cv::Point2i p, int n){
    cv::Point2i r;
    switch(n){
    case 0:
        r.x = p.x+1;
        r.y = p.y;
        break;
    case 1:
        r.x = p.x;
        r.y = p.y+1;
        break;
    case 2:
        r.x = p.x-1;
        r.y = p.y;
        break;
    case 3:
        r.x = p.x;
        r.y = p.y-1;
        break;
    case 4:
        r.x = p.x+1;
        r.y = p.y+1;
        break;
    case 5:
        r.x = p.x-1;
        r.y = p.y+1;
        break;
    case 6:
        r.x = p.x-1;
        r.y = p.y-1;
        break;
    case 7:
        r.x = p.x+1;
        r.y = p.y-1;
        break;
    default:
        r.x = p.x;
        r.y = p.y;
    }
    return r;
}



/******** Init ***********/
void
Contours::init(){
    _init = true;

    QObject::connect(_ui->findChild<QPushButton*>("conGerme"),
                     SIGNAL(pressed()), this, SLOT(editSeed()));

    QObject::connect(_ui->findChild<QSlider*>("conThresh"),
                     SIGNAL(valueChanged(int)), this, SLOT(setThresh(int)));

    QObject::connect(_ui->findChild<QComboBox*>("conHomo"),
                     SIGNAL(activated(int)), this, SLOT(setHomoType(int)));

    QObject::connect(_viewer, SIGNAL(mouseClickedImage(int,int)),
                     this, SLOT(placeSeed(int,int)));

    QObject::connect(_ui->findChild<QCheckBox*>("conDisplayContours"),
                     SIGNAL(clicked(bool)), this, SLOT(displayContours(bool)));

    QObject::connect(_ui->findChild<QSpinBox*>("conHarmoniques"),
                     SIGNAL(valueChanged(int)), this, SLOT(setHarmNb(int)));
}

void
Contours::initPlots(){
    /* Mise en place des graphiques */
    _shape = _ui->findChild<QCustomPlot*>("conShapeViewer");
    _flat = _ui->findChild<QCustomPlot*>("conFlatSViewer");
    _var  = _ui->findChild<QCustomPlot*>("conVarViewer");
    _fourier = _ui->findChild<QCustomPlot*>("conFourierViewer");

    /* Réglage de l'affichage des graphiques */
    _shape->axisRect()->setAutoMargins(QCP::msNone);
    _shape->axisRect()->setMargins(QMargins(5,5,5,5));
    _flat->axisRect()->setAutoMargins(QCP::msNone);
    _flat->axisRect()->setMargins(QMargins(5,5,5,5));
    _var->axisRect()->setAutoMargins(QCP::msNone);
    _var->axisRect()->setMargins(QMargins(5,5,5,5));
    _fourier->axisRect()->setAutoMargins(QCP::msNone);
    _fourier->axisRect()->setMargins(QMargins(5,5,5,5));

    _shape->xAxis->setRange(-1.0, 1.0);
    _shape->yAxis->setRange(-1.0, 1.0);
    _flat->xAxis->setRange(0.0, 2.0 * M_PI);
    _flat->yAxis->setRange(-1.0, 1.0);

    _shape->xAxis->setTickLength(0,3);
    _shape->yAxis->setTickLength(0,3);
    _shape->xAxis->setSubTickLength(0,1);
    _shape->yAxis->setSubTickLength(0,1);
    _flat->xAxis->setTickLength(0,3);
    _flat->yAxis->setTickLength(0,3);
    _flat->xAxis->setSubTickLength(0,1);
    _flat->yAxis->setSubTickLength(0,1);
    _var->xAxis->setTickLength(0,3);
    _var->yAxis->setTickLength(0,3);
    _var->xAxis->setSubTickLength(0,1);
    _var->yAxis->setSubTickLength(0,1);
    _fourier->xAxis->setTickLength(0,3);
    _fourier->yAxis->setTickLength(0,3);
    _fourier->xAxis->setSubTickLength(0,1);
    _fourier->yAxis->setSubTickLength(0,1);

    QPen pen(QColor::fromRgb(120,120,120));
    _shape->xAxis->setBasePen(pen);
    _shape->xAxis->setTickPen(pen);
    _shape->xAxis->setSubTickPen(pen);
    _shape->yAxis->setBasePen(pen);
    _shape->yAxis->setTickPen(pen);
    _shape->yAxis->setSubTickPen(pen);
    _flat->xAxis->setBasePen(pen);
    _flat->xAxis->setTickPen(pen);
    _flat->xAxis->setSubTickPen(pen);
    _flat->yAxis->setBasePen(pen);
    _flat->yAxis->setTickPen(pen);
    _flat->yAxis->setSubTickPen(pen);
    _var->xAxis->setBasePen(pen);
    _var->xAxis->setTickPen(pen);
    _var->xAxis->setSubTickPen(pen);
    _var->yAxis->setBasePen(pen);
    _var->yAxis->setTickPen(pen);
    _var->yAxis->setSubTickPen(pen);
    _fourier->xAxis->setBasePen(pen);
    _fourier->xAxis->setTickPen(pen);
    _fourier->xAxis->setSubTickPen(pen);
    _fourier->yAxis->setBasePen(pen);
    _fourier->yAxis->setTickPen(pen);
    _fourier->yAxis->setSubTickPen(pen);

    //* Création des courbes
    _flatCurve = new QCPCurve(_flat->xAxis, _flat->yAxis);
    _shapeCurve = new QCPCurve(_shape->xAxis, _shape->yAxis);
    _varCurve = new QCPCurve(_var->xAxis, _var->yAxis);
    _fourierCurve = new QCPBars(_fourier->xAxis, _fourier->yAxis);

    _firstB = new QCPCurve(_var->xAxis, _var->yAxis);
    _firstB->setPen(pen);
    _firstB->addData(0.0, -M_PI);
    _firstB->addData(2*M_PI, M_PI);

    _flatCurve->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDisc, 2));
    _flatCurve->setLineStyle(QCPCurve::LineStyle::lsNone);
    _varCurve->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDisc, 2));
    _varCurve->setLineStyle(QCPCurve::LineStyle::lsNone);

    _shape->addPlottable(_shapeCurve);
    _flat->addPlottable(_flatCurve);
    _var->addPlottable(_varCurve);
    _var->addPlottable(_firstB);

    _var->rescaleAxes();
    _var->replot();
}


/*************************************************
 *             PRÉDICATS D'HOMOG.
 *************************************************/

_MeanPredicate::_MeanPredicate(int t) :
    _thresh(t),
    _sum(0),
    _n(0)
{}

bool
_MeanPredicate::operator()(const cv::Mat& ims, const cv::Point2i& p){
    _sum += (int)(ims.at<uchar>(p));
    _n += 1;
    int mean = _sum / _n;
    return ((int)(ims.at<uchar>(p)) - mean < _thresh)
            && ((int)(ims.at<uchar>(p)) - mean > -_thresh);
}


_ValuePredicate::_ValuePredicate(int v, int t) :
    _val(v),
    _thresh(t)
{}

bool
_ValuePredicate::operator()(const cv::Mat& ims, const cv::Point2i& p){
    return ((int)(ims.at<uchar>(p)) - _val < _thresh)
            && ((int)(ims.at<uchar>(p)) - _val > -_thresh);
}


