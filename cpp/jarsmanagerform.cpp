#include "conndialog.h"
#include "jarinfoeditdialog.h"
#include "jarsmanagerform.h"
#include "ui_jarsmanagerform.h"
#include "widgetutils.h"
#include "globalutils.h"

#include <QDesktopServices>
#include <QFileDialog>
#include <QMessageBox>
#include <QSettings>
#include <QTimer>

JarsManagerForm::JarsManagerForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::JarsManagerForm)
{
    ui->setupUi(this);

    WidgetUtils::setSafeDeleteOnClose(this);

    this->ui->tree_main->setColumnCount(3);
    this->ui->tree_main->setColumnWidth(1,Utils::getCharGuiFontRect().width()*20);
    this->ui->tree_main->header()->setSectionResizeMode(0,QHeaderView::Stretch);
    this->ui->tree_main->header()->setSectionResizeMode(1,QHeaderView::Fixed);
    this->ui->tree_main->header()->setSectionResizeMode(2,QHeaderView::Stretch);
    this->ui->tree_main->setHeaderLabels(QStringList{"File","Database Type","Driver Class"});

    this->flushLibTree();
}

JarsManagerForm::~JarsManagerForm()
{
    delete ui;
}

void JarsManagerForm::flushLibTree(QString name)
{
    auto runRef=WidgetUtils::createRunRef(this);

    LastRun lr{[=](){
        if(name.length()>0){
            for (int i=0;i<ui->tree_main->topLevelItemCount();i++) {
                auto item=ui->tree_main->topLevelItem(i);
                if(item){
                    if(StringUtils::equalCase(item->text(0),name)){
                        ui->tree_main->setCurrentItem(item);
                    }
                }
            }
        }
    }};

    ui->tree_main->clear();

    QDir libDir{Utils::getAppDataPath()+"lib/"};
    QStringList jarFileNameList;
    if(libDir.exists()){
        libDir.setFilter(QDir::Files|QDir::NoSymLinks);
        libDir.setNameFilters({"*.jar"});
        jarFileNameList=libDir.entryList();
    }

    QSettings ini(Utils::getConfigFilePath(), QSettings::IniFormat);
    ini.beginGroup(STR_OPTIONS);
    QJsonObject jars = ini.value(STR_JARS).toJsonObject();

    for (auto iter=jars.begin();iter!=jars.end();){
        QJsonObject jarJson=iter.value().toObject();
        QString jarName=iter.key();
        int idx=StringUtils::indexOfIncase(jarFileNameList,jarName);
        if(idx>=0){
            jarFileNameList.removeAt(idx);

            QString dbType=jarJson.value("dbType").toString();
            QString driverClass=jarJson.value("driverClass").toString();
            QTreeWidgetItem *item=new QTreeWidgetItem{ui->tree_main,{jarName,dbType,driverClass}};

            iter++;
        }else{
            jars.erase(iter);
        }
    }

    ini.setValue(STR_JARS,jars);
    ini.endGroup();

    for(QString name:jarFileNameList){
        QTreeWidgetItem *item=new QTreeWidgetItem{ui->tree_main,{name,"",""}};
    }

    ui->tree_main->sortItems(0,Qt::AscendingOrder);

//    {//调整位置
//        QList<QTreeWidgetItem*> itemList;
//        for (int i=0;i<ui->tree_main->topLevelItemCount();) {
//            auto item=ui->tree_main->topLevelItem(i);

//            if(item&&item->text(1).length()>0&&item->text(2).length()>0){
//                i++;
//            }else{
//                item=ui->tree_main->takeTopLevelItem(i);
//                itemList.push_back(item);
//            }
//        }

//        ui->tree_main->addTopLevelItems(itemList);
//    }
}

void JarsManagerForm::closeEvent(QCloseEvent *event)
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

void JarsManagerForm::on_btn_add_driver_clicked()
{
    auto runRef=WidgetUtils::createRunRef(this);

    JarInfoEditDialog dlg{this};
    dlg.setType(this->getType());
    if(dlg.exec()){
        QString file=dlg.getFile();
        QString dbType=dlg.getType();
        QString driverClass=dlg.getClassName();

        if(file.length()>0){

            QString target=Utils::getAppDataPath()+"lib/";
            Utils::createDirs(target);

            QDir libDir{target};

            QFile srcFile{file};
            QFileInfo srcFileInfo{srcFile.fileName()};
            QString srcFileName=srcFileInfo.fileName();

            QFile tgtFile{libDir.absoluteFilePath(srcFileName)};

            if(!StringUtils::equalIncase(srcFile.fileName(),tgtFile.fileName())){
                if(tgtFile.exists()){
                    bool ok=tgtFile.remove();
                    if(!ok){//jar文件删除失败，关闭Java服务删除后再重启
                        Utils::stopServer();
                        ok=tgtFile.remove();
                        Utils::restartServer();
                    }
                }
                bool ok=srcFile.copy(tgtFile.fileName());
            }

            if(tgtFile.exists()){
                QSettings ini(Utils::getConfigFilePath(), QSettings::IniFormat);
                ini.beginGroup(STR_OPTIONS);
                QJsonObject jars=ini.value(STR_JARS).toJsonObject();
                QJsonObject json;
                json.insert("dbType",dbType);
                json.insert("driverClass",driverClass);
                jars.insert(srcFileName,json);
                ini.setValue(STR_JARS,jars);
                ini.endGroup();

                ini.sync();

                this->flushLibTree(srcFileName);

                Utils::restartServer();
            }else{
                QMessageBox::critical(this, QCoreApplication::applicationName(),tr("Failed!"));
                return;
            }
        }
    }
}

void JarsManagerForm::on_btn_add_jar_clicked()
{
    auto runRef=WidgetUtils::createRunRef(this);

    QString lastPath="";
    QFileDialog fdialog(this,QObject::tr("Add Jar"),lastPath,QObject::tr("Jar (*.jar);;All files(*.*)"));
    fdialog.setOption(QFileDialog::DontUseNativeDialog,true);
    fdialog.setWindowModality(Qt::WindowModal);
    fdialog.setAcceptMode(QFileDialog::AcceptOpen);
    if (fdialog.exec() == QDialog::Accepted && fdialog.selectedFiles().length()>0){
        QStringList fnames=fdialog.selectedFiles();
        QString lastName=fnames.last();

        QString target=Utils::getAppDataPath()+"lib/";
        Utils::createDirs(target);

        QDir libDir{target};

        QFile srcFile{lastName};
        QFileInfo srcFileInfo{srcFile.fileName()};
        QString srcFileName=srcFileInfo.fileName();

        QFile tgtFile{libDir.absoluteFilePath(srcFileName)};
        if(!StringUtils::equalIncase(srcFile.fileName(),tgtFile.fileName())){
            if(tgtFile.exists()){
                bool ok=tgtFile.remove();
                if(!ok){//jar文件删除失败，关闭Java服务删除后再重启
                    Utils::stopServer();
                    ok=tgtFile.remove();
                    Utils::restartServer();
                }
            }
            bool ok=srcFile.copy(tgtFile.fileName());
        }

        if(tgtFile.exists()){
            QTreeWidgetItem *item=new QTreeWidgetItem{ui->tree_main,{srcFileName,"",""}};

            QSettings ini(Utils::getConfigFilePath(), QSettings::IniFormat);
            ini.beginGroup(STR_OPTIONS);
            QJsonObject jars=ini.value(STR_JARS).toJsonObject();
            QJsonObject json;
            json.insert("dbType","");
            json.insert("driverClass","");
            jars.insert(srcFileName,json);
            ini.setValue(STR_JARS,jars);
            ini.endGroup();

            ini.sync();

            this->flushLibTree(srcFileName);

            Utils::restartServer();
        }else{
            QMessageBox::critical(this, QCoreApplication::applicationName(),tr("Failed!"));
            return;
        }
    }
}

void JarsManagerForm::on_btn_delete_clicked()
{
    auto runRef=WidgetUtils::createRunRef(this);

    auto item=ui->tree_main->currentItem();
    if(!item){
        QMessageBox::critical(this, QCoreApplication::applicationName(),tr("No rows selected"));
        return;
    }

    QString sql=QString{"Delete file %1"}.arg(item->text(0));

    const int ret
        = QMessageBox::warning(this, QCoreApplication::applicationName(),
                               tr("%1?").arg(sql),
                               tr("Delete") ,tr("Cancel"),"",1);
    switch (ret) {
    case 0:{
        }
        break;
    case 1:{
        return;
        }
        break;
    default:
        return;
    }

    QDir libDir{Utils::getAppDataPath()+"lib/"};

    QFile tgtFile{libDir.absoluteFilePath(item->text(0))};
    if(tgtFile.exists()){
        bool ok=tgtFile.remove();
        if(!ok){//jar文件删除失败，关闭Java服务删除后再重启
            Utils::stopServer();
            ok=tgtFile.remove();
            Utils::restartServer();
            if(!ok){
                QMessageBox::critical(this, QCoreApplication::applicationName(),tr("File deletion failed!"));
                return;
            }
        }
    }

    this->flushLibTree();

}

void JarsManagerForm::on_btn_edit_clicked()
{
    auto runRef=WidgetUtils::createRunRef(this);

    auto item=ui->tree_main->currentItem();
    if(!item){
        QMessageBox::critical(this, QCoreApplication::applicationName(),tr("No rows selected"));
        return;
    }

    QString file=item->text(0);
    QString dbType=item->text(1);
    QString driverClass=item->text(2);

    JarInfoEditDialog dlg{this};
    dlg.setClassName(driverClass);//在setFile之前setClassName,让className为空时有默认值
    dlg.setFile(file);
    dlg.setType(dbType);
    if(dlg.exec()){
        file=dlg.getFile();
        dbType=dlg.getType();
        driverClass=dlg.getClassName();

        if(file.length()>0){

            QString target=Utils::getAppDataPath()+"lib/";
            Utils::createDirs(target);

            QDir libDir{target};

            QFile srcFile{file};
            QFileInfo srcFileInfo{srcFile.fileName()};
            QString srcFileName=srcFileInfo.fileName();

            QFile tgtFile{libDir.absoluteFilePath(srcFileName)};

            if(!StringUtils::equalIncase(srcFile.fileName(),tgtFile.fileName())){
                if(tgtFile.exists()){
                    bool ok=tgtFile.remove();
                    if(!ok){//jar文件删除失败，关闭Java服务删除后再重启
                        Utils::stopServer();
                        ok=tgtFile.remove();
                        Utils::restartServer();
                    }
                }
                bool ok=srcFile.copy(tgtFile.fileName());
            }

            if(tgtFile.exists()){
                QSettings ini(Utils::getConfigFilePath(), QSettings::IniFormat);
                ini.beginGroup(STR_OPTIONS);
                QJsonObject jars=ini.value(STR_JARS).toJsonObject();
                jars.remove(item->text(0));
                QJsonObject json;
                json.insert("dbType",dbType);
                json.insert("driverClass",driverClass);
                jars.insert(srcFileName,json);
                ini.setValue(STR_JARS,jars);
                ini.endGroup();

                ini.sync();

                this->flushLibTree(srcFileName);

                Utils::restartServer();
            }else{
                QMessageBox::critical(this, QCoreApplication::applicationName(),tr("Failed!"));
                return;
            }
        }
    }

}

void JarsManagerForm::on_btn_lib_path_clicked()
{
    auto runRef=WidgetUtils::createRunRef(this);

    QDir libDir{Utils::getAppDataPath()+"lib/"};
    Utils::createDirs(libDir.absolutePath());

    QDesktopServices::openUrl(QUrl::fromLocalFile(libDir.absolutePath()));
}

void JarsManagerForm::on_tree_main_itemDoubleClicked(QTreeWidgetItem *item, int column)
{
    this->on_btn_edit_clicked();
}

QString JarsManagerForm::getType() const
{
    return type;
}

void JarsManagerForm::setType(const QString &value)
{
    type = value;
}
