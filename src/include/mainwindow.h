#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDomDocument>

#include "apropos.h"
#include "parameters.h"

#include "slicetool.h"
#include "player.h"

#include "population.h"
#include "contours.h"
#include "lib/edimageprocessor.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    EdImageProcessor *imgProcessor();

public slots:
    /**
     * @brief Afficher / Cacher les outils de profils et de seuil
     */
    void showHideGraphicTools();


    /**
     * @brief Met à jour le numéro affiché de l'image courante
     * @param nbImg numéro à afficher
     */
    void updatePlayerSlider(int nbImg);

    /**
     * @brief Sauvegarde les résultats du composant actuel dans un fichier XML
     */
    void saveResults();
    void saveResInFile(QString fileName);
    //void savePresets();

    void enableContours(bool e);
    void enablePopulation(bool e);

    void setPlayerBarRange(int max);

private:
    void connectSlots(); /**< Connecte les signaux et slots */

private:
    Ui::MainWindow *ui;
    Apropos *aboutWin;
    Parameters *paramWin;

    QFileDialog* fileDialog;

private:
    SliceTool* _sliceTools;
    Player* _player;
    EdImageProcessor* _imgProc;

private:
    Population* _population;
    Contours* _contours;
};

#endif // MAINWINDOW_H
