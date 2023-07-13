#include "aboutdialog.h"
#include "activatedialog.h"
#include "globalutils.h"
#include "mainwindow.h"
#include "ui_aboutdialog.h"
#include "widgetutils.h"

#include <QDesktopServices>
#include <QTimer>

AboutDialog::AboutDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AboutDialog)
{
    ui->setupUi(this);

    WidgetUtils::setSafeDeleteOnClose(this);

#if defined(Q_OS_MAC)
    this->setWindowFlag(Qt::Tool,true);
#endif

    ui->stack_main->setCurrentIndex(0);

    ui->lb_home->setText(ui->lb_home->text().replace("${home_uri}",HOME_URI));

    this->testLicense();
}

AboutDialog::~AboutDialog()
{
    delete ui;
}

void AboutDialog::testLicense()
{
    auto runRef=WidgetUtils::createRunRef(this);

    QJsonObject license_json=GlobalUtils::getLicense();
    QString email=Utils::getString(license_json,"email");

    ui->lb_version->setText(QCoreApplication::applicationVersion());
    ui->lb_username->setText(email);

    bool is_subscription=false;
    if(!email.isEmpty()){
        is_subscription=true;
    }

    ui->wd_subscription->setVisible(is_subscription);
    ui->btn_register->setVisible(!is_subscription);
}

void AboutDialog::checkNewVersion()
{
    auto runRef=WidgetUtils::createRunRef(this);

    ui->stack_main->setCurrentWidget(ui->page_newVersion);
    ui->lb_have_new_version->setHidden(true);

    QJsonObject json;
    json.insert("func_id",FUNC_DOWNLOAD_FILE);
    QJsonObject resJson=Utils::postEsbWithNS(MainWindow::instance(),json);

    if(Utils::testResult(resJson,false)){
        QJsonArray arr=Utils::getArray(resJson,"download_list");

        if(arr.size()>0){
            QJsonObject json=arr[0].toObject();
            QString version=json.value("version").toString();
            if(!version.isEmpty()){
                ui->lb_version_last->setText(version);
                if(StringUtils::equalIncase("zh_cn",MainWindow::instance()->defaultLanguage)){
                    ui->te_release_log->setText(json.value("release_log_cn").toString());
                }else{
                    ui->te_release_log->setText(json.value("release_log_en").toString());
                }

                QStringList versionList=version.split(".");
                QStringList localVersionList=QString{VERSION_STR}.split(".");
                int eq=0;
                for (int i=0;i<(std::max(versionList.size(),localVersionList.size()));i++) {
                    if(eq!=0)break;
                    int rv=(i<versionList.size())?versionList[i].trimmed().toInt():0;
                    int lv=(i<localVersionList.size())?localVersionList[i].trimmed().toInt():0;
                    if(rv>lv){
                        eq=1;
                        break;
                    }else if(rv<lv){
                        eq=-1;
                        break;
                    }
                }
                if(eq>0){
                    ui->btn_download->setEnabled(true);
                    ui->lb_have_new_version->setHidden(false);
                }else{
                    ui->btn_download->setEnabled(false);
                    ui->lb_have_new_version->setHidden(true);
                }
            }
        }

    }
}

void AboutDialog::closeEvent(QCloseEvent *event)
{
    this->hide();

    stop=true;

    Utils::stopResEventLoop(this);

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

void AboutDialog::on_btn_register_clicked()
{
    ActivateDialog *dlg=new ActivateDialog{this};
    dlg->exec();
    this->testLicense();
}

void AboutDialog::on_btn_ok_clicked()
{
    this->close();
}

void AboutDialog::on_btn_download_clicked()
{
    QDesktopServices::openUrl(QUrl(DOWNLOAD_URI));
}

void AboutDialog::on_btn_cancel_clicked()
{
    this->close();
}
