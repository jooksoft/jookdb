#include "spinboxdelegate.h"
#include "widgetutils.h"

#include <QSpinBox>

SpinBoxDelegate::SpinBoxDelegate(QObject *parent):QStyledItemDelegate(parent)
{

}

void SpinBoxDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    auto opt=option;
    WidgetUtils::paint(painter,opt,index,alternatingRowColors);
}

QWidget *SpinBoxDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QSpinBox *editor = new QSpinBox(parent);

    editor->setFrame(false);
    editor->setMinimum(minVal);
    editor->setMaximum(maxVal);
    editor->setSingleStep(step);

    if(initFunc){
        initFunc(editor);
    }

    return editor;
}

void SpinBoxDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    QSpinBox *spinBox = static_cast<QSpinBox*>(editor);

    auto var=WidgetUtils::getData(index);
    if(var.isValid()&&!var.isNull()){
        spinBox->setValue(var.toInt());
    }

}

void SpinBoxDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    if(this->readonly)return;

    QSpinBox *spinBox = static_cast<QSpinBox*>(editor);
    QVariant var;
    if(!spinBox->text().isEmpty()){
        var=spinBox->value();
    }

    bool changed=WidgetUtils::setData(model,index,var);
    if(changed){
        if(dataChangedFunc){
            dataChangedFunc(index);
        }
    }
}

void SpinBoxDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    editor->setGeometry(option.rect);
}
