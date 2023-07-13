#include "lineeditdelegate.h"
#include "texteditdialog.h"
#include "utils.h"
#include "widgetutils.h"

#include <QAction>
#include <QLayout>
#include <QPushButton>
#include <QPainter>
#include <QApplication>

LineEditDelegate::LineEditDelegate(QObject *parent) : QStyledItemDelegate(parent)
{

}

void LineEditDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    auto opt=option;
    opt.displayAlignment=this->textAlign;
    WidgetUtils::paint(painter,opt,index);

}

QSize LineEditDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    return QStyledItemDelegate::sizeHint(option, index);
}

QWidget *LineEditDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QLineEdit *editor = new QLineEdit(parent);

    editor->setReadOnly(readonly);
    editor->setMaxLength(50*1024*1024);

    editor->setProperty("useAutoComplete",this->useAutoComplete);
    editor->setProperty("useLexer",this->useLexer);

    if(useTextEdit){
        WidgetUtils::customLineEditWidgetAddTextEdit(editor);
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

void LineEditDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    if(editor->inherits(QLineEdit::staticMetaObject.className())){
        QString value = WidgetUtils::getData(index).toString();
        QLineEdit *lineEdit = static_cast<QLineEdit *>(editor);
        lineEdit->setAlignment(textAlign);
        lineEdit->setText(value);
    }
}

void LineEditDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
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

bool LineEditDelegate::getReadonly() const
{
    return readonly;
}

void LineEditDelegate::setReadonly(bool value)
{
    readonly = value;
}

bool LineEditDelegate::getUseTextEdit() const
{
    return useTextEdit;
}

void LineEditDelegate::setUseTextEdit(bool value)
{
    useTextEdit = value;
}

Qt::Alignment LineEditDelegate::getTextAlign() const
{
    return textAlign;
}

void LineEditDelegate::setTextAlign(const Qt::Alignment &value)
{
    textAlign = value;
}

bool LineEditDelegate::getUseAutoComplete() const
{
    return useAutoComplete;
}

void LineEditDelegate::setUseAutoComplete(bool value)
{
    useAutoComplete = value;
}

bool LineEditDelegate::getUseLexer() const
{
    return useLexer;
}

void LineEditDelegate::setUseLexer(bool value)
{
    useLexer = value;
}
