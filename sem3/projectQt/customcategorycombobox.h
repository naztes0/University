#ifndef CUSTOMCATEGORYCOMBOBOX_H
#define CUSTOMCATEGORYCOMBOBOX_H

#include <QWidget>
#include<QComboBox>
#include<QStyledItemDelegate>
#include<QPushButton>
#include<QHBoxLayout>
#include<QLabel>
#include<MouseEvent>

namespace Ui {
class CustomCategoryComboBox;
}

class CustomCategoryComboBox : public QWidget
{
    Q_OBJECT

public:
    explicit CustomCategoryComboBox(QWidget *parent = nullptr);
    ~CustomCategoryComboBox();

private:
    Ui::CustomCategoryComboBox *ui;
};

#endif // CUSTOMCATEGORYCOMBOBOX_H
