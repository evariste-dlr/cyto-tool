#ifndef PLAYER_H
#define PLAYER_H

#include <QWidget>
#include <QFileDialog>
#include <QVector>
#include <QString>

#include "viewercvgl.h"

/**
 * @brief Classe gérant le lecteur d'images
 *
 * La classe player est utilisée pour gérer le
 * lecteur de fichiers images. Dans la mesure
 * où les fichiers sont lus à la volée, le temps
 * de latence entre chaque image est limité. Il
 * est calculé lors de la lecture du premier
 * fichier.
 */
class Player : public QObject{

    Q_OBJECT

public:
    explicit Player(
            ViewerCVGl* view,   /**< Le widget servant d'affichage */
            int timeStep = 50,  /**< Temps entre chaque image */
            QWidget *parent=0);

    ~Player();

    /*
     * Utilisation du buffer
     * circulaire
     */
public:
    void setCurrent(const int& id);  /**< Lit et enregistre le fichier à afficher*/
    void setPrevious(const int& id); /**< Lit et enregistre le fichier précédent */
    void setNext(const int& id);  /**< Lit et enregistre le prochain fichier */

    int fileListLength();
    int currentId();


    /*
     * Utilisation des fichiers
     */
public slots:
    /**
     * @brief Ajoute la liste des fichiers du répertoire
     *      donné au vecteur d'images à lire
     * @return true si le répertoire existe
     */
    bool openDirectory(const QString& dirName);

    /**
     * @brief Ajoute le fichier à la liste de lecture
     * @return true si le fichier existe
     */
    bool openFile(const QString& fileName);
    bool openFiles(const QStringList &fileNames);

    /**
     * @brief vide la liste de lecture
     */
    void clearFileList();


    /*
     * MàJ depuis l'extérieur
     */
public slots:
    void nextImg();     /**< Affiche l'image suivante du buffer */
    void previousImg(); /**< Affiche l'image précédente du buffer (rwd) */

    void openFileDialog();
    void openDirDialog();

    /*
     * Slots
     */
public slots:
    void playPause();
    void stop();
    void forward();
    void rewind();
    void first();
    void last();

    void setTimeStep(int ts);

signals:
    void fileListChangedLen(int l);
    void fileListIdChanged(int i);

    /*
     * Protected functions
     */
protected:
    /**
     * Créer le buffer en fonction du premier fichier de la liste.
     * Le fichier est alors lu et l'image est considérée comme
     * l'image courante (et est affichée).
     */
    void createBuffer();

    int nxtBufferId();
    int prvBufferId();

protected:
    ViewerCVGl* _viewer;

    QVector<QString> _fileNames;
    QString _dirName;
    QVector<cv::Mat> _buffer; /**< Buffer circulaire */

    int _timeStep;  /**< Temps entre deux images en ms*/
    int _currentId; /**< Id de l'image actuellement affichée */
    int _curBufferId; /**< Id de la case actuelle du buffer */

    QFileDialog* _fileDialog;
    QFileDialog* _dirDialog;

    bool _playing;
    bool _init;
};

#endif // PLAYER_H

