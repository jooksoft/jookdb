#ifndef COPYTABLEDIALOG_H
#define COPYTABLEDIALOG_H

#include "utils.h"

#include <QDialog>

namespace Ui {
class CopyTableDialog;
}

class CopyTableDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CopyTableDialog(QSharedPointer<ConnData> connData,QString dbName,QString tableName,bool withData,QWidget *parent = nullptr);

private slots:
    void on_btn_cancel_clicked();

    void on_btn_ok_clicked();

private:
    Ui::CopyTableDialog *ui;

    ~CopyTableDialog();

    QSharedPointer<ConnData> connData;
    QString dbName;
    QString tableName;

public:

    void closeEvent(QCloseEvent *event) override;

private:

    std::atomic_bool stop=false;

};

#endif // COPYTABLEDIALOG_H
