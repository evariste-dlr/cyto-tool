#include <QtSvg/QtSvg>
#include <QtSvg/QSvgRenderer>

#include "include/mainwindow.h"
#include "ui_mainwindow.h"

/*****************************************
 *      Constructeur / Destructeur       *
 *****************************************/

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    aboutWin(new Apropos(this)),
    paramWin(new Parameters(this)),
    _imgProc(new EdImageProcessor(this))
{
    /* Préparation de la fenêtre */
    ui->setupUi(this);

    /* Initialisation des éléments internes */
    _sliceTools = new SliceTool(ui->viewerSliceToolH,
                                ui->viewerSliceToolV,
                                ui->viewerRenderer);

    _player = new Player(ui->viewerRenderer, 50, this);

    _imgProc->addPriorList(EdImageProcessor::THRESH);
    ui->viewerRenderer->setImgProc(_imgProc);

    /* On cache les éléments à cacher */
    ui->viewerSliceToolH->hide();
    ui->viewerSliceToolV->hide();
    ui->viewerThreshToolLabel->hide();
    ui->viewerThreshToolSlider->hide();

    /* Composants */
    _population = new Population(this,
                                 ui->PopulVLayout,
                                 ui->viewerRenderer,
                                 _player);
    _contours = new Contours(this,
                             ui->ContoursVLayout,
                             ui->viewerRenderer,
                             _player);

    fileDialog = new QFileDialog(this, Qt::Dialog);

    /* Connexion signaux / slots */
    connectSlots();
}

MainWindow::~MainWindow()
{
    delete _population;
    delete _contours;
    delete _player;
    delete _sliceTools;
    delete _imgProc;
    delete fileDialog;
    delete aboutWin;
    delete paramWin;
    delete ui;
}


/*****************************************
 *         Gestion des événements        *
 *****************************************/

void MainWindow::saveResults(){
    if (_population->isEnabled() | _contours->isEnabled()){
        fileDialog->setAcceptMode(QFileDialog::AcceptSave);

        QObject::connect(fileDialog, SIGNAL(fileSelected(QString)),
                         this, SLOT(saveResInFile(QString)));

        fileDialog->exec();
    }
    else{
        QErrorMessage dialog(this);
        dialog.showMessage("Aucun composant n'est actif.");
        dialog.exec();
    }
}

void MainWindow::saveResInFile(QString fileName){
    QDomDocument* xml;
    if    (_population->isEnabled()) xml = _population->getXml();
    else if (_contours->isEnabled()) xml = _contours->getXml();

    QFile file(fileName);
    if (! file.open(QIODevice::ReadWrite | QIODevice::Text)){
        std::cerr << "[E] Impossible d'enregistrer le fichier  : "
                  << fileName.toStdString() << std::endl;
    }
    else{
        QTextStream out(&file);
        xml->save(out, 2);
        file.close();
    }
}

/*****************************************
 *         Connexions
 ****************************************/

void MainWindow::enableContours(bool e){
    if (e && _population->isEnabled()){
        ui->actionPopulation->setChecked(false);
        _population->setEnabled(false);
    }

    if (e){
        ui->pluginTab->setCurrentIndex(1);
    }

    _contours->setEnabled(e);
}

void MainWindow::enablePopulation(bool e){
    if (e && _contours->isEnabled()){
        ui->actionContours->setChecked(false);
        _contours->setEnabled(false);
    }

    if (e){
        ui->pluginTab->setCurrentIndex(0);
    }
    _population->setEnabled(e);
}

void MainWindow::connectSlots(){
    QObject::connect(ui->actionOutil_de_coupe_verticale, SIGNAL(triggered()),
                     this, SLOT(showHideGraphicTools()));

    QObject::connect(ui->actionOutil_de_coupe_horizontale, SIGNAL(triggered()),
                     this, SLOT(showHideGraphicTools()));

    QObject::connect(ui->actionOutil_de_seuilage, SIGNAL(triggered()),
                     this, SLOT(showHideGraphicTools()));


    QObject::connect(ui->actionCe_programme, SIGNAL(triggered()),
                     aboutWin, SLOT(exec()));

    QObject::connect(ui->actionReglages, SIGNAL(triggered()),
                     paramWin, SLOT(exec()));

    QObject::connect(ui->actionAfficher_les_dessins, SIGNAL(triggered()),
                     this, SLOT(showHideGraphicTools()));

    QObject::connect(ui->viewerThreshToolSlider, SIGNAL(valueChanged(int)),
                     ui->viewerRenderer, SLOT(setThreshold(int)));

    QObject::connect(ui->viewerRenderer, SIGNAL(mouseClicked(int,int)),
                     _sliceTools, SLOT(placeSlice(int,int)));

    QObject::connect(ui->viewerRenderer, SIGNAL(sceneChanged()),
                     _sliceTools, SLOT(update()));

    QObject::connect(ui->actionOuvrir_une_liste_images, SIGNAL(triggered()),
                     _player, SLOT(openFileDialog()));
    QObject::connect(ui->actionOuvir_un_repertoire, SIGNAL(triggered()),
                     _player, SLOT(openDirDialog()));

    /* Player bar */
    QObject::connect(ui->playerBarFButton, SIGNAL(pressed()),
                     _player, SLOT(forward()));

    QObject::connect(ui->playerBarFwButton, SIGNAL(pressed()),
                     _player, SLOT(last()));

    QObject::connect(ui->playerBarRButton, SIGNAL(pressed()),
                     _player, SLOT(previousImg()));

    QObject::connect(ui->playerBarRwButton, SIGNAL(pressed()),
                     _player, SLOT(first()));

    /* Player bar Indice images */
    QObject::connect(_player, SIGNAL(fileListChangedLen(int)),
                     ui->playerFViewLabel, SLOT(setNum(int)));
    QObject::connect(_player, SIGNAL(fileListIdChanged(int)),
                     ui->playerFViewSpinBox, SLOT(setValue(int)));
    QObject::connect(_player, SIGNAL(fileListIdChanged(int)),
                     ui->playerHSlider, SLOT(setValue(int)));
    QObject::connect(_player, SIGNAL(fileListChangedLen(int)),
                     this, SLOT(setPlayerBarRange(int)));


    /* Enabling Components layout */
    QObject::connect(ui->actionPopulation, SIGNAL(toggled(bool)),
                     this, SLOT(enablePopulation(bool)));
    QObject::connect(ui->actionContours, SIGNAL(toggled(bool)),
                     this, SLOT(enableContours(bool)));

    /* Results and datasets */
    QObject::connect(ui->actionEnregistrer_les_r_sultats, SIGNAL(triggered()),
                     this, SLOT(saveResults()));
}

void MainWindow::setPlayerBarRange(int max){
    ui->playerHSlider->setRange(1, max);
}

void MainWindow::showHideGraphicTools(){

    if (ui->actionOutil_de_coupe_verticale->isChecked()
        || ui->actionOutil_de_coupe_horizontale->isChecked() )
        ui->viewerRenderer->showTools();
    else
        ui->viewerRenderer->hideTools();

    if (ui->actionOutil_de_coupe_verticale->isChecked())
        ui->viewerSliceToolV->show();
    else
        ui->viewerSliceToolV->hide();

    if (ui->actionOutil_de_coupe_horizontale->isChecked())
        ui->viewerSliceToolH->show();
    else
        ui->viewerSliceToolH->hide();

    if (ui->actionOutil_de_seuilage->isChecked()){
        ui->viewerThreshToolLabel->show();
        ui->viewerThreshToolSlider->show();
    }
    else{
        ui->viewerThreshToolLabel->hide();
        ui->viewerThreshToolSlider->hide();
    }

    if (ui->actionAfficher_les_dessins->isChecked())
        ui->viewerRenderer->showDrawings();
    else
        ui->viewerRenderer->hideDrawings();
}



void MainWindow::updatePlayerSlider(int nbImg){
    ui->playerHSlider->setMinimum(1);
    ui->playerHSlider->setMaximum(nbImg);
}


EdImageProcessor* MainWindow::imgProcessor(){
    return _imgProc;
}
