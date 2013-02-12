#ifndef QAPP_H
#define QAPP_H

#include <QtGui/QMainWindow>
#include <QTimer>
#include "ui_qapp.h"
#include "GeneticAPI/CLaboratory.h"
#include "Tools/Logger.h"

class QApp : public QMainWindow
{
    Q_OBJECT

	enum Mode
	{
		DISABLED,
		STOPPED,
		PAUSED,
		RUNNING
	};
public:
    QApp(QWidget *parent = 0, Qt::WFlags flags = 0);
    ~QApp();
    
protected slots:
    bool openCmd();
    bool saveCmd();
    void exitCmd();
    bool viewCmd();
	void buttonPressed();
	void timerEvent();
	void drawGenerationInfo();

private:
	void freeLaboratory();
    Ui::QAppClass ui;
	QTimer timer;
    CLaboratoryPtr laboratory;
	std::vector<CMapPtr> maps;
	boost::shared_ptr< QGraphicsScene > graphScene;
	int lastNumber;
	int maxFitness;
	Mode mode;
	Tools::Logger logger;
};

#endif // QAPP_H
