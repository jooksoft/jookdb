#ifndef LISTWIDGETDELEGATE_H
#define LISTWIDGETDELEGATE_H

#include <QListWidget>
#include <QObject>
#include <QStyledItemDelegate>
#include <QWidget>

class ListWidgetDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit ListWidgetDelegate(QObject *parent = nullptr);

    void paint(QPainter *painter, const QStyleOptionViewItem &option,const QModelIndex &index) const override;
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,const QModelIndex &index) const override;
    void setEditorData(QWidget *editor, const QModelIndex &index) const override;
    void setModelData(QWidget *editor, QAbstractItemModel *model,const QModelIndex &index) const override;

    std::function<void(QLineEdit*)> initFunc=nullptr;
    std::function<void(QLineEdit*)> mousePressFunc=nullptr;

    std::function<void(const QModelIndex &index)> dataChangedFunc=nullptr;
    std::function<void(const QModelIndex &index,bool changed)> dataWriteFunc=nullptr;
    std::function<QStringList(QLineEdit*)> valuesFunc=nullptr;

    QStringList rightValues;

    bool findCaseSensitive=true;

    bool getReadonly() const;
    void setReadonly(bool value);

public slots:

private:
    bool readonly=false;

    QListWidget *getListWidget(QWidget *comb);
};

#endif // LISTWIDGETDELEGATE_H
