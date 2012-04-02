#ifndef QAPP_H
#define QAPP_H

#include <QtGui/QMainWindow>
#include "ui_qapp.h"
#include "GeneticAPI/CLaboratory.h"

class QApp : public QMainWindow
{
    Q_OBJECT

public:
    QApp(QWidget *parent = 0, Qt::WFlags flags = 0);
    ~QApp();
    
    bool openCmd();
    bool saveCmd();
    void exitCmd();
    bool viewCmd();

private:
    Ui::QAppClass ui;
    CLaboratoryPtr laboratory;
};

#endif // QAPP_H
