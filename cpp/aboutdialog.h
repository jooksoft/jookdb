#ifndef ABOUTDIALOG_H
#define ABOUTDIALOG_H

#include <QDialog>

namespace Ui {
class AboutDialog;
}

class AboutDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AboutDialog(QWidget *parent = nullptr);
    ~AboutDialog();

    void testLicense();

    void checkNewVersion();

private:
    Ui::AboutDialog *ui;

public:

    void closeEvent(QCloseEvent *event) override;

private slots:
    void on_btn_register_clicked();

    void on_btn_ok_clicked();

    void on_btn_download_clicked();

    void on_btn_cancel_clicked();

private:

    std::atomic_bool stop=false;
};

#endif // ABOUTDIALOG_H
