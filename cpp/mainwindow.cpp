#include "aboutdialog.h"
#include "activatedialog.h"
#include "databasedefinitionexoprtdialog.h"
#include "datasyncdialog.h"
#include "globalutils.h"
#include "mainwindow.h"
#include "optionsdialog.h"
#include "runsqldialog.h"
#include "searchreplaceform.h"
#include "sqlstyle.h"
#include "structsyncdialog.h"
#include "usermanagerdialog.h"
#include "widgetutils.h"

#include <QtWidgets>
#include <fstream>
#include <QDebug>
#include <math.h>
#include <QTextCodec>

MainWindow::MainWindow()
{
    QSharedPointer<QSettings> ini=loadConfig();

    readSettings(ini);

#ifndef QT_NO_SESSIONMANAGER
//    QGuiApplication::setFallbackSessionManagementEnabled(false);
//    connect(qApp, &QGuiApplication::commitDataRequest,
//            this, &MainWindow::commitData);
#endif

    setUnifiedTitleAndToolBarOnMac(true);

    QTimer::singleShot(0,this,[=](){

        this->loadQSS();

        QTimer::singleShot(10,this,[=](){

            this->loadTranslator();

            this->content=new ContentWidget{this};

            this->setCentralWidget(content);

            this->createActions();
            this->createStatusBar();

            if(ini){
                ini->beginGroup(STR_OPTIONS);
                const QList<QVariant> sizes = ini->value("geometry2").toList();
                this->content->setContentSplitterSizes(sizes);
                ini->endGroup();
            }

        });

    });

}

MainWindow* MainWindow::instance()
{
    static MainWindow* ins=new MainWindow();
    return ins;
}

ContentWidget* MainWindow::getContent()
{
    return instance()->content;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (maybeSave()) {

        writeSettings();
        event->accept();

        Utils::stopServer();

    } else {

        event->ignore();
    }

}

void MainWindow::newFile()
{

    auto qf=this->content->getCurrQueryForm();

    QString db;
    QString dbName;
    if(qf){
        db=qf->getDbCombox()->currentText();
        dbName=qf->getDBNameCombox()->currentText();

    }else{
        auto item=this->content->getLeftTree()->currentItem();
        if(item&&item->isSelected()){
            QSharedPointer<ConnData> connData=Utils::getConnData(item);

            if(connData){

                switch (connData->typeId) {
                default:{
                    db=connData->connName;
                    while(item->parent()){
                        if(item->type()==TREE_ITEM_DB){
                            dbName=item->text(0);
                            break;
                        }
                        item=item->parent();
                    }
                }
                    break;
                }
            }

        }
    }

    this->content->addTab("",db,dbName);
    setCurrentTitle("");
}

void MainWindow::open()
{
    QSettings setting(QCoreApplication::applicationName());
    QString lastPath = setting.value("LastFilePath").toString();

    QFileDialog fdialog(this,QObject::tr("Open File"),lastPath,QObject::tr("Query file (*.sql *.js *.txt);;All files(*.*)"));
    fdialog.setOption(QFileDialog::DontUseNativeDialog,true);
    fdialog.setWindowModality(Qt::WindowModal);
    fdialog.setAcceptMode(QFileDialog::AcceptOpen);

    fdialog.layout()->addWidget(new QLabel("Encoding:",&fdialog));
    QComboBox * cboxEncoding=new QComboBox(&fdialog);
    QString autoDetect=QObject::tr("Auto detect");
    cboxEncoding->addItem(autoDetect);
    auto codecList=QTextCodec::availableCodecs();
    std::sort(codecList.begin(), codecList.end());
    for(QByteArray ba:codecList){
        cboxEncoding->addItem(ba);
    }
    fdialog.layout()->addWidget(cboxEncoding);
    if (fdialog.exec() == QDialog::Accepted && fdialog.selectedFiles().length()>0){
        QStringList fnames=fdialog.selectedFiles();
        QString lastName=fnames.last();
        QString codec=cboxEncoding->currentText();
        if(codec==autoDetect){
            codec="";
        }

        if(fnames.count()>0&&!lastName.isEmpty()){
            for(QString name:fnames){
                if(!name.isEmpty()){
                    loadFile(name,codec.toUtf8());
                }
            }

            QDir dir(fnames.last());
            dir.cdUp();
            setting.setValue("LastFilePath",dir.path());
        }
    }

}

bool MainWindow::save()
{
    auto qf=this->content->getCurrQueryForm();
    if(qf){
        if (qf->getMyEdit()->fileName.isEmpty()) {
            return saveAs();
        } else {
            return saveFile(qf->getMyEdit()->fileName);
        }
    }
    auto tf=this->content->getCurrTableForm();
    if(tf){
        return tf->save();
    }
    auto tdf=this->content->getCurrTableDataForm();
    if(tdf){
        return tdf->save();
    }
    return false;
}

void MainWindow::setStatus(const QString &text, int timeout)
{
    statusBar()->showMessage(text, timeout);
}

void MainWindow::clearStatusLabels()
{
    this->sqlLabel.clear();
    this->sqlLabel.setHidden(true);
    this->statusLabel.clear();
    this->statusLabel.setHidden(true);
    this->codecLabel.clear();
    this->codecLabel.setHidden(true);
}

int MainWindow::getIconThumbSize()
{
    return int(this->defaultFontSize*1.2);
}

QString MainWindow::getAppQfFontFamily()
{
    QString ret;

    QFontDatabase fdb;
    QStringList sList=fdb.families();


    //Consolas,Source Code Pro,Courier New,Lucida Console,Monaco,Menlo,DejaVu Sans Mono,Bitstream Vera Sans Mono,Droid Sans Mono,Monospace

    return ret;
}

void MainWindow::loadQSS()
{

    QString qss;
    if(this->defaultTheme==THEME_DARK){

        QFile qssfile(":/qss/mainwindow_dk.qss");
        qssfile.open(QFile::ReadOnly);
        qss = qssfile.readAll();

        qssfile.close();

    }else{

        QFile qssfile(":/qss/mainwindow.qss");
        qssfile.open(QFile::ReadOnly);
        qss = qssfile.readAll();

        qss.append(QString{"\nQWidget{font-family:\"%1\";font-size:%2pt;}"}.arg(this->defaultFontFamily).arg(this->defaultFontSize));

        if(this->defaultTheme==THEME_GREEN_LIGHT){

        }

        qssfile.close();

    }

//    qss.append(QString{"\nQAbstractItemView{icon-size:%1pt;}"}.arg(this->getIconThumbSize()));
    this->setStyleSheet(qss);
}

void MainWindow::loadTranslator()
{
    if(StringUtils::equalIncase("zh_cn",this->defaultLanguage)){
        QTranslator *translator=new QTranslator{this};
        translator->load(":/language/zh_cn.qm");
        qApp->installTranslator(translator);
    }
}

QRect MainWindow::getGuiFontRect(const QString &text,const QWidget * wid)
{
    QRect rect;
    if(wid){
        rect=wid->fontMetrics().boundingRect(text);
    }else{
        rect=this->fontMetrics().boundingRect(text);
    }
    return rect;
}

QRect MainWindow::getCharGuiFontRect()
{
    return this->getGuiFontRect("M");
}

bool MainWindow::saveAs()
{
    QString suggFileName;
    QString suggCodec="UTF-8";

    QString ext="sql";

    auto qf=this->content->getCurrQueryForm();
    if(qf){
        auto te=qf->getMyEdit();
        if (!te->fileName.isEmpty()) {
            suggFileName=te->fileName;
        }
        if(te->codecName.length()>0){
            suggCodec=te->codecName;
        }

        auto connData=qf->getCurrConnData();
        if(connData){
            ext=StringUtils::fileExtensionName(connData);
        }
    }

    QFileDialog fdialog(this,QObject::tr("Save As"),suggFileName,QObject::tr("Query file (*.%1);;All files(*.*)").arg(ext));
    fdialog.setOption(QFileDialog::DontUseNativeDialog,true);
    fdialog.setWindowModality(Qt::WindowModal);
    fdialog.setAcceptMode(QFileDialog::AcceptSave);

    fdialog.layout()->addWidget(new QLabel("Encoding:",&fdialog));
    QComboBox * cboxEncoding=new QComboBox(&fdialog);
    auto codecList=QTextCodec::availableCodecs();
    std::sort(codecList.begin(), codecList.end());
    for(QByteArray ba:codecList){
        cboxEncoding->addItem(ba);
    }
    WidgetUtils::setOrAddCurrItem(cboxEncoding,suggCodec,false);
    fdialog.layout()->addWidget(cboxEncoding);
    if (fdialog.exec() == QDialog::Accepted && fdialog.selectedFiles().length()>0){

        QString selectedFilter=fdialog.selectedNameFilter();
        QRegularExpression re{"\\*\\s*\\.\\s*([\\w\\d]+)"};
        QRegularExpressionMatch match=re.match(selectedFilter);
        if(match.hasMatch()){
            QString suffix=match.captured(1);
            suffix=suffix.trimmed();
            if(suffix.length()>0){
                fdialog.setDefaultSuffix(suffix);
            }
        }

        QStringList fnames=fdialog.selectedFiles();
        QString lastName=fnames.last();
        QString codec=cboxEncoding->currentText();

        if(lastName.isEmpty())return false;
        return saveFile(lastName,codec.toUtf8());
    }

    return false;
}

void MainWindow::about()
{
    AboutDialog *dlg=new AboutDialog{this};
    dlg->deleteLater();
    dlg->exec();
}

void MainWindow::createActions()
{

    QMenu *fileMenu = menuBar()->addMenu(tr("&File"));
    QToolBar *fileToolBar = addToolBar(tr("File"));

    this->connMenu=fileMenu->addMenu(tr("New Data Source"));
    connMenu->setIcon(QIcon{":/images/connection.svg"});
    connMenu->setStatusTip(tr("New Data Source"));

    connect(connMenu,&QMenu::aboutToShow,this,[=](){
        connMenu->clear();

        QStringList slist=APP_DB_TYPES_FREE;
        for(const auto &s:slist){
            if(s.length()>0){
                QAction *act = new QAction(s+"...", connMenu);
                connect(act, &QAction::triggered, this, [=](){this->content->openConnDlg(s);});
                connMenu->addAction(act);
            }
        }
        connMenu->addSeparator();
        slist=APP_DB_TYPES_PRO;
        for(const auto &s:slist){
            if(s.length()>0){
                QAction *act = new QAction(s+"...", connMenu);
                connect(act, &QAction::triggered, this, [=](){this->content->openConnDlg(s);});
                connMenu->addAction(act);
            }
        }
    });

    QToolButton* connTButton = new QToolButton{this};
    connTButton->setMenu(connMenu);
    connTButton->setPopupMode(QToolButton::InstantPopup);

    connTButton->setIcon(connMenu->icon());
    connTButton->setStatusTip(connMenu->statusTip());
    connTButton->setToolTip(connMenu->statusTip());

    connTButton->setStyleSheet("QToolButton::menu-indicator{image:none;}");

    fileToolBar->addWidget(connTButton);

    fileMenu->addSeparator();
    fileToolBar->addSeparator();

    QAction *newAct = new QAction(QIcon(":/images/new.svg"), tr("New Editor"), this);
    newAct->setShortcuts(QKeySequence::New);
    newAct->setStatusTip(tr("Create a new file"));
    connect(newAct, &QAction::triggered, this, &MainWindow::newFile);
    fileMenu->addAction(newAct);
    fileToolBar->addAction(newAct);

    const QIcon openIcon = QIcon(":/images/open.svg");
    QAction *openAct = new QAction(openIcon, tr("Open..."), this);
    openAct->setShortcuts(QKeySequence::Open);
    openAct->setStatusTip(tr("Open an existing file"));
    connect(openAct, &QAction::triggered, this, &MainWindow::open);
    fileMenu->addAction(openAct);

    QMenu * reopenMenu=fileMenu->addMenu(tr("Reopen..."));
    connect(reopenMenu,&QMenu::aboutToShow,this,[=](){
        reopenMenu->clear();
        QSettings settings(QCoreApplication::applicationName());
        QJsonArray openHis = settings.value("openHis", QJsonArray{}).value<QJsonArray>();
        while(openHis.count()>30){
            openHis.removeLast();
        }
        for(int i=0;i<openHis.count();i++){
            QJsonObject j=openHis[i].toObject();
            QString file=j["file"].toString();
            QString codec=j["codec"].toString();
            QAction *act = new QAction(file, reopenMenu);
            connect(act, &QAction::triggered, this, [=](){
                this->loadFile(file,codec.toUtf8());
            });
            reopenMenu->addAction(act);
        }

        reopenMenu->addSeparator();
        QAction *act = new QAction(QIcon(":/images/clear.svg"),"Clear open history", reopenMenu);
        connect(act, &QAction::triggered, this, [=](){
            QSettings settings(QCoreApplication::applicationName());
            settings.setValue("openHis",QJsonArray{});
        });
        reopenMenu->addAction(act);

    });

    QToolButton* reopenTButton = new QToolButton{this};
    reopenTButton->setIcon(openIcon);
    reopenTButton->setMenu(reopenMenu);
    reopenTButton->setPopupMode(QToolButton::MenuButtonPopup);
    connect(reopenTButton, &QToolButton::clicked, this, &MainWindow::open);
    reopenTButton->setStatusTip(openAct->statusTip());
    reopenTButton->setToolTip(openAct->statusTip());
#if defined(Q_OS_MAC)
    reopenTButton->setStyle(WidgetUtils::getFusionStyle());
#endif

    fileToolBar->addWidget(reopenTButton);

    saveAct = new QAction(QIcon(":/images/save.svg"), tr("Save"), this);
    saveAct->setShortcuts(QKeySequence::Save);
    saveAct->setStatusTip(tr("Save the document to disk"));
    connect(saveAct, &QAction::triggered, this, &MainWindow::save);
    fileMenu->addAction(saveAct);
    fileToolBar->addAction(saveAct);

    QAction *saveAsAct = fileMenu->addAction(tr("Save As..."), this, &MainWindow::saveAs);
    saveAsAct->setShortcuts(QKeySequence::SaveAs);
    saveAsAct->setStatusTip(tr("Save the document under a new name"));

    fileMenu->addSeparator();

    QAction *impSettingAct = new QAction(tr("Import Settings..."), this);
    impSettingAct->setStatusTip(tr("Import Settings File..."));
    connect(impSettingAct, &QAction::triggered, this, [&](){


        QFileDialog fdialog(this,QObject::tr("Import Settings File"),"",QObject::tr("Settings file (*.ini);;All files(*.*)"));
        fdialog.setOption(QFileDialog::DontUseNativeDialog,true);
        fdialog.setWindowModality(Qt::WindowModal);
        fdialog.setAcceptMode(QFileDialog::AcceptOpen);

        if (fdialog.exec() == QDialog::Accepted && fdialog.selectedFiles().length()>0){
            QStringList fnames=fdialog.selectedFiles();
            QString lastName=fnames.last();

            QSettings ini_src(lastName, QSettings::IniFormat);

            QSettings ini(Utils::getConfigFilePath(), QSettings::IniFormat);
//            ini.clear();

            for(const auto &s:ini_src.allKeys()){
                ini.setValue(s,ini_src.value(s));
            }

            QTimer::singleShot(50,this,[=](){
                this->content->getLeftTree()->refreshTree();
            });
        }

    });
    fileMenu->addAction(impSettingAct);

    QAction *expSettingAct = new QAction(tr("Export Settings..."), this);
    expSettingAct->setStatusTip(tr("Export Settings File..."));
    connect(expSettingAct, &QAction::triggered, this, [&](){

        QFileDialog fdialog(this,QObject::tr("Export Settings File"),"config.ini",QObject::tr("Settings file (*.ini);;All files(*.*)"));
        fdialog.setOption(QFileDialog::DontUseNativeDialog,true);
        fdialog.setWindowModality(Qt::WindowModal);
        fdialog.setAcceptMode(QFileDialog::AcceptSave);

        if (fdialog.exec() == QDialog::Accepted && fdialog.selectedFiles().length()>0){

            QString selectedFilter=fdialog.selectedNameFilter();
            QRegularExpression re{"\\*\\s*\\.\\s*([\\w\\d]+)"};
            QRegularExpressionMatch match=re.match(selectedFilter);
            if(match.hasMatch()){
                QString suffix=match.captured(1);
                suffix=suffix.trimmed();
                if(suffix.length()>0){
                    fdialog.setDefaultSuffix(suffix);
                }
            }

            QStringList fnames=fdialog.selectedFiles();
            QString lastName=fnames.last();

            QSettings ini_src(Utils::getConfigFilePath(), QSettings::IniFormat);

            QSettings ini(lastName, QSettings::IniFormat);

            for(const auto &s:ini_src.allKeys()){
                ini.setValue(s,ini_src.value(s));
            }
        }

    });
    fileMenu->addAction(expSettingAct);

    fileMenu->addSeparator();

    QAction *exitAct = fileMenu->addAction(tr("Exit"), this, &QWidget::close);
    exitAct->setMenuRole(QAction::QuitRole);

    exitAct->setStatusTip(tr("Exit the application"));

    QMenu *editMenu = menuBar()->addMenu(tr("&Edit"));
    QToolBar *editToolBar = addToolBar(tr("Edit"));

#ifndef QT_NO_CLIPBOARD
    QAction *cutAct = new QAction(tr("Cut"), this);
    cutAct->setShortcuts(QKeySequence::Cut);
    cutAct->setStatusTip(tr("Cut the current selection's contents to the "
                            "clipboard"));
    connect(cutAct, &QAction::triggered, this, [&](){
        auto widget=this->focusWidget();
        if(widget){
            if(widget->inherits(MyEdit::staticMetaObject.className())){
                auto wid=(MyEdit*)widget;
                wid->cut();
            }else if(widget->inherits(QLineEdit::staticMetaObject.className())){
                auto wid=(QLineEdit*)widget;
                wid->cut();
            }else if(widget->inherits(QPlainTextEdit::staticMetaObject.className())){
                auto wid=(QPlainTextEdit*)widget;
                wid->cut();
            }else if(widget->inherits(QTextEdit::staticMetaObject.className())){
                auto wid=(QTextEdit*)widget;
                wid->cut();
            }else if(widget->inherits(QComboBox::staticMetaObject.className())){
                auto wid=(QComboBox*)widget;
                wid->lineEdit()->cut();
            }else{
                auto te=this->content->getCurrTextEdit();
                if(te){

                    te->cut();
                }
            }
        }
    });
    editMenu->addAction(cutAct);

    QAction *copyAct = new QAction(tr("Copy"), this);
    copyAct->setShortcuts(QKeySequence::Copy);
    copyAct->setStatusTip(tr("Copy the current selection's contents to the "
                             "clipboard"));
    connect(copyAct, &QAction::triggered, this, [&](){
        auto widget=this->focusWidget();
        if(widget){
            if(widget->inherits(MyEdit::staticMetaObject.className())){
                auto wid=(MyEdit*)widget;
                wid->copy();
            }else if(widget->inherits(QLineEdit::staticMetaObject.className())){
                auto wid=(QLineEdit*)widget;
                wid->copy();
            }else if(widget->inherits(QPlainTextEdit::staticMetaObject.className())){
                auto wid=(QPlainTextEdit*)widget;
                wid->copy();
            }else if(widget->inherits(QTextEdit::staticMetaObject.className())){
                auto wid=(QTextEdit*)widget;
                wid->copy();
            }else if(widget->inherits(QComboBox::staticMetaObject.className())){
                auto wid=(QComboBox*)widget;
                wid->lineEdit()->copy();
            }else if(widget->inherits(QLabel::staticMetaObject.className())){
                auto wid=(QLabel*)widget;
                qApp->clipboard()->setText(wid->selectedText());
            }else if(widget->inherits(QTableWidget::staticMetaObject.className())){
                auto wid=(QTableWidget*)widget;
                WidgetUtils::copyTableWidget(wid);
            }else{
                auto te=this->content->getCurrTextEdit();
                if(te){

                    te->copy();
                }
            }
        }
    });
    editMenu->addAction(copyAct);

    QAction *pasteAct = new QAction(tr("Paste"), this);
    pasteAct->setShortcuts(QKeySequence::Paste);
    pasteAct->setStatusTip(tr("Paste the clipboard's contents into the current "
                              "selection"));
    connect(pasteAct, &QAction::triggered, this, [&](){
        auto widget=this->focusWidget();
        if(widget){
            if(widget->inherits(MyEdit::staticMetaObject.className())){
                auto wid=(MyEdit*)widget;
                if(!wid->readOnly()){
                    wid->pasteAdvance();
                }
            }else if(widget->inherits(QLineEdit::staticMetaObject.className())){
                auto wid=(QLineEdit*)widget;
                if(!wid->isReadOnly()){
                    wid->paste();
                }
            }else if(widget->inherits(QPlainTextEdit::staticMetaObject.className())){
                auto wid=(QPlainTextEdit*)widget;
                if(!wid->isReadOnly()){
                    wid->paste();
                }
            }else if(widget->inherits(QTextEdit::staticMetaObject.className())){
                auto wid=(QTextEdit*)widget;
                if(!wid->isReadOnly()){
                    wid->paste();
                }
            }else if(widget->inherits(QComboBox::staticMetaObject.className())){
                auto wid=(QComboBox*)widget;
                if(!wid->lineEdit()->isReadOnly()){
                    wid->lineEdit()->paste();
                }
            }else if(widget->inherits(QTableWidget::staticMetaObject.className())){
                auto wid=(QTableWidget*)widget;
                QVariant var=wid->property("ownerClass");
                if(!var.isNull()){

                    QString oc=var.toString();
                    if(oc=="tdf"){
                        auto tdf=this->content->getCurrTableDataForm();
                        if(tdf){
                            tdf->pasteItemDataAdvance("tsv");;
                        }
                    }else{
                        WidgetUtils::pasteTableWidget(wid);
                    }
                }else{
                    WidgetUtils::pasteTableWidget(wid);
                }

            }else{
                auto te=this->content->getCurrTextEdit();
                if(te){

                    if(!te->readOnly()){
                        te->paste();
                    }
                }
            }
        }
    });
    editMenu->addAction(pasteAct);

    editMenu->addSeparator();

    QAction *undoAct = new QAction(tr("Undo"), this);
    undoAct->setShortcuts(QKeySequence::Undo);
    undoAct->setStatusTip(tr("Undo"));
    connect(undoAct, &QAction::triggered, this, [&](){
        auto widget=this->focusWidget();
        if(widget){
            if(widget->inherits(MyEdit::staticMetaObject.className())){
                auto wid=(MyEdit*)widget;
                wid->undo();
            }else if(widget->inherits(QLineEdit::staticMetaObject.className())){
                auto wid=(QLineEdit*)widget;
                wid->undo();
            }else if(widget->inherits(QPlainTextEdit::staticMetaObject.className())){
                auto wid=(QPlainTextEdit*)widget;
                wid->undo();
            }else if(widget->inherits(QTextEdit::staticMetaObject.className())){
                auto wid=(QTextEdit*)widget;
                wid->undo();
            }else if(widget->inherits(QComboBox::staticMetaObject.className())){
                auto wid=(QComboBox*)widget;
                wid->lineEdit()->undo();
            }
        }
    });
    editMenu->addAction(undoAct);

    QAction *redoAct = new QAction(tr("Redo"), this);
    redoAct->setShortcuts(QKeySequence::Redo);
    redoAct->setStatusTip(tr("Redo"));
    connect(redoAct, &QAction::triggered, this, [&](){
        auto widget=this->focusWidget();
        if(widget){
            if(widget->inherits(MyEdit::staticMetaObject.className())){
                auto wid=(MyEdit*)widget;
                wid->redo();
            }else if(widget->inherits(QLineEdit::staticMetaObject.className())){
                auto wid=(QLineEdit*)widget;
                wid->redo();
            }else if(widget->inherits(QPlainTextEdit::staticMetaObject.className())){
                auto wid=(QPlainTextEdit*)widget;
                wid->redo();
            }else if(widget->inherits(QTextEdit::staticMetaObject.className())){
                auto wid=(QTextEdit*)widget;
                wid->redo();
            }else if(widget->inherits(QComboBox::staticMetaObject.className())){
                auto wid=(QComboBox*)widget;
                wid->lineEdit()->redo();
            }
        }
    });
    editMenu->addAction(redoAct);

    QAction *selectAllAct = new QAction(tr("Select All"), this);
    selectAllAct->setShortcuts(QKeySequence::SelectAll);
    selectAllAct->setStatusTip(tr("Select All"));
    connect(selectAllAct, &QAction::triggered, this, [&](){
        auto widget=this->focusWidget();
        if(widget){
            if(widget->inherits(MyEdit::staticMetaObject.className())){
                auto wid=(MyEdit*)widget;
                wid->selectAll();
            }else if(widget->inherits(QLineEdit::staticMetaObject.className())){
                auto wid=(QLineEdit*)widget;
                wid->selectAll();
            }else if(widget->inherits(QPlainTextEdit::staticMetaObject.className())){
                auto wid=(QPlainTextEdit*)widget;
                wid->selectAll();
            }else if(widget->inherits(QTextEdit::staticMetaObject.className())){
                auto wid=(QTextEdit*)widget;
                wid->selectAll();
            }else if(widget->inherits(QComboBox::staticMetaObject.className())){
                auto wid=(QComboBox*)widget;
                wid->lineEdit()->selectAll();
            }
        }
    });
    editMenu->addAction(selectAllAct);

    QAction *selectCurrStmtAct = new QAction(tr("Select Current Statement"), this);
    selectCurrStmtAct->setShortcut(QKeySequence(Qt::CTRL+Qt::SHIFT+Qt::Key_M));
    selectCurrStmtAct->setStatusTip(tr("Select Current Statement"));
    connect(selectCurrStmtAct, &QAction::triggered, this, [&](){
        auto qf=this->content->getCurrQueryForm();
        if(qf){
            qf->selCurrStatement_v3();
        }
    });
    editMenu->addAction(selectCurrStmtAct);

    editMenu->addSeparator();

    QAction *commentLineAct = new QAction(tr("Comment with Line Comment"), this);
    commentLineAct->setShortcut(QKeySequence(Qt::CTRL+Qt::Key_Slash));
    commentLineAct->setStatusTip(tr("Comment with Line Comment"));
    connect(commentLineAct, &QAction::triggered, this, [&](){
        auto qf=this->content->getCurrQueryForm();
        if(qf){
            auto connData=qf->getCurrConnData();
            int dbType=-1;
            if(connData){
                dbType=connData->typeId;
            }
            auto te=qf->getMyEdit();
            if(te){

                const int64_t lengthDoc = te->length();
                const char* documentBytes=reinterpret_cast<const char *>(te->characterPointer());

                int64_t start=te->selectionStart();
                int64_t end=te->selectionEnd();
                if(start==end){

                    int64_t curr_pos=te->currentPos();
                    int64_t curr_line=te->lineFromPosition(curr_pos);
                    int64_t line_start_pos=te->positionFromLine(curr_line);
                    int64_t line_end_pos=te->lineEndPosition(curr_line);
                    QByteArray ba=QByteArray::fromRawData(documentBytes+line_start_pos,line_end_pos-line_start_pos);
                    QByteArray left_ba=StringUtils::leftSpace(ba);
                    ba=ba.right(ba.length()-left_ba.length());
                    int pos_diff=0;
                    if(ba.startsWith("--")){
                        ba=ba.right(ba.length()-2);
                        ba=left_ba+ba;
                        pos_diff=-2;
                    }else if(ba.startsWith("#")){
                        ba=ba.right(ba.length()-1);
                        ba=left_ba+ba;
                        pos_diff=-1;
                    }else{
                        ba=left_ba+ba;

                        switch (dbType) {
                        case DB_TYPE_MYSQL:{
                            ba="#"+ba;
                            pos_diff=1;
                        }
                            break;
                        default:{
                            ba="--"+ba;
                            pos_diff=2;
                        }
                            break;
                        }
                    }

                    te->setTargetStart(line_start_pos);
                    te->setTargetEnd(line_end_pos);
                    te->replaceTarget(ba.length(),ba.constData());

                    te->gotoPos(curr_pos+pos_diff);

//                    int64_t line2_start_pos=te->positionFromLine(curr_line+1);
//                    te->gotoPos(line2_start_pos);
                }else{
                    if(start>end){
                        std::swap(start,end);
                    }

                    int64_t start_line=te->lineFromPosition(start);
                    int64_t start_line_start_pos=te->positionFromLine(start_line);
                    int64_t end_line=te->lineFromPosition(end);
                    int64_t end_line_end_pos=te->lineEndPosition(end_line);
                    QByteArray ba=QByteArray::fromRawData(documentBytes+start_line_start_pos,end_line_end_pos-start_line_start_pos);
                    QString str=ba;
                    QStringList slist=str.split(QRegularExpression{"\r\n?|\n"});
                    bool inZS=true;
                    QStringList slist2=slist;
                    StringUtils::trimListAndRemoveblank(slist2);
                    for(QString s:slist2){
                        QString left_str=StringUtils::leftSpace(s);
                        s=s.right(s.length()-left_str.length());
                        if(s.startsWith("--")||s.startsWith("#")){
                            continue;
                        }else{
                            inZS=false;
                            break;
                        }
                    }
                    QStringList targetList;
                    if(inZS){
                        for(QString s:slist){
                            QString left_str=StringUtils::leftSpace(s);
                            s=s.right(s.length()-left_str.length());
                            if(s.startsWith("--")){
                                s=s.right(s.length()-2);
                                s=left_str+s;
                            }else if(s.startsWith("#")){
                                s=s.right(s.length()-1);
                                s=left_str+s;
                            }
                            targetList.append(s);
                        }
                    }else{
                        for(QString s:slist){
                            switch (dbType) {
                            case DB_TYPE_MYSQL:{
                                s="#"+s;
                            }
                                break;
                            default:{
                                s="--"+s;
                            }
                                break;
                            }
                            targetList.append(s);
                        }
                    }
                    str=targetList.join("\n");
                    ba=str.toUtf8();

                    te->setTargetStart(start_line_start_pos);
                    te->setTargetEnd(end_line_end_pos);
                    te->replaceTarget(ba.length(),ba.constData());

                    te->setSel(start_line_start_pos,start_line_start_pos+ba.length());
                }
            }
        }
    });
    editMenu->addAction(commentLineAct);

    QAction *commentBlockAct = new QAction(tr("Comment with Block Comment"), this);
    commentBlockAct->setShortcut(QKeySequence(Qt::CTRL+Qt::SHIFT+Qt::Key_Slash));
    commentBlockAct->setStatusTip(tr("Comment with Block Comment"));
    connect(commentBlockAct, &QAction::triggered, this, [&](){
        auto qf=this->content->getCurrQueryForm();
        if(qf){
            auto connData=qf->getCurrConnData();
            int dbType=-1;
            if(connData){
                dbType=connData->typeId;
            }
            auto te=qf->getMyEdit();
            if(te){

                const int64_t lengthDoc = te->length();
                const char* documentBytes=reinterpret_cast<const char *>(te->characterPointer());

                int64_t start=te->selectionStart();
                int64_t end=te->selectionEnd();
                if(start==end){

                    int64_t curr_pos=te->currentPos();

                    te->insertText(curr_pos,"/**/");
                    te->gotoPos(curr_pos+2);
                }else{
                    if(start>end){
                        std::swap(start,end);
                    }

                    QByteArray ba=QByteArray::fromRawData(documentBytes+start,end-start);
                    QByteArray left_ba=StringUtils::leftSpace(ba);
                    ba=ba.right(ba.length()-left_ba.length());
                    QByteArray right_ba=StringUtils::rightSpace(ba);
                    ba=ba.left(ba.length()-right_ba.length());
                    if(ba.startsWith("/*")&&ba.endsWith("*/")){
                        ba=ba.mid(2,ba.length()-4);
                        ba=left_ba+ba+right_ba;
                    }else{
                        ba=left_ba+ba+right_ba;
                        ba="/*"+ba+"*/";
                    }

                    te->setTargetStart(start);
                    te->setTargetEnd(end);
                    te->replaceTarget(ba.length(),ba.constData());

                    te->setSel(start,start+ba.length());

                }
            }
        }
    });
    editMenu->addAction(commentBlockAct);

    editMenu->addSeparator();

    auto findAct = new QAction(QIcon{":/images/find.svg"}, tr("Find && Replace..."), this);
    findAct->setShortcut(QKeySequence::Find);
    findAct->setStatusTip(tr("Find & Replace..."));
    connect(findAct, &QAction::triggered, this, [&](){
        QueryForm *qf=this->content->getCurrQueryForm();
        if(qf){
            auto wid=this->findChild<SearchReplaceForm*>();
            if(wid){
                SearchReplaceForm *srForm=static_cast<SearchReplaceForm*>(wid);
//                srForm->useReplace(false);
                srForm->setCurrSelect();
                srForm->show();
                srForm->activateWindow();
            }else{

                SearchReplaceForm *srForm=new SearchReplaceForm(this);
                srForm->useReplace(false);
                srForm->show();
            }
        }
    });

    editMenu->addAction(findAct);
    editToolBar->addAction(findAct);

    auto replaceAct = new QAction(tr("Replace..."), this);
    replaceAct->setShortcut(QKeySequence::Replace);
    replaceAct->setStatusTip(tr("Replace..."));
    connect(replaceAct, &QAction::triggered, this, [&](){
        QueryForm *qf=this->content->getCurrQueryForm();
        if(qf){
            auto wid=this->findChild<SearchReplaceForm*>();
            if(wid){
                SearchReplaceForm *srForm=static_cast<SearchReplaceForm*>(wid);
                srForm->useReplace(true);
                srForm->setCurrSelect();
                srForm->show();
            }else{

                SearchReplaceForm *srForm=new SearchReplaceForm(this);
                srForm->useReplace(true);
                srForm->show();
            }
        }
    });

    editMenu->addAction(replaceAct);

    editMenu->addSeparator();

    auto tableFilterAct = new QAction(tr("Table Filter"), this);
    tableFilterAct->setShortcut(QKeySequence(Qt::CTRL+Qt::Key_E));
    tableFilterAct->setStatusTip(tr("Table Filter"));
    connect(tableFilterAct, &QAction::triggered, this, [&](){
        QLineEdit* le=this->content->getLeftTree_filter();
        if(le){
            le->selectAll();
            le->setFocus();
        }
    });

    editMenu->addAction(tableFilterAct);

#endif // !QT_NO_CLIPBOARD

    QMenu *queryMenu = menuBar()->addMenu(tr("&Query"));
//    QToolBar *queryToolBar = addToolBar(tr("Query"));

    QAction *runCurrAct = new QAction(QIcon{":/images/run.svg"},tr("Run Current Statement"), this);
    runCurrAct->setShortcut(Qt::Key_F8);
    runCurrAct->setStatusTip(tr("Run Current Statement"));
    connect(runCurrAct, &QAction::triggered, this, [&](){
        auto qf=this->content->getCurrQueryForm();
        if(qf){
            qf->runQuery();
        }else{
            auto tdf=this->content->getCurrTableDataForm();
            if(tdf){
                tdf->runApplyFilter();
            }
        }

    });
    queryMenu->addAction(runCurrAct);

    QAction *runAct = new QAction(tr("Run Script"), this);
    runAct->setShortcut(Qt::CTRL+Qt::Key_R);
    runAct->setStatusTip(tr("Run Script"));
    connect(runAct, &QAction::triggered, this, [&](){
        auto qf=this->content->getCurrQueryForm();
        if(qf){
            qf->runQuery(QueryRunType::script);
        }else{
            auto tdf=this->content->getCurrTableDataForm();
            if(tdf){
                tdf->runApplyFilter();
            }
        }

    });
    queryMenu->addAction(runAct);

    QAction *runOneAct = new QAction(tr("Run as One Statement"), this);
    runOneAct->setShortcut(Qt::CTRL+Qt::ALT+Qt::Key_R);
    runOneAct->setStatusTip(tr("Run as One Statement"));
    connect(runOneAct, &QAction::triggered, this, [&](){
        auto qf=this->content->getCurrQueryForm();
        if(qf){
            qf->runQuery(QueryRunType::one_script);
        }else{
            auto tdf=this->content->getCurrTableDataForm();
            if(tdf){
                tdf->runApplyFilter();
            }
        }

    });
    queryMenu->addAction(runOneAct);

    QAction *stopAct = new QAction(QIcon{":/images/stop.svg"}, tr("Stop"), this);
    stopAct->setShortcut(QKeySequence(Qt::SHIFT+Qt::Key_Escape));
    stopAct->setStatusTip(tr("Stop"));
    connect(stopAct, &QAction::triggered, this, [&](){
        auto qf=this->content->getCurrQueryForm();
        if(qf){
            qf->on_btn_stop_clicked();
        }

    });
    queryMenu->addAction(stopAct);

    queryMenu->addSeparator();

    QAction *commitAct = new QAction(QIcon{":/images/commit.svg"}, tr("Commit"), this);
    commitAct->setShortcut(Qt::Key_F10);
    commitAct->setStatusTip(tr("Commit"));
    connect(commitAct, &QAction::triggered, this, [&](){
        auto qf=this->content->getCurrQueryForm();
        if(qf){
            qf->on_btnCommit_clicked();
        }

    });
    queryMenu->addAction(commitAct);

    QAction *rollbackAct = new QAction(QIcon{":/images/rollback.svg"}, tr("Rollback"), this);
    rollbackAct->setShortcut(QKeySequence(Qt::SHIFT+Qt::Key_F10));
    rollbackAct->setStatusTip(tr("Rollback"));
    connect(rollbackAct, &QAction::triggered, this, [&](){
        auto qf=this->content->getCurrQueryForm();
        if(qf){
            qf->on_btnRollback_clicked();
        }

    });
    queryMenu->addAction(rollbackAct);

    queryMenu->addSeparator();

    QAction *explainAct = new QAction(QIcon{":/images/explain.svg"}, tr("Explain"), this);
    explainAct->setStatusTip(tr("Explain"));
    connect(explainAct, &QAction::triggered, this, [&](){
        auto qf=this->content->getCurrQueryForm();
        if(qf){
            qf->on_btn_explain_clicked();
        }

    });
    queryMenu->addAction(explainAct);

    queryMenu->addSeparator();

    QAction *autoCompleteAct = new QAction(tr("Code Completion"), this);
    autoCompleteAct->setShortcut(QKeySequence(Qt::CTRL+Qt::Key_Down));
    autoCompleteAct->setShortcutContext(Qt::WidgetShortcut);//编辑器已经支持此快捷键,这里不必使用
    autoCompleteAct->setStatusTip(tr("Code Completion"));
    connect(autoCompleteAct, &QAction::triggered, this, [=](){
        auto qf=this->content->getCurrQueryForm();
        if(qf){
            MyEdit* myEdit=qf->getMyEdit();
            if(myEdit&&myEdit->isVisible()){
                myEdit->showAutoComplete(".");
            }
            return;
        }

        auto tdf=this->content->getCurrTableDataForm();
        if(tdf){
            MyEdit* myEdit=tdf->getFilterEdit();
            if(myEdit&&myEdit->isVisible()){
                myEdit->showAutoComplete(".");
            }
            return;
        }
    });
    queryMenu->addAction(autoCompleteAct);

    queryMenu->addSeparator();

    QMenu *toolMenu = menuBar()->addMenu(tr("&Tools"));
    QToolBar *toolToolBar = addToolBar(tr("Tools"));

    auto formatAct = new QAction(QIcon{":/images/format.svg"}, tr("Reformat Code"), this);
    formatAct->setShortcut(QKeySequence(Qt::CTRL+Qt::ALT+Qt::Key_L));
    formatAct->setStatusTip(tr("Reformat Code(SQL, JSON, XML)"));
    formatAct->setToolTip(formatAct->statusTip());
    connect(formatAct, &QAction::triggered, this, [&](){
        QueryForm *qf=this->content->getCurrQueryForm();
        if(qf){
            qf->formatSql();
        }
    });

    auto usermAct = new QAction(QIcon{":/images/user.svg"},tr("User Manager"),this);
    usermAct->setStatusTip(tr("User Manager"));
    connect(usermAct, &QAction::triggered, this, [&](){

        MyTreeWidget *tree=this->getContent()->getLeftTree();
        if(tree){

            auto item=tree->currentItem();
            if(!item){
                QMessageBox::warning(this, QCoreApplication::applicationName(),tr("No data source selected!").trimmed());
                return;
            }
            item=Utils::getDsItem(item);

            if(item&&item->type()==TREE_ITEM_TOP_DS){

                auto connData=Utils::getConnData(item);
                if(Utils::testOrReconn(connData)){
                    auto userManager=new UserManagerDialog{connData,this};
                    userManager->deleteLater();
                    userManager->exec();
                }
            }
        }
    });

    toolMenu->addAction(formatAct);

    toolMenu->addSeparator();

    toolMenu->addAction(usermAct);

    toolToolBar->addAction(usermAct);

    toolToolBar->addAction(formatAct);

    auto dbmAct = new QAction(tr("Databases Manager"),this);
    dbmAct->setStatusTip(tr("Databases Manager"));
    connect(dbmAct, &QAction::triggered, this, [&](){
        MyTreeWidget *tree=this->getContent()->getLeftTree();
        if(tree){
            auto item=tree->currentItem();
            if(!item){
                QMessageBox::warning(this, QCoreApplication::applicationName(),tr("No data source selected!").trimmed());
                return;
            }
            item=Utils::getDsItem(item);

            if(item&&item->type()==TREE_ITEM_TOP_DS){

                auto connData=Utils::getConnData(item);
                if(Utils::testOrReconn(connData)){
                    MainWindow::getContent()->addDatabaseManager(connData);
                }
            }
        }
    });

    toolMenu->addAction(dbmAct);

    auto obmAct = new QAction(tr("Objects Manager"),this);
    obmAct->setStatusTip(tr("Objects Manager"));
    connect(obmAct, &QAction::triggered, this, [&](){
        MyTreeWidget *tree=this->getContent()->getLeftTree();
        if(tree){
            auto item=tree->currentItem();
            if(!item){
                QMessageBox::warning(this, QCoreApplication::applicationName(),tr("No data source selected!").trimmed());
                return;
            }

            auto connData=Utils::getConnData(item);
            QString dbName=this->content->getLeftTree()->getDbName(item);

            if(connData&&dbName.length()>0){

                if(Utils::testOrReconn(connData)){
                    MainWindow::getContent()->addObjectManager(connData,dbName);
                }
            }
        }
    });

    toolMenu->addAction(obmAct);

    toolMenu->addSeparator();

    auto dataTransAct = new QAction(tr("Data Synchronization..."),this);
    connect(dataTransAct, &QAction::triggered, this, [&](){
        if(!GlobalUtils::checkProLicense(true)){
            return;
        }
        DataSyncDialog * dsd=new DataSyncDialog{DataSyncType::dataTrans,this};
        dsd->show();
    });

    toolMenu->addAction(dataTransAct);

    auto structSyncAct = new QAction(tr("Structure Synchronization..."),this);
    connect(structSyncAct, &QAction::triggered, this, [&](){
        if(!GlobalUtils::checkProLicense(true)){
            return;
        }
        StructSyncDialog * ssd=new StructSyncDialog{this};
        ssd->show();
    });

    toolMenu->addAction(structSyncAct);

    auto genDocAct = new QAction(tr("Generate DB Doc..."),this);
    genDocAct->setStatusTip(tr("Generate DB Doc..."));
    connect(genDocAct, &QAction::triggered, this, [&](){
        MyTreeWidget *tree=this->getContent()->getLeftTree();
        if(tree){
            auto item=tree->currentItem();
            if(!item){
                QMessageBox::warning(this, QCoreApplication::applicationName(),tr("No data source selected!").trimmed());
                return;
            }

            auto connData=Utils::getConnData(item);
            QString dbName=this->content->getLeftTree()->getDbName(item);

            if(connData){

                if(Utils::testOrReconn(connData)){
                    DatabaseDefinitionExoprtDialog *ed=new DatabaseDefinitionExoprtDialog(connData,dbName,"",MainWindow::instance());
                    ed->show();
                }
            }
        }
    });

    toolMenu->addAction(genDocAct);

    auto runSqlAct = new QAction(tr("Execute File..."),this);
    runSqlAct->setStatusTip(tr("Execute File..."));
    connect(runSqlAct, &QAction::triggered, this, [&](){
        MyTreeWidget *tree=this->getContent()->getLeftTree();
        if(tree){
            auto item=tree->currentItem();
            if(!item){
                QMessageBox::warning(this, QCoreApplication::applicationName(),tr("No data source selected!").trimmed());
                return;
            }

            auto connData=Utils::getConnData(item);
            QString dbName=this->content->getLeftTree()->getDbName(item);

            if(connData){

                switch (connData->typeId) {
                }

                if(Utils::testOrReconn(connData)){
                    RunSqlDialog * rsd=new RunSqlDialog{connData,dbName,this};
                    rsd->show();
                }
            }
        }
    });

    toolMenu->addAction(runSqlAct);

    toolMenu->addSeparator();

    auto loadLogAct = new QAction(QIcon{":/images/log.svg"},tr("History Log"),this);
    loadLogAct->setStatusTip(tr("History Log"));
    connect(loadLogAct, &QAction::triggered, this, [&](){
        QString fpath=Utils::getAppDataPath()+"logs/";
        Utils::createDirs(fpath);

        QString currDate=QDateTime::currentDateTime().toString("yyyyMMdd");
        QString logname=currDate+".log";

        QString fileName=fpath+logname;

        if(!Utils::isFileExist(fileName)){//如果文件不存在则创建
            QFile file{fileName};
            if (!file.open(QFile::WriteOnly | QFile::Append)) {

            }

            file.close();
        }

        if(Utils::isFileExist(fileName)){
            auto qf=this->loadFile(fileName,"UTF-8",false);
            if(qf){
                qf->setLogForm();
            }
        }else{

            QMessageBox::about(this, QCoreApplication::applicationName(),
                                   tr("Log file does not exist \n%1").arg(fpath+logname));
        }

    });

    toolMenu->addAction(loadLogAct);
    toolToolBar->addAction(loadLogAct);

    toolMenu->addSeparator();

    auto optionsAct = new QAction(tr("Preferences..."),this);
    optionsAct->setMenuRole(QAction::PreferencesRole);
    optionsAct->setStatusTip(tr("Preferences..."));
    connect(optionsAct, &QAction::triggered, this, [&](){
        OptionsDialog *optionsdialog=new OptionsDialog{this};
        optionsdialog->deleteLater();
        optionsdialog->exec();
    });

    toolMenu->addAction(optionsAct);

    QMenu *windowMenu = menuBar()->addMenu(tr("&Window"));

    QAction *previousAct = new QAction(tr("Previous Tab"), this);
    previousAct->setShortcut(QKeySequence(Qt::ALT + Qt::Key_Left));
    previousAct->setStatusTip(tr("Previous Tab"));
    connect(previousAct, &QAction::triggered, this, [&](){

        int idx=this->content->getRightTab()->currentIndex();
        int count=this->content->getRightTab()->count();
        int pre_idx=idx-1;
        if(pre_idx>=0){
            this->content->getRightTab()->setCurrentIndex(pre_idx);
        }

    });
    windowMenu->addAction(previousAct);

    auto nextAct = new QAction(tr("Next Tab"), this);
    nextAct->setShortcut(QKeySequence(Qt::ALT + Qt::Key_Right));
    nextAct->setStatusTip(tr("Next tab"));
    connect(nextAct, &QAction::triggered, this, [&](){

        int idx=this->content->getRightTab()->currentIndex();
        int count=this->content->getRightTab()->count();
        int next_idx=idx+1;
        if(next_idx<count){
            this->content->getRightTab()->setCurrentIndex(next_idx);
        }

    });
    windowMenu->addAction(nextAct);

    windowMenu->addSeparator();

    auto closeAct = new QAction(tr("Close Tab"), this);
    closeAct->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_W));
    closeAct->setStatusTip(tr("Close tab"));
    connect(closeAct, &QAction::triggered, this, [&](){
        int idx=this->content->getRightTab()->currentIndex();
        if(idx>=0){
            emit this->content->getRightTab()->tabCloseRequested(idx);
        }
    });
    windowMenu->addAction(closeAct);

    auto closeOthersAct = new QAction(tr("Close Others"), this);
    closeOthersAct->setStatusTip(tr("Close Others"));
    connect(closeOthersAct, &QAction::triggered, this, [&](){
        int idx=this->content->getRightTab()->currentIndex();
        int count=this->content->getRightTab()->count();
        for(int i=count-1;i>=0;i--){
            if(i!=idx){
                if(this->content->closeTab(i)==false){
                    break;
                }
            }
        }
    });
    windowMenu->addAction(closeOthersAct);

    auto closeRightOthersAct = new QAction(tr("Close Right Others"), this);
    closeRightOthersAct->setStatusTip(tr("Close Right Others"));
    connect(closeRightOthersAct, &QAction::triggered, this, [&](){
        int idx=this->content->getRightTab()->currentIndex();
        int count=this->content->getRightTab()->count();
        for(int i=count-1;i>=0;i--){
            if(i!=idx){
                if(this->content->closeTab(i)==false){
                    break;
                }
            }else{
                break;
            }
        }
    });
    windowMenu->addAction(closeRightOthersAct);

    auto closeAllAct = new QAction(tr("Close All"), this);
    closeAllAct->setShortcut(QKeySequence(Qt::CTRL +Qt::SHIFT+ Qt::Key_W));
    closeAllAct->setStatusTip(tr("Close All"));
    connect(closeAllAct, &QAction::triggered, this, [&](){
        int idx=this->content->getRightTab()->currentIndex();
        int count=this->content->getRightTab()->count();
        for(int i=count-1;i>=0;i--){
            if(this->content->closeTab(i)==false){
                break;
            }
        }
    });
    windowMenu->addAction(closeAllAct);

    windowMenu->addSeparator();

    auto collapseAct = new QAction(tr("Collapse Sidebar"), this);
    collapseAct->setShortcut(QKeySequence(Qt::CTRL +Qt::SHIFT+ Qt::Key_C));
    collapseAct->setStatusTip(tr("Collapse Sidebar"));
    connect(collapseAct, &QAction::triggered, this, [&](){
        auto item=this->content->getLeftTree()->currentItem();
        if(item){
            this->content->getLeftTree()->collapseItem(item);
            if(item->parent()){
                this->content->getLeftTree()->collapseItem(item->parent());
                this->content->getLeftTree()->setCurrentItem(item->parent());
            }
        }
    });
    windowMenu->addAction(collapseAct);

    menuBar()->addSeparator();

    QMenu *helpMenu = menuBar()->addMenu(tr("&Help"));

    QAction *registerAct = helpMenu->addAction(tr("Register..."), this, [this](){
        ActivateDialog *dlg=new ActivateDialog{this};
        dlg->deleteLater();
        dlg->exec();

        GlobalUtils::checkLicense();
    });

    helpMenu->addAction(tr("Check for updates..."), this, [this](){
        AboutDialog *dlg=new AboutDialog{this};
        dlg->deleteLater();
        dlg->checkNewVersion();
        dlg->exec();
    });

    helpMenu->addAction(tr("Download page..."), this, [this](){
        QDesktopServices::openUrl(QUrl(DOWNLOAD_URI));
    });

    QAction *aboutAct = helpMenu->addAction(tr("About"), this, &MainWindow::about);
    aboutAct->setMenuRole(QAction::AboutRole);
    aboutAct->setStatusTip(tr("Show the application's About box"));

}

void MainWindow::createStatusBar()
{
    statusBar()->showMessage(tr("Ready"));

    statusLabel.setFrameStyle(QFrame::Box | QFrame::Sunken);
    statusLabel.setMaximumWidth(Utils::getCharGuiFontRect().width()*80);
    statusLabel.setTextInteractionFlags(Qt::TextSelectableByMouse);
    statusLabel.setTextFormat(Qt::RichText);
    statusBar()->addPermanentWidget(&statusLabel);

    sqlLabel.setFrameStyle(QFrame::Box | QFrame::Sunken);
    sqlLabel.setMaximumWidth(Utils::getCharGuiFontRect().width()*50);
    sqlLabel.setTextInteractionFlags(Qt::TextSelectableByMouse);
    sqlLabel.setTextFormat(Qt::RichText);
    statusBar()->addPermanentWidget(&sqlLabel);

    codecLabel.setFrameStyle(QFrame::Box | QFrame::Sunken);
    codecLabel.setTextInteractionFlags(Qt::TextSelectableByMouse);
    codecLabel.setTextFormat(Qt::RichText);
    statusBar()->addPermanentWidget(&codecLabel);

    this->clearStatusLabels();
}

void MainWindow::readSettings(QSharedPointer<QSettings> ini)
{
    if(!ini)return;

    ini->beginGroup(STR_OPTIONS);

//    const QRect geometry = ini->value("geometry1").toRect();
    const QByteArray geometry = ini->value("geometry1").toByteArray();
    if (geometry.isEmpty()) {
        const QRect availableGeometry = QApplication::desktop()->availableGeometry(this);
        resize(availableGeometry.width() / 1.2, availableGeometry.height() / 1.1);
        move((availableGeometry.width() - width()) / 2,
             (availableGeometry.height() - height()) / 2);
    } else {
        this->restoreGeometry(geometry);
    }

    ini->endGroup();

}

void MainWindow::writeSettings()
{
    QSettings ini(Utils::getConfigFilePath(), QSettings::IniFormat);
    ini.beginGroup(STR_OPTIONS);
//    ini.setValue("geometry1", this->geometry());
    ini.setValue("geometry1", this->saveGeometry());
    QSplitter * splitter=this->content->getSplitter();
    QList<QVariant> sizes{splitter->sizes()[0],splitter->sizes()[1]};
    ini.setValue("geometry2",sizes);
    ini.endGroup();
}

bool MainWindow::maybeSave()
{
    auto tab=this->content->getRightTab();
    while (tab->count()>0) {
        int idx=tab->currentIndex();
        if(!this->content->closeTab(idx)){
            return false;
        }
    }
    return true;
}
#include "compact_enc_det/compact_enc_det.h"
QueryForm* MainWindow::loadFile(QString fileName,const QByteArray codec,const bool logHis)
{
    QueryForm* qf=nullptr;

    QString targetName=QFile::symLinkTarget(fileName);
    if(targetName.length()>0){
        fileName=targetName;
    }

    bool find=false;
    for (int i=0;i<this->content->getRightTab()->count();i++) {
        auto wd=this->content->getRightTab()->widget(i);
        if(wd&&wd->inherits(QueryForm::staticMetaObject.className())){
            auto qf=static_cast<QueryForm*>(wd);
            auto te=qf->getMyEdit();
            if(te){
                if(te->fileName==fileName){
                    this->content->getRightTab()->setCurrentIndex(i);
                    find=true;
                    break;
                }
            }
        }
    }

    if(!find){
        qf=this->content->addTab("");

        content->setTabTitle(qf,fileName.section("/", -1, -1),fileName);

        QTimer::singleShot(0,this,[=](){
            qf->loadFile(fileName,codec);

            QTimer::singleShot(50,this,[=](){
                if(logHis&&fileName.length()<1024&&Utils::isFileExist(fileName)){
                    QSettings settings(QCoreApplication::applicationName());
                    QJsonArray replaceHis = settings.value("openHis", QJsonArray{}).value<QJsonArray>();
                    while(replaceHis.count()>30){
                        replaceHis.removeLast();
                    }
                    QJsonObject currJson;
                    for (int i=0;i<replaceHis.count();) {
                        QJsonObject j=replaceHis[i].toObject();
                        if(j["file"].toString()==fileName){
                            replaceHis.removeAt(i);
                            currJson=j;
                        }else{
                            i++;
                        }
                    }
                    if(fileName.length()<2048){
                        currJson["file"]=fileName;
                        if(codec.length()>0){
                            currJson["codec"]=QString{codec};
                        }
                        replaceHis.push_front(currJson);
                    }
                    settings.setValue("openHis",replaceHis);
                }
            });
        });

    }

    return qf;
}

bool MainWindow::saveFile(const QString &fileName,QByteArray saveCodec)
{

    auto qf=this->content->getCurrQueryForm();
    if(!qf){
        return false;
    }

    QFile file(fileName);
    if (!file.open(QFile::WriteOnly | QFile::Truncate)) {
        QMessageBox::warning(this, QCoreApplication::applicationName(),
                             tr("Cannot write file %1:\n%2.")
                             .arg(QDir::toNativeSeparators(fileName),
                                  file.errorString()));
        return false;
    }

    QString fTmp;
    while(true){
        int t1=10+QRandomGenerator::global()->bounded(9999);
        int t2=10+QRandomGenerator::global()->bounded(9999);
        fTmp=QString{"%1_bak%2_%3"}.arg(fileName).arg(QString::number(t1)).arg(QString::number(t2));

        if(!Utils::isFileExist(fTmp)){
            break;
        }
    }

    QFile fileTmp(fTmp);
    if (!fileTmp.open(QFile::WriteOnly | QFile::Truncate)) {
        QMessageBox::warning(this, QCoreApplication::applicationName(),tr("File save error."));
        return false;
    }

#ifndef QT_NO_CURSOR
    QGuiApplication::setOverrideCursor(Qt::WaitCursor);
#endif
    const int blockSize = 204800;

    auto te=qf->getMyEdit();

    qf->stopWatchFileChange();

    bool readOnlyOld=te->readOnly();
    te->setReadOnly(true);
    LastRun lr{[=](){

            qf->watchFileChange();

            te->setReadOnly(readOnlyOld);
               }};


    if(saveCodec.isEmpty()){

        if(te->codecName.isEmpty()){
            te->codecName="UTF-8";
        }

        saveCodec=te->codecName;
    }

    QTextCodec::ConverterState state;
    QTextCodec *codec = QTextCodec::codecForName(saveCodec);
    if(codec==nullptr){
        QMessageBox::warning(this, QCoreApplication::applicationName(),tr("File save error."));
    }else{

        std::atomic<bool> finish{false};
        bool saveSucc{false};
        std::thread th([&](){
            int64_t lengthDoc = te->length();
            const char* documentBytes=reinterpret_cast<const char *>(te->characterPointer());
            try {
                for (int64_t i = 0; i < lengthDoc; i += blockSize) {
                    int64_t grabSize = lengthDoc - i;
                    if (grabSize > blockSize)
                        grabSize = blockSize;
                    int ends=0;
                    while(true){//utf8 end char
                        int endPos=i+grabSize+ends;
                        if(endPos<lengthDoc && StringUtils::isUtf8Tail(documentBytes[endPos])){
                            ends++;
                            continue;
                        }
                        break;
                    }
                    QString str=QString::fromUtf8(documentBytes+i,grabSize+ends);
                    i+=ends;
                    QByteArray arr=codec->fromUnicode(str);
                    std::string data=arr.toStdString();
                    fileTmp.write(data.c_str(),data.length());
                }
                saveSucc=true;
            } catch (...) {
                saveSucc=false;
            }
            finish=true;
        });
        th.detach();
        while (!finish) {
            qApp->processEvents();
            std::this_thread::sleep_for(std::chrono::milliseconds(5));
        }
        if(!saveSucc){
            QMessageBox::warning(this, QCoreApplication::applicationName(),tr("File save error."));
            return false;
        }
    }

#ifndef QT_NO_CURSOR
    QGuiApplication::restoreOverrideCursor();
#endif

    if(file.remove()){

        file.close();

        fileTmp.rename(fileName);
        fileTmp.close();

    }else {
        QMessageBox::warning(this, QCoreApplication::applicationName(),tr("File save error."));
        return false;
    }


    te->setSavePoint();
    te->fileName=fileName;
    te->codecName=saveCodec;

    content->setTabTitle(qf,fileName.section("/", -1, -1),fileName,false);
    this->saveAct->setEnabled(te->modify());

    setCurrentTitle(fileName);
    statusBar()->showMessage(tr("File saved"), 3000);

    QTimer::singleShot(50,this,[=](){
        if(fileName.length()<1024&&Utils::isFileExist(fileName)){
            QSettings settings(QCoreApplication::applicationName());
            QJsonArray replaceHis = settings.value("openHis", QJsonArray{}).value<QJsonArray>();
            while(replaceHis.count()>30){
                replaceHis.removeLast();
            }
            QJsonObject currJson;
            for (int i=0;i<replaceHis.count();) {
                QJsonObject j=replaceHis[i].toObject();
                if(j["file"].toString()==fileName){
                    replaceHis.removeAt(i);
                    currJson=j;
                }else{
                    i++;
                }
            }
            if(fileName.length()<2048){
                currJson["file"]=fileName;
                if(saveCodec.length()>0){
                    currJson["codec"]=QString{saveCodec};
                }
                replaceHis.push_front(currJson);
            }
            settings.setValue("openHis",replaceHis);
        }
    });

    return true;
}

void MainWindow::setCurrentTitle(const QString &fileName)
{
    setWindowModified(false);

    QString shownName = fileName;
    if (shownName.isEmpty()){
        shownName = QCoreApplication::applicationName();
    }else{
        shownName += " - "+QCoreApplication::applicationName();
    }
    setWindowTitle(shownName);

    auto te=this->content->getCurrTextEdit();
    if(te){
        this->codecLabel.setHidden(false);
        codecLabel.setText(te->codecName);
    }else{
        codecLabel.setText("");
    }
}

QString MainWindow::strippedName(const QString &fullFileName)
{
    return QFileInfo(fullFileName).fileName();
}

QSharedPointer<QSettings> MainWindow::loadConfig(bool useDefault)
{
    QString iniName=Utils::getConfigFilePath(false);

    if(useDefault){
        iniName="";
    }

    QSharedPointer<QSettings> ini{new QSettings{iniName, QSettings::IniFormat}};

    ini->beginGroup(STR_OPTIONS);

    auto var=ini->value("language");
    if(var.isNull()){
        QLocale locale=QLocale::system();
        if(locale.language()==QLocale::Chinese&&locale.country()==QLocale::China){
            this->defaultLanguage="zh_cn";
        }else{
            this->defaultLanguage="en";

        }
    }else{

        this->defaultLanguage=var.toString().trimmed();
    }

    var=ini->value("theme");
    if(var.isNull()){
        this->defaultTheme=THEME_LIGHT;
    }else{
        QString theme=var.toString().trimmed().toLower();
        if(theme=="dark"){
            this->defaultTheme=THEME_DARK;
        }else if(theme=="green_light"){
            this->defaultTheme=THEME_GREEN_LIGHT;
        }else{
            this->defaultTheme=THEME_LIGHT;
        }
    }

    var=ini->value("qfCalcRowCout");
    if(var.isNull()){
        this->qf_defaultCalcRowCout=false;
    }else{

        this->qf_defaultCalcRowCout=var.toBool();
    }

    var=ini->value("tdCalcRowCout");
    if(var.isNull()){
        this->td_defaultCalcRowCout=false;
    }else{

        this->td_defaultCalcRowCout=var.toBool();
    }

    var=ini->value("pageSize");
    if(var.isNull()||var.toInt()<=0){
        this->defaultPageSize=1000;
    }else{

        this->defaultPageSize=var.toInt();
    }

    var=ini->value("guiFont");
    if(var.isNull()){

        this->defaultFontFamily="";//QApplication::font().defaultFamily()
    }else{
        this->defaultFontFamily=var.toString().trimmed();
    }

    int defFontSize=QApplication::font().pointSize();
    if(defFontSize<=0){
        defFontSize=12;
    }

    var=ini->value("guiFontSize");
    if(var.isNull()){

        this->defaultFontSize=defFontSize;
    }else{

        this->defaultFontSize=var.toInt();
    }
    if(this->defaultFontSize<=0){
        this->defaultFontSize=defFontSize;
    }

    var=ini->value("qfFont");
    if(var.isNull()){
        this->defaultQfFontFamily="";

    }else{
        this->defaultQfFontFamily=var.toString().trimmed();
    }

    var=ini->value("qfFontSize");
    if(var.isNull()){

        this->defaultQfFontSize=defFontSize*1.3;
    }else{

        this->defaultQfFontSize=var.toInt();
    }
    if(this->defaultQfFontSize<=0){
        this->defaultQfFontSize=defFontSize*1.5;
    }

    var=ini->value("useCodeCompletion");
    if(var.isNull()){

        this->defaultUseCodeCompletion=true;
    }else{
        this->defaultUseCodeCompletion=var.toBool();
    }

    var=ini->value("showLineNumber");
    if(var.isNull()){

        this->defaultShowLineNumber=true;
    }else{
        this->defaultShowLineNumber=var.toBool();
    }

    var=ini->value("useCodeFloding");
    if(var.isNull()){

        this->defaultUseCodeFloding=true;
    }else{
        this->defaultUseCodeFloding=var.toBool();
    }

    var=ini->value("useSyntaxHighlighting");
    if(var.isNull()){

        this->defaultUseSyntaxHighlighting=true;
    }else{
        this->defaultUseSyntaxHighlighting=var.toBool();
    }

    var=ini->value("useCurrLineHL");
    if(var.isNull()){

        this->defaultUseCurrLineHL=true;
    }else{
        this->defaultUseCurrLineHL=var.toBool();
    }

    var=ini->value("disableIfFileLargerMib");
    if(var.isNull()||var.toInt()<=0){

        this->defaultDisableIfFileLargerMib=20;
    }else{
        this->defaultDisableIfFileLargerMib=var.toInt();
    }

    var=ini->value("qfTabWidth");
    if(var.isNull()||var.toInt()<=0){

        this->defaultQfTabWidth=4;
    }else{
        this->defaultQfTabWidth=var.toInt();
    }

    var=ini->value("doLog");
    if(var.isNull()){

        LogUtils::doLog=true;
    }else{
        LogUtils::doLog=var.toBool();
    }

    var=ini->value("clearLog");
    if(var.isNull()){

        LogUtils::clearLog=true;
    }else{
        LogUtils::clearLog=var.toBool();
    }

    var=ini->value("clearLogDays");
    if(var.isNull()||var.toInt()<=0){

        LogUtils::clearLogDays=90;
    }else{
        LogUtils::clearLogDays=var.toInt();
    }

    var=ini->value("qfColorCommon");
    if(var.isNull()){

        this->defaultQfColorCommon="000000";
    }else{
        this->defaultQfColorCommon=var.toString().trimmed();
    }

    var=ini->value("qfColorKeyword");
    if(var.isNull()){

        this->defaultQfColorKeyword="22227F";
    }else{
        this->defaultQfColorKeyword=var.toString().trimmed();
    }

    var=ini->value("qfColorString");
    if(var.isNull()){

        this->defaultQfColorString="F51642";
    }else{
        this->defaultQfColorString=var.toString().trimmed();
    }

    var=ini->value("qfColorNumber");
    if(var.isNull()){

        this->defaultQfColorNumber="00b569";
    }else{
        this->defaultQfColorNumber=var.toString().trimmed();
    }

    var=ini->value("qfColorComment");
    if(var.isNull()){

        this->defaultQfColorComment="7F7F7F";
    }else{
        this->defaultQfColorComment=var.toString().trimmed();
    }

    var=ini->value("qfColorBackground");
    if(var.isNull()){

        this->defaultQfColorBackground="F9F9F9";
    }else{
        this->defaultQfColorBackground=var.toString().trimmed();
    }

    ini->endGroup();

    return ini;
}

#ifndef QT_NO_SESSIONMANAGER
void MainWindow::commitData(QSessionManager &manager)
{
    if (manager.allowsInteraction()) {
        if (!maybeSave())
            manager.cancel();
    } else {
        // Non-interactive: save without asking
        if (this->content->getCurrTextEdit()->modify())
            save();
    }
}
#endif
