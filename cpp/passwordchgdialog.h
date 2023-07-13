#ifndef PASSWORDCHGDIALOG_H
#define PASSWORDCHGDIALOG_H

#include <QDialog>

namespace Ui {
class PasswordChgDialog;
}

class PasswordChgDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PasswordChgDialog(QWidget *parent = nullptr);
    ~PasswordChgDialog();

    QString getOldPassword() const;
    QString getNewPassword() const;

private slots:
    void on_btn_ok_clicked();

    void on_btn_cancel_clicked();

private:
    Ui::PasswordChgDialog *ui;
};

#endif // PASSWORDCHGDIALOG_H
