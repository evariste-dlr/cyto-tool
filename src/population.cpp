#include <QObject>
#include <QLineEdit>
#include <QSlider>
#include <QCheckBox>
#include <QComboBox>
#include <QPushButton>
#include <QTableWidget>

#include "include/population.h"

Population::Population(MainWindow* w, QWidget* ui, ViewerCVGl *v, Player *p) :
    Component(w,ui),
    _viewer(v),
    _player(p),
    _invThresh(true),  _thresh(0), _contrast(1.0), _lumin(0),
    _eltSize(1), _init(false), _eltShape(cv::MORPH_ELLIPSE),
    _threshEn(false){
}

Population::~Population(){
}


void Population::enable(){
    copyOrigImage();

    // Connect particulier (disconnected si disabled)
    QObject::connect(_player, SIGNAL(fileListIdChanged(int)),
                     this, SLOT(copyOrigImage())
                    );

    if (!_init)  init();
}

void Population::copyOrigImage(){
    _viewer->originImage().copyTo(_origin);
    render();
    updateTableSize(_player->fileListLength());
}


void Population::disable(){
    QObject::disconnect(_player, SIGNAL(fileListIdChanged(int)),
                     this, SLOT(copyOrigImage())
                    );

    _viewer->showImage(_origin);
}




/**********************************
 *           Slots                *
 **********************************/


/* --------- Accesseurs ---------*/

void Population::enThresh(bool e){
    _threshEn = e;
    render();
}

void Population::setThresh(int t){
    _thresh = t;
    render();
}


void Population::setContrast(int c){
    _contrast = double(c) / 100;
    render();
}

void Population::setLumin(int l){
    _lumin = l;
    render();
}

void Population::setEltSize(int s){
    _eltSize = s;
}

void Population::setEltShape(int s){
    switch (s){
    case 0:
        _eltShape = cv::MORPH_ELLIPSE;
        break;
    case 1:
        _eltShape = cv::MORPH_RECT;
        break;
    case 3:
        _eltShape = cv::MORPH_CROSS;
    }
}

void Population::invThresh(bool i){
    _invThresh = i;
    if (_threshEn)  render();
}

/* --------- Opérations ---------*/

void Population::render(){
    linearTransform(_origin);

    if (_threshEn) {
        int type;
        if (_invThresh)  type = cv::THRESH_BINARY_INV;
        else             type = cv::THRESH_BINARY;

        if (_rendered.channels() == 3 || _rendered.channels() == 4)
            cv::cvtColor(_rendered, _tmpImage, cv::COLOR_RGB2GRAY);
        else
            _tmpImage = _rendered;
        cv::threshold(_tmpImage, _rendered, _thresh, 255, type);
    }

    _viewer->showImage(_rendered);
}


void Population::linearTransform(const cv::Mat &src){
    _rendered = cv::Mat::zeros(src.size(), src.type());

    // Contraste et luminosité pour chaque point
    for (int y=0; y<src.rows; y++){
      for (int x=0; x<src.cols; x++){
        for (int c=0; c<src.channels(); c++){
          _rendered.at<cv::Vec3b>(y,x)[c] =
           cv::saturate_cast<uchar>(_contrast * (src.at<cv::Vec3b>(y,x)[c]) + _lumin);
        }
      }
    }
}


void Population::resetLinear(){
    _ui->findChild<QSlider*>("popContrasteSlider")->setValue(100);
    _ui->findChild<QSlider*>("popLuminSlider")->setValue(0);
}


void Population::erode(){
    cv::Point2i center = cv::Point2i(_eltSize, _eltSize);

    cv::Mat kernel = cv::getStructuringElement(_eltShape,
                                               cv::Size(2*_eltSize+1, 2*_eltSize+1),
                                               center);
    _tmpImage = _rendered;

    cv::erode(_tmpImage, _rendered, kernel, center);
    _viewer->showImage(_rendered);
}

void Population::dilate(){
    cv::Point2i center = cv::Point2i(_eltSize, _eltSize);

    cv::Mat kernel = cv::getStructuringElement(_eltShape,
                                               cv::Size(2*_eltSize+1, 2*_eltSize+1),
                                               center);
    _tmpImage = _rendered;

    cv::dilate(_tmpImage, _rendered, kernel, center);
    _viewer->showImage(_rendered);
}


void Population::count(){
    cv::Mat img;
    if (_rendered.channels() == 3 || _rendered.channels() == 4)
        cv::cvtColor(_rendered, img, cv::COLOR_RGB2GRAY);
    else
        img = _rendered;

    cv::vector<cv::vector<cv::Point> > contours;

    // Trouver les contours des formes
    cv::findContours(img, contours, cv::RETR_LIST, cv::CHAIN_APPROX_SIMPLE);
    int n = contours.size();

    // Rendu
    for (int i=0; i<contours.size(); i++)
        cv::drawContours(_rendered, contours, i, cv::Scalar(255,0,0), 2);


    // Màj de l'interface

    QLineEdit* localLE = _ui->findChild<QLineEdit*>("popLocalLineEdit");
    QLineEdit* maxLE = _ui->findChild<QLineEdit*>("popMaxLineEdit");
    QLineEdit* minLE = _ui->findChild<QLineEdit*>("popMinLineEdit");
    QTableWidget* table = _ui->findChild<QTableWidget*>("popTable");

    localLE->setText(QString::number(n));

    if (n > maxLE->text().toInt())
        maxLE->setText(QString::number(n));

    if (n < minLE->text().toInt() || minLE->text().toInt() == 0)
        minLE->setText(QString::number(n));

    table->item(0, _player->currentId())->setText(QString::number(n));

    _viewer->showImage(_rendered);
}

/*************************
 *    TODO
 *************************/
void Population::equalize(){
    std::cout << " EQ " << std::endl;
}


/*************************
 * XML
 *************************/
void Population::updateXml(){
    _data = QDomDocument("Population");

    QDomElement root = _data.createElement("population");
    _data.appendChild(root);

    QDomElement node;
    QDomText value;
    QTableWidget* table = _ui->findChild<QTableWidget*>("popTable");

    for (int i=0; i<_player->fileListLength(); i++){
        node = _data.createElement("frame");
        node.setAttribute("id", i);
        value = _data.createTextNode(table->item(0,i)->text());
        node.appendChild(value);
        root.appendChild(node);
    }
}



void Population::resetDisplay(){
    render();
}


void Population::updateTableSize(int columns){
    int n = _ui->findChild<QTableWidget*>("popTable")->columnCount();
    int d = columns - n;

    if (d > 0) {
        for (int i=0; i<d; i++){
            _ui->findChild<QTableWidget*>("popTable")->insertColumn(n+i);
        }
    }
    else{
        for (int i=0; i>d; i--){
            _ui->findChild<QTableWidget*>("popTable")->removeColumn(n+i-1);
        }
    }
}

/******** Init ***********/
void Population::init(){
    _init = true;

    for (int i=0; i<_player->fileListLength(); i++){
        QTableWidgetItem* cell = new QTableWidgetItem();
        _ui->findChild<QTableWidget*>("popTable")->setItem(0, i, cell);
    }

    QObject::connect(_ui->findChild<QSlider*>("popSeuilSlider"),
                     SIGNAL(valueChanged(int)),
                     this, SLOT(setThresh(int))
                    );

    QObject::connect(_ui->findChild<QSlider*>("popContrasteSlider"),
                     SIGNAL(valueChanged(int)),
                     this, SLOT(setContrast(int))
                    );

    QObject::connect(_ui->findChild<QSlider*>("popLuminSlider"),
                     SIGNAL(valueChanged(int)),
                     this, SLOT(setLumin(int))
                    );

    QObject::connect(_ui->findChild<QSlider*>("popMorphoSlider"),
                     SIGNAL(valueChanged(int)),
                     this, SLOT(setEltSize(int))
                    );

    QObject::connect(_ui->findChild<QCheckBox*>("popSeuilInvCheckBox"),
                     SIGNAL(clicked(bool)),
                     this, SLOT(invThresh(bool))
                    );

    QObject::connect(_ui->findChild<QCheckBox*>("popSeuillageCheckBox"),
                     SIGNAL(clicked(bool)),
                     this, SLOT(enThresh(bool))
                    );

    QObject::connect(_ui->findChild<QPushButton*>("popResetLin"),
                     SIGNAL(pressed()),
                     this, SLOT(resetLinear())
                    );

    QObject::connect(_ui->findChild<QPushButton*>("popMorphoErode"),
                     SIGNAL(pressed()),
                     this, SLOT(erode())
                    );

    QObject::connect(_ui->findChild<QPushButton*>("popMorphoDilate"),
                     SIGNAL(pressed()),
                     this, SLOT(dilate())
                    );

    QObject::connect(_ui->findChild<QComboBox*>("popMorphoShape"),
                     SIGNAL(currentIndexChanged(int)),
                     this, SLOT(setEltShape(int))
                    );

    QObject::connect(_ui->findChild<QPushButton*>("cmdResetDisplay"),
                     SIGNAL(pressed()),
                     this, SLOT(resetDisplay())
                    );

    QObject::connect(_ui->findChild<QPushButton*>("popCompterButton"),
                     SIGNAL(pressed()),
                     this, SLOT(count())
                    );

    QObject::connect(_player, SIGNAL(fileListChangedLen(int)),
                     this, SLOT(updateTableSize(int))
                    );
}
