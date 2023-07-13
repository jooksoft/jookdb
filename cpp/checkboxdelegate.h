#ifndef CHECKBOXDELEGATE_H
#define CHECKBOXDELEGATE_H

#include "customcheckbox.h"

#include <QItemDelegate>
#include <QStyledItemDelegate>



class CheckBoxDelegate : public QStyledItemDelegate
{
public:
    CheckBoxDelegate(QObject *parent = nullptr);

    void paint(QPainter *painter, const QStyleOptionViewItem &option,const QModelIndex &index) const override;
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,const QModelIndex &index) const override;
    void setEditorData(QWidget *editor, const QModelIndex &index) const override;
    void setModelData(QWidget *editor, QAbstractItemModel *model,const QModelIndex &index) const override;

    bool findCaseSensitive=true;

    std::function<void(CustomCheckBox*)> chkInitFunc=nullptr;
    std::function<void(CustomCheckBox*,bool)> chkCheckedFunc=nullptr;

    std::function<void(const QModelIndex &index)> dataChangedFunc=nullptr;
    std::function<void(const QModelIndex &index,bool changed)> dataWriteFunc=nullptr;

    bool getReadonly() const;
    void setReadonly(bool value);

public slots:

private:
    bool readonly=false;
};

#endif // CHECKBOXDELEGATE_H
