#ifndef PARAMETERS_H
#define PARAMETERS_H

#include <QColorDialog>
#include <QDialog>
#include <QColor>

namespace Ui {
class Parameters;
}

class Parameters : public QDialog
{
    Q_OBJECT

public:
    explicit Parameters(QWidget *parent = 0);
    ~Parameters();

public slots:
    void updatePDelayEdit(int value);
    void updatePSpeedSlider(QString value);

    void updateARenderFromEdit();
    void updateARender(QString color);
    void updateAColorUi(QColor color); /**< Met à jour tous les widgets concernés */

    void openColorDialog();

    void resetUi();

public:
    const int& playerDelay() const;  /**< Délais entre chaque image (lecteur) */
    const QColor& annotColor() const;   /**< Couleur des annotations (dessins) */

    void accept();
    void reject();

protected:
    /**
     * Appelée par le constructeur pour
     * connecter signaux et slots
     */
    void connectSolts();

protected:
    int _playerDelay;
    QColor _annotColor;

private:
    Ui::Parameters *ui;
    QColorDialog* _colorDialog;
};

#endif // PARAMETERS_H
