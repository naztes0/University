#ifndef SECTIONSCOLUMN_H
#define SECTIONSCOLUMN_H

#include <QWidget>
#include<QStatusBar>

namespace Ui {
class SectionsColumn;
}

class SectionsColumn : public QWidget
{
    Q_OBJECT

public:
    explicit SectionsColumn(QWidget *parent = nullptr);
    ~SectionsColumn();

    void setStatusBar(QStatusBar*statusBar);
private slots:
    void on_homeButton_clicked();

private:
    Ui::SectionsColumn *ui;
    QStatusBar*statusBar;
};

#endif // SECTIONSCOLUMN_H
