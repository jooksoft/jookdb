#ifndef PASSWORDINPUTDIALOG_H
#define PASSWORDINPUTDIALOG_H

#include "utils.h"

#include <QDialog>
#include <QLabel>

namespace Ui {
class PasswordInputDialog;
}

class PasswordInputDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PasswordInputDialog(QSharedPointer<ConnData> connData,QWidget *parent = nullptr);
    ~PasswordInputDialog();

    QString getPassword();
    bool isSave();

    QLabel * getPwdLabel();

private slots:

private:
    Ui::PasswordInputDialog *ui;

    QSharedPointer<ConnData> connData;

};

#endif // PASSWORDINPUTDIALOG_H
