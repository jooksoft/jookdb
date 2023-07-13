#ifndef DBOBJECTSELECTDIALOG_H
#define DBOBJECTSELECTDIALOG_H

#include <QDialog>
#include "mytreewidget.h"

namespace Ui {
class DbObjectSelectDialog;
}

class DbObjectSelectDialog : public QDialog
{
    Q_OBJECT

public:
    explicit DbObjectSelectDialog(QSharedPointer<ConnData> connData,QWidget *parent = nullptr);

private:
    ~DbObjectSelectDialog();
    Ui::DbObjectSelectDialog *ui;

    QSharedPointer<ConnData> connData;

    bool isOK=false;


public:

    void closeEvent(QCloseEvent *event) override;

    bool getIsOK() const;

    MyTreeWidget * getTree();

    QStringList getSelect();

private slots:
    void on_btn_ok_clicked();

    void on_btn_cancel_clicked();

    void on_le_filter_textChanged(const QString &arg1);

    void on_le_filter_returnPressed();

    void on_btn_collapse_clicked();

private:

    std::atomic_bool stop=false;

};

#endif // DBOBJECTSELECTDIALOG_H
