#include "cmask.h"
#include "databasemanagerform.h"
#include "editdatabasedialog.h"
#include "globalutils.h"
#include "lineeditdelegate.h"
#include "mainwindow.h"
#include "ui_databasemanagerform.h"
#include "usermanagerdialog.h"
#include "widgetutils.h"

#include <QMessageBox>
#include <QMessageBox>
#include <qmenu.h>
#include <qtimer.h>

static const int COL_ID=0;
static const int COL_NAME=1;
static const int COL_SIZE=2;
static const int COL_CREATED=3;
static const int COL_LAST_UPDATED=4;
static const int COL_TABLES=5;
static const int COL_VIEWS=6;
static const int COL_FUNCTIONS=7;
static const int COL_PROCEDURES=8;
static const int COL_TRIGGERS=9;
static const int COL_EVENTS=10;
static const int COL_DEFAULT_CHARSET=11;
static const int COL_DEFAULT_COLLATION=12;

static const int COL_PRO_ID=1;
static const int COL_PRO_USER=2;
static const int COL_PRO_HOST=3;
static const int COL_PRO_DB=4;
static const int COL_PRO_COMMAND=5;
static const int COL_PRO_TIME=6;
static const int COL_PRO_STATE=7;
static const int COL_PRO_INFO=8;

static const int COL_VAR_NAME=1;
static const int COL_VAR_SESSION=2;
static const int COL_VAR_GLOBAL=3;
static const int COL_VAR_FILE=4;
static const int COL_VAR_INFO=5;

DatabaseManagerForm::DatabaseManagerForm(QSharedPointer<ConnData> connData,QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DatabaseManagerForm),
    connData(connData)
{
    ui->setupUi(this);

    WidgetUtils::setSafeDeleteOnClose(this);

    this->ui->tw_databases->setColumnCount(13);
    this->ui->tw_databases->setHorizontalHeaderLabels({"ID","Database","Size (MiB)","Created","Updated","Tables","Views","Functions","Procedures","Triggers","Events","Default Chatset","Default Collation"});
    this->ui->tw_databases->hideColumn(0);

    this->ui->tw_processes->setColumnCount(9);
    this->ui->tw_processes->setHorizontalHeaderLabels({"ID","ID","User","Host","DB","Command","Time","State","Info"});
    this->ui->tw_processes->hideColumn(0);

    this->ui->tw_variables->setColumnCount(6);
    this->ui->tw_variables->setHorizontalHeaderLabels({"ID","Variable","Session","Global","File","Info"});
    this->ui->tw_variables->hideColumn(0);

    LineEditDelegate *led=new LineEditDelegate{this};
    led->setReadonly(true);
    led->setUseTextEdit(true);

    LineEditDelegate *strEditLed=new LineEditDelegate{this};
    strEditLed->setReadonly(false);
    strEditLed->setUseTextEdit(true);

    LineEditDelegate *numLed=new LineEditDelegate{this};
    numLed->setReadonly(true);
    numLed->setUseTextEdit(false);
    numLed->setTextAlign(Qt::AlignRight);

    this->ui->tw_databases->setItemDelegate(led);

    this->ui->tw_processes->setItemDelegate(led);

    this->ui->tw_variables->setItemDelegate(led);

    this->ui->tw_databases->setItemDelegateForColumn(COL_SIZE,numLed);
    this->ui->tw_databases->setItemDelegateForColumn(COL_TABLES,numLed);
    this->ui->tw_databases->setItemDelegateForColumn(COL_VIEWS,numLed);
    this->ui->tw_databases->setItemDelegateForColumn(COL_FUNCTIONS,numLed);
    this->ui->tw_databases->setItemDelegateForColumn(COL_PROCEDURES,numLed);
    this->ui->tw_databases->setItemDelegateForColumn(COL_TRIGGERS,numLed);
    this->ui->tw_databases->setItemDelegateForColumn(COL_EVENTS,numLed);

    this->ui->tw_processes->setItemDelegateForColumn(COL_PRO_ID,numLed);
    this->ui->tw_processes->setItemDelegateForColumn(COL_PRO_TIME,numLed);

    this->ui->tw_variables->setItemDelegateForColumn(COL_VAR_SESSION,strEditLed);
    this->ui->tw_variables->setItemDelegateForColumn(COL_VAR_GLOBAL,strEditLed);


    auto sortFunc=[this](int idx){

        if(idx<0)return;
        auto runRef=WidgetUtils::createRunRef(this);

        QTableWidgetItem *item=this->ui->tw_databases->horizontalHeaderItem(idx);
        QVariant var=item->data(Qt::UserRole);
        for(int i=0;i<this->ui->tw_databases->columnCount();i++){
            if(i==idx)continue;
            auto colItem=this->ui->tw_databases->horizontalHeaderItem(i);
            colItem->setIcon(QIcon{":/images/sort.svg"});
            colItem->setData(Qt::UserRole,0);
        }
        if(var.isNull()||var.toInt()==0){
            item->setIcon(QIcon{":/images/sort_asc.svg"});
            WidgetUtils::sortTableWidget(this->ui->tw_databases,idx,Qt::AscendingOrder);
            item->setData(Qt::UserRole,1);
        }else if(var.toInt()==1){
            item->setIcon(QIcon{":/images/sort_desc.svg"});
            WidgetUtils::sortTableWidget(this->ui->tw_databases,idx,Qt::DescendingOrder);
            item->setData(Qt::UserRole,2);
        }else{
            item->setIcon(QIcon{":/images/sort.svg"});
            this->ui->tw_databases->sortByColumn(0,Qt::AscendingOrder);
            item->setData(Qt::UserRole,0);
        }
    };

    auto menuFunc=[this](){
        auto runRef=WidgetUtils::createRunRef(this);

        int currRow=this->ui->tw_databases->currentRow();

        QMenu *menu=new QMenu(this->ui->tw_databases);
        menu->deleteLater();

        if(currRow>=0){
            QString dbName=this->ui->tw_databases->item(currRow,COL_NAME)->data(Qt::EditRole).toString();

            auto act = new QAction(tr("Objects Manager"), menu);
            connect(act, &QAction::triggered, this, [=](){
                auto runRef=WidgetUtils::createRunRef(this);

                MainWindow::getContent()->addObjectManager(this->connData,dbName);

            });
            menu->addAction(act);

            menu->addSeparator();

            if(this->connData){
                auto addEditDbAction=[=](){
                    auto act = new QAction(GlobalUtils::getLabel(this->connData,LABEL_EDIT_DB), menu);
                    connect(act, &QAction::triggered, this, [=](){
                        auto runRef=WidgetUtils::createRunRef(this);

                        EditDatabaseDialog *edia=new EditDatabaseDialog{this->connData,dbName,EditDatabaseType::editDatabase,this};

                        edia->show();
                    });
                    menu->addAction(act);

                    act = new QAction(GlobalUtils::getLabel(this->connData,LABEL_CREATE_DB), menu);
                    connect(act, &QAction::triggered, this, [=](){
                        auto runRef=WidgetUtils::createRunRef(this);

                        EditDatabaseDialog *edia=new EditDatabaseDialog{this->connData,"",EditDatabaseType::newDatabase,this};

                        edia->show();
                    });
                    menu->addAction(act);
                };

                switch (this->connData->typeId) {
                }
                }
            }

            act = new QAction(GlobalUtils::getLabel(this->connData,LABEL_DROP_DB), menu);
            connect(act, &QAction::triggered, this, [=](){
                auto runRef=WidgetUtils::createRunRef(this);

                if(Utils::testOrReconn(this->connData)){
                    QString database=dbName;

                    QString sql;
                    switch (this->connData->typeId) {
                    }
                    sql=sql.arg(StringUtils::wrapName(this->connData,database));

                    if(sql.length()>0){
                        const int ret
                            = QMessageBox::warning(this, QCoreApplication::applicationName(),
                                                   tr("%1?").arg(sql),
                                                   tr("Drop") ,tr("Cancel"),"",1);
                        switch (ret) {
                        case 0:{
                            auto resJson=Utils::executeSql(this->connData,"drop",sql,{});
                            if(Utils::testResult(resJson)){

                                //Drop成功同时删除item
                                this->ui->tw_databases->removeRow(currRow);

                            }
                            }
                            break;
                        case 1:{
                            }
                            break;
                        default:
                            break;
                        }
                    }
                }
            });
            menu->addAction(act);

            menu->addSeparator();
        }

        QAction *act = new QAction(tr("Open in Excel"), menu);
        connect(act, &QAction::triggered, this, [=](){
            auto runRef=WidgetUtils::createRunRef(this);
            this->copyToExcel(this->ui->tw_databases,true);
        });
        menu->addAction(act);

        menu->addSeparator();

        act = new QAction(tr("Refresh"), menu);
        connect(act, &QAction::triggered, this, [=](){
            auto runRef=WidgetUtils::createRunRef(this);
            QSharedPointer<ConnData> connData=this->connData;
            if(Utils::testOrReconn(connData)){

                this->refresh();
            }
        });
        menu->addAction(act);

        menu->exec(QCursor::pos());
    };


    WidgetUtils::customTableWidget(this->ui->tw_databases,sortFunc,menuFunc);
    this->ui->tw_databases->setSelectionBehavior(QAbstractItemView::SelectRows);
    this->ui->tw_databases->setSelectionMode(QAbstractItemView::SingleSelection);

    auto sortFuncProcess=[this](int idx){

        if(idx<0)return;
        auto runRef=WidgetUtils::createRunRef(this);

        QTableWidgetItem *item=this->ui->tw_processes->horizontalHeaderItem(idx);
        QVariant var=item->data(Qt::UserRole);
        for(int i=0;i<this->ui->tw_processes->columnCount();i++){
            if(i==idx)continue;
            auto colItem=this->ui->tw_processes->horizontalHeaderItem(i);
            colItem->setIcon(QIcon{":/images/sort.svg"});
            colItem->setData(Qt::UserRole,0);
        }
        if(var.isNull()||var.toInt()==0){
            item->setIcon(QIcon{":/images/sort_asc.svg"});
            WidgetUtils::sortTableWidget(this->ui->tw_processes,idx,Qt::AscendingOrder);
            item->setData(Qt::UserRole,1);
        }else if(var.toInt()==1){
            item->setIcon(QIcon{":/images/sort_desc.svg"});
            WidgetUtils::sortTableWidget(this->ui->tw_processes,idx,Qt::DescendingOrder);
            item->setData(Qt::UserRole,2);
        }else{
            item->setIcon(QIcon{":/images/sort.svg"});
            this->ui->tw_processes->sortByColumn(0,Qt::AscendingOrder);
            item->setData(Qt::UserRole,0);
        }
    };

    auto menuFuncProcess=[this](){

        int currRow=this->ui->tw_processes->currentRow();

        QMenu *menu=new QMenu(this->ui->tw_processes);
        menu->deleteLater();

        if(currRow>=0){
            QString id=this->ui->tw_processes->item(currRow,COL_PRO_ID)->data(Qt::EditRole).toString();

            auto act = new QAction(tr("Kill Process"), menu);
            connect(act, &QAction::triggered, this, [=](){
                auto runRef=WidgetUtils::createRunRef(this);

                const int ret
                    = QMessageBox::warning(this, QCoreApplication::applicationName(),
                                           tr("Kill Process (%1)?").arg(id),
                                           tr("Kill") ,tr("Cancel"),"",1);
                switch (ret) {
                case 0:{
                    QString sql="kill "+id;
                    if(this->connData){
                        switch (this->connData->typeId) {
                    }
                    auto resJson=Utils::executeSql(this->connData,"kill",sql,{},{},Utils::logQueryProp());
                    if(Utils::testResult(resJson)){
                        this->ui->tw_processes->removeRow(currRow);
                        break;
                    }
                    break;
                }
                case 1:
                    break;
                default:
                    break;
                }

            });
            menu->addAction(act);

            menu->addSeparator();
        }

        QAction* act = new QAction(tr("Open in Excel"), menu);
        connect(act, &QAction::triggered, this, [=](){
            auto runRef=WidgetUtils::createRunRef(this);
            this->copyToExcel(this->ui->tw_processes,true);
        });
        menu->addAction(act);

        menu->addSeparator();

        act = new QAction(tr("Refresh"), menu);
        connect(act, &QAction::triggered, this, [=](){
            auto runRef=WidgetUtils::createRunRef(this);
            QSharedPointer<ConnData> connData=this->connData;
            if(Utils::testOrReconn(connData)){

                this->refresh_process();
            }
        });
        menu->addAction(act);

        menu->exec(QCursor::pos());
    };

    WidgetUtils::customTableWidget(this->ui->tw_processes,sortFuncProcess,menuFuncProcess);
    this->ui->tw_processes->setSelectionBehavior(QAbstractItemView::SelectRows);
    this->ui->tw_processes->setSelectionMode(QAbstractItemView::SingleSelection);

    auto sortFuncVariables=[this](int idx){

        if(idx<0)return;

        QTableWidgetItem *item=this->ui->tw_variables->horizontalHeaderItem(idx);
        QVariant var=item->data(Qt::UserRole);
        for(int i=0;i<this->ui->tw_variables->columnCount();i++){
            if(i==idx)continue;
            auto colItem=this->ui->tw_variables->horizontalHeaderItem(i);
            colItem->setIcon(QIcon{":/images/sort.svg"});
            colItem->setData(Qt::UserRole,0);
        }
        if(var.isNull()||var.toInt()==0){
            item->setIcon(QIcon{":/images/sort_asc.svg"});
            WidgetUtils::sortTableWidget(this->ui->tw_variables,idx,Qt::AscendingOrder);
            item->setData(Qt::UserRole,1);
        }else if(var.toInt()==1){
            item->setIcon(QIcon{":/images/sort_desc.svg"});
            WidgetUtils::sortTableWidget(this->ui->tw_variables,idx,Qt::DescendingOrder);
            item->setData(Qt::UserRole,2);
        }else{
            item->setIcon(QIcon{":/images/sort.svg"});
            this->ui->tw_variables->sortByColumn(0,Qt::AscendingOrder);
            item->setData(Qt::UserRole,0);
        }
    };

    auto menuFuncVariables=[this](){

        int currRow=this->ui->tw_variables->currentRow();

        QMenu *menu=new QMenu(this->ui->tw_variables);
        menu->deleteLater();

        if(currRow>=0){
            QString varName=this->ui->tw_variables->item(currRow,COL_VAR_NAME)->data(Qt::EditRole).toString();

        }

        auto act = new QAction(tr("Open in Excel"), menu);
        connect(act, &QAction::triggered, this, [=](){
            auto runRef=WidgetUtils::createRunRef(this);
            this->copyToExcel(this->ui->tw_variables,true);
        });
        menu->addAction(act);

        menu->addSeparator();

        act = new QAction(tr("Refresh"), menu);
        connect(act, &QAction::triggered, this, [=](){
            auto runRef=WidgetUtils::createRunRef(this);
            QSharedPointer<ConnData> connData=this->connData;
            if(Utils::testOrReconn(connData)){

                this->refresh_variables();
            }
        });
        menu->addAction(act);

        menu->exec(QCursor::pos());
    };

    WidgetUtils::setTableWidgetEditable(this->ui->tw_variables,true);
    WidgetUtils::customTableWidget(this->ui->tw_variables,sortFuncVariables,menuFuncVariables);

    ui->tw_databases->setColumnHidden(COL_EVENTS,true);

    ui->tw_variables->setColumnHidden(COL_VAR_FILE,true);
    ui->tw_variables->setColumnHidden(COL_VAR_INFO,true);

    if(this->connData){
        switch (this->connData->typeId)
        {
        case DB_TYPE_MYSQL:{

            this->ui->tw_databases->setColumnHidden(COL_EVENTS,false);
        }
            break;
        default:{

            WidgetUtils::hideTabWidget(this->ui->tabWidget,this->ui->tab_variables);
        }
            break;
        }
    }

    QTimer::singleShot(0,this,[=](){
        this->refresh();
    });
}

DatabaseManagerForm::~DatabaseManagerForm()
{
    delete ui;
}

void DatabaseManagerForm::safeDelete()
{

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
}

void DatabaseManagerForm::closeEvent(QCloseEvent *event)
{
    this->hide();

    this->safeDelete();

    event->ignore();
}

void DatabaseManagerForm::refresh()
{

    auto runRef=WidgetUtils::createRunRef(this);
    this->ui->tw_databases->viewport()->hide();
    this->ui->tw_databases->setUpdatesEnabled(false);
    LastRun lr{[=](){
            this->ui->tw_databases->viewport()->show();
            this->ui->tw_databases->setUpdatesEnabled(true);
               }};

    CMask loadingMask{this->ui->tab_databases};
    loadingMask.setStopFunction([=](){
        this->stop=true;
    });
    loadingMask.show();

    this->ui->tw_databases->clearContents();
    this->ui->tw_databases->setRowCount(0);

    if(!Utils::testOrReconn(this->connData))return;

    this->updateStatusBar();

    switch (this->connData->typeId) {
    case DB_TYPE_MYSQL:{
        {
            QString sql_databases="select * from information_schema.SCHEMATA a "\
                    "left join ( "\
                    "select table_schema,sum(data_length) data_length,sum(index_length) index_length,max(create_time) create_time,max(update_time) update_time, "\
                    "sum(case when upper(table_type)!='VIEW' then 1 else 0 end) table_count,sum(case when upper(table_type)='VIEW' then 1 else 0 end) view_count "\
                    "from information_schema.TABLES "\
                    "group by table_schema ) b "\
                    "on a.schema_name=b.table_schema "\
                    "left join ( "\
                    "select trigger_schema,count(*) trigger_count from information_schema.TRIGGERS group by trigger_schema "\
                    ") c  "\
                    "on a.schema_name=c.trigger_schema "\
                    "left join ( "\
                    "select event_schema,count(*) event_count from information_schema.EVENTS group by event_schema "\
                    ") d  "\
                    "on a.schema_name=d.event_schema "\
                    "left join ( "\
                    "select routine_schema,sum(case when upper(routine_type)='FUNCTION' then 1 else 0 end) function_count, "\
                    "sum(case when upper(routine_type)='PROCEDURE' then 1 else 0 end) procedure_count from information_schema.ROUTINES group by routine_schema ) e "\
                    "on a.schema_name=e.routine_schema";

            auto json=Utils::executeQuery(this->connData,sql_databases,{});

            Result res{json};
            if(res.testResult()){
                int idx_name=res.getColIndex("schema_name");
                int idx_size=res.getColIndex("data_length");
                int idx_index_size=res.getColIndex("index_length");
                int idx_created=res.getColIndex("create_time");
                int idx_updated=res.getColIndex("update_time");
                int idx_tables=res.getColIndex("table_count");
                int idx_views=res.getColIndex("view_count");
                int idx_functions=res.getColIndex("function_count");
                int idx_procedures=res.getColIndex("procedure_count");
                int idx_triggers=res.getColIndex("trigger_count");
                int idx_events=res.getColIndex("event_count");
                int idx_charset=res.getColIndex("default_character_set_name");
                int idx_collation=res.getColIndex("default_collation_name");

                for(int i=0;i<res.rowCount();i++){
                    int tw_row=this->ui->tw_databases->rowCount();
                    this->ui->tw_databases->setRowCount(tw_row+1);

                    QJsonArray arr=res.getRow(i);

                    auto item=new QTableWidgetItem{};
                    item->setData(Qt::EditRole,this->ui->tw_databases->rowCount());
                    this->ui->tw_databases->setItem(tw_row,COL_ID,item);

                    auto nameItem=new QTableWidgetItem{};
                    if(idx_name>=0){
                        nameItem->setData(Qt::EditRole,arr.at(idx_name).toVariant());
                    }
                    this->ui->tw_databases->setItem(tw_row,COL_NAME,nameItem);

                    item=new QTableWidgetItem{};

                    int64_t size=0;

                    if(idx_size>=0){
                        size+=arr.at(idx_size).toVariant().toLongLong();//表大小包括数据大小和索引大小
                    }

                    if(idx_index_size>=0){
                        size+=arr.at(idx_index_size).toVariant().toLongLong();//表大小包括数据大小和索引大小
                    }

                    item->setData(Qt::EditRole,Utils::toMib(size,1));

                    this->ui->tw_databases->setItem(tw_row,COL_SIZE,item);

                    item=new QTableWidgetItem{};
                    if(idx_created>=0){
                        item->setData(Qt::EditRole,arr.at(idx_created).toVariant());
                    }

                    this->ui->tw_databases->setItem(tw_row,COL_CREATED,item);

                    item=new QTableWidgetItem{};
                    if(idx_updated>=0){
                        item->setData(Qt::EditRole,arr.at(idx_updated).toVariant());
                    }

                    this->ui->tw_databases->setItem(tw_row,COL_LAST_UPDATED,item);

                    item=new QTableWidgetItem{};
                    if(idx_tables>=0){
                        item->setData(Qt::EditRole,arr.at(idx_tables).toVariant());
                    }

                    this->ui->tw_databases->setItem(tw_row,COL_TABLES,item);

                    item=new QTableWidgetItem{};
                    if(idx_views>=0){
                        item->setData(Qt::EditRole,arr.at(idx_views).toVariant());
                    }

                    this->ui->tw_databases->setItem(tw_row,COL_VIEWS,item);

                    item=new QTableWidgetItem{};
                    if(idx_functions>=0){
                        item->setData(Qt::EditRole,arr.at(idx_functions).toVariant());
                    }

                    this->ui->tw_databases->setItem(tw_row,COL_FUNCTIONS,item);

                    item=new QTableWidgetItem{};
                    if(idx_procedures>=0){
                        item->setData(Qt::EditRole,arr.at(idx_procedures).toVariant());
                    }

                    this->ui->tw_databases->setItem(tw_row,COL_PROCEDURES,item);

                    item=new QTableWidgetItem{};
                    if(idx_triggers>=0){
                        item->setData(Qt::EditRole,arr.at(idx_triggers).toVariant());
                    }

                    this->ui->tw_databases->setItem(tw_row,COL_TRIGGERS,item);

                    item=new QTableWidgetItem{};
                    if(idx_events>=0){
                        item->setData(Qt::EditRole,arr.at(idx_events).toVariant());
                    }

                    this->ui->tw_databases->setItem(tw_row,COL_EVENTS,item);

                    item=new QTableWidgetItem{};
                    if(idx_charset>=0){
                        item->setData(Qt::EditRole,arr.at(idx_charset).toVariant());
                    }

                    this->ui->tw_databases->setItem(tw_row,COL_DEFAULT_CHARSET,item);

                    item=new QTableWidgetItem{};
                    if(idx_collation>=0){
                        item->setData(Qt::EditRole,arr.at(idx_collation).toVariant());
                    }

                    this->ui->tw_databases->setItem(tw_row,COL_DEFAULT_COLLATION,item);

                }


            }
        }

    }
        break;

    }

    WidgetUtils::autoResizeTableWidget(this->ui->tw_databases);

    WidgetUtils::fillTableWidget(this->ui->tw_databases);

    QString str=this->ui->le_twfilter_db->text();
    if(str.length()>0){
        this->on_le_twfilter_db_textChanged(str);
    }

}

void DatabaseManagerForm::refresh_process()
{

    auto runRef=WidgetUtils::createRunRef(this);
    this->ui->tw_processes->viewport()->hide();
    this->ui->tw_processes->setUpdatesEnabled(false);
    LastRun lr{[=](){
            this->ui->tw_processes->viewport()->show();
            this->ui->tw_processes->setUpdatesEnabled(true);
               }};

    CMask loadingMask{this->ui->tab_processes};
    loadingMask.setStopFunction([=](){
        this->stop=true;
    });
    loadingMask.show();

    this->ui->tw_processes->clearContents();
    this->ui->tw_processes->setRowCount(0);

    if(!Utils::testOrReconn(this->connData))return;

    this->updateStatusBar();

    switch (this->connData->typeId) {
    case DB_TYPE_MYSQL:{
        {
            QString sql_databases="SELECT ID, USER, HOST, DB, COMMAND, TIME, STATE, LEFT(INFO, 51200) AS Info FROM information_schema.PROCESSLIST";

            auto json=Utils::executeQuery(this->connData,sql_databases,{});

            Result res{json};
            if(res.testResult()){
                int idx_id=res.getColIndex("id");
                int idx_user=res.getColIndex("user");
                int idx_host=res.getColIndex("host");
                int idx_db=res.getColIndex("db");
                int idx_command=res.getColIndex("command");
                int idx_time=res.getColIndex("time");
                int idx_state=res.getColIndex("state");
                int idx_info=res.getColIndex("info");

                for(int i=0;i<res.rowCount();i++){
                    int tw_row=this->ui->tw_processes->rowCount();
                    this->ui->tw_processes->setRowCount(tw_row+1);

                    QJsonArray arr=res.getRow(i);

                    auto item=new QTableWidgetItem{};
                    item->setData(Qt::EditRole,this->ui->tw_processes->rowCount());
                    this->ui->tw_processes->setItem(tw_row,COL_ID,item);

                    item=new QTableWidgetItem{};
                    if(idx_id>=0){
                        item->setData(Qt::EditRole,arr.at(idx_id).toVariant());
                    }
                    this->ui->tw_processes->setItem(tw_row,COL_PRO_ID,item);

                    item=new QTableWidgetItem{};
                    if(idx_user>=0){
                        item->setData(Qt::EditRole,arr.at(idx_user).toVariant());
                    }
                    this->ui->tw_processes->setItem(tw_row,COL_PRO_USER,item);

                    item=new QTableWidgetItem{};
                    if(idx_host>=0){
                        item->setData(Qt::EditRole,arr.at(idx_host).toVariant());
                    }
                    this->ui->tw_processes->setItem(tw_row,COL_PRO_HOST,item);

                    item=new QTableWidgetItem{};
                    if(idx_db>=0){
                        item->setData(Qt::EditRole,arr.at(idx_db).toVariant());
                    }
                    this->ui->tw_processes->setItem(tw_row,COL_PRO_DB,item);

                    item=new QTableWidgetItem{};
                    if(idx_command>=0){
                        item->setData(Qt::EditRole,arr.at(idx_command).toVariant());
                    }
                    this->ui->tw_processes->setItem(tw_row,COL_PRO_COMMAND,item);

                    item=new QTableWidgetItem{};
                    if(idx_time>=0){
                        item->setData(Qt::EditRole,arr.at(idx_time).toVariant());
                    }
                    this->ui->tw_processes->setItem(tw_row,COL_PRO_TIME,item);

                    item=new QTableWidgetItem{};
                    if(idx_state>=0){
                        item->setData(Qt::EditRole,arr.at(idx_state).toVariant());
                    }
                    this->ui->tw_processes->setItem(tw_row,COL_PRO_STATE,item);

                    item=new QTableWidgetItem{};
                    if(idx_info>=0){
                        item->setData(Qt::EditRole,arr.at(idx_info).toVariant());
                    }
                    this->ui->tw_processes->setItem(tw_row,COL_PRO_INFO,item);

                }


            }
        }

    }
        break;
    default:{

    }
        break;
    }

    WidgetUtils::autoResizeTableWidget(this->ui->tw_processes);

    QString str=this->ui->le_twfilter_proc->text();
    if(str.length()>0){
        this->on_le_twfilter_proc_textChanged(str);
    }
}

void DatabaseManagerForm::refresh_variables()
{
    auto runRef=WidgetUtils::createRunRef(this);
    const QSignalBlocker _blk(this->ui->tw_variables);
    this->ui->tw_variables->viewport()->hide();
    this->ui->tw_variables->setUpdatesEnabled(false);
    LastRun lr{[=](){
            this->ui->tw_variables->viewport()->show();
            this->ui->tw_variables->setUpdatesEnabled(true);
               }};

    CMask loadingMask{this->ui->tab_variables};
    loadingMask.setStopFunction([=](){
        this->stop=true;
    });
    loadingMask.show();

    this->ui->tw_variables->clearContents();
    this->ui->tw_variables->setRowCount(0);

    varMap.clear();

    if(!Utils::testOrReconn(this->connData))return;

    this->updateStatusBar();

    switch (this->connData->typeId) {
    case DB_TYPE_MYSQL:{
        {
            QString sql_sessionVar="SHOW SESSION VARIABLES";

            auto json=Utils::executeQuery(this->connData,sql_sessionVar,{});

            Result res{json};
            if(res.testResult(false)){
                int idx_name=res.getColIndex("variable_name");
                int idx_value=res.getColIndex("value");

                for(int i=0;i<res.rowCount();i++){

                    QJsonArray arr=res.getRow(i);


                    if(idx_name>=0&&idx_value>0){
                        QString name=arr.at(idx_name).toVariant().toString();
                        QString var=arr.at(idx_value).toVariant().toString();

                        varMap.insert(name,{var,""});
                    }

                }


            }
        }

        {

            QString sql_globalVar="SHOW GLOBAL VARIABLES";

            auto json=Utils::executeQuery(this->connData,sql_globalVar,{});

            Result res{json};
            if(res.testResult(false)){
                int idx_name=res.getColIndex("variable_name");
                int idx_value=res.getColIndex("value");

                for(int i=0;i<res.rowCount();i++){

                    QJsonArray arr=res.getRow(i);


                    if(idx_name>=0&&idx_value>0){
                        QString name=arr.at(idx_name).toVariant().toString();
                        QString var=arr.at(idx_value).toVariant().toString();

                        auto pair=varMap.value(name);

                        pair.second=var;

                        varMap.insert(name,pair);
                    }

                }


            }
        }

        QMapIterator<QString,QPair<QString,QString>> iter{varMap};

        while (iter.hasNext()) {
            iter.next();

            QString name=iter.key();
            QPair pair=iter.value();

            int tw_row=this->ui->tw_variables->rowCount();
            this->ui->tw_variables->setRowCount(tw_row+1);

            auto item=new QTableWidgetItem{};
            item->setData(Qt::DisplayRole,tw_row);
            this->ui->tw_variables->setItem(tw_row,COL_ID,item);

            item=new QTableWidgetItem{};
            item->setData(Qt::DisplayRole,name);
            this->ui->tw_variables->setItem(tw_row,COL_VAR_NAME,item);

            item=new QTableWidgetItem{};
            item->setData(Qt::DisplayRole,pair.first);
            this->ui->tw_variables->setItem(tw_row,COL_VAR_SESSION,item);

            item=new QTableWidgetItem{};
            item->setData(Qt::DisplayRole,pair.second);
            this->ui->tw_variables->setItem(tw_row,COL_VAR_GLOBAL,item);

        }

    }
        break;
        default:{
            {
                QString sql="select a.name,a.value sys_value,a.type,b.value ses_value,a.description\n" \
                            "from v$system_parameter a\n" \
                            "left join v$parameter b on a.name=b.name";

                auto json=Utils::executeQuery(this->connData,sql,{});

                Result res{json};
                if(res.testResult(false)){

                    for(int i=0;i<res.rowCount();i++){

                        int tw_row=this->ui->tw_variables->rowCount();
                        this->ui->tw_variables->setRowCount(tw_row+1);

                        auto item=new QTableWidgetItem{};
                        item->setData(Qt::DisplayRole,tw_row);
                        this->ui->tw_variables->setItem(tw_row,COL_ID,item);

                        item=new QTableWidgetItem{};
                        item->setData(Qt::DisplayRole,res.getString(tw_row,"name"));
                        this->ui->tw_variables->setItem(tw_row,COL_VAR_NAME,item);

                        item=new QTableWidgetItem{};
                        item->setData(Qt::DisplayRole,res.getString(tw_row,"ses_value"));
                        this->ui->tw_variables->setItem(tw_row,COL_VAR_SESSION,item);

                        item=new QTableWidgetItem{};
                        item->setData(Qt::DisplayRole,res.getString(tw_row,"sys_value"));
                        this->ui->tw_variables->setItem(tw_row,COL_VAR_GLOBAL,item);

                        item=new QTableWidgetItem{};
                        item->setData(Qt::DisplayRole,res.getString(tw_row,"file_value"));
                        this->ui->tw_variables->setItem(tw_row,COL_VAR_FILE,item);

                        item=new QTableWidgetItem{};
                        item->setData(Qt::DisplayRole,res.getString(tw_row,"type")+"; "+res.getString(tw_row,"description"));
                        this->ui->tw_variables->setItem(tw_row,COL_VAR_INFO,item);

                    }

                }
            }
        }
        }

    }
        break;
    default:{

    }
        break;
    }

    WidgetUtils::autoResizeTableWidget(this->ui->tw_variables);

    QString str=this->ui->le_twfilter_var->text();
    if(str.length()>0){
        this->on_le_twfilter_var_textChanged(str);
    }
}

QSharedPointer<ConnData> DatabaseManagerForm::getConnData() const
{
    return connData;
}

void DatabaseManagerForm::copyToExcel(QTableWidget *tw, bool isXlsx)
{

    auto runRef=WidgetUtils::createRunRef(this);


    WidgetUtils::copyFetchedToExcel(this->connData,this->connData->dbName,"",isXlsx,tw);
}

void DatabaseManagerForm::updateStatusBar()
{
    MainWindow::instance()->statusLabel.setHidden(true);
    if(this->connData){
        MainWindow::instance()->statusLabel.setHidden(false);
        MainWindow::instance()->statusLabel.setText("Databases "+this->connData->connName);
    }

    MainWindow::instance()->sqlLabel.setHidden(true);
    if(this->connData){

        if(connData&&connData->databaseProductName.length()>0&&connData->databaseProductVersion.length()>0){
            MainWindow::instance()->sqlLabel.setHidden(false);
            MainWindow::instance()->sqlLabel.setText(connData->databaseProductName+" "+connData->databaseProductVersion);
        }
    }
}

void DatabaseManagerForm::on_tabWidget_currentChanged(int index)
{
    auto runRef=WidgetUtils::createRunRef(this);
    if(this->ui->tabWidget->currentWidget()==this->ui->tab_processes){
        if(this->ui->tw_processes->rowCount()<=0){
            this->refresh_process();
        }
    }else if(this->ui->tabWidget->currentWidget()==this->ui->tab_variables){
        if(this->ui->tw_variables->rowCount()<=0){
            this->refresh_variables();
        }
    }
}

void DatabaseManagerForm::on_tw_variables_cellChanged(int row, int column)
{

    auto runRef=WidgetUtils::createRunRef(this);
    if(column!=COL_VAR_SESSION&&column!=COL_VAR_GLOBAL)return;
    if(!Utils::testOrReconn(this->connData))return;

    auto nameItem=this->ui->tw_variables->item(row,COL_VAR_NAME);
    auto valueItem=this->ui->tw_variables->item(row,column);

    if(!nameItem||!valueItem)return;

    QString name=nameItem->text();
    QString value=valueItem->text();

    if(name.length()>0){

        QString oldvalue;
        if(column==COL_VAR_SESSION){
            oldvalue=this->varMap.value(name).first;
        }else{
            oldvalue=this->varMap.value(name).second;
        }

        QString sql;
        switch (this->connData->typeId) {
        case DB_TYPE_MYSQL:{
            sql=QString{"SET %1.%2 = %3"}.arg(column==COL_VAR_SESSION?"@@session":"@@global",name,value);
        }
            break;
        }

        if(sql.isEmpty())return;

        const int ret
            = QMessageBox::warning(this, QCoreApplication::applicationName(),
                                   sql,
                                   tr("OK") ,tr("Cancel") ,"" ,1);

        switch (ret) {
        case 0:{

            auto json=Utils::executeQuery(this->connData,sql,{},{},Utils::logQueryProp());

            Result res{json};
            if(res.testResult()){

            }
            this->refresh_variables();
        }
            break;
        case 1:{
            const QSignalBlocker _blk(this->ui->tw_variables);
            valueItem->setData(Qt::DisplayRole,oldvalue);
        }
            break;
        default:
            break;
        }
    }
}

void DatabaseManagerForm::on_le_twfilter_db_textChanged(const QString &arg1)
{
    auto runRef=WidgetUtils::createRunRef(this);
    WidgetUtils::twFilter(this->stop,this->ui->tw_databases,this->ui->le_twfilter_db);
}

void DatabaseManagerForm::on_le_twfilter_proc_textChanged(const QString &arg1)
{
    auto runRef=WidgetUtils::createRunRef(this);
    WidgetUtils::twFilter(this->stop,this->ui->tw_processes,this->ui->le_twfilter_proc);
}

void DatabaseManagerForm::on_le_twfilter_var_textChanged(const QString &arg1)
{
    auto runRef=WidgetUtils::createRunRef(this);
    WidgetUtils::twFilter(this->stop,this->ui->tw_variables,this->ui->le_twfilter_var);
}

void DatabaseManagerForm::on_le_twfilter_db_returnPressed()
{
    this->on_le_twfilter_db_textChanged("");
}

void DatabaseManagerForm::on_le_twfilter_proc_returnPressed()
{
    this->on_le_twfilter_proc_textChanged("");
}

void DatabaseManagerForm::on_le_twfilter_var_returnPressed()
{
    this->on_le_twfilter_var_textChanged("");
}
