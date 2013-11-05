#ifndef QAPP_H
#define QAPP_H

#include <QtWidgets/QMainWindow>
#include <QTimer>
#include "ui_qapp.h"
#include "Genetic/CLaboratoryMulti.h"
#include "Genetic/CMap.h"
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
    QApp(QWidget *parent = 0, Qt::WindowFlags flags = 0);
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
    CLaboratoryMultiPtr laboratory;
	std::vector<CMapPtr> maps;
	boost::shared_ptr< QGraphicsScene > graphScene;
	int lastNumber;
	int maxFitness;
	Mode mode;
	Tools::Logger logger;
};

#endif // QAPP_H
