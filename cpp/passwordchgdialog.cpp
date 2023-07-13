#include "passwordchgdialog.h"
#include "ui_passwordchgdialog.h"

#include <QMessageBox>

PasswordChgDialog::PasswordChgDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PasswordChgDialog)
{
    ui->setupUi(this);
}

PasswordChgDialog::~PasswordChgDialog()
{
    delete ui;
}

QString PasswordChgDialog::getOldPassword() const
{
    return ui->le_oldPwd->text().trimmed();
}

QString PasswordChgDialog::getNewPassword() const
{
    return this->ui->le_newPwd->text().trimmed();
}

void PasswordChgDialog::on_btn_ok_clicked()
{
    if(ui->le_newPwd->text()==ui->le_confirmPwd->text()){
        this->accept();
    }else{
        QMessageBox::warning(this, QCoreApplication::applicationName(),"New password does not match");

    }
}

void PasswordChgDialog::on_btn_cancel_clicked()
{
    this->close();
}
