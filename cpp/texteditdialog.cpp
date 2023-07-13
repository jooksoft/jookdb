#include "sqlstyle.h"
#include "texteditdialog.h"
#include "ui_texteditdialog.h"
#include "widgetutils.h"

#include <QMessageBox>
#include <QTimer>
#include <QPushButton>
#include <QSettings>
#include <QFileDialog>
#include <QScreen>

TextEditDialog::TextEditDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TextEditDialog)
{
    ui->setupUi(this);

    WidgetUtils::setSafeDeleteOnClose(this);

    Qt::WindowFlags flags=Qt::Dialog;
    flags |=Qt::WindowMinMaxButtonsHint;
    flags |=Qt::WindowCloseButtonHint;
    setWindowFlags(flags);

#if defined(Q_OS_MAC)
    this->setWindowFlag(Qt::Tool,true);
#endif

//    QScreen *screen=QGuiApplication::primaryScreen();
//    const QRect availableGeometry = screen->availableGeometry();
//    this->resize(availableGeometry.width() / 2, availableGeometry.height() / 2);
//    this->move((availableGeometry.width() - this->width()) / 2,
//         (availableGeometry.height() - this->height()) / 2);

    this->ui->buttonBox->setFocus();
}

TextEditDialog::~TextEditDialog()
{
    delete ui;
}

bool TextEditDialog::getReadOnly() const
{
    return readOnly;
}

void TextEditDialog::setReadOnly(bool value)
{
    readOnly = value;
    QPushButton *btn=this->ui->buttonBox->button(QDialogButtonBox::Ok);
    if(btn){
        btn->setEnabled(!readOnly);
    }
    this->ui->txtEdit->setReadOnly(readOnly);

}

MyEdit *TextEditDialog::getMyEdit()
{
    return this->ui->txtEdit;
}

void TextEditDialog::closeEvent(QCloseEvent *event)
{
    if (this->ui->txtEdit->modify()) {
        const int ret = QMessageBox::warning(this, QCoreApplication::applicationName(),
                                   tr("Close %1 without saving changes?").arg(this->windowTitle()),
                                   tr("Close"),tr("Cancel"),"",1);
        switch (ret) {
        case 0:
            break;
        default:
            event->ignore();
            return;
            break;
        }
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

void TextEditDialog::on_btn_file2blob_clicked()
{
    QSettings setting(QCoreApplication::applicationName());
    QString lastPath = setting.value("ExportPath").toString();

    QString filter=QObject::tr("All files(*.*);;Data file(*.dat);;Text file(*.txt);;CSV file(*.csv);;Bitmap file(*.bmp);;JPEG file(*.jpg);;GIP file(*.gif);;PNG file(*.png)");
    QFileDialog fdialog(this,QObject::tr("Load File to BLOB"),lastPath,filter);
    fdialog.setOption(QFileDialog::DontUseNativeDialog,true);
    fdialog.setWindowModality(Qt::WindowModal);
    fdialog.setAcceptMode(QFileDialog::AcceptOpen);

    if (fdialog.exec() == QDialog::Accepted && fdialog.selectedFiles().length()>0){
        QStringList fnames=fdialog.selectedFiles();
        QString lastName=fnames.last();

        if(fnames.count()>0&&!lastName.isEmpty()){
            QFile file{lastName};
            if(file.open(QIODevice::ReadOnly)){
                QByteArray ba=file.readAll();
                ba=ba.toHex();
                this->ui->txtEdit->setText(ba.constData());

                QDir dir(lastName);
                dir.cdUp();
                setting.setValue("ExportPath",dir.path());
            }
            file.close();
        }
    }
}

void TextEditDialog::on_btn_blob2file_clicked()
{
    QSettings setting(QCoreApplication::applicationName());
    QString lastPath = setting.value("ExportPath").toString();

    QString filter=QObject::tr("All files(*.*);;Data file(*.dat);;Text file(*.txt);;CSV file(*.csv);;Bitmap file(*.bmp);;JPEG file(*.jpg);;GIP file(*.gif);;PNG file(*.png)");
    QFileDialog fdialog(this,QObject::tr("Save BLOB to File"),lastPath,filter);
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

        if(fnames.count()>0&&!lastName.isEmpty()){
            QFile file{lastName};
            if(file.open(QIODevice::WriteOnly | QFile::Truncate)){

                const int64_t lengthDoc = this->ui->txtEdit->length();
                const char* documentBytes=reinterpret_cast<const char *>(this->ui->txtEdit->characterPointer());

                QByteArray ba=QByteArray::fromRawData(documentBytes,lengthDoc);

                QByteArray hex_ba=QByteArray::fromHex(ba);

                file.write(hex_ba);

                QDir dir(lastName);
                dir.cdUp();
                setting.setValue("ExportPath",dir.path());
            }
            file.close();
        }
    }
}

void TextEditDialog::on_btn_file2text_clicked()
{
    QSettings setting(QCoreApplication::applicationName());
    QString lastPath = setting.value("ExportPath").toString();

    QString filter=QObject::tr("All files(*.*);;Data file(*.dat);;Text file(*.txt);;CSV file(*.csv);;Bitmap file(*.bmp);;JPEG file(*.jpg);;GIP file(*.gif);;PNG file(*.png)");
    QFileDialog fdialog(this,QObject::tr("Load File to Text"),lastPath,filter);
    fdialog.setOption(QFileDialog::DontUseNativeDialog,true);
    fdialog.setWindowModality(Qt::WindowModal);
    fdialog.setAcceptMode(QFileDialog::AcceptOpen);

    if (fdialog.exec() == QDialog::Accepted && fdialog.selectedFiles().length()>0){
        QStringList fnames=fdialog.selectedFiles();
        QString lastName=fnames.last();

        if(fnames.count()>0&&!lastName.isEmpty()){
            QFile file{lastName};
            if(file.open(QIODevice::ReadOnly)){
                QByteArray ba=file.readAll();
                this->ui->txtEdit->setText(ba.constData());

                QDir dir(lastName);
                dir.cdUp();
                setting.setValue("ExportPath",dir.path());
            }
            file.close();
        }
    }
}

void TextEditDialog::on_btn_text2file_clicked()
{
    QSettings setting(QCoreApplication::applicationName());
    QString lastPath = setting.value("ExportPath").toString();

    QString filter=QObject::tr("All files(*.*);;Data file(*.dat);;Text file(*.txt);;CSV file(*.csv);;Bitmap file(*.bmp);;JPEG file(*.jpg);;GIP file(*.gif);;PNG file(*.png)");
    QFileDialog fdialog(this,QObject::tr("Save Text to File"),lastPath,filter);
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

        if(fnames.count()>0&&!lastName.isEmpty()){
            QFile file{lastName};
            if(file.open(QIODevice::WriteOnly | QFile::Truncate)){

                const int64_t lengthDoc = this->ui->txtEdit->length();
                const char* documentBytes=reinterpret_cast<const char *>(this->ui->txtEdit->characterPointer());

                QByteArray ba=QByteArray::fromRawData(documentBytes,lengthDoc);

                file.write(ba);

                QDir dir(lastName);
                dir.cdUp();
                setting.setValue("ExportPath",dir.path());
            }
            file.close();
        }
    }
}

void TextEditDialog::on_chk_line_wrap_stateChanged(int arg1)
{
    if(ui->chk_line_wrap->isChecked()){
        ui->txtEdit->setWrapMode(SC_WRAP_WORD);
        ui->txtEdit->setWrapVisualFlags(SC_WRAPVISUALFLAG_END);
    }else{
        ui->txtEdit->setWrapMode(SC_WRAP_NONE);
    }
}

