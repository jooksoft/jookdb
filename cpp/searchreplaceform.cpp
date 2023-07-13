#include "mainwindow.h"
#include "searchreplaceform.h"
#include "ui_searchreplaceform.h"
#include "widgetutils.h"

#include <QMessageBox>
#include <QSettings>
#include <QTimer>
#include <QToolTip>
#include <QCompleter>
#include <QMenu>
#include <QWidgetAction>

SearchReplaceForm::SearchReplaceForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SearchReplaceForm)
{
    ui->setupUi(this);

    Qt::WindowFlags flags=Qt::Dialog;
    flags |=Qt::WindowMinMaxButtonsHint;
    flags |=Qt::WindowCloseButtonHint;
    setWindowFlags(flags);

#if defined(Q_OS_MAC)
    this->setWindowFlag(Qt::Tool,true);
#endif

    WidgetUtils::setSafeDeleteOnClose(this);

    freshFindHis();
    freshReplaceHis();

    this->ui->cbox_find->setCompleter(nullptr);
    this->ui->cbox_replace->setCompleter(nullptr);

    setCurrSelect();

    CustomEventFilter *eventFilter=new CustomEventFilter{this->ui->cbox_find};
    eventFilter->setFunc([=](QObject *obj, QEvent *event){
        switch (event->type())
        {
        case QEvent::KeyPress:{
                QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
                if(keyEvent->key()==Qt::Key_Enter||keyEvent->key()==Qt::Key_Return){
                    QTimer::singleShot(0,this,[=](){
                        this->on_btn_findNext_clicked();
                    });
                }
            }
            break;
        }
        return false;
    });
    this->ui->cbox_find->installEventFilter(eventFilter);

    CustomEventFilter *eventFilter2=new CustomEventFilter{this->ui->cbox_replace};
    eventFilter2->setFunc([=](QObject *obj, QEvent *event){
        switch (event->type())
        {
        case QEvent::KeyPress:{
                QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
                if(keyEvent->key()==Qt::Key_Enter||keyEvent->key()==Qt::Key_Return){
                    QTimer::singleShot(0,this,[=](){
                        this->on_btn_replaceFind_clicked();
                    });
                }
            }
            break;
        }
        return false;
    });
    this->ui->cbox_replace->installEventFilter(eventFilter2);

}

SearchReplaceForm::~SearchReplaceForm()
{
    delete ui;
}

void SearchReplaceForm::useReplace(bool use)
{
    this->on_chb_useRegular_clicked();

    if(use){
        this->ui->wd_replace->show();
        this->ui->btn_findPre->hide();
        this->ui->btn_replace->show();
        this->ui->btn_replaceAll->show();
        this->ui->btn_replaceFind->show();
        this->ui->chb_useReplace->setChecked(true);
    }else{
        this->ui->wd_replace->hide();
        this->ui->btn_findPre->show();
        this->ui->btn_replace->hide();
        this->ui->btn_replaceAll->hide();
        this->ui->btn_replaceFind->hide();
        this->ui->chb_useReplace->setChecked(false);
    }

    this->resize(this->width(),1);

}

void SearchReplaceForm::saveFindHis(const QString &findBa)
{
    //save find his
    if(findBa.length()<2048&&findBa.trimmed().length()>0){
        QSettings settings(QCoreApplication::applicationName());
        QStringList findHis = settings.value("findHis", QStringList{}).value<QStringList>();
        while(findHis.length()>30){
            findHis.removeLast();
        }
        findHis.removeAll(findBa);
        findHis.push_front(findBa);
        settings.setValue("findHis",findHis);
        this->ui->cbox_find->clear();
        for(auto &s:findHis){
            this->ui->cbox_find->addItem(s);
        }
    }
}

void SearchReplaceForm::saveReplaceHis(const QString &replaceBa)
{
    //save replace his
    if(replaceBa.length()<2048&&replaceBa.trimmed().length()>0){
        QSettings settings(QCoreApplication::applicationName());
        QStringList replaceHis = settings.value("replaceHis", QStringList{}).value<QStringList>();
        while(replaceHis.length()>30){
            replaceHis.removeLast();
        }
        replaceHis.removeAll(replaceBa);
        replaceHis.push_front(replaceBa);
        settings.setValue("replaceHis",replaceHis);
        this->ui->cbox_replace->clear();
        for(auto &s:replaceHis){
            this->ui->cbox_replace->addItem(s);
        }
    }
}

void SearchReplaceForm::freshFindHis()
{
    QSettings settings(QCoreApplication::applicationName());
    QStringList findHis = settings.value("findHis", QStringList{}).value<QStringList>();
    while(findHis.length()>30){
        findHis.removeLast();
    }
    this->ui->cbox_find->clear();
    this->ui->cbox_find->addItems(findHis);
    this->ui->cbox_find->setCurrentText("");
}

void SearchReplaceForm::freshReplaceHis()
{
    QSettings settings(QCoreApplication::applicationName());
    QStringList replaceHis = settings.value("replaceHis", QStringList{}).value<QStringList>();
    while(replaceHis.length()>30){
        replaceHis.removeLast();
    }
    this->ui->cbox_replace->clear();
    this->ui->cbox_replace->addItems(replaceHis);
    this->ui->cbox_replace->setCurrentText("");
}

void SearchReplaceForm::setCurrSelect()
{

    MyEdit *te=MainWindow::getContent()->getCurrTextEdit();
    if(te){
        int64_t selStart=te->selectionStart();
        int64_t selEnd=te->selectionEnd();
        if(selEnd>selStart){
            const char* documentBytes=reinterpret_cast<const char *>(te->characterPointer());
            QByteArray ba=QByteArray::fromRawData(documentBytes+selStart,selEnd-selStart);
            this->ui->cbox_find->setCurrentText(ba);
        }
    }
}

void SearchReplaceForm::on_chb_useReplace_clicked(bool checked)
{
    useReplace(checked);
}

void SearchReplaceForm::on_btn_close_clicked()
{
    this->close();
}

void SearchReplaceForm::on_btn_findNext_clicked()
{
    this->ui->lb_findStat->setText("");

    QByteArray findBa=this->ui->cbox_find->currentText().toUtf8();
    if(findBa.trimmed().length()<=0)return;

    saveFindHis(findBa);

    MyEdit *te=MainWindow::getContent()->getCurrTextEdit();
    if(!te)return;

    const char* documentBytes=reinterpret_cast<const char *>(te->characterPointer());

    int64_t lengthDoc = te->length();
    int64_t start=0;
    int64_t end=lengthDoc;

    if(this->ui->chb_inSelection->isChecked()){

        int64_t selStart=te->selectionStart();
        int64_t selEnd=te->selectionEnd();

        if(selStart==selEnd&&this->lastModifyNum!=te->modifyNum){
            QPoint pnt=this->ui->btn_findNext->mapToGlobal(QPoint{});
            pnt.setY(pnt.y()+this->ui->btn_findNext->height()/2);
            QToolTip::showText(pnt,"No selection",this->ui->btn_findNext);
            return;
        }

        if(selStart>selEnd){
            int64_t a=selStart;
            selStart=selEnd;
            selEnd=a;
        }

        if(selStart!=selEnd&&selStart!=this->lastFindStart&&selEnd!=this->lastFindEnd){
            this->lastSelStart=selStart;
            this->lastSelEnd=selEnd;
            this->lastModifyNum=te->modifyNum;
            SqlStyle::setFindIndic(te,selStart,selEnd-selStart);
        }

        start=this->lastSelStart;
        end=this->lastSelEnd;
    }

//    const char* documentBytes=reinterpret_cast<const char *>(te->characterPointer());

    int64_t currentPos=te->currentPos();

    if((this->lastFindstr==findBa&&this->lastStat==FIND_STATUS_FAIL)||currentPos>=end){
        findStart=start;
        findEnd=end;
    }else{
        if(this->lastFindstr==findBa&&this->lastType==FIND_TYPE_PRE&&this->lastStat==FIND_STATUS_SUCC&&currentPos==this->lastCursorPos){
            findStart=this->lastFindEnd;
        }else{
            findStart=currentPos;
        }
        findEnd=end;
    }

    int flags=0;
    if(this->ui->chb_matchCase->isChecked()){
        flags|=SCFIND_MATCHCASE;
    }
    if(this->ui->chb_wholeWord->isChecked()){
        flags|=SCFIND_WHOLEWORD;
    }
    if(this->ui->chb_useRegular->isChecked()){
        flags|=SCFIND_REGEXP|SCFIND_CXX11REGEX;
    }

    QPair<int, int> pair=te->findText(flags,findBa,findStart,findEnd);
    this->lastFindstr=findBa;
    if(pair.first>=0&&pair.first!=pair.second){
        te->setSel(pair.first,pair.second);
        this->lastCursorPos=te->currentPos();
        this->lastFindStart=pair.first;
        this->lastFindEnd=pair.second;
        this->lastType=FIND_TYPE_NEXT;
        this->lastStat=FIND_STATUS_SUCC;
    }else{
        this->lastType=FIND_TYPE_NEXT;
        this->lastStat=FIND_STATUS_FAIL;
        QPoint pnt=this->ui->btn_findNext->mapToGlobal(QPoint{});
        pnt.setY(pnt.y()+this->ui->btn_findNext->height()/2);
        QToolTip::showText(pnt,"Searched to the ending",this->ui->btn_findNext);
    }
}

void SearchReplaceForm::on_btn_findPre_clicked()
{
    this->ui->lb_findStat->setText("");

    QByteArray findBa=this->ui->cbox_find->currentText().toUtf8();
    if(findBa.trimmed().length()<=0)return;

    saveFindHis(findBa);

    MyEdit *te=MainWindow::getContent()->getCurrTextEdit();
    if(!te)return;

    int64_t lengthDoc = te->length();
    int64_t start=0;
    int64_t end=lengthDoc;

    if(this->ui->chb_inSelection->isChecked()){

        int64_t selStart=te->selectionStart();
        int64_t selEnd=te->selectionEnd();

        if(selStart==selEnd&&this->lastModifyNum!=te->modifyNum){
            QPoint pnt=this->ui->btn_findPre->mapToGlobal(QPoint{});
            pnt.setY(pnt.y()+this->ui->btn_findPre->height()/2);
            QToolTip::showText(pnt,"No selection",this->ui->btn_findPre);
            return;
        }

        if(selStart>selEnd){
            int64_t a=selStart;
            selStart=selEnd;
            selEnd=a;
        }

        if(selStart!=selEnd&&selStart!=this->lastFindStart&&selEnd!=this->lastFindEnd){
            this->lastSelStart=selStart;
            this->lastSelEnd=selEnd;
            this->lastModifyNum=te->modifyNum;
            SqlStyle::setFindIndic(te,selStart,selEnd-selStart);
        }

        start=this->lastSelStart;
        end=this->lastSelEnd;
    }

//    const char* documentBytes=reinterpret_cast<const char *>(te->characterPointer());

    int64_t currentPos=te->currentPos();

    if((this->lastFindstr==findBa&&this->lastStat==FIND_STATUS_FAIL)||currentPos<=start){
        findStart=end;
        findEnd=start;
    }else{
        if(this->lastFindstr==findBa&&this->lastType==FIND_TYPE_NEXT&&this->lastStat==FIND_STATUS_SUCC&&currentPos==this->lastCursorPos){
            findStart=this->lastFindStart;
        }else{
            findStart=currentPos;
        }
        findEnd=start;
    }

    int flags=0;
    if(this->ui->chb_matchCase->isChecked()){
        flags|=SCFIND_MATCHCASE;
    }
    if(this->ui->chb_wholeWord->isChecked()){
        flags|=SCFIND_WHOLEWORD;
    }
    if(this->ui->chb_useRegular->isChecked()){
        flags|=SCFIND_REGEXP|SCFIND_CXX11REGEX;
    }
    QPair<int, int> pair=te->findText(flags,findBa,findStart,findEnd);
    this->lastFindstr=findBa;
    if(pair.first>=0&&pair.first!=pair.second){
        te->setSel(pair.second,pair.first);
        this->lastCursorPos=te->currentPos();
        this->lastFindStart=pair.first;
        this->lastFindEnd=pair.second;
        this->lastType=FIND_TYPE_PRE;
        this->lastStat=FIND_STATUS_SUCC;
    }else{
        this->lastType=FIND_TYPE_PRE;
        this->lastStat=FIND_STATUS_FAIL;
        QPoint pnt=this->ui->btn_findPre->mapToGlobal(QPoint{});
        pnt.setY(pnt.y()+this->ui->btn_findPre->height()/2);
        QToolTip::showText(pnt,"Searched to the beginning",this->ui->btn_findPre);
    }
}

void SearchReplaceForm::on_chb_inSelection_clicked(bool checked)
{
    MyEdit *te=MainWindow::getContent()->getCurrTextEdit();
    if(!te)return;

    if(checked){
        int64_t selStart=te->selectionStart();
        int64_t selEnd=te->selectionEnd();

        if(selStart>selEnd){
            int64_t a=selStart;
            selStart=selEnd;
            selEnd=a;
        }

        this->lastSelStart=selStart;
        this->lastSelEnd=selEnd;
        SqlStyle::setFindIndic(te,selStart,selEnd-selStart);
    }else{
        SqlStyle::clearFindIndic(te);
    }
}

void SearchReplaceForm::on_btn_count_clicked()
{
    QByteArray findBa=this->ui->cbox_find->currentText().toUtf8();
    if(findBa.trimmed().length()<=0)return;

    saveFindHis(findBa);

    MyEdit *te=MainWindow::getContent()->getCurrTextEdit();
    if(!te)return;

    int64_t lengthDoc = te->length();
    int64_t start=0;
    int64_t end=lengthDoc;

    if(this->ui->chb_inSelection->isChecked()){

        int64_t selStart=te->selectionStart();
        int64_t selEnd=te->selectionEnd();

        if(selStart==selEnd&&this->lastModifyNum!=te->modifyNum){
            QPoint pnt=this->ui->btn_count->mapToGlobal(QPoint{});
            pnt.setY(pnt.y()+this->ui->btn_count->height()/2);
            QToolTip::showText(pnt,"No selection",this->ui->btn_count);
            return;
        }

        if(selStart>selEnd){
            int64_t a=selStart;
            selStart=selEnd;
            selEnd=a;
        }

        if(selStart!=selEnd&&selStart!=this->lastFindStart&&selEnd!=this->lastFindEnd){
            this->lastSelStart=selStart;
            this->lastSelEnd=selEnd;
            this->lastModifyNum=te->modifyNum;
            SqlStyle::setFindIndic(te,selStart,selEnd-selStart);
        }

        start=this->lastSelStart;
        end=this->lastSelEnd;
    }

    int flags=0;
    if(this->ui->chb_matchCase->isChecked()){
        flags|=SCFIND_MATCHCASE;
    }
    if(this->ui->chb_wholeWord->isChecked()){
        flags|=SCFIND_WHOLEWORD;
    }
    if(this->ui->chb_useRegular->isChecked()){
        flags|=SCFIND_REGEXP|SCFIND_CXX11REGEX;
    }

    int count=0;
    int lines=te->lineFromPosition(end)-te->lineFromPosition(start)+1;

    this->ui->lb_findStat->setText("");

    int64_t i=start;
    while(true){

        QPair<int, int> pair=te->findText(flags,findBa,i,end);
        this->lastFindstr=findBa;
        if(pair.first<0||pair.first==pair.second){
            this->ui->lb_findStat->setText(QString{"%1 matches are found in %2 lines."}.arg(count).arg(lines));
            break;
        }
        i=pair.second;
        count++;
    }
}

void SearchReplaceForm::on_btn_replaceAll_clicked()
{
    QByteArray findBa=this->ui->cbox_find->currentText().toUtf8();
    if(findBa.trimmed().length()<=0)return;

    saveFindHis(findBa);

    QByteArray replaceBa=this->ui->cbox_replace->currentText().toUtf8();

    saveReplaceHis(replaceBa);

    MyEdit *te=MainWindow::getContent()->getCurrTextEdit();
    if(!te)return;

    bool oldUseAC=te->getUseAutoComplete();
    te->setUseAutoComplete(false);
    LastRun lr{[=](){

            te->setUseAutoComplete(oldUseAC);
    }};

    int64_t lengthDoc = te->length();
    int64_t start=0;
    int64_t end=lengthDoc;

    if(this->ui->chb_inSelection->isChecked()){
        int64_t selStart=te->selectionStart();
        int64_t selEnd=te->selectionEnd();

        if(selStart==selEnd&&this->lastModifyNum!=te->modifyNum){
            QPoint pnt=this->ui->btn_replaceAll->mapToGlobal(QPoint{});
            pnt.setY(pnt.y()+this->ui->btn_replaceAll->height()/2);
            QToolTip::showText(pnt,"No selection",this->ui->btn_replaceAll);
            return;
        }

        if(selStart>selEnd){
            int64_t a=selStart;
            selStart=selEnd;
            selEnd=a;
        }

        if(selStart!=selEnd&&selStart!=this->lastFindStart&&selEnd!=this->lastFindEnd){
            this->lastSelStart=selStart;
            this->lastSelEnd=selEnd;
            this->lastModifyNum=te->modifyNum;
            SqlStyle::setFindIndic(te,selStart,selEnd-selStart);
        }

        start=this->lastSelStart;
        end=this->lastSelEnd;
    }

    if(start>=end)return;

    int flags=0;
    if(this->ui->chb_matchCase->isChecked()){
        flags|=SCFIND_MATCHCASE;
    }
    if(this->ui->chb_wholeWord->isChecked()){
        flags|=SCFIND_WHOLEWORD;
    }
    if(this->ui->chb_useRegular->isChecked()){
        flags|=SCFIND_REGEXP|SCFIND_CXX11REGEX;
    }

    te->beginUndoAction();
    int64_t i=start;
    while(true){

        QPair<int, int> pair=te->findText(flags,findBa,i,end);
        this->lastFindstr=findBa;
        if(pair.first<0||pair.first==pair.second){
            break;
        }

        te->setTargetStart(pair.first);
        te->setTargetEnd(pair.second);

        int replaceLen=0;
        if(this->ui->chb_useRegular->isChecked()){
            replaceLen=te->replaceTargetRE(replaceBa.length(),replaceBa.constData());
        }else{
            replaceLen=te->replaceTarget(replaceBa.length(),replaceBa.constData());
        }
        i=pair.first+replaceLen;
        end+=(replaceLen-(pair.second-pair.first));
    }
    te->endUndoAction();

    if(this->ui->chb_inSelection->isChecked()){
        if(end>start){
            this->lastSelStart=start;
            this->lastSelEnd=end;
            this->lastModifyNum=te->modifyNum;
            SqlStyle::setFindIndic(te,start,end-start);
        }
    }
}

void SearchReplaceForm::on_cbox_find_currentTextChanged(const QString &arg1)
{
    this->ui->lb_findStat->clear();
}

void SearchReplaceForm::closeEvent(QCloseEvent *event)
{
    this->hide();

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

void SearchReplaceForm::on_btn_replaceFind_clicked()
{

    QByteArray findBa=this->ui->cbox_find->currentText().toUtf8();
    if(findBa.trimmed().length()<=0)return;

    QByteArray replaceBa=this->ui->cbox_replace->currentText().toUtf8();

    saveReplaceHis(replaceBa);

    MyEdit *te=MainWindow::getContent()->getCurrTextEdit();
    if(!te)return;

    bool oldUseAC=te->getUseAutoComplete();
    te->setUseAutoComplete(false);
    LastRun lr{[=](){

            te->setUseAutoComplete(oldUseAC);
    }};

    int64_t selStart=te->selectionStart();
    int64_t selEnd=te->selectionEnd();

    if(selStart==selEnd){
        this->on_btn_findNext_clicked();
        selStart=te->selectionStart();
        selEnd=te->selectionEnd();
    };

    if(selStart>=selEnd)return;

    int findIndStart=0;
    int findIndEnd=0;
    if(this->ui->chb_inSelection->isChecked()){
        findIndStart=this->lastSelStart;
        findIndEnd=this->lastSelEnd;
    }

    te->setTargetStart(selStart);
    te->setTargetEnd(selEnd);

    int replaceLen=0;
    if(this->ui->chb_useRegular->isChecked()){
        replaceLen=te->replaceTargetRE(replaceBa.length(),replaceBa.constData());
    }else{
        replaceLen=te->replaceTarget(replaceBa.length(),replaceBa.constData());
    }
    te->setCurrentPos(selStart+replaceLen);

    if(this->ui->chb_inSelection->isChecked()){
        findIndEnd=findIndEnd+(replaceLen-(selEnd-selStart));
        if(findIndEnd>findIndStart){
            this->lastSelStart=findIndStart;
            this->lastSelEnd=findIndEnd;
            SqlStyle::setFindIndic(te,findIndStart,findIndEnd-findIndStart);
            this->lastModifyNum=te->modifyNum;
        }
    }

    this->on_btn_findNext_clicked();
}

void SearchReplaceForm::on_btn_replace_clicked()
{

    QByteArray findBa=this->ui->cbox_find->currentText().toUtf8();
    if(findBa.trimmed().length()<=0)return;

    QByteArray replaceBa=this->ui->cbox_replace->currentText().toUtf8();

    saveReplaceHis(replaceBa);

    MyEdit *te=MainWindow::getContent()->getCurrTextEdit();
    if(!te)return;

    bool oldUseAC=te->getUseAutoComplete();
    te->setUseAutoComplete(false);
    LastRun lr{[=](){

            te->setUseAutoComplete(oldUseAC);
    }};

    int64_t selStart=te->selectionStart();
    int64_t selEnd=te->selectionEnd();

    if(selStart==selEnd){
        this->on_btn_findNext_clicked();
        selStart=te->selectionStart();
        selEnd=te->selectionEnd();
    };

    if(selStart>=selEnd)return;

    int findIndStart=0;
    int findIndEnd=0;
    if(this->ui->chb_inSelection->isChecked()){
        findIndStart=this->lastSelStart;
        findIndEnd=this->lastSelEnd;
    }

    te->setTargetStart(selStart);
    te->setTargetEnd(selEnd);

    int replaceLen=0;
    if(this->ui->chb_useRegular->isChecked()){
        replaceLen=te->replaceTargetRE(replaceBa.length(),replaceBa.constData());
    }else{
        replaceLen=te->replaceTarget(replaceBa.length(),replaceBa.constData());
    }
    te->setCurrentPos(selStart+replaceLen);

    if(this->ui->chb_inSelection->isChecked()){
        findIndEnd=findIndEnd+(replaceLen-(selEnd-selStart));
        if(findIndEnd>findIndStart){
            this->lastSelStart=findIndStart;
            this->lastSelEnd=findIndEnd;
            SqlStyle::setFindIndic(te,findIndStart,findIndEnd-findIndStart);
            this->lastModifyNum=te->modifyNum;
        }
    }

}

void SearchReplaceForm::on_btn_find_reg_clicked()
{
    QMenu *menu=new QMenu{this};
    menu->deleteLater();

    QListWidget *lw=new QListWidget{menu};
    auto r=Utils::getCharGuiFontRect();
    lw->setFixedSize({r.width()*60,r.height()*50});

    QList<QPair<QString,QString>> list;

    list.push_back({"^","Beginning of Line"});
    list.push_back({"$","End of Line"});

    list.push_back({".","Any Single Character"});
    list.push_back({"*","Zero or More"});
    list.push_back({"+","One or More"});
    list.push_back({"?","Zero or One"});
    list.push_back({"{n,m}","n through m"});

    list.push_back({"*?","Non-greedy Zero or More"});
    list.push_back({"+?","Non-greedy One or More"});
    list.push_back({"??","Non-greedy Zero or One"});
    list.push_back({"{n,m}?","Non-greedy n through m"});

    list.push_back({"()","Group and Set Back Reference"});
    list.push_back({"|","Or"});
    list.push_back({"[]","Any One Character in the Set"});
    list.push_back({"[^]","Any One Character not in the Set"});

    list.push_back({"\\n","Line Feed(LF) or Newline"});
    list.push_back({"\\r","Carriage Return(CR)"});
    list.push_back({"\\t","Tab"});
    list.push_back({"\\w","Word"});
    list.push_back({"\\s","Space"});
    list.push_back({"\\d","Digit"});
    list.push_back({"\\l","Lowercase"});
    list.push_back({"\\u","Uppercase"});
    list.push_back({"\\Q","Begin Quote"});
    list.push_back({"\\E","End Quote"});
    list.push_back({"\\","Escape Special Character"});

    list.push_back({"(?:pattern)","Non-capturing Group"});
    list.push_back({"(?=pattern)","Positive Lookahead"});
    list.push_back({"(?!pattern)","Negative Lookahead"});
    list.push_back({"(?<=pattern)","Positive Lookbehind"});
    list.push_back({"(?<!pattern)","Negative Lookbehind"});

    for(const auto &pair:list){

        QListWidgetItem *item=new QListWidgetItem{pair.first};
        lw->addItem(item);
        QLabel* labRight = new QLabel(pair.second,lw);
        labRight->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
        labRight->setStyleSheet("background-color: transparent");
        lw->setItemWidget(item,labRight);
    }

    QWidgetAction *wact=new QWidgetAction{menu};
    wact->setDefaultWidget(lw);
    menu->addAction(wact);

    connect(lw,&QListWidget::itemClicked,[=](QListWidgetItem *item){
        this->ui->cbox_find->lineEdit()->insert(item->text());
        menu->close();
    });

    menu->exec(QCursor::pos());
}

void SearchReplaceForm::on_btn_replace_reg_clicked()
{
    QMenu *menu=new QMenu{this};
    menu->deleteLater();

    QListWidget *lw=new QListWidget{menu};
    auto r=Utils::getCharGuiFontRect();
    lw->setFixedSize({r.width()*60,r.height()*25});

    QList<QPair<QString,QString>> list;

    list.push_back({"\\0","Entire Back Reference"});

    list.push_back({"\\1","Back Reference 1"});
    list.push_back({"\\2","Back Reference 2"});
    list.push_back({"\\3","Back Reference 3"});
    list.push_back({"\\4","Back Reference 4"});
    list.push_back({"\\5","Back Reference 5"});
    list.push_back({"\\6","Back Reference 6"});
    list.push_back({"\\7","Back Reference 7"});
    list.push_back({"\\8","Back Reference 8"});
    list.push_back({"\\9","Back Reference 9"});

    list.push_back({"\\n","Line Feed(LF) or Newline"});
    list.push_back({"\\r","Carriage Return(CR)"});
    list.push_back({"\\t","Tab"});

    for(const auto &pair:list){

        QListWidgetItem *item=new QListWidgetItem{pair.first};
        lw->addItem(item);
        QLabel* labRight = new QLabel(pair.second,lw);
        labRight->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
        labRight->setStyleSheet("background-color: transparent");
        lw->setItemWidget(item,labRight);
    }

    QWidgetAction *wact=new QWidgetAction{menu};
    wact->setDefaultWidget(lw);
    menu->addAction(wact);

    connect(lw,&QListWidget::itemClicked,[=](QListWidgetItem *item){
        this->ui->cbox_replace->lineEdit()->insert(item->text());
        menu->close();
    });

    menu->exec(QCursor::pos());
}

void SearchReplaceForm::on_chb_useRegular_clicked()
{
    this->ui->btn_find_reg->setHidden(!ui->chb_useRegular->isChecked());
    this->ui->btn_replace_reg->setHidden(!ui->chb_useRegular->isChecked());
}
