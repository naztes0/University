#include "customcategorycombobox.h"
#include "ui_customcategorycombobox.h"



CategoryItemDelegate::CategoryItemDelegate(QObject *parent)
    :QStyledItemDelegate(parent){}

void CategoryItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const{
    QStyledItemDelegate::paint(painter,option,index);

    //if this is not the first item(placeholder)
    if(index.row()>0&&option.state&QStyle::State_MouseOver){
        QRect iconRect(option.rect.right()-20,option.rect.top()+2,12,12);
        QIcon deleteIcon(":/img/img/trash.svg");
        deleteIcon.paint(painter,iconRect);
    }
}
bool CategoryItemDelegate::editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index){
    if(index.row()==0){
        return QStyledItemDelegate::editorEvent(event,model,option,index);
    }
    if (event->type() == QEvent::MouseButtonPress) {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
        QRect iconRect(option.rect.right() - 20, option.rect.top() + 2, 16, 16);

        // Check if click is on delete icon
        if (iconRect.contains(mouseEvent->pos())) {
            emit deleteCategory(index.data().toString());
            return true;
        }
    }

    return QStyledItemDelegate::editorEvent(event, model, option, index);
}
//Custom category ComboBox Implementation
CustomCategoryComboBox::CustomCategoryComboBox(QWidget *parent)
    : QComboBox(parent),manager(nullptr),m_userId(-1)
    , ui(new Ui::CustomCategoryComboBox)
{
    ui->setupUi(this);
    m_addButton=new QPushButton("+",this);
    m_addButton->setFixedSize(20,20);
    connect(m_addButton,&QPushButton::clicked,this,&CustomCategoryComboBox::onAddCategoryClicked);

    // Set custom item delegate
    CategoryItemDelegate *delegate = new CategoryItemDelegate(this);
    this->setItemDelegate(delegate);

    // Connect delegate's delete signal
    connect(delegate, &CategoryItemDelegate::deleteCategory,
            this, &CustomCategoryComboBox::deleteCategoryRequested);

}

CustomCategoryComboBox::~CustomCategoryComboBox()
{
    delete ui;
}

void CustomCategoryComboBox::paintEvent(QPaintEvent *event) {
    QComboBox::paintEvent(event);

    // Position add button at the right side of the combobox
    m_addButton->move(width() - m_addButton->width() - 2,
                      (height() - m_addButton->height()) / 2);
}

void CustomCategoryComboBox::mousePressEvent(QMouseEvent *event) {
    // Check if click is outside the main combo box area (on the add button)
    if (event->pos().x() >= width() - m_addButton->width()) {
        m_addButton->click();
        return;
    }
    QComboBox::mousePressEvent(event);
}

void CustomCategoryComboBox::onAddCategoryClicked() {
    emit addCategoryRequested();
}
