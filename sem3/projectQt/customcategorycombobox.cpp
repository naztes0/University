#include "customcategorycombobox.h"
#include "ui_customcategorycombobox.h"

CustomCategoryComboBox::CustomCategoryComboBox(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::CustomCategoryComboBox)
{
    ui->setupUi(this);
}

CustomCategoryComboBox::~CustomCategoryComboBox()
{
    delete ui;
}
