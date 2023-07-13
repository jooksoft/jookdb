#include "driverdownloadform.h"
#include "ui_driverdownloadform.h"
#include "utils.h"
#include "widgetutils.h"

#include <QDesktopServices>
#include <QMessageBox>
#include <QMovie>
#include <QTimer>

DriverDownloadForm::DriverDownloadForm(QJsonObject res,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DriverDownloadForm)
{
    ui->setupUi(this);

    WidgetUtils::setSafeDeleteOnClose(this);

    this->setModal(true);

    this->ui->lb_info->setText(Utils::getString(res,"errInfo"));
    this->ui->le_driverClass->setText(Utils::getString(res,"driverClass"));
//    this->ui->le_driverJar->setText(Utils::getString(res,"driverJar"));

    QString target=Utils::getLibPath();
    this->ui->le_lib_path->setText(target);

    QString downLoadUrl=Utils::getString(res,"downloadUrl");
    QString downLoadPage=Utils::getString(res,"downloadPage");
    if(downLoadUrl.trimmed().length()>0){
        this->ui->te_downloadUrl->setText(downLoadUrl.trimmed());

        this->ui->lb_urlType->setText("Download Url:");
        this->ui->btn_download->setText("Download");
    }
    if(downLoadPage.trimmed().length()>0){
        this->ui->te_downloadUrl->setText(downLoadPage.trimmed());

        this->ui->lb_urlType->setText("Download Page:");
        this->ui->btn_download->setText("OpenPage");
    }

}

DriverDownloadForm::~DriverDownloadForm()
{
    delete ui;
}

void DriverDownloadForm::closeEvent(QCloseEvent *event)
{
    this->hide();

    stop=true;

    if(this->connData){
        this->connData->stopConn();
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

void DriverDownloadForm::on_btn_download_clicked()
{
    this->ui->btn_download->setEnabled(false);
    auto runRef=WidgetUtils::createRunRef(this);
    LastRun lr{[=](){
            this->ui->btn_download->setEnabled(true);
            
               }};
    if(this->ui->btn_download->text()=="Download"){

        BtnLoadingIcon bli{this->ui->btn_download};

        QStringList list=this->ui->te_downloadUrl->toPlainText().trimmed().split('\n');

        if(list.isEmpty())return;

        int succNum=0;
        QString errInfo;
        for(QString urlStr:list){
            urlStr=urlStr.trimmed();
            if(urlStr.isEmpty())continue;

            QUrl url{urlStr};

            QString target=Utils::getLibPath();

            target+=url.fileName();

            if(Utils::testConnData(connData)){
                QJsonObject json;
                json.insert("funcId", DOWNLOAD_URL);
                json.insert("file", target);
                json.insert("url", url.toString());

                Result res{connData->getResJson(json)};
                if(res.testResult(false)){
                    succNum++;
                }else{
                    errInfo=res.getErrInfo();
                    if(res.getErrNo()==-901){//如果是文件只读导致的错误则不结束,继续其他文件下载
                        continue;
                    }
                    break;
                }
            }
        }

        if(succNum<=0&&errInfo.length()>0){
            QMessageBox::critical(this, QCoreApplication::applicationName(),errInfo);
            return;
        }

        WidgetUtils::showSuccDialog(QCoreApplication::applicationName(),tr("Download Success."),tr("OK"));
        this->close();

        Utils::restartServer();

    }else if(this->ui->btn_download->text()=="OpenPage"){

        QUrl url{this->ui->te_downloadUrl->toPlainText().trimmed()};
        QDesktopServices::openUrl(url);
    }
}

void DriverDownloadForm::on_btn_cancel_clicked()
{
    this->close();
}

void DriverDownloadForm::on_btn_open_clicked()
{
    QDesktopServices::openUrl(QUrl::fromLocalFile(this->ui->le_lib_path->text()));
}
