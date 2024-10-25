#ifndef SECTIONSCOLUMN_H
#define SECTIONSCOLUMN_H

#include <QWidget>

namespace Ui {
class SectionsColumn;
}

class SectionsColumn : public QWidget
{
    Q_OBJECT

public:
    explicit SectionsColumn(QWidget *parent = nullptr);
    ~SectionsColumn();

private:
    Ui::SectionsColumn *ui;
};

#endif // SECTIONSCOLUMN_H
