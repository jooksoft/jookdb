#include "comboboxdelegate.h"
#include "utils.h"

#include <QComboBox>
#include <QCompleter>
#include <QLineEdit>
#include "widgetutils.h"

ComboBoxDelegate::ComboBoxDelegate(QObject *parent):QStyledItemDelegate(parent)
{

}

void ComboBoxDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    auto opt=option;
    WidgetUtils::paint(painter,opt,index);
}

QSize ComboBoxDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    return QStyledItemDelegate::sizeHint(option, index);
}

QWidget *ComboBoxDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if(readonly)return nullptr;

    QComboBox *editor = new QComboBox(parent);

    editor->setProperty("item_row",index.row());
    editor->setProperty("item_col",index.column());

    editor->setEditable(true);
    WidgetUtils::customComboBoxWidget(editor);

    if(comboInitFunc){
        comboInitFunc(editor);
    }

    if(comboMousePressFunc){
        CustomEventFilter *eventFilter=new CustomEventFilter(const_cast<QComboBox *>(editor));
        eventFilter->setFunc([=](QObject *obj, QEvent *event){
            if (event->type() == QEvent::MouseButtonPress && obj->inherits(QComboBox::staticMetaObject.className()))
            {
                comboMousePressFunc((QComboBox*)obj);
            }
            return false;
        });
        editor->installEventFilter(eventFilter);
    }

    return editor;
}

void ComboBoxDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    if(editor->inherits(QComboBox::staticMetaObject.className())){
        QString value = WidgetUtils::getData(index).toString();
        QComboBox *combox = static_cast<QComboBox *>(editor);
        int i2=-1;
        if(findCaseSensitive){
            i2=combox->findText(value);
        }else{
            i2=combox->findText(value,Qt::MatchFixedString);
        }
        if(i2>=0){
            combox->setCurrentIndex(i2);
        }else{
            combox->insertItem(0,value);
            combox->setCurrentIndex(0);
        }

        if(comboCurrTextChangedFunc){
            connect(combox,&QComboBox::currentTextChanged,this,[=](QString str){
                if(comboCurrTextChangedFunc && QObject::sender() && QObject::sender()->inherits(QComboBox::staticMetaObject.className())){
                    QComboBox* comb=(QComboBox*)QObject::sender();
                    comb->setProperty("item_row",index.row());
                    comb->setProperty("item_col",index.column());
                    comboCurrTextChangedFunc(comb,str);
                }
            });
        }
    }
}

void ComboBoxDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    if(this->readonly)return;

    if(editor->inherits(QComboBox::staticMetaObject.className())){
        QComboBox *combox = static_cast<QComboBox *>(editor);
        bool changed=WidgetUtils::setData(model,index,combox->currentText());
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

bool ComboBoxDelegate::getReadonly() const
{
    return readonly;
}

void ComboBoxDelegate::setReadonly(bool value)
{
    readonly = value;
}
