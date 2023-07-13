#include "dbobjectselectdialog.h"
#include "ui_dbobjectselectdialog.h"
#include "widgetutils.h"

#include <QTimer>

DbObjectSelectDialog::DbObjectSelectDialog(QSharedPointer<ConnData> connData,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DbObjectSelectDialog),
    connData(connData)
{
    ui->setupUi(this);

    WidgetUtils::setSafeDeleteOnClose(this);

#if defined(Q_OS_MAC)
    this->setWindowFlag(Qt::Tool,true);
#endif

    this->ui->tree_myTree->clear();

    QTimer::singleShot(0,this,[=](){
        auto runRef=WidgetUtils::createRunRef(this);
        if(connData){
            this->ui->tree_myTree->viewport()->setHidden(true);

            this->ui->tree_myTree->setInSelectModel(true);

            this->ui->tree_myTree->refreshTree();
            auto dsItem=this->ui->tree_myTree->deleteOtherDsItem(connData);
            if(dsItem){
                this->ui->tree_myTree->expandItem(dsItem);
                this->ui->tree_myTree->expandItem(dsItem->parent());
            }

            this->ui->tree_myTree->setSelectionMode(QAbstractItemView::ExtendedSelection);
            this->ui->tree_myTree->setDragEnabled(false);

            this->ui->tree_myTree->viewport()->setHidden(false);

        }
    });


}

DbObjectSelectDialog::~DbObjectSelectDialog()
{
    delete ui;
}

bool DbObjectSelectDialog::getIsOK() const
{
    return isOK;
}

MyTreeWidget *DbObjectSelectDialog::getTree()
{
    return this->ui->tree_myTree;
}

QStringList DbObjectSelectDialog::getSelect()
{
    auto runRef=WidgetUtils::createRunRef(this);

    this->exec();

    QStringList ret;

    if(this->isOK){

        QList<QTreeWidgetItem*> list=this->ui->tree_myTree->selectedItems();

        for(auto item:list){
            if(item){
                QString str;
                switch (item->type()) {
                case TREE_ITEM_DB:{
                    str="Database: "+item->text(0);
                }break;
                case TREE_ITEM_TABLE:{
                    str="Table: "+this->ui->tree_myTree->getDbName(item)+"."+item->text(0);
                }break;
                case TREE_ITEM_VIEW:{
                    str="View: "+this->ui->tree_myTree->getDbName(item)+"."+item->text(0);
                }break;
                case TREE_ITEM_FUNCTION:{
                    str="Function: "+this->ui->tree_myTree->getDbName(item)+"."+item->text(0);
                }break;
                case TREE_ITEM_PROCEDURE:{
                    str="Procedure: "+this->ui->tree_myTree->getDbName(item)+"."+item->text(0);
                }break;
                case TREE_ITEM_COLUMN:{
                    str="Column: "+this->ui->tree_myTree->getDbName(item)+"."+item->parent()->text(0)+"."+item->text(0);
                }break;
                case TREE_ITEM_SEQUENCE:{
                    str="Sequence: "+this->ui->tree_myTree->getDbName(item)+"."+item->text(0);
                }break;
                }
                if(!str.isEmpty()){
                    ret.push_back(str);
                }
            }
        }

    }

    return ret;
}

void DbObjectSelectDialog::closeEvent(QCloseEvent *event)
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

void DbObjectSelectDialog::on_btn_ok_clicked()
{
    this->isOK=true;
    this->close();
}

void DbObjectSelectDialog::on_btn_cancel_clicked()
{
    this->isOK=false;
    this->close();
}

void DbObjectSelectDialog::on_le_filter_textChanged(const QString &arg1)
{
    WidgetUtils::treeFilter(this->stop,ui->tree_myTree,ui->le_filter);
}

void DbObjectSelectDialog::on_le_filter_returnPressed()
{
    this->on_le_filter_textChanged("");
}

void DbObjectSelectDialog::on_btn_collapse_clicked()
{
    QTreeWidgetItem *item=ui->tree_myTree->currentItem();
    if(!item)return;
    item->setExpanded(false);
    if(item->parent()){
        item->parent()->setExpanded(false);
        ui->tree_myTree->setCurrentItem(item->parent());
    }
}
