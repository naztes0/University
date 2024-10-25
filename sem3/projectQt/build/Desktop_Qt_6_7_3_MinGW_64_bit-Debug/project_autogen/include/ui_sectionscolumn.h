/********************************************************************************
** Form generated from reading UI file 'sectionscolumn.ui'
**
** Created by: Qt User Interface Compiler version 6.7.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SECTIONSCOLUMN_H
#define UI_SECTIONSCOLUMN_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_SectionsColumn
{
public:
    QVBoxLayout *verticalLayout;
    QSpacerItem *verticalSpacer_2;
    QPushButton *pushButton;
    QPushButton *pushButton_2;
    QPushButton *pushButton_3;
    QPushButton *pushButton_4;
    QSpacerItem *verticalSpacer;

    void setupUi(QWidget *SectionsColumn)
    {
        if (SectionsColumn->objectName().isEmpty())
            SectionsColumn->setObjectName("SectionsColumn");
        SectionsColumn->resize(414, 616);
        verticalLayout = new QVBoxLayout(SectionsColumn);
        verticalLayout->setObjectName("verticalLayout");
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        verticalSpacer_2 = new QSpacerItem(20, 40, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        verticalLayout->addItem(verticalSpacer_2);

        pushButton = new QPushButton(SectionsColumn);
        pushButton->setObjectName("pushButton");

        verticalLayout->addWidget(pushButton);

        pushButton_2 = new QPushButton(SectionsColumn);
        pushButton_2->setObjectName("pushButton_2");

        verticalLayout->addWidget(pushButton_2);

        pushButton_3 = new QPushButton(SectionsColumn);
        pushButton_3->setObjectName("pushButton_3");

        verticalLayout->addWidget(pushButton_3);

        pushButton_4 = new QPushButton(SectionsColumn);
        pushButton_4->setObjectName("pushButton_4");

        verticalLayout->addWidget(pushButton_4);

        verticalSpacer = new QSpacerItem(20, 400, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Preferred);

        verticalLayout->addItem(verticalSpacer);


        retranslateUi(SectionsColumn);

        QMetaObject::connectSlotsByName(SectionsColumn);
    } // setupUi

    void retranslateUi(QWidget *SectionsColumn)
    {
        SectionsColumn->setWindowTitle(QCoreApplication::translate("SectionsColumn", "Form", nullptr));
        pushButton->setText(QCoreApplication::translate("SectionsColumn", "PushButton", nullptr));
        pushButton_2->setText(QCoreApplication::translate("SectionsColumn", "PushButton", nullptr));
        pushButton_3->setText(QCoreApplication::translate("SectionsColumn", "PushButton", nullptr));
        pushButton_4->setText(QCoreApplication::translate("SectionsColumn", "PushButton", nullptr));
    } // retranslateUi

};

namespace Ui {
    class SectionsColumn: public Ui_SectionsColumn {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SECTIONSCOLUMN_H
