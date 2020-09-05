#include "edimageprocessor.h"

EdImageProcessor::EdImageProcessor(QObject *parent) :
    QObject(parent),
    _contrastLvl(0.0),
    _threshLvl(0),
    _threshType(cv::THRESH_TOZERO ),
    _eltSize(0),
    _evenImg(false){
}

EdImageProcessor::~EdImageProcessor(){}


/*****************************
 *  Mutateurs / Accesseurs
 * **************************/

void EdImageProcessor::setContrast(double lvl){
    if (lvl >= 0)
        _contrastLvl = lvl;
}

void EdImageProcessor::setEltSize(int size){
    _eltSize = size;
}

void EdImageProcessor::setThresh(int lvl){
    _threshLvl = lvl;
}

void EdImageProcessor::setThreshType(int type){
    _threshType = type;
}


/****************************
 *  Traitement
 * **************************/

cv::Mat EdImageProcessor::process(cv::Mat src){
    _evenImg = false;

    _img1 = src;
    _img2 = cv::Mat::zeros(src.size(), src.type());

    std::deque<PCode>::iterator it = _priority.begin();
    while(it != _priority.end()){
        switch (*it) {
         case CTRST:
            contrast();
            break;

         case THRESH:
            threshold();
            break;

         case ERODE:
            erode();
            break;

         case DILATE:
            dilate();
        }
        it++;
    }

    if (_evenImg) return _img2;
    else return _img1;
}


void EdImageProcessor::resetPriority(){
    _priority.clear();
}

void EdImageProcessor::addPriorList(PCode p){
    _priority.push_back(p);
}


/**************************
 * Méthodes de traitement
 * ************************/


void edImageProcessor::chooseSourceImage(cv::Mat **src, cv::Mat **dest){
    if (_evenImg){
        *dst = &_img1;
        *src = &_img2;
    }
    else{
        *dst = &_img2;
        *src = &_img1;
    }
}


void EdImageProcessor::contrast(){
    cv::Mat *src = NULL;
    cv::Mat *dst = NULL;

    this->chooseSourceImage(&src, &dst);

    // Contraste pour chaque point
    for (int y=0; y<src->rows; y++){
      for (int x=0; x<src->cols; x++){
        for (int c=0; c<src->channels(); c++){
          dst->at<cv::Vec3b>(y,x)[c] =
           cv::saturate_cast<uchar>(_contrastLvl * (src->at<cv::Vec3b>(y,x)[c]));
        }
      }
    }

    _evenImg = !_evenImg;
}


void EdImageProcessor::threshold(){
    cv::Mat *src = NULL;
    cv::Mat *dst = NULL;

    this->chooseSourceImage(&src, &dst);

    cv::threshold(*src, *dst, _threshLvl, 255, _threshType);
    _evenImg = !_evenImg;
}



void EdImageProcessor::erode(){
    cv::Mat *src = NULL;
    cv::Mat *dst = NULL;

    this->chooseSourceImage(&src, &dst);

    cv::erode( *src,
               *dst,
               cv::getStructuringElement( cv.MORPH_ELLIPSE, (_eltSize, _eltSize))
             );

    _evenImg = !_evenImg;
}

void EdImageProcessor::dilate(){
    cv::Mat *src = NULL;
    cv::Mat *dst = NULL;

    this->chooseSourceImage(&src, &dst);

    cv::dilate( *src,
                *dst,
                cv::getStructuringElement( cv.MORPH_ELLIPSE, (_eltSize, _eltSize))
              );

    _evenImg = !_evenImg;
}




