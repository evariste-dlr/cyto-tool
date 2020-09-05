#include "include/component.h"

Component::Component(MainWindow* w, QWidget *ui) :
    QObject(),
    _window(w),
    _ui(ui),
    _enabled(false){
}

Component::~Component(){}


void
Component::setEnabled(bool e){
    _ui->setEnabled(e);
    _enabled = e;
    if (e)  enable();
    else    disable();
}


QDomDocument*
Component::getXml(){
    updateXml();
    return &_data;
}


bool
Component::isEnabled() const{
    return _enabled;
}
