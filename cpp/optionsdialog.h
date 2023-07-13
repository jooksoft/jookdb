#ifndef OPTIONSDIALOG_H
#define OPTIONSDIALOG_H

#include <QDialog>

namespace Ui {
class OptionsDialog;
}

class OptionsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit OptionsDialog(QWidget *parent = nullptr);

    void loadConfig();

    void setModify();

private:
    Ui::OptionsDialog *ui;

    ~OptionsDialog();

    QString defaultQfColorCommon;
    QString defaultQfColorKeyword;
    QString defaultQfColorString;
    QString defaultQfColorNumber;
    QString defaultQfColorComment;
    QString defaultQfColorBackground;

    bool modify=false;

public:

    void closeEvent(QCloseEvent *event) override;

    void flushColor();

private slots:

    void on_btn_cancel_clicked();

    void on_btn_apply_clicked();

    void on_btn_ok_clicked();

    void on_btn_restore_clicked();

    void on_btn_qfColorCommon_clicked();

    void on_btn_qfColorKeyword_clicked();

    void on_btn_qfColorString_clicked();

    void on_btn_qfColorNumber_clicked();

    void on_btn_qfColorComment_clicked();

    void on_btn_qfColorBackground_clicked();

    void on_cbox_theme_currentTextChanged(const QString &arg1);

private:

    std::atomic_bool stop=false;
};

#endif // OPTIONSDIALOG_H
