#ifndef LINEEDITDELEGATE_H
#define LINEEDITDELEGATE_H

#include <QItemDelegate>
#include <QLineEdit>
#include <QObject>
#include <QStyledItemDelegate>

class LineEditDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit LineEditDelegate(QObject *parent = nullptr);

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

    bool getReadonly() const;
    void setReadonly(bool value);

    bool getUseTextEdit() const;
    void setUseTextEdit(bool value);

    Qt::Alignment getTextAlign() const;
    void setTextAlign(const Qt::Alignment &value);

    bool getUseAutoComplete() const;
    void setUseAutoComplete(bool value);

    bool getUseLexer() const;
    void setUseLexer(bool value);

signals:

public slots:

private:

    bool readonly=false;
    bool useTextEdit=false;

    bool useAutoComplete=false;
    bool useLexer=false;

    Qt::Alignment textAlign=Qt::AlignLeft|Qt::AlignVCenter;
};

#endif // LINEEDITDELEGATE_H
