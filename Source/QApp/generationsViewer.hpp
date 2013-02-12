#pragma once
#include <QDialog>
#include "ui_generationsViewer.h"
#include "GeneticAPI/CLaboratory.h"

class generationsViewer : public QDialog
{
    Q_OBJECT

public:
    generationsViewer( QWidget *parent = 0, Qt::WFlags flags = 0 );
    ~generationsViewer();

    void setLaboratory( CLaboratoryPtr laboratory );
    
protected slots:

private:
    Ui::Dialog ui;
    CLaboratoryPtr laboratory;
};