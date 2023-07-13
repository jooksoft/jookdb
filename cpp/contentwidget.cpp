#include "conndialog.h"
#include "contentwidget.h"
#include "mainwindow.h"
#include "queryform.h"
#include "sqlstyle.h"
#include "tabledataform.h"
#include "tableform.h"
#include "utils.h"
#include "widgetutils.h"
#include "globalutils.h"
#include "activatedialog.h"

#include <QLayout>
#include <QDebug>
#include <QSplitter>
#include <QListView>
#include <QTextEdit>
#include <QSettings>
#include <QMessageBox>
#include <QMenu>
#include <map>
#include <thread>
#include <QGuiApplication>
#include <QTimer>
#include <QDesktopServices>

#define TITLE_MAX_LEN 40

ContentWidget::ContentWidget(QWidget *parent) : QWidget(parent),leftWidgetForm(new LeftWidgetForm{this}),rightTab(new QTabWidget{this})
{
    setLayout(new QHBoxLayout());
    this->layout()->setContentsMargins(0, 0, 0, 0);
    this->layout()->setSpacing(0);

//    WidgetUtils::setSafeDeleteOnClose(this);

    contentSplitter = new QSplitter(Qt::Horizontal,this);
    contentSplitter->setContentsMargins(0, 0, 0, 0);
    contentSplitter->addWidget(this->leftWidgetForm);
    contentSplitter->addWidget(this->rightTab);

    this->layout()->addWidget(contentSplitter);

    connect(this->rightTab,&QTabWidget::currentChanged,this,[&](int index){

        if(index<0)return;

        this->tabChgCount++;

        MainWindow::instance()->clearStatusLabels();

        int count=this->rightTab->count();
        for (int i=0;i<count;i++) {
            if(i==index){//设置只有当前页有关闭按钮，其他页隐藏关闭按钮
                if(this->rightTab->tabBar()->tabButton(i, QTabBar::RightSide)){
                    this->rightTab->tabBar()->tabButton(i, QTabBar::RightSide)->setVisible(true);
                }
                if(this->rightTab->tabBar()->tabButton(i, QTabBar::LeftSide)){
                    this->rightTab->tabBar()->tabButton(i, QTabBar::LeftSide)->setVisible(true);
                }

                QString title=this->rightTab->tabToolTip(index);
                MainWindow::instance()->setCurrentTitle(title);

            }else{
                if(this->rightTab->tabBar()->tabButton(i, QTabBar::RightSide)){
                    this->rightTab->tabBar()->tabButton(i, QTabBar::RightSide)->setVisible(false);
                }
                if(this->rightTab->tabBar()->tabButton(i, QTabBar::LeftSide)){
                    this->rightTab->tabBar()->tabButton(i, QTabBar::LeftSide)->setVisible(false);
                }
            }
        }

        auto qf=this->getCurrQueryForm();
        if(qf){
            MainWindow::instance()->setCurrentTitle(qf->getMyEdit()->fileName);
            MainWindow::instance()->saveAct->setEnabled(qf->getMyEdit()->modify());
//            MainWindow::instance()->saveAct->setVisible(true);
            qf->updateStatusBar();
            qf->updateSqlStatus();
            return;
        }
//        MainWindow::instance()->saveAct->setVisible(false);

        auto tf=this->getCurrTableForm();
        if(tf){
            MainWindow::instance()->saveAct->setEnabled(tf->modified);
            tf->updateStatusBar();
            return;
        }
        auto tdf=this->getCurrTableDataForm();
        if(tdf){
            MainWindow::instance()->saveAct->setEnabled(tdf->modified);
            tdf->updateStatusBar();//更新状态栏
            return;
        }

        auto omf=this->getCurrObjectManagerForm();
        if(omf){
            omf->updateStatusBar();//更新状态栏
            return;
        }

        auto dbf=this->getCurrDatabaseManagerForm();
        if(dbf){
            dbf->updateStatusBar();//更新状态栏
            return;
        }

        MainWindow::instance()->saveAct->setEnabled(false);

    },Qt::QueuedConnection);

    this->rightTab->setMovable(true);
    this->rightTab->setTabsClosable(true);
    connect(this->rightTab, &QTabWidget::tabCloseRequested,this, [this](const int &idx){
        if(idx<0)return;

        QWidget* gotoWd=nullptr;
        {
            QWidget* currWd=this->rightTab->widget(idx);
            if(currWd){
                int tabChgCount=currWd->property("sy_tabChgCount").toInt();
                if(tabChgCount==this->tabChgCount){
                    gotoWd=currWd->property("sy_tabPreWid").value<QWidget*>();
                }
            }
        }

        if(this->closeTab(idx)){

            if(gotoWd){
                for (int i=0;i<this->rightTab->count();i++) {
                    QWidget* wd=this->rightTab->widget(i);
                    if(wd==gotoWd){
                        this->rightTab->setCurrentIndex(i);
                        break;
                    }
                }
            }
        }


    });

    this->rightTab->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this->rightTab, &QTabWidget::customContextMenuRequested,this, [this](const QPoint &point){
        int clickedItem = -1;
        QTabBar* tabBars=this->rightTab->tabBar();
        if(tabBars){
            for(int i=0;i<tabBars->count();i++)
            {
                if (tabBars->tabRect(i).contains(point))
                {
                        clickedItem = i;
                        break;
                }
            }
        }
        if(clickedItem>=0){
            QMenu *menu=new QMenu{this};
            menu->deleteLater();
            QAction *act = new QAction(tr("Close"), menu);
            connect(act, &QAction::triggered, this, [&](){
                MainWindow::getContent()->closeTab(clickedItem);
            });
            menu->addAction(act);

            act = new QAction(tr("Close Others"), menu);
            connect(act, &QAction::triggered, this, [&](){

                int count=this->getRightTab()->count();
                for(int i=count-1;i>=0;i--){
                    if(i!=clickedItem){
                        if(!this->closeTab(i)){
                            break;
                        }
                    }
                }
            });
            menu->addAction(act);

            act = new QAction(tr("Close Right Others"), menu);
            connect(act, &QAction::triggered, this, [&](){

                int count=this->getRightTab()->count();
                for(int i=count-1;i>=0;i--){
                    if(i!=clickedItem){
                        if(!this->closeTab(i)){
                            break;
                        }
                    }else{
                        break;
                    }
                }
            });
            menu->addAction(act);

            act = new QAction(tr("Close All"), menu);
            connect(act, &QAction::triggered, this, [&](){

                int count=this->getRightTab()->count();
                for(int i=count-1;i>=0;i--){
                    if(!this->closeTab(i)){
                        break;
                    }
                }
            });
            menu->addAction(act);

            auto widg=this->rightTab->widget(clickedItem);

            if(widg&&widg->inherits(QueryForm::staticMetaObject.className())){

                QueryForm * qf=static_cast<QueryForm *>(widg);
                auto te=qf->getMyEdit();
                if(te->fileName.length()>0&&Utils::isFileExist(te->fileName)){

                    menu->addSeparator();

                    act = new QAction(tr("Show in Explorer"), menu);
                    connect(act, &QAction::triggered, this, [=](){

                        WidgetUtils::showFileInFolder(te->fileName);
                    });
                    menu->addAction(act);
                }

            }

            menu->exec(QCursor::pos());
        }

    });

}

MyEdit *ContentWidget::getCurrTextEdit()
{
    QueryForm *qf=getCurrQueryForm();
    if(!qf){
        return nullptr;
    }
    return qf->getMyEdit();
}

QueryForm *ContentWidget::getCurrQueryForm()
{
    auto widget=this->rightTab->currentWidget();
    if(!widget){
        return nullptr;
    }
    if(widget->inherits(QueryForm::staticMetaObject.className())){
        return (QueryForm *)widget;
    }
    return nullptr;
}

TableForm *ContentWidget::getCurrTableForm()
{
    auto widget=this->rightTab->currentWidget();
    if(!widget){
        return nullptr;
    }
    if(widget->inherits(TableForm::staticMetaObject.className())){
        return (TableForm *)widget;
    }
    return nullptr;
}

TableDataForm *ContentWidget::getCurrTableDataForm()
{
    auto widget=this->rightTab->currentWidget();
    if(!widget){
        return nullptr;
    }
    if(widget->inherits(TableDataForm::staticMetaObject.className())){
        return (TableDataForm *)widget;
    }
    return nullptr;
}

ObjectManagerForm *ContentWidget::getCurrObjectManagerForm()
{
    auto widget=this->rightTab->currentWidget();
    if(!widget){
        return nullptr;
    }
    if(widget->inherits(ObjectManagerForm::staticMetaObject.className())){
        return (ObjectManagerForm *)widget;
    }
    return nullptr;
}

DatabaseManagerForm *ContentWidget::getCurrDatabaseManagerForm()
{
    auto widget=this->rightTab->currentWidget();
    if(!widget){
        return nullptr;
    }
    if(widget->inherits(DatabaseManagerForm::staticMetaObject.className())){
        return (DatabaseManagerForm *)widget;
    }
    return nullptr;
}

MyTreeWidget *ContentWidget::getLeftTree() const
{
    return this->leftWidgetForm->getLeftTree();
}

QTabWidget *ContentWidget::getRightTab() const
{
    return this->rightTab;
}


QSplitter *ContentWidget::getSplitter()
{
    return this->contentSplitter;
}

int ContentWidget::addTabWithToolTip(QWidget *wid, const QString &title, const QString &tip)
{
    if(!wid)return -1;

    int idx=this->rightTab->addTab(wid,(title.length()>TITLE_MAX_LEN?title.left(TITLE_MAX_LEN-3)+"...":title));
    if(idx>=0){
        wid->setProperty("sy_tabChgCount",QVariant::fromValue(this->tabChgCount+1));
        wid->setProperty("sy_tabPreWid",QVariant::fromValue(this->rightTab->currentWidget()));

        this->rightTab->setCurrentIndex(idx);
        this->rightTab->setTabToolTip(idx,tip);
    }
    return idx;
}

QueryForm * ContentWidget::addTab(QString title,QString db,QString dbName)
{
    QueryForm *qf=new QueryForm(this);
    if(title.isEmpty()){
        title="Untitled";
    }

    this->addTabWithToolTip(qf,title,title);

    if(!db.isEmpty()){
        qf->refreshDB();
        qf->setCurrDbComboxItem(db);
        if(!dbName.isEmpty()){
            qf->setCurrDBNameComboxItem(dbName);
        }
    }
    return qf;
}

TableForm * ContentWidget::addTableTab(QSharedPointer<ConnData> connData, QString tableName, QString dbName,bool readonly,bool isView)
{

    if(!connData){
        return nullptr;
    }
    QString title;
    QString titleDetail;

    title=tableName+"@"+dbName;
    titleDetail=tableName+"@"+dbName+"/"+connData->connName;

//    int count=this->rightTab->count();
//    for(int i=count-1;i>=0;i--){//如果已经存在则直接返回
//        auto widg=this->rightTab->widget(i);
//        if(!widg){
//            continue;
//        }
//        if(widg->inherits(TableForm::staticMetaObject.className())){
//            auto tf=(TableForm*)widg;
//            if(!tf->getConnData()){
//                continue;
//            }
//            if(tf->getConnData()->connName==connData->connName&&tf->getDBName()==dbName&&tf->getTableName()==tableName&&tf->getReadonly()==readonly){
//                this->rightTab->setCurrentIndex(i);
//                return tf;
//            }
//        }
//    }

    TableForm *tf=new TableForm(connData,tableName,dbName,this);
    tf->setReadonly(readonly);
    if(title.isEmpty()){
        title="Untitled";
    }

    int idx=this->addTabWithToolTip(tf,title,titleDetail);
    if(idx>=0){
        if(isView){
            this->rightTab->setTabIcon(idx,QIcon{":/images/view.svg"});
        }else{
            this->rightTab->setTabIcon(idx,QIcon{":/images/table.svg"});
        }
    }
    return tf;
}

TableDataForm * ContentWidget::addTableDataTab(QSharedPointer<ConnData> connData, QString tableName, QString dbName, bool readonly)
{

    if(!connData){
        return nullptr;
    }
    QString title;
    QString titleDetail;

    title=tableName+"@"+dbName;
    titleDetail=tableName+"@"+dbName+"/"+connData->connName;

//    int count=this->rightTab->count();
//    for(int i=count-1;i>=0;i--){//如果已经存在则直接返回
//        auto widg=this->rightTab->widget(i);
//        if(!widg){
//            continue;
//        }
//        if(widg->inherits(TableDataForm::staticMetaObject.className())){
//            auto tf=(TableDataForm*)widg;
//            if(!tf->getConnData()){
//                continue;
//            }
//            if(tf->getConnData()->connName==connData->connName&&tf->getDBName()==dbName&&tf->getTableName()==tableName&&tf->getReadonly()==readonly){
//                this->rightTab->setCurrentIndex(i);
//                return tf;
//            }
//        }
//    }

    TableDataForm *tdf=new TableDataForm(connData,tableName,dbName,this);
    tdf->setReadonly(readonly);
    if(title.isEmpty()){
        title="Untitled";
    }

    int idx=this->addTabWithToolTip(tdf,title,titleDetail);
    if(idx>=0){
        if(readonly){
            this->rightTab->setTabIcon(idx,QIcon{":/images/view_data.svg"});
        }else{
            this->rightTab->setTabIcon(idx,QIcon{":/images/edit_data.svg"});
        }
    }

    return tdf;
}

ObjectManagerForm * ContentWidget::addObjectManager(QSharedPointer<ConnData> connData, QString dbName,bool isDump,QString dumpTableName)
{


    if(!connData){
        return nullptr;
    }
    QString title;
    QString titleDetail;
    if(isDump){

        title="Dump "+dbName;;
        titleDetail="Dump "+dbName+"@"+connData->connName;
    }else{

        title="Objects "+dbName;;
        titleDetail="Objects "+dbName+"@"+connData->connName;
    }

//    int count=this->rightTab->count();
//    for(int i=count-1;i>=0;i--){//如果已经存在则直接返回
//        auto widg=this->rightTab->widget(i);
//        if(!widg){
//            continue;
//        }
//        if(widg->inherits(ObjectManagerForm::staticMetaObject.className())){
//            auto omf=(ObjectManagerForm*)widg;
//            if(!omf->getConnData()){
//                continue;
//            }
//            if(omf->getConnData()->connName==connData->connName&&omf->getDBName()==dbName&&omf->getIsDump()==isDump){
//                this->rightTab->setCurrentIndex(i);
//                if(isDump&&!dumpTableName.isEmpty()){
//                    omf->setDumpTableName(dumpTableName);
//                    omf->refresh();
//                }
//                return omf;
//            }
//        }
//    }

    ObjectManagerForm *omf=new ObjectManagerForm(connData,dbName,isDump,this);

    if(isDump&&!dumpTableName.isEmpty()){
        omf->setDumpTableName(dumpTableName);
    }

    if(title.isEmpty()){
        title="Untitled";
    }

    this->addTabWithToolTip(omf,title,titleDetail);

    return omf;
}

DatabaseManagerForm *ContentWidget::addDatabaseManager(QSharedPointer<ConnData> connData)
{

    if(!connData){
        return nullptr;
    }
    QString title;
    title="Databases "+connData->connName;
    QString titleDetail="Databases "+connData->connName;;

//    int count=this->rightTab->count();
//    for(int i=count-1;i>=0;i--){//如果已经存在则直接返回
//        auto widg=this->rightTab->widget(i);
//        if(!widg){
//            continue;
//        }
//        if(widg->inherits(DatabaseManagerForm::staticMetaObject.className())){
//            auto dbmf=(DatabaseManagerForm*)widg;
//            if(!dbmf->getConnData()){
//                continue;
//            }
//            if(dbmf->getConnData()->connName==connData->connName){
//                this->rightTab->setCurrentIndex(i);
//                return dbmf;
//            }
//        }
//    }

    DatabaseManagerForm *dbmf=new DatabaseManagerForm(connData,this);

    if(title.isEmpty()){
        title="Untitled";
    }

    this->addTabWithToolTip(dbmf,title,titleDetail);

    return dbmf;
}

bool ContentWidget::closeTab(const int idx)
{
    auto widg=this->rightTab->widget(idx);
    if(!widg){
        return false;
    }
    if(widg->inherits(QueryForm::staticMetaObject.className())){

        QueryForm * qf=(QueryForm *)widg;
        if(WidgetUtils::isRunning(qf)){

            MainWindow::instance()->setCurrentTitle(qf->getMyEdit()->fileName);
            this->rightTab->setCurrentIndex(idx);

            const int ret
                = QMessageBox::warning(this, QCoreApplication::applicationName(),
                                       tr("The window is running, do you want to stop it?"),
                                       tr("Stop"),tr("Cancel"));
            switch (ret) {
            case 0:
                break;
            case 1:
                return false;
            default:
                return false;
            }
        }

        if(!qf->getMyEdit()->modify()){
            this->rightTab->removeTab(idx);
            qf->safeDelete();
            return true;
        }else{

            MainWindow::instance()->setCurrentTitle(qf->getMyEdit()->fileName);
            this->rightTab->setCurrentIndex(idx);

            const int ret
                = QMessageBox::warning(this, QCoreApplication::applicationName(),
                                       tr("The document has been modified.\nDo you want to save your changes?"),
                                       tr("Save") ,tr("Don't Save") ,tr("Cancel"));
            switch (ret) {
            case 0:
                if(MainWindow::instance()->save()){
                    this->rightTab->removeTab(idx);
                    qf->safeDelete();
                    return true;
                }
                break;
            case 1:
                this->rightTab->removeTab(idx);
                qf->safeDelete();
                return true;
            case 2:
                return false;
            default:
                break;
            }
        }
    }else if(widg->inherits(TableForm::staticMetaObject.className())){
        TableForm * tf=(TableForm *)widg;
        if(WidgetUtils::isRunning(tf)){

            this->rightTab->setCurrentIndex(idx);

            const int ret
                = QMessageBox::warning(this, QCoreApplication::applicationName(),
                                       tr("The window is running, do you want to stop it?"),
                                       tr("Stop"),tr("Cancel"));
            switch (ret) {
            case 0:
                break;
            case 1:
                return false;
            default:
                return false;
            }
        }

        if(!tf->modified){
            this->rightTab->removeTab(idx);
            tf->safeDelete();
            return true;
        }else{

            this->rightTab->setCurrentIndex(idx);

            const int ret
                = QMessageBox::warning(this, QCoreApplication::applicationName(),
                                       tr("The Table has been modified.\nDo you want to save your changes?"),
                                       tr("Save") ,tr("Don't Save") ,tr("Cancel"));
            switch (ret) {
            case 0:
                if(tf->save()){
                    this->rightTab->removeTab(idx);
                    tf->safeDelete();
                    return true;
                }
                break;
            case 1:
                this->rightTab->removeTab(idx);
                tf->safeDelete();
                return true;
            case 2:
                return false;
            default:
                break;
            }
        }
    }else if(widg->inherits(TableDataForm::staticMetaObject.className())){
        TableDataForm * tf=(TableDataForm *)widg;
        if(WidgetUtils::isRunning(tf)){

            this->rightTab->setCurrentIndex(idx);

            const int ret
                = QMessageBox::warning(this, QCoreApplication::applicationName(),
                                       tr("The window is running, do you want to stop it?"),
                                       tr("Stop"),tr("Cancel"));
            switch (ret) {
            case 0:
                break;
            case 1:
                return false;
            default:
                return false;
            }
        }

        if(!tf->modified){
            this->rightTab->removeTab(idx);
            tf->safeDelete();
            return true;
        }else{

            this->rightTab->setCurrentIndex(idx);

            const int ret
                = QMessageBox::warning(this, QCoreApplication::applicationName(),
                                       tr("The Data has been modified.\nDo you want to save your changes?"),
                                       tr("Save") ,tr("Don't Save") ,tr("Cancel"));
            switch (ret) {
            case 0:
                if(tf->save()){
                    this->rightTab->removeTab(idx);
                    tf->safeDelete();
                    return true;
                }
                break;
            case 1:
                this->rightTab->removeTab(idx);
                tf->safeDelete();
                return true;
            case 2:
                return false;
            default:
                break;
            }
        }
    }else if(widg->inherits(ObjectManagerForm::staticMetaObject.className())){
        ObjectManagerForm * omf=(ObjectManagerForm *)widg;
        if(WidgetUtils::isRunning(omf)){

            this->rightTab->setCurrentIndex(idx);

            const int ret
                = QMessageBox::warning(this, QCoreApplication::applicationName(),
                                       tr("The window is running, do you want to stop it?"),
                                       tr("Stop"),tr("Cancel"));
            switch (ret) {
            case 0:
                break;
            case 1:
                return false;
            default:
                return false;
            }
        }

        this->rightTab->removeTab(idx);
        omf->safeDelete();
        return true;

    }else if(widg->inherits(DatabaseManagerForm::staticMetaObject.className())){
        DatabaseManagerForm * dbmf=(DatabaseManagerForm *)widg;
        if(WidgetUtils::isRunning(dbmf)){

            this->rightTab->setCurrentIndex(idx);

            const int ret
                = QMessageBox::warning(this, QCoreApplication::applicationName(),
                                       tr("The window is running, do you want to stop it?"),
                                       tr("Stop"),tr("Cancel"));
            switch (ret) {
            case 0:
                break;
            case 1:
                return false;
            default:
                return false;
            }
        }

        this->rightTab->removeTab(idx);
        dbmf->safeDelete();
        return true;

    }
    return false;
}

void ContentWidget::setTabModified(QWidget *widget, bool modified)
{
    auto rt=getRightTab();
    for (int i=0;i<rt->count();i++) {
        if(widget==rt->widget(i)){
            QString title=rt->tabText(i);
            if(modified){
                if(!title.startsWith("* ")){
                    rt->setTabText(i,"* "+title);
                }
            }else{
                if(title.startsWith("* ")){
                    rt->setTabText(i,title.replace("* ",""));
                }
            }

            MainWindow::instance()->saveAct->setEnabled(modified);
            break;
        }
    }
}

void ContentWidget::setTabTitle(QWidget *widget, const QString &title, const QString &tip,bool keepModify)
{
    auto rt=getRightTab();
    for (int i=0;i<rt->count();i++) {
        if(widget==rt->widget(i)){
            QString oldtitle=rt->tabText(i);
            QString modifyFlag;
            if(keepModify&&oldtitle.startsWith("* ")){
                modifyFlag="* ";
            }
            rt->setTabText(i,modifyFlag+(title.length()>TITLE_MAX_LEN?title.left(TITLE_MAX_LEN-3)+"...":title));
            rt->setTabToolTip(i,tip);

            break;
        }
    }
}

void ContentWidget::setContentSplitterSizes(const QList<QVariant> &sizes)
{
    if(sizes.size()<2||sizes[0].toInt()<10||sizes[1].toInt()<10){
        contentSplitter->setSizes(QList{250,750});
    }else{
        contentSplitter->setSizes(QList{sizes[0].toInt(),sizes[1].toInt()});
    }
    contentSplitter->setStretchFactor(1,1);//右边控件拉伸，左边控件固定
}


void ContentWidget::documentWasModified()
{
    //    setWindowModified(this->textEdit->modify());
}

void ContentWidget::openConnDlg(const QString &connType)
{
    if(!GlobalUtils::checkProLicense(false,connType)){
        return;
    }

    ConnDialog* dlg=new ConnDialog(connType,nullptr,this);
    dlg->show();
}

QLineEdit *ContentWidget::getLeftTree_filter() const
{
    return this->leftWidgetForm->getLeftTreeFilter();
}

void ContentWidget::closeEvent(QCloseEvent *event)
{
    this->hide();

    stop=true;

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


