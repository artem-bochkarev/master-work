#include "qapp.h"
#include "Genetic/CLaboratoryFactory.h"
#include <QFileDialog>

QApp::QApp(QWidget *parent, Qt::WFlags flags)
    : QMainWindow(parent, flags)
{
    ui.setupUi(this);
    QObject::connect( ui.actionExit, SIGNAL( triggered() ), this, SLOT( exitCmd() ) );
    QObject::connect( ui.actionOpen, SIGNAL( triggered() ), this, SLOT( openCmd() ) );
    QObject::connect( ui.actionView, SIGNAL( triggered() ), this, SLOT( viewCmd() ) );
    QObject::connect( ui.actionSave, SIGNAL( triggered() ), this, SLOT( saveCmd() ) );
}

QApp::~QApp()
{

}


bool QApp::openCmd()
{
    QString fileName = QFileDialog::getOpenFileName(this,
        tr("Open config file"), "", tr("Text config file (*.txt)"));
    laboratory = CLaboratoryFactory::getLaboratory( fileName.toAscii() );
    return true;
}
bool QApp::saveCmd()
{
    return false;
}
void QApp::exitCmd()
{

}
bool QApp::viewCmd()
{
    return false;
}