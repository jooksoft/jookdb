#include "listwidgetdelegate.h"
#include "widgetutils.h"
#include <QCompleter>

ListWidgetDelegate::ListWidgetDelegate(QObject *parent) : QStyledItemDelegate(parent)
{

}

void ListWidgetDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    auto opt=option;
    WidgetUtils::paint(painter,opt,index);
}

QSize ListWidgetDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    return QStyledItemDelegate::sizeHint(option, index);
}

QWidget *ListWidgetDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{

    QLineEdit *editor = new QLineEdit(parent);

    editor->setReadOnly(readonly);

    editor->setProperty("item_row",index.row());
    editor->setProperty("item_col",index.column());


    WidgetUtils::customLineEditWidgetAddMutiChk(editor,valuesFunc,rightValues);

    if(initFunc){
        initFunc(editor);
    }

    if(mousePressFunc){
        CustomEventFilter *eventFilter=new CustomEventFilter(editor);
        eventFilter->setFunc([=](QObject *obj, QEvent *event){
            if (event->type() == QEvent::MouseButtonPress && obj->inherits(QLineEdit::staticMetaObject.className()))
            {
                mousePressFunc(editor);
            }
            return false;
        });
        editor->installEventFilter(eventFilter);
    }

    return editor;
}

void ListWidgetDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    if(editor->inherits(QLineEdit::staticMetaObject.className())){
        QString value = WidgetUtils::getData(index).toString();
        QLineEdit *le = static_cast<QLineEdit *>(editor);

        le->setText(value);

    }
}

void ListWidgetDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    if(this->readonly)return;

    if(editor->inherits(QLineEdit::staticMetaObject.className())){
        QLineEdit *le = static_cast<QLineEdit *>(editor);

        QString value=le->text();

        bool changed=WidgetUtils::setData(model,index,value);
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

bool ListWidgetDelegate::getReadonly() const
{
    return readonly;
}

void ListWidgetDelegate::setReadonly(bool value)
{
    readonly = value;
}

QListWidget *ListWidgetDelegate::getListWidget(QWidget *comb)
{
    if(comb->inherits(QComboBox::staticMetaObject.className())){
        QComboBox *combox = static_cast<QComboBox *>(comb);

        if(combox->view()&&combox->view()->inherits(QListWidget::staticMetaObject.className())){
            return static_cast<QListWidget *>(combox->view());
        }
    }
    return nullptr;
}
