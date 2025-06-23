#ifndef CUSTOMCATEGORYCOMBOBOX_H
#define CUSTOMCATEGORYCOMBOBOX_H

#include <QWidget>
#include<QComboBox>
#include<QStyledItemDelegate>
#include<QPushButton>
#include<QHBoxLayout>
#include<QLabel>
#include<QMouseEvent>
#include"databasemanager.h"
#include<QPainter>
#include<QInputDialog>
#include<QMessageBox>

namespace Ui {
class CustomCategoryComboBox;
}

class CategoryItemDelegate:public QStyledItemDelegate{
    Q_OBJECT
public:
    explicit CategoryItemDelegate(QObject*parent=nullptr);
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
signals:
    void deleteCategory(const QString& category) const;
protected:
    bool editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index);
};

class CustomCategoryComboBox : public QComboBox
{
    Q_OBJECT

public:
    explicit CustomCategoryComboBox(QWidget *parent = nullptr);
    ~CustomCategoryComboBox();
    void setDBManager(DatabaseManager* dbManager, int userId){manager = dbManager,m_userId = userId;}
signals:
    void addCategoryRequested();
    void deleteCategoryRequested(const QString& category);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;

private:
    Ui::CustomCategoryComboBox *ui;
    DatabaseManager* manager;
    int m_userId;
    QPushButton* m_addButton;
private slots:
    void onAddCategoryClicked();

};

#endif // CUSTOMCATEGORYCOMBOBOX_H
