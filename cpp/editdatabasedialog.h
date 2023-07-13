#ifndef EDITDATABASEDIALOG_H
#define EDITDATABASEDIALOG_H

#include "utils.h"

#include <QDialog>

namespace Ui {
class EditDatabaseDialog;
}

enum class EditDatabaseType{
    newDatabase,
    editDatabase,
    viewDatabase
};

class EditDatabaseDialog : public QDialog
{
    Q_OBJECT

public:
    explicit EditDatabaseDialog(QSharedPointer<ConnData> connData, const QString &dbName,EditDatabaseType edtype,QWidget *parent = nullptr);

    QString getUpdateSql();

private slots:
    void on_cbox_charset_currentTextChanged(const QString &arg1);

    void on_btn_cancel_clicked();

    void on_btn_ok_clicked();

private:
    Ui::EditDatabaseDialog *ui;

    ~EditDatabaseDialog();

    QSharedPointer<ConnData> connData;
    QString dbName;
    EditDatabaseType edtype;

public:

    void closeEvent(QCloseEvent *event) override;

private:

    std::atomic_bool stop=false;

};

#endif // EDITDATABASEDIALOG_H
