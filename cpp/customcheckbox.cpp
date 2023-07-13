#include "customcheckbox.h"
#include "widgetutils.h"

#include <QHBoxLayout>
#include <QTimer>

CustomCheckBox::CustomCheckBox(QWidget *parent) : QWidget(parent)
{
    WidgetUtils::setSafeDeleteOnClose(this);

    this->setLayout(new QHBoxLayout(this));
    this->layout()->setAlignment(Qt::AlignCenter);
    this->layout()->setContentsMargins(0,0,0,0);
//    this->setAutoFillBackground(true);
    chkBox=new QCheckBox(this);
    this->layout()->addWidget(chkBox);

    CustomEventFilter *eventFilter=new CustomEventFilter{chkBox};
    eventFilter->setFunc([=](QObject *obj, QEvent *event){
        switch (event->type())
        {
        case QEvent::MouseButtonRelease:{
                QTimer::singleShot(0,this,[=](){
                    this->mouseReleaseEvent((QMouseEvent*)event);
                });
                return true;
            }
            break;
        }
        return false;
    });
    chkBox->installEventFilter(eventFilter);
}

void CustomCheckBox::setOldChecked(bool checked)
{
    this->old_chked=checked;
}

bool CustomCheckBox::getOldChecked()
{
    return this->old_chked;
}

void CustomCheckBox::setChecked(bool checked)
{
    if(readonly)return;

    chkBox->setChecked(checked);
    emit this->checked(checked);
}

bool CustomCheckBox::getChecked()
{
    return chkBox->isChecked();
}

void CustomCheckBox::setText(const QString &text)
{
    if(readonly)return;
    chkBox->setText(text);
}

QString CustomCheckBox::getText() const
{
    return chkBox->text();
}

void CustomCheckBox::setOldText(const QString &text)
{
    this->old_text=text;
}

QString CustomCheckBox::getOldText() const
{
    return old_text;
}

void CustomCheckBox::mouseReleaseEvent(QMouseEvent *event)
{
    if(!this->ignoreMouseEvent){
        bool chked=this->getChecked();
        this->setChecked(!chked);
    }
    emit this->mouseRelease(this->getChecked());
}

bool CustomCheckBox::getReadonly() const
{
    return readonly;
}

void CustomCheckBox::setReadonly(bool value)
{
    readonly = value;
    chkBox->setDisabled(readonly);
}

bool CustomCheckBox::hasChanged()
{
    return ((chkBox->isChecked()!=this->old_chked)||(!StringUtils::equalIncase(chkBox->text(),this->old_text)));
}

bool CustomCheckBox::getIgnoreMouseEvent() const
{
    return ignoreMouseEvent;
}

void CustomCheckBox::setIgnoreMouseEvent(bool value)
{
    ignoreMouseEvent = value;
}

void CustomCheckBox::closeEvent(QCloseEvent *event)
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
