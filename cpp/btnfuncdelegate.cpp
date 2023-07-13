#include "btnfuncdelegate.h"
#include "strmappingdialog.h"
#include "widgetutils.h"

BtnFuncDelegate::BtnFuncDelegate(QObject *parent) : QStyledItemDelegate(parent)
{

}

void BtnFuncDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    auto opt=option;
    opt.displayAlignment=this->textAlign;
    WidgetUtils::paint(painter,opt,index);
}

QSize BtnFuncDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    return QStyledItemDelegate::sizeHint(option, index);
}

QWidget *BtnFuncDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QLineEdit *editor = new QLineEdit(parent);

    editor->setReadOnly(readonly);

    editor->setProperty("item_row",index.row());
    editor->setProperty("item_col",index.column());

    if(btnClkFunc){
        WidgetUtils::customLineEditWidgetAddBtnFunc(editor,[=](){
            btnClkFunc(editor);
        });
    }

    if(lineEditInitFunc){
        lineEditInitFunc(editor);
    }

    if(lineEditMousePressFunc){
        CustomEventFilter *eventFilter=new CustomEventFilter(const_cast<QLineEdit *>(editor));
        eventFilter->setFunc([=](QObject *obj, QEvent *event){
            if (event->type() == QEvent::MouseButtonPress && obj->inherits(QLineEdit::staticMetaObject.className()))
            {
                lineEditMousePressFunc((QLineEdit*)obj);
            }
            return false;
        });
        editor->installEventFilter(eventFilter);
    }

    if(lineEditChangedFunc){
        connect(editor,&QLineEdit::textChanged,this,[=](const QString text){
            lineEditChangedFunc(editor,text);
        });
    }
    return editor;
}

void BtnFuncDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    if(editor->inherits(QLineEdit::staticMetaObject.className())){
        QString value = WidgetUtils::getData(index).toString();
        QLineEdit *lineEdit = static_cast<QLineEdit *>(editor);
        lineEdit->setAlignment(textAlign);
        lineEdit->setText(value);
    }
}

void BtnFuncDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
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
