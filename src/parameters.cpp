#include "include/parameters.h"
#include "ui_parameters.h"

Parameters::Parameters(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Parameters),
    _colorDialog(new QColorDialog),
    _playerDelay(100),
    _annotColor(QColor(Qt::red))
{
    ui->setupUi(this);
    resetUi();
    connectSolts();
}

Parameters::~Parameters()
{
    delete _colorDialog;
    delete ui;
}


/*****************
 * Connection Signaux / Slots
 *****************/

void
Parameters::connectSolts(){
    QObject::connect(ui->pSpeedSlider, SIGNAL(valueChanged(int)),
                     this, SLOT(updatePDelayEdit(int)));

    QObject::connect(ui->pDelayEdit, SIGNAL(textChanged(QString)),
                     this, SLOT(updatePSpeedSlider(QString)));

    QObject::connect(ui->aColorEdit, SIGNAL(editingFinished()),
                     this, SLOT(updateARenderFromEdit()));

    QObject::connect(ui->aColorSelecter, SIGNAL(pressed()),
                     this, SLOT(openColorDialog()));
}

void
Parameters::accept(){
    _playerDelay = ui->pSpeedSlider->value();
    _annotColor = ui->aColorRender->palette().window().color();
    done(QDialog::Accepted);
}

void
Parameters::reject(){
    resetUi();
    done(QDialog::Rejected);
}


void
Parameters::openColorDialog(){
    _colorDialog->setCurrentColor(ui->aColorRender->palette().window().color());
    connect(_colorDialog, SIGNAL(colorSelected(QColor)),
            this, SLOT(updateAColorUi(QColor)));
    _colorDialog->open();
}



/******************
 * Updaters
 ******************/
void
Parameters::updatePDelayEdit(int value){
    QString str = QString::number(value);
    ui->pDelayEdit->setText(str);
}

void
Parameters::updatePSpeedSlider(QString value){
    ui->pSpeedSlider->setValue(value.toInt());
}

void
Parameters::updateARender(QString color){
    // On fait confiance aux vérifs de Qt
    QPalette p;
    p.setColor(QPalette::Background, QColor(color));
    ui->aColorRender->setPalette(p);
}

void
Parameters::updateARenderFromEdit(){
    updateARender(ui->aColorEdit->text());
}

void
Parameters::updateAColorUi(QColor color){
    QPalette p;
    p.setColor(QPalette::Background, color);
    ui->aColorRender->setPalette(p);
    ui->aColorEdit->setText(color.name());
}

void
Parameters::resetUi(){
    ui->pSpeedSlider->setValue(_playerDelay);

    QPalette p;
    p.setColor(QPalette::Background, _annotColor);
    ui->aColorRender->setPalette(p);
    ui->aColorEdit->setText(_annotColor.name());
}

/*****************
 * Accesseurs
 *****************/

const int&
Parameters::playerDelay() const {
    return playerDelay();
}

const QColor&
Parameters::annotColor() const {
    return annotColor();
}
