#include "datetimeeditdelegate.h"
#include "widgetutils.h"

#include <QHBoxLayout>
#include <QToolButton>

DateTimeEditDelegate::DateTimeEditDelegate(DateTimeEditType editType,QObject *parent) : QStyledItemDelegate(parent),editType(editType)
{

}

void DateTimeEditDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    auto opt=option;
    WidgetUtils::paint(painter,opt,index);
}

QSize DateTimeEditDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{

    return QStyledItemDelegate::sizeHint(option, index);
}

QWidget *DateTimeEditDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{

    QLineEdit *editor = new QLineEdit(parent);
    editor->setReadOnly(this->readonly);
    WidgetUtils::customLineEditWidgetAddDateTimeEdit(this->editType,editor);

    if(initFunc){
        initFunc(editor);
    }

    return editor;
}

void DateTimeEditDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{

    if(editor->inherits(QLineEdit::staticMetaObject.className())){
        QString value = WidgetUtils::getData(index).toString();
        QLineEdit *lineEdit = static_cast<QLineEdit *>(editor);
        lineEdit->setText(value);
    }
}

void DateTimeEditDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    if(this->readonly)return;

    if(editor->inherits(QLineEdit::staticMetaObject.className())){
        QLineEdit *lineEdit = static_cast<QLineEdit *>(editor);
        bool changed=WidgetUtils::setData(model,index,lineEdit->text());
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

bool DateTimeEditDelegate::getReadonly() const
{
    return readonly;
}

void DateTimeEditDelegate::setReadonly(bool value)
{
    readonly = value;
}
