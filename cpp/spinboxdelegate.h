#ifndef SPINBOXDELEGATE_H
#define SPINBOXDELEGATE_H

#include <QSpinBox>
#include <QStyledItemDelegate>

class SpinBoxDelegate : public QStyledItemDelegate
{
public:

    SpinBoxDelegate(QObject *parent = nullptr);

    void paint(QPainter *painter, const QStyleOptionViewItem &option,const QModelIndex &index) const override;
    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                          const QModelIndex &index) const override;

    void setEditorData(QWidget *editor, const QModelIndex &index) const override;
    void setModelData(QWidget *editor, QAbstractItemModel *model,
                      const QModelIndex &index) const override;

    void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option,
                              const QModelIndex &index) const override;


    std::function<void(QSpinBox*)> initFunc=nullptr;

    std::function<void(const QModelIndex &index)> dataChangedFunc=nullptr;

    int minVal=INT_MIN;
    int maxVal=INT_MAX;
    int step=1;

    bool alternatingRowColors=true;

private:
    bool readonly=false;

};

#endif // SPINBOXDELEGATE_H
