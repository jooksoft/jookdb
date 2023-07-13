#include "strmappingdialog.h"
#include "ui_strmappingdialog.h"
#include "widgetutils.h"

#include <QTimer>

StrMappingDialog::StrMappingDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::StrMappingDialog)
{
    ui->setupUi(this);

    WidgetUtils::setSafeDeleteOnClose(this);

//    Qt::WindowFlags flags=Qt::Dialog;
//    flags |=Qt::WindowMinMaxButtonsHint;
//    flags |=Qt::WindowCloseButtonHint;
//    setWindowFlags(flags);

#if defined(Q_OS_MAC)
    this->setWindowFlag(Qt::Tool,true);
#endif

    this->ui->btn_ok->setFocus();

}

StrMappingDialog::~StrMappingDialog()
{
    delete ui;
}

void StrMappingDialog::setMappingStrs(QStringList srcList, QStringList tgtList, QList<QPair<QString, QString> > mappings)
{
    this->ui->lw_src->clear();
    this->ui->lw_tgt->clear();
    this->ui->lw_mapping->clear();

    this->ui->lw_src->addItems(srcList);
    this->ui->lw_tgt->addItems(tgtList);
    for(const auto& s:mappings){
        this->ui->lw_mapping->addItem(s.first+MAP_FGF+s.second);
        for (int i=0;i<ui->lw_src->count();i++) {
            auto item=ui->lw_src->item(i);
            if(item){
                if(item->text()==s.first){
                    item->setHidden(true);
                    break;
                }
            }
        }
        for (int i=0;i<ui->lw_tgt->count();i++) {
            auto item=ui->lw_tgt->item(i);
            if(item){
                if(item->text()==s.second){
                    item->setHidden(true);
                    break;
                }
            }
        }
    }
}

QList<QPair<QString, QString> > StrMappingDialog::getMappingList()
{
    QList<QPair<QString,QString>> mappingList;

    for (int i=0;i<ui->lw_mapping->count();i++) {
        auto item=ui->lw_mapping->item(i);
        if(item){
            QString str=item->text();
            QStringList sList=str.split(MAP_FGF);
            if(sList.size()>1){
                mappingList.push_back({sList.at(0).trimmed(),sList.at(1).trimmed()});
            }
        }
    }
    return mappingList;
}

void StrMappingDialog::on_btn_ok_clicked()
{
    this->close();
    this->accept();
}

void StrMappingDialog::closeEvent(QCloseEvent *event)
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

void StrMappingDialog::on_btn_cancel_clicked()
{
    this->close();
    this->reject();
}

void StrMappingDialog::on_btn_add_clicked()
{
    auto item_src=ui->lw_src->currentItem();
    auto item_tgt=ui->lw_tgt->currentItem();

    if(item_src&&item_tgt&&
            item_src->isSelected()&&!item_src->isHidden()&&
            item_src->isSelected()&&!item_src->isHidden()){
        ui->lw_mapping->addItem(item_src->text()+MAP_FGF+item_tgt->text());
        item_src->setHidden(true);
        item_tgt->setHidden(true);
    }
}

void StrMappingDialog::on_btn_delete_clicked()
{
    int currRow=ui->lw_mapping->currentRow();
    if(currRow<0)return;
    auto item=ui->lw_mapping->takeItem(currRow);
    if(item){
        QStringList sList=item->text().split(MAP_FGF);
        if(sList.size()>1){
            QString key=sList.at(0);
            QString val=sList.at(1);
            bool find=false;
            for (int i=0;i<ui->lw_src->count();i++) {
                auto item=ui->lw_src->item(i);
                if(item){
                    if(item->text()==key){
                        item->setHidden(false);
                        find=true;
                        break;
                    }
                }
            }
            if(!find){
                ui->lw_src->addItem(key);
            }

            find=false;
            for (int i=0;i<ui->lw_tgt->count();i++) {
                auto item=ui->lw_tgt->item(i);
                if(item){
                    if(item->text()==val){
                        item->setHidden(false);
                        find=true;
                        break;
                    }
                }
            }
            if(!find){
                ui->lw_tgt->addItem(val);
            }
        }
    }
}

void StrMappingDialog::on_btn_auto_clicked()
{
    for (int i=0;i<ui->lw_src->count();i++) {
        auto item_src=ui->lw_src->item(i);
        if(item_src){
            if(item_src->isHidden()){
                continue;
            }
            QString key=item_src->text();
            for (int i=0;i<ui->lw_tgt->count();i++) {
                auto item_tgt=ui->lw_tgt->item(i);
                if(item_tgt){
                    if(item_tgt->isHidden()){
                        continue;
                    }
                    QString val=item_tgt->text();
                    if(key==val){
                        ui->lw_mapping->addItem(item_src->text()+MAP_FGF+item_tgt->text());
                        item_src->setHidden(true);
                        item_tgt->setHidden(true);
                        break;
                    }
                }
            }
        }
    }

    for (int i=0;i<ui->lw_src->count();i++) {
        auto item_src=ui->lw_src->item(i);
        if(item_src){
            if(item_src->isHidden()){
                continue;
            }
            QString key=item_src->text();
            for (int i=0;i<ui->lw_tgt->count();i++) {
                auto item_tgt=ui->lw_tgt->item(i);
                if(item_tgt){
                    if(item_tgt->isHidden()){
                        continue;
                    }
                    QString val=item_tgt->text();
                    if(StringUtils::equalIncase(key,val)){
                        ui->lw_mapping->addItem(item_src->text()+MAP_FGF+item_tgt->text());
                        item_src->setHidden(true);
                        item_tgt->setHidden(true);
                        break;
                    }
                }
            }
        }
    }

    for (int i=0;i<ui->lw_src->count();i++) {
        auto item_src=ui->lw_src->item(i);
        if(item_src){
            if(item_src->isHidden()){
                continue;
            }
            QString key=item_src->text();
            key=StringUtils::trimKuoHao(key);
            key=StringUtils::trimDigit(key);
            for (int i=0;i<ui->lw_tgt->count();i++) {
                auto item_tgt=ui->lw_tgt->item(i);
                if(item_tgt){
                    if(item_tgt->isHidden()){
                        continue;
                    }
                    QString val=item_tgt->text();
                    val=StringUtils::trimKuoHao(val);
                    val=StringUtils::trimDigit(val);
                    if(StringUtils::equalIncase(key,val)){
                        ui->lw_mapping->addItem(item_src->text()+MAP_FGF+item_tgt->text());
                        item_src->setHidden(true);
                        item_tgt->setHidden(true);
                        break;
                    }
                }
            }
        }
    }
}

void StrMappingDialog::on_btn_clear_clicked()
{
    while (ui->lw_mapping->count()>0) {
        auto item=ui->lw_mapping->takeItem(0);
        if(item){
            QStringList sList=item->text().split(MAP_FGF);
            if(sList.size()>1){
                QString key=sList.at(0);
                QString val=sList.at(1);
                bool find=false;
                for (int i=0;i<ui->lw_src->count();i++) {
                    auto item=ui->lw_src->item(i);
                    if(item){
                        if(item->text()==key){
                            item->setHidden(false);
                            find=true;
                            break;
                        }
                    }
                }
                if(!find){
                    ui->lw_src->addItem(key);
                }

                find=false;
                for (int i=0;i<ui->lw_tgt->count();i++) {
                    auto item=ui->lw_tgt->item(i);
                    if(item){
                        if(item->text()==val){
                            item->setHidden(false);
                            find=true;
                            break;
                        }
                    }
                }
                if(!find){
                    ui->lw_tgt->addItem(val);
                }
            }

        }
    }
}

void StrMappingDialog::on_lw_src_itemClicked(QListWidgetItem *item)
{
    LastRun lr{[=](){
            this->ui->lw_src->setProperty("itemClk",true);
            this->ui->lw_tgt->setProperty("itemClk",false);
               }};

    if(ui->chk_auto_tgt_sel->isChecked()&&ui->lw_tgt->property("itemClk").toBool()==false){
        QString str=item->text();
        for (int i=0;i<ui->lw_tgt->count();i++) {
            auto item2=ui->lw_tgt->item(i);
            if(item2&&item2->isHidden()==false){
                QString str2=item2->text();
                if(str==str2){
                    ui->lw_tgt->setCurrentItem(item2);
                    item2->setSelected(true);
                    return;
                }
            }
        }

        str=item->text();
        for (int i=0;i<ui->lw_tgt->count();i++) {
            auto item2=ui->lw_tgt->item(i);
            if(item2&&item2->isHidden()==false){
                QString str2=item2->text();
                if(StringUtils::equalIncase(str,str2)){
                    ui->lw_tgt->setCurrentItem(item2);
                    item2->setSelected(true);
                    return;
                }
            }
        }

        str=item->text();
        str=StringUtils::trimKuoHao(str);
        for (int i=0;i<ui->lw_tgt->count();i++) {
            auto item2=ui->lw_tgt->item(i);
            if(item2&&item2->isHidden()==false){
                QString str2=item2->text();
                str2=StringUtils::trimKuoHao(str2);
                if(StringUtils::equalIncase(str,str2)){
                    ui->lw_tgt->setCurrentItem(item2);
                    item2->setSelected(true);
                    return;
                }
            }
        }

        str=item->text();
        str=StringUtils::trimKuoHao(str);
        str=StringUtils::trimDigit(str);
        for (int i=0;i<ui->lw_tgt->count();i++) {
            auto item2=ui->lw_tgt->item(i);
            if(item2&&item2->isHidden()==false){
                QString str2=item2->text();
                str2=StringUtils::trimKuoHao(str2);
                str2=StringUtils::trimDigit(str2);
                if(StringUtils::equalIncase(str,str2)){
                    ui->lw_tgt->setCurrentItem(item2);
                    item2->setSelected(true);
                    return;
                }
            }
        }
    }

}

void StrMappingDialog::on_lw_tgt_itemClicked(QListWidgetItem *item)
{
    LastRun lr{[=](){
            this->ui->lw_src->setProperty("itemClk",false);
            this->ui->lw_tgt->setProperty("itemClk",true);
               }};

    if(ui->chk_auto_src_sel->isChecked()&&ui->lw_src->property("itemClk").toBool()==false){
        QString str=item->text();
        for (int i=0;i<ui->lw_src->count();i++) {
            auto item2=ui->lw_src->item(i);
            if(item2&&item2->isHidden()==false){
                QString str2=item2->text();
                if(str==str2){
                    ui->lw_src->setCurrentItem(item2);
                    item2->setSelected(true);
                    return;
                }
            }
        }

        str=item->text();
        for (int i=0;i<ui->lw_src->count();i++) {
            auto item2=ui->lw_src->item(i);
            if(item2&&item2->isHidden()==false){
                QString str2=item2->text();
                if(StringUtils::equalIncase(str,str2)){
                    ui->lw_src->setCurrentItem(item2);
                    item2->setSelected(true);
                    return;
                }
            }
        }

        str=item->text();
        str=StringUtils::trimKuoHao(str);
        for (int i=0;i<ui->lw_src->count();i++) {
            auto item2=ui->lw_src->item(i);
            if(item2&&item2->isHidden()==false){
                QString str2=item2->text();
                str2=StringUtils::trimKuoHao(str2);
                if(StringUtils::equalIncase(str,str2)){
                    ui->lw_src->setCurrentItem(item2);
                    item2->setSelected(true);
                    return;
                }
            }
        }

        str=item->text();
        str=StringUtils::trimKuoHao(str);
        str=StringUtils::trimDigit(str);
        for (int i=0;i<ui->lw_src->count();i++) {
            auto item2=ui->lw_src->item(i);
            if(item2&&item2->isHidden()==false){
                QString str2=item2->text();
                str2=StringUtils::trimKuoHao(str2);
                str2=StringUtils::trimDigit(str2);
                if(StringUtils::equalIncase(str,str2)){
                    ui->lw_src->setCurrentItem(item2);
                    item2->setSelected(true);
                    return;
                }
            }
        }
    }
}

void StrMappingDialog::on_lw_mapping_itemDoubleClicked(QListWidgetItem *item)
{
    this->on_btn_delete_clicked();
}

void StrMappingDialog::on_btn_auto_2_clicked()
{
    this->on_btn_auto_clicked();
}
