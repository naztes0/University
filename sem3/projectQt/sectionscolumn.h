#ifndef SECTIONSCOLUMN_H
#define SECTIONSCOLUMN_H

#include <QWidget>
#include<QPushButton>
#include<QDebug>

namespace Ui {
class SectionsColumn;
}

class SectionsColumn : public QWidget
{
    Q_OBJECT

public:
    explicit SectionsColumn(QWidget *parent = nullptr);
    ~SectionsColumn();

signals:
    void homeButtonClicked();

private slots:
    void on_homeButton_clicked();

private:
    Ui::SectionsColumn *ui;

};

#endif // SECTIONSCOLUMN_H
