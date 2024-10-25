#include "sectionscolumn.h"
#include "ui_sectionscolumn.h"

SectionsColumn::SectionsColumn(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::SectionsColumn),
    statusBar(nullptr)
{
    ui->setupUi(this);

}

SectionsColumn::~SectionsColumn()
{
    delete ui;
}

void SectionsColumn::setStatusBar(QStatusBar*bar){
    statusBar=bar;
}
void SectionsColumn::on_homeButton_clicked(){
    if(statusBar){
        statusBar->showMessage("Ok");
    }
}
