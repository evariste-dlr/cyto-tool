#ifndef COMPONENT
#define COMPONENT

#include <QObject>
#include <QWidget>
#include <QMainWindow>
#include <QDomDocument>

class MainWindow;

/**
 * @brief Classe de base pour les composants
 * @see Population
 * @see Fourier
 */
class Component : public QObject {

    Q_OBJECT

public :
    Component(MainWindow* w, QWidget* ui);
    virtual ~Component() = 0;

    bool isEnabled() const;

public slots:
    /**
     * @brief Slot utilisé pour activer et désactiver le composant.
     * Lorsque setEnabled() est appelé, l'ui est activée ou
     * désactivée, et le slot approprié parmis `enable()` et `disable()`
     * est appelé.
     * @see enable
     * @see disable
     */
    void setEnabled(bool);

    /**
     * @brief Instructions permettant d'activer et initialiser le
     * composant.
     * @see setEnabled
     */
    virtual void enable() = 0;

    /**
     * @brief Instructions permettant de désactiver le composant et
     * rendre la main à l'interface principale.
     * @see setEnabled
     */
    virtual void disable() = 0;

    /**
     * @brief Retourne les résultats obtenus sous forme de document DOM
     * @return un QDomDocument représentant les résultats obtenus
     * @see updateXml
     */
    virtual QDomDocument* getXml();

protected:
    /**
     * @brief Instructions permettant de créer le document XML présentant
     * les résultats obtenus (appelé par `getXml()`)
     * @see getXml
     */
    virtual void updateXml() = 0;

protected:
    MainWindow* _window;
    QWidget* _ui;
    QDomDocument _data;

    bool _enabled;
};

#endif // COMPONENT
