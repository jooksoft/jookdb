#include "attachdbdialog.h"
#include "mainwindow.h"
#include "ui_attachdbdialog.h"
#include "widgetutils.h"

#include <QFileDialog>
#include <QMessageBox>

AttachDbDialog::AttachDbDialog(QSharedPointer<ConnData> connData,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AttachDbDialog),connData(connData)
{
    ui->setupUi(this);

    this->on_ch_encrypted_stateChanged(-1);

    if(connData){
        this->setWindowTitle(connData->connName+" - "+this->windowTitle());
    }

    auto getFileBtnFunc=[=](){

        QFileDialog fdialog(this,QObject::tr("Open File"),"",QObject::tr("All SQLite files (*.db *.sdb *.sqlite *.db3 *.s3db *.sqlite3 *.sl3);;SQLite3 files (*.db3 *.s3db *.sqlite3 *.sl3);;All files(*.*)"));
        fdialog.setOption(QFileDialog::DontUseNativeDialog,true);
        fdialog.setWindowModality(Qt::WindowModal);
        fdialog.setAcceptMode(QFileDialog::AcceptOpen);

        if (fdialog.exec() == QDialog::Accepted && fdialog.selectedFiles().length()>0){
            QStringList fnames=fdialog.selectedFiles();
            if(fnames.count()>0){
                QString lastName=fnames.last();

                this->ui->le_file->setText(lastName);
                QFileInfo fileInfo(lastName);
                this->ui->le_name->setText(fileInfo.baseName());

            }

        }
    };
    WidgetUtils::customLineEditWidgetAddBtnFunc(ui->le_file,getFileBtnFunc,true,QIcon {":/images/open.svg"});
}

AttachDbDialog::~AttachDbDialog()
{
    delete ui;
}

void AttachDbDialog::on_ch_encrypted_stateChanged(int arg1)
{
    bool checked=ui->ch_encrypted->isChecked();
    ui->lb_password->setEnabled(checked);
    ui->le_password->setEnabled(checked);
}

void AttachDbDialog::on_btn_cancel_clicked()
{
    this->close();
}

void AttachDbDialog::on_btn_ok_clicked()
{
    if(!connData)return;

    if(ui->le_file->text().trimmed().isEmpty()){
        QMessageBox::warning(this, QCoreApplication::applicationName(),tr("Database file cannot be empty!"));
        return;
    }
    if(ui->le_name->text().trimmed().isEmpty()){
        QMessageBox::warning(this, QCoreApplication::applicationName(),tr("Database name cannot be empty!"));
        return;
    }

    switch (connData->typeId) {

        QString file=ui->le_file->text().trimmed();
        QString name=ui->le_name->text().trimmed();
        QString password;
        if(ui->ch_encrypted->isChecked()){
            password=ui->le_password->text().trimmed();
        }

        QString sql;
        sql="ATTACH DATABASE '%1' AS '%2' KEY '%3'";
        sql=sql.arg(StringUtils::escapeSqlSingleQuotes(file),
                    StringUtils::escapeSqlSingleQuotes(name),
                    password);

        if(Utils::testOrReconn(connData)){
            QJsonObject json=Utils::executeQuery(connData,sql,{});
            if(Utils::testResult(json)){

                if(this->saveCfg){
                    QJsonObject j;
                    j.insert("file",file);
                    j.insert("name",name);
                    j.insert("encrypt",password);
                    attachDbArr.append(j);

                    connJson.insert("attachDb",attachDbArr);

                    connData->connStr=Utils::jsonToString(connJson);

                    QSettings ini(Utils::getConfigFilePath(), QSettings::IniFormat);
                    ini.beginGroup(connData->connName);
                    ini.setValue("connStr",connData->connStr);
                    ini.endGroup();

                    auto currItem=MainWindow::getContent()->getLeftTree()->currentItem();
                    auto dsItem=Utils::getDsItem(currItem);
                    if(dsItem&&currItem&&dsItem->isExpanded()){
                        MainWindow::getContent()->getLeftTree()->refreshItem(dsItem);
                    }
                }

                this->accept();
            }else{

            }
        }
    }
        break;
    }


}

bool AttachDbDialog::getSaveCfg() const
{
    return saveCfg;
}

void AttachDbDialog::setSaveCfg(bool value)
{
    saveCfg = value;
}

QString AttachDbDialog::getFile()
{
    return ui->le_file->text().trimmed();
}

QString AttachDbDialog::getName()
{
    return ui->le_name->text().trimmed();
}

QString AttachDbDialog::getPassword()
{
    return ui->le_password->text().trimmed();
}

bool AttachDbDialog::getEncrypted()
{
    return ui->ch_encrypted->isChecked();
}
