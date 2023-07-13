#include "mainwindow.h"
#include "optionsdialog.h"
#include "ui_optionsdialog.h"
#include "widgetutils.h"
#include "globalutils.h"

#include <QColorDialog>
#include <QFontDatabase>
#include <QMessageBox>
#include <QSettings>
#include <QTimer>

OptionsDialog::OptionsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::OptionsDialog)
{
    ui->setupUi(this);

    WidgetUtils::setSafeDeleteOnClose(this);

//    QFontDatabase fontDatabase;

    WidgetUtils::customComboBoxWidget(this->ui->cbox_guiFont);

    WidgetUtils::customComboBoxWidget(this->ui->cbox_editorFont);

    WidgetUtils::customComboBoxWidget(this->ui->cbox_records);

    this->ui->cbox_records->setValidator(new QIntValidator{this});

    loadConfig();

    this->ui->btn_apply->setEnabled(false);
    this->ui->btn_ok->setEnabled(false);

    ui->tabWidget->setCurrentIndex(0);

    auto ch=this->findChildren<QLineEdit*>();
    for(auto c:ch){
        if(!c->parent()->inherits(QComboBox::staticMetaObject.className())){

            connect(c,&QLineEdit::textChanged,this,[=](QString txt){
                QLineEdit *le=qobject_cast<QLineEdit *>(sender());

                this->setModify();

            },Qt::UniqueConnection);
        }
    }
    auto ch2=this->findChildren<QComboBox*>();
    for(auto c:ch2){
        connect(c,&QComboBox::currentTextChanged,this,[=](QString txt){

            this->setModify();

        },Qt::UniqueConnection);
    }
    auto ch3=this->findChildren<QCheckBox*>();
    for(auto c:ch3){
        connect(c,&QCheckBox::clicked,[=](){

            this->setModify();
        });
    }
    auto ch4=this->findChildren<QSpinBox*>();
    for(auto c:ch4){
        connect(c,static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged),[=](int val){

            this->setModify();
        });
    }
}

void OptionsDialog::loadConfig()
{

    int idx=this->ui->cbox_language->findText(MainWindow::instance()->defaultLanguage+":",Qt::MatchStartsWith);
    if(idx>=0){
        this->ui->cbox_language->setCurrentIndex(idx);
    }
    if(MainWindow::instance()->defaultTheme==THEME_DARK){
        WidgetUtils::setOrAddCurrItem(this->ui->cbox_theme,"dark",false);
    }else if(MainWindow::instance()->defaultTheme==THEME_GREEN_LIGHT){
        WidgetUtils::setOrAddCurrItem(this->ui->cbox_theme,"green_light",false);
    }else{
        WidgetUtils::setOrAddCurrItem(this->ui->cbox_theme,"light",false);
    }

    if(MainWindow::instance()->defaultFontFamily.length()>0){
        this->ui->cbox_guiFont->setCurrentFont(QFont{MainWindow::instance()->defaultFontFamily});
    }else{
        this->ui->cbox_guiFont->setCurrentText(MainWindow::instance()->defaultFontFamily);
    }

    this->ui->spin_guiFontSize->setValue(MainWindow::instance()->defaultFontSize);

    this->ui->chk_autoCalcRowCount->setChecked(MainWindow::instance()->qf_defaultCalcRowCout);
    this->ui->chk_autoCalcRowCount_td->setChecked(MainWindow::instance()->td_defaultCalcRowCout);
    this->ui->chk_codeCompetion->setChecked(MainWindow::instance()->defaultUseCodeCompletion);
    this->ui->chk_codeCompetion->setChecked(MainWindow::instance()->defaultUseCodeCompletion);
    this->ui->chk_lineNumber->setChecked(MainWindow::instance()->defaultShowLineNumber);
    this->ui->chk_codefold->setChecked(MainWindow::instance()->defaultUseCodeFloding);
    this->ui->chk_currLineHL->setChecked(MainWindow::instance()->defaultUseCurrLineHL);
    this->ui->chk_useHL->setChecked(MainWindow::instance()->defaultUseSyntaxHighlighting);
    this->ui->spin_disableHL->setValue(MainWindow::instance()->defaultDisableIfFileLargerMib);
    if(MainWindow::instance()->defaultQfFontFamily.length()>0){
        this->ui->cbox_editorFont->setCurrentFont(QFont{MainWindow::instance()->defaultQfFontFamily});
    }else{
        this->ui->cbox_editorFont->setCurrentText(MainWindow::instance()->defaultQfFontFamily);
    }
    this->ui->spin_editorfontSize->setValue(MainWindow::instance()->defaultQfFontSize);
    this->ui->spin_tabWidth->setValue(MainWindow::instance()->defaultQfTabWidth);
    this->ui->cbox_records->setCurrentText(QString::number(MainWindow::instance()->defaultPageSize));

    this->ui->chk_doLog->setChecked(LogUtils::doLog);
    this->ui->chk_clearLog->setChecked(LogUtils::clearLog);
    this->ui->spin_clearLogDays->setValue(LogUtils::clearLogDays);

    this->defaultQfColorCommon=MainWindow::instance()->defaultQfColorCommon;
    this->defaultQfColorKeyword=MainWindow::instance()->defaultQfColorKeyword;
    this->defaultQfColorString=MainWindow::instance()->defaultQfColorString;
    this->defaultQfColorNumber=MainWindow::instance()->defaultQfColorNumber;
    this->defaultQfColorComment=MainWindow::instance()->defaultQfColorComment;
    this->defaultQfColorBackground=MainWindow::instance()->defaultQfColorBackground;

    this->flushColor();
}

void OptionsDialog::setModify()
{
    this->modify=true;
    this->ui->btn_apply->setEnabled(true);
    this->ui->btn_ok->setEnabled(true);
}

OptionsDialog::~OptionsDialog()
{
    delete ui;
}

void OptionsDialog::closeEvent(QCloseEvent *event)
{
    MainWindow::instance()->loadConfig(false);

    if(MainWindow::instance()->defaultQfFontFamily.isEmpty()){
        MainWindow::instance()->defaultQfFontFamily=MainWindow::instance()->getAppQfFontFamily();
    }

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

void OptionsDialog::flushColor()
{

    this->ui->btn_qfColorCommon->setFixedWidth(this->ui->btn_qfColorCommon->height());
    this->ui->btn_qfColorCommon->setStyleSheet(QString{"QToolButton{background-color:#%1}"}.arg(this->defaultQfColorCommon));

    this->ui->btn_qfColorKeyword->setFixedWidth(this->ui->btn_qfColorKeyword->height());
    this->ui->btn_qfColorKeyword->setStyleSheet(QString{"QToolButton{background-color:#%1}"}.arg(this->defaultQfColorKeyword));

    this->ui->btn_qfColorString->setFixedWidth(this->ui->btn_qfColorString->height());
    this->ui->btn_qfColorString->setStyleSheet(QString{"QToolButton{background-color:#%1}"}.arg(this->defaultQfColorString));

    this->ui->btn_qfColorNumber->setFixedWidth(this->ui->btn_qfColorNumber->height());
    this->ui->btn_qfColorNumber->setStyleSheet(QString{"QToolButton{background-color:#%1}"}.arg(this->defaultQfColorNumber));

    this->ui->btn_qfColorComment->setFixedWidth(this->ui->btn_qfColorComment->height());
    this->ui->btn_qfColorComment->setStyleSheet(QString{"QToolButton{background-color:#%1}"}.arg(this->defaultQfColorComment));

    this->ui->btn_qfColorBackground->setFixedWidth(this->ui->btn_qfColorBackground->height());
    this->ui->btn_qfColorBackground->setStyleSheet(QString{"QToolButton{background-color:#%1}"}.arg(this->defaultQfColorBackground));

}


void OptionsDialog::on_btn_cancel_clicked()
{
    this->close();
}

void OptionsDialog::on_btn_apply_clicked()
{
    this->ui->btn_apply->setEnabled(false);
    this->ui->btn_ok->setEnabled(false);

    QSettings ini(Utils::getConfigFilePath(), QSettings::IniFormat);
    QStringList groupList = ini.childGroups();
    ini.beginGroup(STR_OPTIONS);

    QString str=this->ui->cbox_language->currentText();
    str=str.left(str.indexOf(':'));
    ini.setValue("language",str);
    if(MainWindow::instance()->defaultLanguage!=str){
        QTimer::singleShot(10,this,[=](){
            QMessageBox::about(MainWindow::instance(),QCoreApplication::applicationName(),
                                 tr("The language change will take effect after restart"));
        });
    }
    ini.setValue("theme",this->ui->cbox_theme->currentText());
    ini.setValue("guiFont",this->ui->cbox_guiFont->currentText());
    ini.setValue("guiFontSize",this->ui->spin_guiFontSize->value());
    ini.setValue("pageSize",this->ui->cbox_records->currentText());

    ini.setValue("qfCalcRowCout",this->ui->chk_autoCalcRowCount->isChecked());
    ini.setValue("tdCalcRowCout",this->ui->chk_autoCalcRowCount_td->isChecked());
    ini.setValue("useCodeCompletion",this->ui->chk_codeCompetion->isChecked());
    ini.setValue("showLineNumber",this->ui->chk_lineNumber->isChecked());
    ini.setValue("useCodeFloding",this->ui->chk_codefold->isChecked());
    ini.setValue("useCurrLineHL",this->ui->chk_currLineHL->isChecked());
    ini.setValue("useSyntaxHighlighting",this->ui->chk_useHL->isChecked());
    ini.setValue("disableIfFileLargerMib",this->ui->spin_disableHL->value());
    ini.setValue("qfFont",this->ui->cbox_editorFont->currentText());
    ini.setValue("qfFontSize",this->ui->spin_editorfontSize->value());
    ini.setValue("qfTabWidth",this->ui->spin_tabWidth->value());

    ini.setValue("doLog",this->ui->chk_doLog->isChecked());
    ini.setValue("clearLog",this->ui->chk_clearLog->isChecked());
    ini.setValue("clearLogDays",this->ui->spin_clearLogDays->value());

    ini.setValue("qfColorCommon",this->defaultQfColorCommon);
    ini.setValue("qfColorKeyword",this->defaultQfColorKeyword);
    ini.setValue("qfColorString",this->defaultQfColorString);
    ini.setValue("qfColorNumber",this->defaultQfColorNumber);
    ini.setValue("qfColorComment",this->defaultQfColorComment);
    ini.setValue("qfColorBackground",this->defaultQfColorBackground);

    ini.endGroup();

    ini.sync();

    MainWindow::instance()->loadConfig();

    if(MainWindow::instance()->defaultQfFontFamily.isEmpty()){
        MainWindow::instance()->defaultQfFontFamily=MainWindow::instance()->getAppQfFontFamily();
    }

    MainWindow::instance()->loadQSS();

    LogUtils::doLog=this->ui->chk_doLog->isChecked();
    LogUtils::clearLog=this->ui->chk_clearLog->isChecked();
    LogUtils::clearLogDays=this->ui->spin_clearLogDays->value();

    //更新现有控件样式
//    MainWindow::getContent()->setStyleSheet(MainWindow::getContent()->styleSheet());
//    MainWindow::getContent()->getLeftTree()->setStyleSheet(MainWindow::getContent()->getLeftTree()->styleSheet());
//    MainWindow::getContent()->getRightTab()->setStyleSheet(MainWindow::getContent()->getRightTab()->styleSheet());
    this->setStyleSheet(this->styleSheet());
    auto Widgets=MainWindow::instance()->findChildren<QWidget*>(QString{}, Qt::FindDirectChildrenOnly);
    for(auto &wid:Widgets){
        if(wid){
            wid->setStyleSheet(wid->styleSheet());
        }
    }

    auto tabs=MainWindow::getContent()->getRightTab();
    for (int i=0;i<tabs->count();i++) {
        auto wid=tabs->widget(i);
        if(wid){
            if(wid->inherits(QueryForm::staticMetaObject.className())){
                auto qf=static_cast<QueryForm*>(wid);

                qf->getMyEdit()->useLineNumber=MainWindow::instance()->defaultShowLineNumber;
                qf->getMyEdit()->useLexer=MainWindow::instance()->defaultUseSyntaxHighlighting;
                qf->getMyEdit()->useBraceHighlighting=true;
                qf->getMyEdit()->useCurrLineHL=MainWindow::instance()->defaultUseCurrLineHL;
                qf->getMyEdit()->setUseAutoComplete(MainWindow::instance()->defaultUseCodeCompletion);

                SqlStyle::setStyle(qf->getMyEdit());
            }
            wid->setStyleSheet(wid->styleSheet());
        }
    }
}

void OptionsDialog::on_btn_ok_clicked()
{
    this->ui->btn_apply->setEnabled(false);
    this->ui->btn_ok->setEnabled(false);

    this->on_btn_apply_clicked();
    this->on_btn_cancel_clicked();
}

void OptionsDialog::on_btn_restore_clicked()
{
    MainWindow::instance()->loadConfig(true);

    if(MainWindow::instance()->defaultQfFontFamily.isEmpty()){
        MainWindow::instance()->defaultQfFontFamily=MainWindow::instance()->getAppQfFontFamily();
    }

    this->loadConfig();

    this->setModify();
}

void OptionsDialog::on_btn_qfColorCommon_clicked()
{
    QColorDialog colorDialog{this};
    QColor oldC{this->defaultQfColorCommon.toUInt(nullptr,16)};
    QColor c = colorDialog.getColor(oldC,this);
    if(!c.isValid())return;
    this->defaultQfColorCommon=c.name().replace("#","");
    this->ui->btn_qfColorCommon->setStyleSheet(QString{"QToolButton{background-color:#%1}"}.arg(this->defaultQfColorCommon));

    this->setModify();
}

void OptionsDialog::on_btn_qfColorKeyword_clicked()
{
    QColorDialog colorDialog{this};
    QColor oldC{this->defaultQfColorKeyword.toUInt(nullptr,16)};
    QColor c = colorDialog.getColor(oldC,this);
    if(!c.isValid())return;
    this->defaultQfColorKeyword=c.name().replace("#","");
    this->ui->btn_qfColorKeyword->setStyleSheet(QString{"QToolButton{background-color:#%1}"}.arg(this->defaultQfColorKeyword));

    this->setModify();
}

void OptionsDialog::on_btn_qfColorString_clicked()
{
    QColorDialog colorDialog{this};
    QColor oldC{this->defaultQfColorString.toUInt(nullptr,16)};
    QColor c = colorDialog.getColor(oldC,this);
    if(!c.isValid())return;
    this->defaultQfColorString=c.name().replace("#","");
    this->ui->btn_qfColorString->setStyleSheet(QString{"QToolButton{background-color:#%1}"}.arg(this->defaultQfColorString));

    this->setModify();
}

void OptionsDialog::on_btn_qfColorNumber_clicked()
{
    QColorDialog colorDialog{this};
    QColor oldC{this->defaultQfColorNumber.toUInt(nullptr,16)};
    QColor c = colorDialog.getColor(oldC,this);
    if(!c.isValid())return;
    this->defaultQfColorNumber=c.name().replace("#","");
    this->ui->btn_qfColorNumber->setStyleSheet(QString{"QToolButton{background-color:#%1}"}.arg(this->defaultQfColorNumber));

    this->setModify();
}

void OptionsDialog::on_btn_qfColorComment_clicked()
{
    QColorDialog colorDialog{this};
    QColor oldC{this->defaultQfColorComment.toUInt(nullptr,16)};
    QColor c = colorDialog.getColor(oldC,this);
    if(!c.isValid())return;
    this->defaultQfColorComment=c.name().replace("#","");
    this->ui->btn_qfColorComment->setStyleSheet(QString{"QToolButton{background-color:#%1}"}.arg(this->defaultQfColorComment));

    this->setModify();
}

void OptionsDialog::on_btn_qfColorBackground_clicked()
{
    QColorDialog colorDialog{this};
    QColor oldC{this->defaultQfColorBackground.toUInt(nullptr,16)};
    QColor c = colorDialog.getColor(oldC,this);
    if(!c.isValid())return;
    this->defaultQfColorBackground=c.name().replace("#","");
    this->ui->btn_qfColorBackground->setStyleSheet(QString{"QToolButton{background-color:#%1}"}.arg(this->defaultQfColorBackground));

    this->setModify();
}

void OptionsDialog::on_cbox_theme_currentTextChanged(const QString &arg1)
{
    QString theme=arg1.trimmed().toLower();
    if(theme=="dark"){

        this->defaultQfColorCommon="a7b5c1";
        this->defaultQfColorKeyword="62a3c1";
        this->defaultQfColorString="c14f6e";
        this->defaultQfColorNumber="88c14e";
        this->defaultQfColorComment="808080";
        this->defaultQfColorBackground="2B2B2B";
    }else if(theme=="green_light"){

        this->defaultQfColorCommon="000000";
        this->defaultQfColorKeyword="00007F";
        this->defaultQfColorString="FF1744";
        this->defaultQfColorNumber="009f5f";
        this->defaultQfColorComment="7F7F7F";
        this->defaultQfColorBackground="D0F5D5";
    }else{

        this->defaultQfColorCommon="000000";
        this->defaultQfColorKeyword="22227F";
        this->defaultQfColorString="F51642";
        this->defaultQfColorNumber="00b569";
        this->defaultQfColorComment="7F7F7F";
        this->defaultQfColorBackground="F9F9F9";
    }
    this->flushColor();
}
