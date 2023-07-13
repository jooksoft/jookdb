#include "autocompleteform.h"
#include "copytabledialog.h"
#include "mainwindow.h"
#include "mytreewidget.h"
#include "ui_copytabledialog.h"
#include "widgetutils.h"

#include <QTimer>

CopyTableDialog::CopyTableDialog(QSharedPointer<ConnData> connData,QString dbName,QString tableName,bool withData,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CopyTableDialog)
{
    ui->setupUi(this);

    WidgetUtils::setSafeDeleteOnClose(this);

    this->connData=connData;
    this->dbName=dbName;
    this->tableName=tableName;

    WidgetUtils::customComboBoxWidget(this->ui->cbox_dbName);

    StringListResult dbNameList=WidgetUtils::getDBNameList(connData);

    this->ui->cbox_dbName->addItems(dbNameList.list);

    WidgetUtils::setOrAddCurrItem(this->ui->cbox_dbName,dbName);

    if(connData){
        this->ui->le_tableName->setText(StringUtils::unwrapObjNameOnly(connData,tableName)+"_copy");

        switch(connData->typeId){
        }
    }

    this->ui->ch_withData->setChecked(withData);

    this->ui->le_tableName->setFocus();

}

CopyTableDialog::~CopyTableDialog()
{
    delete ui;
}

void CopyTableDialog::closeEvent(QCloseEvent *event)
{
    this->hide();

    stop=true;
    if(connData){
        connData->stopConn();
    }

    QTimer *timer = new QTimer(this);
    // setup signal and slot
    connect(timer, &QTimer::timeout,
         this, [=](){
     if(!WidgetUtils::isRunning(this)){
         this->deleteLater();
     }
    });
    // msec
    timer->start(50);

    event->ignore();
}

void CopyTableDialog::on_btn_cancel_clicked()
{
    this->close();
}

void CopyTableDialog::on_btn_ok_clicked()
{
    auto runRef=WidgetUtils::createRunRef(this);
    this->ui->btn_ok->setEnabled(false);
    LastRun lr{[=](){
            this->ui->btn_ok->setEnabled(true);
               }};

    QString newDBName=this->ui->cbox_dbName->currentText();
    QString newTableName=this->ui->le_tableName->text();
    if(Utils::testOrReconn(this->connData)){
        if(this->ui->ch_withData->isChecked()){
            QString sql;
            auto resJson=Utils::executeSql(connData,"create",sql,{},this->dbName);
            if(Utils::testResult(resJson)){

                auto item=MainWindow::getContent()->getLeftTree()->getTablesItem(this->connData,newDBName);
                if(item&&item->isExpanded()){

                    MainWindow::getContent()->getLeftTree()->refreshItem(item,newTableName);

                }

                this->close();
            }
        }else{
            QString sql;
            sql=sql.arg(StringUtils::mergeSchema(StringUtils::wrapObjName(connData,newTableName),StringUtils::wrapName(connData,newDBName))).
                    arg(StringUtils::mergeSchema(StringUtils::wrapObjName(connData,this->tableName),StringUtils::wrapName(connData,this->dbName)));
            auto resJson=Utils::executeSql(connData,"create",sql,{},this->dbName);
            if(Utils::testResult(resJson)){

                auto item=MainWindow::getContent()->getLeftTree()->getTablesItem(this->connData,newDBName);
                if(item&&item->isExpanded()){

                    MainWindow::getContent()->getLeftTree()->refreshItem(item,newTableName);
                }

                this->close();
            }
        }
    }
}
