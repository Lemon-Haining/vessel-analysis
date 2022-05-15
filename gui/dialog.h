#ifndef DIALOG_H
#define DIALOG_H

#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui{ class Dialog; }
QT_END_NAMESPACE

class Dialog : public QWidget
{
    Q_OBJECT
public:
    Dialog(QWidget *parent = nullptr);
    ~Dialog();
private:
    Ui::Dialog *ui;
};

#endif