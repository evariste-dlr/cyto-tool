#include "include/app.h"

App::App(int argc, char* argv[]) :
    QApplication(argc, argv){
    connect();
    _mainWin.show();
}


void App::connect(){

}
