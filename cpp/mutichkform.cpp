#include "mutichkform.h"
#include "ui_mutichkform.h"
#include "widgetutils.h"

#include <QTimer>

MutiChkForm::MutiChkForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MutiChkForm)
{
    ui->setupUi(this);

    WidgetUtils::setSafeDeleteOnClose(this);

    setWindowFlag(Qt::Popup);

    ui->splitter->setSizes({700,300});
}

MutiChkForm::~MutiChkForm()
{
    delete ui;
}

void MutiChkForm::waitExit()
{
    auto runRef=WidgetUtils::createRunRef(this);

    QSharedPointer<EventLoop> loop{new EventLoop{this}};

    connect(this,&MutiChkForm::editClosed,[=](){loop->quit();});

    loop->exec();
}

QListWidget *MutiChkForm::getListWidget()
{
    return this->ui->listWidget;
}

QListWidget *MutiChkForm::getListWidgetRight()
{
    return this->ui->listWidget_right;
}

void MutiChkForm::closeEvent(QCloseEvent *event)
{
    this->hide();

    stop=true;

    emit editClosed();

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

bool MutiChkForm::getReadonly() const
{
    return readonly;
}

void MutiChkForm::setReadonly(bool value)
{
    readonly = value;

    this->setEnabled(!this->readonly);
}

void MutiChkForm::on_listWidget_itemClicked(QListWidgetItem *item)
{
    if(item->checkState()==Qt::Checked){
        item->setCheckState(Qt::Unchecked);
    }else{
        item->setCheckState(Qt::Checked);
    }
}

void MutiChkForm::on_btn_selectAll_clicked()
{
    for(int i=0;i<this->ui->listWidget->count();i++){
        auto item=this->ui->listWidget->item(i);
        if(item){
            item->setCheckState(Qt::Checked);
        }
    }
}

void MutiChkForm::on_btn_deselectAll_clicked()
{
    for(int i=0;i<this->ui->listWidget->count();i++){
        auto item=this->ui->listWidget->item(i);
        if(item){
            item->setCheckState(Qt::Unchecked);
        }
    }
}

void MutiChkForm::on_btn_ok_clicked()
{
    this->close();
}
