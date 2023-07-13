#include "runsqldialog.h"
#include "ui_runsqldialog.h"
#include "widgetutils.h"

#include <QTextCodec>
#include <QSettings>
#include <QFileDialog>
#include <QMessageBox>
#include <QTimer>
#include <QTime>

RunSqlDialog::RunSqlDialog(QSharedPointer<ConnData> connData,QString dbName,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RunSqlDialog)
{
    ui->setupUi(this);

    WidgetUtils::setSafeDeleteOnClose(this);

#if defined(Q_OS_MAC)
    this->setWindowFlag(Qt::Tool,true);
#endif

    if(connData){
        this->connData=connData->cloneBase();
        this->connData->dbName=connData->dbName;

        QString str=QString{"%1:%2"}.arg(this->connData->host).arg(this->connData->port);

        this->setWindowTitle(tr("Execute %1 File - %2").arg(StringUtils::fileTypeName(connData),str));
        this->ui->lb_ds->setText(this->connData->connName);

        this->ui->chk_autoCommit->setChecked(false);
        this->ui->chk_autoCommit->setHidden(true);//暂不启用AutoCommit选项

        switch (connData->typeId) {
        }
    }

    WidgetUtils::customComboBoxWidget(ui->cbox_dbName);
    WidgetUtils::widgetDisableWheel(ui->cbox_dbName);
    ui->cbox_dbName->setCurrentText(dbName);

    auto codecList=QTextCodec::availableCodecs();
    std::sort(codecList.begin(), codecList.end());
    for(QByteArray ba:codecList){
        this->ui->cbox_encoding->addItem(ba);
    }
    WidgetUtils::setOrAddCurrItem(this->ui->cbox_encoding,"UTF-8",false);

    WidgetUtils::customComboBoxWidget(this->ui->cbox_encoding);

    auto getFileBtnFunc=[=](){
        QSettings setting(QCoreApplication::applicationName());
        QString lastPath = setting.value("ExportPath").toString();

        QString filter=QObject::tr("Execute file (*.%1 *.txt);;All files(*.*)").arg(StringUtils::fileExtensionName(connData));

        QFileDialog fdialog(this,QObject::tr("Execute File"),lastPath,filter);
        fdialog.setOption(QFileDialog::DontUseNativeDialog,true);
        fdialog.setWindowModality(Qt::WindowModal);
        fdialog.setAcceptMode(QFileDialog::AcceptOpen);

        if (fdialog.exec() == QDialog::Accepted && fdialog.selectedFiles().length()>0){
            QStringList fnames=fdialog.selectedFiles();
            QString lastName=fnames.last();

            if(fnames.count()>0&&!lastName.isEmpty()){
                this->ui->le_file->setText(lastName);
                int idx=lastName.lastIndexOf('.');
                if(idx>=0){
                    QString type=lastName.right(lastName.length()-idx-1);

                }

                QDir dir(lastName);
                dir.cdUp();
                setting.setValue("ExportPath",dir.path());

                QTimer::singleShot(0,this,[=](){
                    this->on_btn_detect_clicked();
                });
            }
        }
    };
    WidgetUtils::customLineEditWidgetAddBtnFunc(ui->le_file,getFileBtnFunc,true,QIcon {":/images/open.svg"});

    QTimer::singleShot(0,this,[=](){
        auto runRef=WidgetUtils::createRunRef(this);

        if(Utils::testOrReconn(connData)){
            StringListResult slr=WidgetUtils::getDBNameList(connData);
            ui->cbox_dbName->clear();
            ui->cbox_dbName->addItem("");
            ui->cbox_dbName->addItems(slr.list);
            WidgetUtils::setOrAddCurrItem(ui->cbox_dbName,dbName);
        }
    });

}

void RunSqlDialog::closeEvent(QCloseEvent *event)
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

RunSqlDialog::~RunSqlDialog()
{
    delete ui;
}

void RunSqlDialog::on_btn_close_clicked()
{
    this->ui->btn_close->setEnabled(false);

    if(this->connData){
        this->connData->stopConn();
    }

    this->close();
}

void RunSqlDialog::on_btn_detect_clicked()
{
    QString fname=this->ui->le_file->text();
    QFile file(fname);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {

        QMessageBox::critical(this, QCoreApplication::applicationName(),
                             tr("File open error!"));
        return;
    }
    LastRun lr{[&](){
            file.close();
               }};

    QString codecName=WidgetUtils::detectEncoding(file);
    if(codecName.length()>0){
        WidgetUtils::setOrAddCurrItem(this->ui->cbox_encoding,codecName,false);
    }
}

void RunSqlDialog::on_btn_start_clicked()
{
    auto runRef=WidgetUtils::createRunRef(this);

    this->ui->btn_start->setEnabled(false);

    LastRun lr{[=](){

            this->ui->btn_start->setEnabled(true);

            
               }};

//    BtnLoadingIcon bli{this->ui->btn_start};

    if(ui->cbox_dbName->currentText().trimmed().isEmpty()){
        QMessageBox::warning(this, QCoreApplication::applicationName(),tr("No database selected!").trimmed());
        return;
    }

    QString fname=this->ui->le_file->text();

    if(!Utils::testOrReconn(this->connData))return;

    QJsonObject json;
    json.insert("funcId", RUN_SQL_FILE);
    json.insert("file", fname);
    json.insert("dbName", ui->cbox_dbName->currentText().trimmed());
    json.insert("autoCommit", this->ui->chk_autoCommit->isChecked());
    json.insert("stopOnError", !this->ui->chk_contionueOnError->isChecked());
    json.insert("batchSize", this->ui->spin_batchSize->value());
    json.insert("codec", this->ui->cbox_encoding->currentText().trimmed());

    int reqNum=Utils::nextReqNum();


    int64_t startTime=QDateTime::currentMSecsSinceEpoch();

    this->ui->progressBar->setValue(0);
    this->ui->lb_times->clear();

    QTimer timer;
    connect(&timer, &QTimer::timeout,
             this, [=](){

        auto runRef=WidgetUtils::createRunRef(this);
        QJsonObject json;
        json.insert("funcId", PROGRESS_RATE);
        json.insert("startReqNum", reqNum);
        QJsonObject res2=this->connData->getResJson(json);
        if(Utils::testResult(res2,false)){
            double rate=Utils::getDouble(res2,"rate");
            this->ui->progressBar->setValue(rate);

            int64_t endTime=QDateTime::currentMSecsSinceEpoch();

            this->ui->lb_times->setText(tr("%1s").arg((endTime-startTime)/1000));
        }
        });
    timer.start(700);


    QJsonObject res=connData->getResJson(json,reqNum);

    timer.stop();
    this->ui->progressBar->setValue(100);

    if(Utils::testResult(res)){

        int errors=Utils::getInt(res,"errorNum");
        if(errors<0)errors=0;

        const int diaRet=WidgetUtils::showSuccDialog(QCoreApplication::applicationName(),
                                                  tr("Dump succeeded.\nFile: %1\nErrors: %2").arg(fname).arg(errors),
                                                  tr("OK"));
    }
}
