#include "leftwidgetform.h"
#include "ui_leftwidgetform.h"
#include "widgetutils.h"

LeftWidgetForm::LeftWidgetForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LeftWidgetForm)
{
    ui->setupUi(this);
}

LeftWidgetForm::~LeftWidgetForm()
{
    delete ui;
}

MyTreeWidget *LeftWidgetForm::getLeftTree() const
{
    return this->ui->leftTree;
}

QLineEdit *LeftWidgetForm::getLeftTreeFilter() const
{
    return this->ui->le_leftTree_filter;
}

void LeftWidgetForm::on_le_leftTree_filter_textChanged(const QString &arg1)
{
    WidgetUtils::treeFilter(this->stop,this->ui->leftTree,this->ui->le_leftTree_filter);
}

void LeftWidgetForm::on_le_leftTree_filter_returnPressed()
{
    this->on_le_leftTree_filter_textChanged("");
}
