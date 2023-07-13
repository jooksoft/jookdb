#ifndef RUNSQLDIALOG_H
#define RUNSQLDIALOG_H

#include "utils.h"

#include <QDialog>

namespace Ui {
class RunSqlDialog;
}

class RunSqlDialog : public QDialog
{
    Q_OBJECT

public:
    explicit RunSqlDialog(QSharedPointer<ConnData> connData,QString dbName,QWidget *parent = nullptr);

private slots:
    void on_btn_close_clicked();

    void on_btn_detect_clicked();

    void on_btn_start_clicked();

private:
    ~RunSqlDialog();

    Ui::RunSqlDialog *ui;
    QSharedPointer<ConnData> connData;

public:

    void closeEvent(QCloseEvent *event) override;

private:

    std::atomic_bool stop=false;

};

#endif // RUNSQLDIALOG_H
