#include "conndialog.h"
#include "jarinfoeditdialog.h"
#include "ui_jarinfoeditdialog.h"
#include "widgetutils.h"

#include <QDesktopServices>
#include <QFileDialog>
#include <QTimer>
#include <QUrl>

JarInfoEditDialog::JarInfoEditDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::JarInfoEditDialog)
{
    ui->setupUi(this);

    WidgetUtils::setSafeDeleteOnClose(this);

    auto getFileBtnFunc=[=](){
        auto runRef=WidgetUtils::createRunRef(this);

        QString lastPath=ui->le_file->text().trimmed();
        if(lastPath.length()>0){
            QFileInfo fileInfo{lastPath};
            lastPath=fileInfo.path();
        }
        QFileDialog fdialog(this,QObject::tr("Open File"),lastPath,QObject::tr("Jar (*.jar);;All files(*.*)"));
        fdialog.setOption(QFileDialog::DontUseNativeDialog,true);
        fdialog.setWindowModality(Qt::WindowModal);
        fdialog.setAcceptMode(QFileDialog::AcceptOpen);
        if (fdialog.exec() == QDialog::Accepted && fdialog.selectedFiles().length()>0){
            QStringList fnames=fdialog.selectedFiles();
            QString lastName=fnames.last();

            ui->cbox_dbType->setCurrentText("");
            ui->cbox_driverClass->clear();
            this->setFile(lastName);
        }
    };
    WidgetUtils::customLineEditWidgetAddBtnFunc(ui->le_file,getFileBtnFunc,true,QIcon {":/images/open.svg"});

    WidgetUtils::customComboBoxWidget(ui->cbox_dbType);
    WidgetUtils::customComboBoxWidget(ui->cbox_driverClass);

    QStringList typeList={""};
    typeList.append(APP_DB_TYPES_FREE);
    typeList.append(APP_DB_TYPES_PRO);
    ui->cbox_dbType->addItems(typeList);
}

JarInfoEditDialog::~JarInfoEditDialog()
{
    delete ui;
}

QString JarInfoEditDialog::getFile() const
{
    return ui->le_file->text().trimmed();
}

void JarInfoEditDialog::setFile(const QString &value)
{
    file = value;

    auto runRef=WidgetUtils::createRunRef(this);

    QDir libDir{Utils::getAppDataPath()+"lib/"};

    ui->le_file->setText(libDir.absoluteFilePath(value));

    QStringList list;
    if(value.trimmed().length()>0){
        list.append(Utils::getDriverClasses(nullptr,ui->le_file->text()));
    }

    QString className=ui->cbox_driverClass->currentText().trimmed();
    ui->cbox_driverClass->clear();
    ui->cbox_driverClass->addItems(list);
    if(className.length()>0){
        WidgetUtils::setOrAddCurrItem(ui->cbox_driverClass,className);
    }
}

QString JarInfoEditDialog::getType() const
{
    return ui->cbox_dbType->currentText().trimmed();
}

void JarInfoEditDialog::setType(const QString &value)
{
    type = value;

    WidgetUtils::setCurrItem(ui->cbox_dbType,value,false);
}

QString JarInfoEditDialog::getClassName() const
{
    return ui->cbox_driverClass->currentText().trimmed();
}

void JarInfoEditDialog::setClassName(const QString &value)
{
    className = value;

    WidgetUtils::setOrAddCurrItem(ui->cbox_driverClass,value);
}

void JarInfoEditDialog::closeEvent(QCloseEvent *event)
{
    this->hide();

    stop=true;

    QTimer *timer = new QTimer(this);
    // setup signal and slot
    connect(timer, &QTimer::timeout,
            this, [=](){
                if(!WidgetUtils::isRunning(this)){
                    timer->stop();
                    this->deleteLater();
                }
            });
    // msec
    timer->start(50);

    event->ignore();
}

void JarInfoEditDialog::on_btn_ok_clicked()
{
    this->close();
    this->accept();
}

void JarInfoEditDialog::on_btn_cancel_clicked()
{
    this->close();
}
