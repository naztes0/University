#include "sectionscolumn.h"
#include "ui_sectionscolumn.h"

SectionsColumn::SectionsColumn(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::SectionsColumn)
{
    ui->setupUi(this);
    qDebug()<<"SectionsColumn constructor was called";
    connect(ui->homeButton,&QPushButton::clicked,this,&SectionsColumn::on_homeButton_clicked);
    if (ui->homeButton) {
        qDebug() << "Home button found and connected";
        // Переконаємося, що кнопка активна
        ui->homeButton->setEnabled(true);
    } else {
        qDebug() << "Home button not found";
    }
    connect(ui->transactionsListButton,&QPushButton::clicked,this,&SectionsColumn::on_transactionsLButton_clicked);
}

SectionsColumn::~SectionsColumn()
{
    delete ui;
}

void SectionsColumn::on_homeButton_clicked(){
    qDebug()<<"Clicke Home Button";
    emit homeButtonClicked();
}
void SectionsColumn::on_transactionsLButton_clicked(){
    qDebug()<<"Clicked Transactions list button";
    emit transactionsListButtonClicked();
}
