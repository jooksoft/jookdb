#ifndef LEFTWIDGETFORM_H
#define LEFTWIDGETFORM_H

#include "mytreewidget.h"

#include <QWidget>

namespace Ui {
class LeftWidgetForm;
}

class LeftWidgetForm : public QWidget
{
    Q_OBJECT

public:
    explicit LeftWidgetForm(QWidget *parent = nullptr);
    ~LeftWidgetForm();

    MyTreeWidget* getLeftTree() const;

    QLineEdit* getLeftTreeFilter() const;

private slots:
    void on_le_leftTree_filter_textChanged(const QString &arg1);

    void on_le_leftTree_filter_returnPressed();

private:
    Ui::LeftWidgetForm *ui;

    std::atomic_bool stop=false;
};

#endif // LEFTWIDGETFORM_H
