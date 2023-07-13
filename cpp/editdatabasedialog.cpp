#include "editdatabasedialog.h"
#include "mainwindow.h"
#include "sqlobject.h"
#include "sqlstyle.h"
#include "ui_editdatabasedialog.h"
#include "widgetutils.h"

#include <QTimer>

EditDatabaseDialog::EditDatabaseDialog(QSharedPointer<ConnData> connData, const QString &dbName,EditDatabaseType edtype,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EditDatabaseDialog),
    connData(connData),dbName(dbName),edtype(edtype)
{
    ui->setupUi(this);

    WidgetUtils::setSafeDeleteOnClose(this);

#if defined(Q_OS_MAC)
    this->setWindowFlag(Qt::Tool,true);
#endif

    ui->tabWidget->setCurrentWidget(ui->tab);

    WidgetUtils::customComboBoxWidget(this->ui->cbox_charset);
    WidgetUtils::customComboBoxWidget(this->ui->cbox_collation);
    WidgetUtils::customComboBoxWidget(this->ui->cbox_user);

    CustomEventFilter *collationFilter=new CustomEventFilter(this);
    collationFilter->setFunc([this](QObject *obj, QEvent *event){
        auto runRef=WidgetUtils::createRunRef(this);
        if (event->type() == QEvent::MouseButtonPress)
        {

            QString currCollationCharset=this->ui->cbox_charset->currentText();

            Table::loadCollation(this->connData,currCollationCharset,this->ui->cbox_collation,this);

//            SuperAttr::setAttr(this->ui->cbox_collation,currCharset,currCollationCharset);
        }
        return false;
    });
    this->ui->cbox_collation->installEventFilter(collationFilter);

    this->ui->te_preview->useLexer=true;
    this->ui->te_preview->useBraceHighlighting=true;
    this->ui->te_preview->useLineNumber=false;
    this->ui->te_preview->setUseAutoComplete(false);
    SqlStyle::setStyle(this->ui->te_preview);

    connect(this->ui->tabWidget,&QTabWidget::currentChanged,this,[&](int index){

        auto w=this->ui->tabWidget->currentWidget();
        if(w==this->ui->tab_sqlPreview){
            QString sql=this->getUpdateSql();
            QByteArray ba=sql.toUtf8();
            ba.append('\n');
            this->ui->te_preview->clearAll();
            this->ui->te_preview->addText(ba.length(),ba.constData());
        }
    },Qt::UniqueConnection);

    this->ui->le_database->setText(this->dbName);

    if(this->edtype==EditDatabaseType::newDatabase){
        this->setWindowTitle(tr("Create %1").arg(StringUtils::dbLabelName(connData)));
    }else{
        this->ui->le_database->setEnabled(false);
    }

    if(this->edtype==EditDatabaseType::viewDatabase){
        this->setWindowTitle(tr("View %1").arg(StringUtils::dbLabelName(connData)));

        this->ui->cbox_charset->setEnabled(false);
        this->ui->cbox_collation->setEnabled(false);
    }

    if(this->edtype==EditDatabaseType::editDatabase){
        this->setWindowTitle(tr("Edit %1").arg(StringUtils::dbLabelName(connData)));
    }

    ui->lb_user->setHidden(true);
    ui->cbox_user->setHidden(true);

    switch (this->connData->typeId) {
    case DB_TYPE_MYSQL:{
    }break;
    }

    QTimer::singleShot(0,this,[=](){
        auto runRef=WidgetUtils::createRunRef(this);
        if(Utils::testOrReconn(this->connData)){

            switch (this->connData->typeId) {
            case DB_TYPE_MYSQL:{

                bool ret=Table::loadCharSet(this->connData,this->ui->cbox_charset,this);
                if(!ret){
//                    this->ui->cbox_charset->setEnabled(false);
//                    this->ui->cbox_collation->setEnabled(false);
                    return;
                }

                QString sql="SELECT schema_name, default_character_set_name , default_collation_name FROM information_schema.SCHEMATA WHERE SCHEMA_NAME like '%1'";
                if(this->edtype==EditDatabaseType::editDatabase||this->edtype==EditDatabaseType::viewDatabase){
                    sql=sql.arg(this->dbName);

                    auto resJson=Utils::executeQuery(this->connData,sql,{});
                    Result res{resJson};
                    if(res.testResult(false)&&res.rowCount()>0){
                        int idx=res.getColIndex("default_character_set_name");
                        if(idx>=0){
                            WidgetUtils::setOrAddCurrItem(this->ui->cbox_charset,res.getString(0,idx));
                            WidgetUtils::setOldVal(this->ui->cbox_charset,res.getString(0,idx));
                        }
                        idx=res.getColIndex("default_collation_name");
                        if(idx>=0){
                            WidgetUtils::setOrAddCurrItem(this->ui->cbox_collation,res.getString(0,idx));
                            WidgetUtils::setOldVal(this->ui->cbox_collation,res.getString(0,idx));
                        }
                    }

                }
            }break;
                default:{
                }
                }
            }break;
            }

        }
    });

    if(this->edtype!=EditDatabaseType::newDatabase){
        ui->cbox_user->setEnabled(false);
    }

    this->ui->le_database->setFocus();
}

EditDatabaseDialog::~EditDatabaseDialog()
{
    delete ui;
}

void EditDatabaseDialog::closeEvent(QCloseEvent *event)
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

QString EditDatabaseDialog::getUpdateSql()
{
    if(!connData)return {};

    QString newDatabase=this->ui->le_database->text().trimmed();
    QString newCharset=this->ui->cbox_charset->currentText().trimmed();
    QString newCollation=this->ui->cbox_collation->currentText().trimmed();
    QString newUserName=this->ui->cbox_user->currentText().trimmed();

    QString sql;
    if(this->edtype==EditDatabaseType::newDatabase){

        if(newDatabase.length()>0){
            sql="CREATE %1 %2";
            sql=sql.arg(StringUtils::dbLabelName(connData));
            sql=sql.arg(StringUtils::wrapName(connData,newDatabase));

    }else if(this->edtype==EditDatabaseType::editDatabase){

        if(newDatabase.length()>0&&(newCharset.length()>0||newCollation.length()>0)&&
                (WidgetUtils::hasChange(ui->cbox_charset)||WidgetUtils::hasChange(ui->cbox_collation))){
            sql="ALTER %1 %2";
            sql=sql.arg(StringUtils::dbLabelName(connData));
            sql=sql.arg(StringUtils::wrapName(connData,newDatabase));

            if(WidgetUtils::hasChange(ui->cbox_charset)&&newCharset.length()>0){
                sql+=QString{" CHARACTER SET %1"}.arg(newCharset);
            }
            if(WidgetUtils::hasChange(ui->cbox_collation)&&newCollation.length()>0){
                sql+=QString{" COLLATE %1"}.arg(newCollation);
            }
        }
    }
    return sql;
}

void EditDatabaseDialog::on_cbox_charset_currentTextChanged(const QString &arg1)
{
    this->ui->cbox_collation->clearEditText();
}

void EditDatabaseDialog::on_btn_cancel_clicked()
{
    this->close();
}

void EditDatabaseDialog::on_btn_ok_clicked()
{
    this->ui->btn_ok->setEnabled(false);
    auto runRef=WidgetUtils::createRunRef(this);
    LastRun lr{[=](){
            this->ui->btn_ok->setEnabled(true);
            
               }};

    QString sql=this->getUpdateSql();
    if(sql.length()>0){
        if(Utils::testOrReconn(this->connData)){
            auto resJson=Utils::executeSql(this->connData,"alter",sql,{});
            if(Utils::testResult(resJson)){

                WidgetUtils::showSuccDialog(QCoreApplication::applicationName(),tr("Success."),tr("OK"));

                auto topItem=MainWindow::getContent()->getLeftTree()->getDsItem(this->connData);
                if(topItem){
                    MainWindow::getContent()->getLeftTree()->refreshItem(topItem,this->ui->le_database->text());
                }
            }else{
                return;
            }
        }
    }
    this->close();
}
