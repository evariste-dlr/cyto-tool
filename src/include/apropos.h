#ifndef APROPOS_H
#define APROPOS_H

#include <QDialog>

namespace Ui {
class Apropos;
}

class Apropos : public QDialog
{
    Q_OBJECT

public:
    explicit Apropos(QWidget *parent = 0);
    ~Apropos();

private:
    Ui::Apropos *ui;
};

#endif // APROPOS_H
