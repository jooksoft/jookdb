#include "cmask.h"
#include "mainwindow.h"
#include "utils.h"

#include <QHBoxLayout>
#include <QMovie>
#include <QPainter>
#include <QStyleOption>
#include <QToolButton>

CMask::CMask(QWidget *parent) : QWidget(parent)
{

    this->setAttribute(Qt::WA_TransparentForMouseEvents,false);

    if(parent){
        this->setGeometry(parent->geometry());
        CustomEventFilter *resizeFilter=new CustomEventFilter{this};
        resizeFilter->setFunc([=](QObject *obj, QEvent *event){
            if (event->type() == QEvent::Resize)
            {
                this->setGeometry(parent->geometry());
            }
            return false;
        });
        parent->installEventFilter(resizeFilter);
    }else{
        this->setGeometry(MainWindow::instance()->geometry());
    }

    this->infoLabel =new QLabel{this};
    this->infoLabel->setAlignment(Qt::AlignCenter);

    QWidget *loadingWid=new QWidget{this};

    QHBoxLayout *mainLayout = new QHBoxLayout{};
    mainLayout->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(loadingWid);
    this->setLayout(mainLayout);

    this->btn=new QToolButton{this};
    QHBoxLayout *btnLayout = new QHBoxLayout{};
    btnLayout->setAlignment(Qt::AlignCenter);
    this->btn->setText("Stop");
    this->btn->setVisible(false);
    btnLayout->addWidget(this->btn);

    QVBoxLayout *loadingLayout = new QVBoxLayout{};
    loadingLayout->setAlignment(Qt::AlignCenter);
    loadingLayout->setContentsMargins(0, 0, 0, 0);
    loadingLayout->addWidget(this->infoLabel);
    loadingLayout->addLayout(btnLayout);
    loadingWid->setLayout(loadingLayout);

    loadingWid->setAutoFillBackground(true);

    this->setGif(":/images/loading.gif");

}

CMask::~CMask()
{
}

void CMask::setGif(QString strPath)
{
    QMovie *movie = new QMovie{this};
    movie->setFileName(strPath);
    movie->setScaledSize(QSize(Utils::getCharGuiFontRect().height()*4,Utils::getCharGuiFontRect().height()*4));
    this->infoLabel->setMovie(movie);
    movie->start();
}

void CMask::setStopFunction(std::function<void ()> stopFunc)
{
    this->stopFunc=stopFunc;
    if(this->stopFunc){
        this->btn->setVisible(true);

        connect(this->btn,&QToolButton::clicked,this,[=](){
            this->btn->setEnabled(false);
            this->stopFunc();
            this->btn->setEnabled(true);
        },Qt::UniqueConnection);

    }else{

        this->btn->setVisible(false);
    }
}

void CMask::paintEvent(QPaintEvent *e)
{
    QPainter p(this);
    p.setPen(Qt::NoPen);
    p.setBrush(QColor{0,0,0,50});
    p.drawRect(rect());
}
