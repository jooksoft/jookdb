#include "passwordinputdialog.h"
#include "ui_passwordinputdialog.h"

PasswordInputDialog::PasswordInputDialog(QSharedPointer<ConnData> connData,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PasswordInputDialog),
    connData(connData)
{
    ui->setupUi(this);

#if defined(Q_OS_MAC)
    this->setWindowFlag(Qt::Tool,true);
#endif

    if(connData){
        QString str="Enter password for %1(%2):";
        str=str.arg(connData->userName,connData->connName);
        this->ui->lb_pwd->setText(str);
    }

}

PasswordInputDialog::~PasswordInputDialog()
{
    delete ui;
}

QString PasswordInputDialog::getPassword()
{
    return this->ui->le_pwd->text();
}

bool PasswordInputDialog::isSave()
{
    return this->ui->chk_savePwd->isChecked();

}

QLabel *PasswordInputDialog::getPwdLabel()
{
    return ui->lb_pwd;
}
