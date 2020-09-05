#ifndef APP
#define APP

#include "mainwindow.h"
#include "apropos.h"

#include "viewercvgl.h"

#include <QApplication>

class App : public QApplication{

public:
    App(int argc, char* argv[]);

private:
    void connect();

private:
    MainWindow _mainWin;
    Apropos _aboutWin;

};

#endif // APP

