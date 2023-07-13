#ifndef DATETIMEEDITDELEGATE_H
#define DATETIMEEDITDELEGATE_H

#include <QObject>
#include <QStyledItemDelegate>
#include "datetimeeditform.h"

class DateTimeEditDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit DateTimeEditDelegate(DateTimeEditType editType,QObject *parent = nullptr);

    void paint(QPainter *painter, const QStyleOptionViewItem &option,const QModelIndex &index) const override;
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,const QModelIndex &index) const override;
    void setEditorData(QWidget *editor, const QModelIndex &index) const override;
    void setModelData(QWidget *editor, QAbstractItemModel *model,const QModelIndex &index) const override;

    std::function<void(QLineEdit*)> initFunc=nullptr;

    std::function<void(const QModelIndex &index)> dataChangedFunc=nullptr;
    std::function<void(const QModelIndex &index,bool changed)> dataWriteFunc=nullptr;

    bool getReadonly() const;
    void setReadonly(bool value);

signals:

public slots:

private:
    bool readonly=false;

    DateTimeEditType editType;
};

#endif // DATETIMEEDITDELEGATE_H
