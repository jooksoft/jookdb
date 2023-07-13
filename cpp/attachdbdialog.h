#ifndef ATTACHDBDIALOG_H
#define ATTACHDBDIALOG_H

#include "utils.h"

#include <QDialog>

namespace Ui {
class AttachDbDialog;
}

class AttachDbDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AttachDbDialog(QSharedPointer<ConnData> connData,QWidget *parent = nullptr);
    ~AttachDbDialog();

    bool getSaveCfg() const;
    void setSaveCfg(bool value);

    QString getFile();
    QString getName();
    QString getPassword();
    bool getEncrypted();

private slots:
    void on_ch_encrypted_stateChanged(int arg1);

    void on_btn_cancel_clicked();

    void on_btn_ok_clicked();

private:
    Ui::AttachDbDialog *ui;

    QSharedPointer<ConnData> connData=nullptr;

    bool saveCfg=false;
};

#endif // ATTACHDBDIALOG_H
