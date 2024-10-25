#include "sectionscolumn.h"
#include "ui_sectionscolumn.h"

SectionsColumn::SectionsColumn(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::SectionsColumn)
{
    ui->setupUi(this);
}

SectionsColumn::~SectionsColumn()
{
    delete ui;
}
