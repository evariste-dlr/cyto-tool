#include <numeric>
#include <chrono>
#include <iostream>

#include <QDir>

#include "include/player.h"


Player::Player(ViewerCVGl *view, int timeStep, QWidget *parent) :
    QObject(parent),
    _fileDialog(new QFileDialog),
    _dirDialog(new QFileDialog),
    _viewer(view),
    _timeStep(timeStep),
    _playing(false),
    _init(false)
{
    _dirName = "N/A";

    _fileDialog->setFileMode(QFileDialog::ExistingFiles);
    _fileDialog->setAcceptMode(QFileDialog::AcceptOpen);

    _dirDialog->setFileMode(QFileDialog::Directory);
    _dirDialog->setOption(QFileDialog::ShowDirsOnly);
    _dirDialog->setAcceptMode(QFileDialog::AcceptOpen);

    QStringList filters;
    filters << "Fichiers Image (*.png *.jpg *.jpeg *.tif *.tiff *.bmp)"
            << "Tous les fichiers (*)";
    _fileDialog->setNameFilters(filters);

    QObject::connect(_fileDialog, SIGNAL(filesSelected(QStringList)),
            this, SLOT(openFiles(QStringList)));

    QObject::connect(_dirDialog, SIGNAL(fileSelected(QString)),
            this, SLOT(openDirectory(QString)));
}

Player::~Player(){
    delete _fileDialog;
    delete _dirDialog;
}


void
Player::openFileDialog(){
    _fileDialog->open();
}

void
Player::openDirDialog(){
    _dirDialog->open();
}


bool
Player::openDirectory(const QString &dirName){
    std::cout << "Open Dir : " << dirName.toStdString() << std::endl;

    QStringList filters;
    filters << "*.jpg" << "*.jpeg" << "*.png" << "*.tiff" << "*.tif" << "*.bmp"
            << "*.JPG" << "*.JPEG" << "*.PNG";

    QDir dir(dirName);
    dir.setNameFilters(filters);
    QStringList files = dir.entryList(QDir::Files | QDir::Readable, QDir::Name);

    for (int i=0; i<files.length(); i++)
        files[i] = dirName + "/" + files[i];

    if (files.length() > 0)
        openFiles(files);

    return true;
}


bool
Player::openFiles(const QStringList &fileNames){
    std::cout << "Open Files " << std::endl;

    if (fileNames.size() == 1 && _init){
        openFile(fileNames[0]);
    }
    else{

        _fileNames.clear();
        for (int i=0; i<fileNames.size(); i++){
            _fileNames.append(fileNames[i]);
            std::cout << "   " << fileNames[i].toStdString() << std::endl;
        }

        createBuffer();
        _currentId = 0;

        emit fileListChangedLen(_fileNames.size());
        emit fileListIdChanged(1);
    }
    return true;
}


bool
Player::openFile(const QString &fileName){
    std::cout << "Open File " << std::endl;
    _fileNames.append(fileName);

    emit fileListChangedLen(_fileNames.size());
    return true;
}


void
Player::clearFileList(){
    _fileNames.clear();
    _dirName = "N/A";
    _currentId = 0;
    _curBufferId = 1;

    emit fileListChangedLen(0);
    emit fileListIdChanged(0);
}

int
Player::fileListLength(){
    return _fileNames.size();
}

int
Player::currentId(){
    return _currentId;
}

void
Player::setCurrent(const int &id){
    if (id >= 0 && id < _fileNames.size()){
        _buffer[_curBufferId] = cv::imread(_fileNames[id].toStdString());
    }
}


void
Player::setNext(const int &id){
    if (id >= 0 && id < _fileNames.size()){
        _buffer[nxtBufferId()] = cv::imread(_fileNames[id].toStdString());
    }
}


void
Player::setPrevious(const int &id){
    if (id >= 0 && id < _fileNames.size()){
        _buffer[prvBufferId()] = cv::imread(_fileNames[id].toStdString());
    }
}




/********************* SLOTS **********************/

void
Player::nextImg(){
    // Si l'image demandée existe
    if (_currentId < _fileNames.size() - 1){
        _currentId++;
        _curBufferId = nxtBufferId();

        // Si il y a une image suivante :
        if (_currentId < _fileNames.size()){
            setNext(_currentId + 1);
        }

        // On affiche
        _viewer->showImage(_buffer[_curBufferId]);

        emit fileListIdChanged(_currentId + 1);
    }
}

void
Player::previousImg(){
    if (_currentId > 0){
        _currentId--;
        _curBufferId = prvBufferId();

        if (_currentId > 0){
            setPrevious(_currentId - 1);
        }

        _viewer->showImage(_buffer[_curBufferId]);

        emit fileListIdChanged(_currentId + 1);
    }
}

void
Player::playPause(){

}


void
Player::stop(){

}


void
Player::forward(){
    nextImg();
}


void
Player::rewind(){
    previousImg();
}


void
Player::first(){
    _currentId = 0;
    setCurrent(0);
    setPrevious(0);
    if (_fileNames.size() > 1)
        setNext(1);
    else
        setNext(0);

    _viewer->showImage(_buffer[_curBufferId]);
    emit fileListIdChanged(_currentId + 1);
}


void
Player::last(){
    _currentId = _fileNames.size() - 1;
    setCurrent(_currentId);
    setNext(_currentId);
    if(_currentId > 0)
        setPrevious(_currentId - 1);
    else
        setPrevious(0);

    _viewer->showImage(_buffer[_curBufferId]);
    emit fileListIdChanged(_currentId + 1);
}


void
Player::setTimeStep(int ts){
    _timeStep = ts;
}


/************************** PROTECTED **********************/

void
Player::createBuffer(){
    _buffer.resize(3);

    cv::Mat src;

    auto start = std::chrono::system_clock::now();
    src = cv::imread(_fileNames[0].toStdString());
    _viewer->showImage(src);
    auto end = std::chrono::system_clock::now();
    std::chrono::duration<double> diff = end - start;

    // TODO minimiser le timestep des parametres
    std::cout << "time : " << diff.count() << std::endl;

    _buffer[1] = src;
    _buffer[0] = src;

    _currentId = 0;
    _curBufferId = 1;

    emit fileListIdChanged(1);
    _init = true;

    if (_fileNames.size() > 1)
        setNext(1);
    else
        setNext(0);
}


int
Player::nxtBufferId(){
    int res = _curBufferId < 2 ? (_curBufferId +1) : 0;
    return res;
}

int
Player::prvBufferId(){
    int res = _curBufferId > 0 ? (_curBufferId -1) : 2;
    return res;
}
