#ifndef DRIVERDOWNLOADFORM_H
#define DRIVERDOWNLOADFORM_H

#include "utils.h"

#include <QDialog>
#include <QJsonObject>
#include <QWidget>

namespace Ui {
class DriverDownloadForm;
}

class DriverDownloadForm : public QDialog
{
    Q_OBJECT

public:
    explicit DriverDownloadForm(QJsonObject res,QWidget *parent = nullptr);
    ~DriverDownloadForm();

private:
    Ui::DriverDownloadForm *ui;

    QSharedPointer<ConnData> connData{new ConnData};

public:

    void closeEvent(QCloseEvent *event) override;

private slots:
    void on_btn_download_clicked();

    void on_btn_cancel_clicked();

    void on_btn_open_clicked();

private:

    std::atomic_bool stop=false;
};

#endif // DRIVERDOWNLOADFORM_H
