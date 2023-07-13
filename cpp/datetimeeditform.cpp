#include "datetimeeditform.h"
#include "ui_datetimeeditform.h"
#include "utils.h"
#include "mainwindow.h"
#include "widgetutils.h"

#include <QTimer>
#include <QToolButton>

DateTimeEditForm::DateTimeEditForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DateTimeEditForm)
{
    ui->setupUi(this);

    WidgetUtils::setSafeDeleteOnClose(this);

    setWindowFlag(Qt::Popup);

#if defined(Q_OS_MAC)
    this->ui->calendarWidget->setStyle(WidgetUtils::getFusionStyle());//Macos默认风格日历等控件后台更改不刷新，所以这里用Fusion
    this->ui->timeEdit->setStyle(WidgetUtils::getFusionStyle());
    this->setStyle(WidgetUtils::getFusionStyle());
#endif

    this->ui->timeEdit->setDisplayFormat("hh:mm:ss");
    this->ui->timeEdit->setWrapping(true);

    this->ui->timeEdit->setStyleSheet("border: 1px solid #ADADAD");

    this->ui->le_microTime->setMaximumWidth(Utils::getCharGuiFontRect().width()*14);

//    QRegularExpression regx("^[0-9]*$");
//    QValidator *validator = new QRegularExpressionValidator(regx, this );
//    this->ui->le_microTime->setValidator( validator );

    this->ui->le_microTime->setStyleSheet("border: 1px solid #ADADAD");

    {
        QToolButton *tools =this->ui->calendarWidget->findChild<QToolButton*>(QLatin1String("qt_calendar_prevmonth"));
        if(tools){
            QIcon i(":/images/dark/calendar_prevmonth.svg");
            tools->setIcon(i);
        }
    }
    {
        QToolButton *tools=this->ui->calendarWidget->findChild<QToolButton*>(QLatin1String("qt_calendar_nextmonth"));
        if(tools){
            QIcon i(":/images/dark/calendar_nextmonth.svg");
            tools->setIcon(i);
        }
    }

}

DateTimeEditForm::~DateTimeEditForm()
{
    delete ui;
}

bool DateTimeEditForm::getReadonly() const
{
    return readonly;
}

void DateTimeEditForm::setReadonly(bool value)
{
    readonly = value;

    this->ui->btn_ok->setEnabled(!readonly);
}

void DateTimeEditForm::on_btn_cancel_clicked()
{
    this->okExit=false;
    
    QWidget::close();
}

QString DateTimeEditForm::getDateTime() const
{
    if(!this->okExit){
        return this->dateTime;
    }

    QString currDate=this->ui->calendarWidget->selectedDate().toString("yyyy-MM-dd");

    QString curTime=this->ui->timeEdit->text();
    if(this->ui->le_microTime->text().trimmed().length()>0){
        curTime+="."+this->ui->le_microTime->text().trimmed();
    }

    return currDate+" "+curTime;
}

void DateTimeEditForm::setDateTime(const QString &value)
{
    this->dateTime = value.trimmed();

    QStringList list=value.split('T');
    if(list.size()<2){
        list=value.split(' ');
    }
    if(list.size()>=2){
        this->date=list.first();
        list.removeFirst();
        this->time=list.join(' ');
    }else{
        this->date=value;
        this->time="";
    }

    this->ui->calendarWidget->setSelectedDate(QDate::fromString(this->date,"yyyy-MM-dd"));

    this->ui->calendarWidget->setFocus();

    int findIdx=time.indexOf('.');
    if(findIdx>2){
        QString t=this->time.left(findIdx);
        this->ui->timeEdit->setTime(QTime::fromString(t,"hh:mm:ss"));

        QString microTime=time.right(time.length()-findIdx-1);
        this->ui->le_microTime->setText(microTime);
    }else{
        this->ui->timeEdit->setTime(QTime::fromString(this->time,"hh:mm:ss"));
    }

    this->ui->calendarWidget->show();
    this->ui->wid_timeEdit->show();
}


QString DateTimeEditForm::getTime() const
{
    if(!this->okExit){
        return this->time;
    }

    QString curTime=this->ui->timeEdit->text();
    if(this->ui->le_microTime->text().trimmed().length()>0){
        curTime+="."+this->ui->le_microTime->text().trimmed();
    }
    return curTime;
}

void DateTimeEditForm::setTime(const QString &value)
{
    time = value.trimmed();

    int findIdx=time.indexOf('.');
    if(findIdx>2){
        QString t=this->time.left(findIdx);
        this->ui->timeEdit->setTime(QTime::fromString(t,"hh:mm:ss"));

        QString microTime=time.right(time.length()-findIdx-1);
        this->ui->le_microTime->setText(microTime);
    }else{
        this->ui->timeEdit->setTime(QTime::fromString(this->time,"hh:mm:ss"));
    }

    this->ui->calendarWidget->hide();
    this->ui->wid_timeEdit->show();

    this->resize(this->width(),1);

}

QString DateTimeEditForm::getDate() const
{
    if(!this->okExit){
        return this->date;
    }

    QString currDate=this->ui->calendarWidget->selectedDate().toString("yyyy-MM-dd");
    return currDate;
}

void DateTimeEditForm::setDate(const QString &value)
{
    date = value;

    this->ui->calendarWidget->setSelectedDate(QDate::fromString(this->date,"yyyy-MM-dd"));

    this->ui->calendarWidget->setFocus();

    this->ui->calendarWidget->show();
    this->ui->wid_timeEdit->hide();

    this->resize(this->width(),1);
}

void DateTimeEditForm::on_btn_ok_clicked()
{
    this->okExit=true;

    QWidget::close();

}

void DateTimeEditForm::closeEvent(QCloseEvent *event)
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

void DateTimeEditForm::waitExit()
{
    auto runRef=WidgetUtils::createRunRef(this);

    this->okExit=false;

    QSharedPointer<EventLoop> loop{new EventLoop{this}};

    connect(this,&DateTimeEditForm::editClosed,[=](){loop->quit();});

    loop->exec();
}

void DateTimeEditForm::on_btn_now_clicked()
{
    this->ui->calendarWidget->setSelectedDate(QDate::currentDate());
    this->ui->timeEdit->setTime(QTime::currentTime());
}
