#ifndef STRMAPPINGDELEGATE_H
#define STRMAPPINGDELEGATE_H

#include <QStyledItemDelegate>


class BtnFuncDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    BtnFuncDelegate(QObject *parent = nullptr);

    void paint(QPainter *painter, const QStyleOptionViewItem &option,const QModelIndex &index) const override;
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,const QModelIndex &index) const override;
    void setEditorData(QWidget *editor, const QModelIndex &index) const override;
    void setModelData(QWidget *editor, QAbstractItemModel *model,const QModelIndex &index) const override;

    std::function<void(QLineEdit*)> lineEditInitFunc=nullptr;
    std::function<void(QLineEdit*)> lineEditMousePressFunc=nullptr;
    std::function<void(QLineEdit*,const QString text)> lineEditChangedFunc=nullptr;

    std::function<void(const QModelIndex &index)> dataChangedFunc=nullptr;
    std::function<void(const QModelIndex &index,bool changed)> dataWriteFunc=nullptr;

    std::function<void(QLineEdit*)> btnClkFunc=nullptr;

private:

    bool readonly=false;
    Qt::Alignment textAlign=Qt::AlignLeft|Qt::AlignVCenter;
};

#endif // STRMAPPINGDELEGATE_H
