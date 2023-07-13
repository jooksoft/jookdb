#ifndef COMBOBOXDELEGATE_H
#define COMBOBOXDELEGATE_H

#include <QItemDelegate>

#include <QComboBox>
#include <QStyledItemDelegate>


class ComboBoxDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    ComboBoxDelegate(QObject *parent = nullptr);

    void paint(QPainter *painter, const QStyleOptionViewItem &option,const QModelIndex &index) const override;
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,const QModelIndex &index) const override;
    void setEditorData(QWidget *editor, const QModelIndex &index) const override;
    void setModelData(QWidget *editor, QAbstractItemModel *model,const QModelIndex &index) const override;

    std::function<void(QComboBox*)> comboInitFunc=nullptr;
    std::function<void(QComboBox*)> comboMousePressFunc=nullptr;
    std::function<void(QComboBox*,QString)> comboCurrTextChangedFunc=nullptr;

    std::function<void(const QModelIndex &index)> dataChangedFunc=nullptr;
    std::function<void(const QModelIndex &index,bool changed)> dataWriteFunc=nullptr;

    bool findCaseSensitive=true;

    bool getReadonly() const;
    void setReadonly(bool value);

public slots:

private:
    bool readonly=false;
};

#endif // COMBOBOXDELEGATE_H
