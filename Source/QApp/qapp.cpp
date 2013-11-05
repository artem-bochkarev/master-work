#include "qapp.h"
#include "Genetic/CLaboratoryFactory.h"
#include <QtWidgets/QFileDialog>
#include "Genetic/CMapFactory.h"
#include "generationsViewer.hpp"

QApp::QApp(QWidget *parent, Qt::WindowFlags flags)
    : QMainWindow(parent, flags)
{
    ui.setupUi(this);
    QObject::connect( ui.actionExit, SIGNAL( triggered() ), this, SLOT( exitCmd() ) );
    QObject::connect( ui.actionOpen, SIGNAL( triggered() ), this, SLOT( openCmd() ) );
    QObject::connect( ui.actionView, SIGNAL( triggered() ), this, SLOT( viewCmd() ) );
    QObject::connect( ui.actionSave, SIGNAL( triggered() ), this, SLOT( saveCmd() ) );
	QObject::connect( ui.pushButton, SIGNAL( clicked() ), this, SLOT( buttonPressed() ) );
	QObject::connect( &timer, SIGNAL( timeout() ), this, SLOT( timerEvent() ) );

	CMapPtr map1 = CMapFactory::getMap( "map1.txt" );
    maps.push_back( map1 );
	mode = DISABLED;
	timer.setInterval(50);
	lastNumber = 0;
	maxFitness = 10;
}

QApp::~QApp()
{
	freeLaboratory();
}


bool QApp::openCmd()
{
    QString fileName = QFileDialog::getOpenFileName(this,
        tr("Open config file"), "", tr("Text config file (*.txt)"));
	if ( fileName != "" )
	{
        try
        {
            laboratory = CLaboratoryFactory::getLaboratory( logger, std::string(fileName.toStdString()) );
		    if ( laboratory.get() != 0 )
		    {
			    mode = STOPPED;
			    ui.deviceLabel->setText( laboratory->getStrategy()->getDeviceType().c_str() );
			    ui.sizeLabel->setText( QString(laboratory->getStrategy()->getPoolInfo().c_str()) );
		    }
        }catch ( std::runtime_error& error )
        {
            ui.deviceLabel->setText("ERROR:");
            ui.sizeLabel->setText( error.what() );
        }
	}
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
    generationsViewer viewer( this );
    viewer.setLaboratory( laboratory );
    viewer.exec();
    return false;
}

void QApp::freeLaboratory()
{
	//timer.Stop();
	if ( laboratory.get() != 0 )
    laboratory->pause();
}

void QApp::buttonPressed()
{
	switch (mode)
	{
	case (DISABLED):
		break;
	case (STOPPED):
		laboratory->setMaps( maps );
		//laboratory->start();
		graphScene = boost::shared_ptr<QGraphicsScene>( new QGraphicsScene() );
		timer.start();
        ui.pushButton->setText("Pause");
		mode = RUNNING;
		break;
	case (PAUSED):
		//laboratory->start();
		timer.start();
        ui.pushButton->setText("Pause");
		mode = RUNNING;
		break;
	case (RUNNING):
		//laboratory->pause();
		timer.stop();
		ui.pushButton->setText("Continue");
		mode = PAUSED;
		break;
	}
}

void QApp::timerEvent()
{
    try
    {
        if (mode == RUNNING)
            laboratory->runForTime( 1 );
        drawGenerationInfo();
    }catch( std::exception& ex )
    {
        laboratory->pause();
        ui.deviceLabel->setText("ERROR:");
        ui.sizeLabel->setText( ex.what() );
        timer.stop();
        mode = DISABLED;
    }catch( ... )
    {
        laboratory->pause();
        ui.deviceLabel->setText("ERROR:");
        ui.sizeLabel->setText( "unknown error" );
        timer.stop();
        mode = DISABLED;
    }
}

void QApp::drawGenerationInfo()
{
    int n = laboratory->getGenerationNumber();
	if ( n == 0 )
		return;
    QString str("Generations: ");
    str.append( QString().setNum( n ) );
    ui.label->setText(str);
    int start = lastNumber;
   
    int px = 4 * lastNumber;
    int py = 4 * laboratory->getMaxFitness( lastNumber );
	QPen pen;
	pen.setColor( QColor( 80, 80, 250 ) );
	pen.setWidth( 2 );
    for ( int i = start; i < n; ++i )
    {
        int x = 4 * i;
        int y = 4 * laboratory->getMaxFitness( i );
		graphScene->addLine( px, py, x, y, pen );
        px = x;
        py = y;
    }
    px = 4 * lastNumber;
    py = 4 * laboratory->getAvgFitness( lastNumber );
    pen.setColor( QColor( 80, 250, 80 ) );
    for ( int i= start; i < n; ++i )
    {
        int x = 4 * i;
        int y = 4 * laboratory->getAvgFitness( i );
		graphScene->addLine( px, py, x, y, pen );
        px = x;
        py = y;
    }
	ui.graphicsView->setScene( graphScene.get() );
	ui.graphicsView->setSceneRect( QRectF( 0, 0, n*4, 300 ));
	ui.graphicsView->centerOn( n*4 - 100, 150 );
	ui.graphicsView->show();
	lastNumber = n - 1;
}
