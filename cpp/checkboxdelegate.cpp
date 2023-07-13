#include "checkboxdelegate.h"
#include <QApplication>
#include "widgetutils.h"

CheckBoxDelegate::CheckBoxDelegate(QObject *parent):QStyledItemDelegate(parent)
{

}

void CheckBoxDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    auto opt=option;
    WidgetUtils::paint(painter,opt,index);
}

QSize CheckBoxDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    return QStyledItemDelegate::sizeHint(option, index);
}

QWidget *CheckBoxDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{

    if(readonly)return nullptr;

    CustomCheckBox *editor = new CustomCheckBox(parent);

    if(chkInitFunc){
        chkInitFunc(editor);
    }

    if(chkCheckedFunc){
        connect(editor,&CustomCheckBox::checked,this,[=](bool checked){
            if(chkCheckedFunc && QObject::sender()->inherits(CustomCheckBox::staticMetaObject.className())){
                chkCheckedFunc((CustomCheckBox*)QObject::sender(),checked);
            }
        });
    }

    return editor;
}

void CheckBoxDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{

    if(editor->inherits(CustomCheckBox::staticMetaObject.className())){
        bool value = WidgetUtils::getData(index).toBool();
        CustomCheckBox *chk = static_cast<CustomCheckBox *>(editor);
        chk->setChecked(value);
    }
}

void CheckBoxDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    if(this->readonly)return;

    if(editor->inherits(CustomCheckBox::staticMetaObject.className())){
        CustomCheckBox *chk = static_cast<CustomCheckBox *>(editor);
        bool changed=WidgetUtils::setData(model,index,chk->getChecked());
        if(changed){
            if(dataChangedFunc){
                dataChangedFunc(index);
            }
        }
        if(dataWriteFunc){
            dataWriteFunc(index,changed);
        }
    }
}

bool CheckBoxDelegate::getReadonly() const
{
    return readonly;
}

void CheckBoxDelegate::setReadonly(bool value)
{
    readonly = value;
}
