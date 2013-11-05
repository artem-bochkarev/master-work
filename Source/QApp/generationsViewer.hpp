#pragma once
#include <QtWidgets/QDialog>
#include "ui_generationsViewer.h"
#include "GeneticAPI/CLaboratory.h"

class generationsViewer : public QDialog
{
    Q_OBJECT

public:
    generationsViewer( QWidget *parent = 0, Qt::WindowFlags flags = 0 );
    ~generationsViewer();

    void setLaboratory( CLaboratoryPtr laboratory );
    
protected slots:

private:
    Ui::Dialog ui;
    CLaboratoryPtr laboratory;
};