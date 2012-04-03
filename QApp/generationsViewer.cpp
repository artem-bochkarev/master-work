#include "generationsViewer.hpp"

generationsViewer::generationsViewer( QWidget *parent, Qt::WFlags flags )
: QDialog( parent, flags )
{
    ui.setupUi(this);
}

generationsViewer::~generationsViewer( )
{

}

void generationsViewer::setLaboratory( CLaboratoryPtr lab )
{
    laboratory = lab;
    size_t n = lab->getGenerationNumber();
    ui.tableWidget->setRowCount(n);
    for ( int i=0; i<n; ++i )
    {
        QTableWidgetItem *newItem = new QTableWidgetItem( "Generation " );
        ui.tableWidget->setItem(i, 0, newItem);
        QTableWidgetItem *newItem2 = new QTableWidgetItem( QString().setNum( lab->getMaxFitness(i), 'g', 2 ) );
        ui.tableWidget->setItem(i, 1, newItem2);
        QTableWidgetItem *newItem3 = new QTableWidgetItem( QString().setNum( lab->getAvgFitness(i), 'g', 2 ) );
        ui.tableWidget->setItem(i, 2, newItem3);
    }
}