#ifndef TEXTEDITDIALOG_H
#define TEXTEDITDIALOG_H

#include "myedit.h"

#include <QDialog>

namespace Ui {
class TextEditDialog;
}

class TextEditDialog : public QDialog
{
    Q_OBJECT

public:
    explicit TextEditDialog(QWidget *parent = nullptr);

    MyEdit * getMyEdit();

signals:

private slots:

    void on_btn_file2blob_clicked();

    void on_btn_blob2file_clicked();

    void on_btn_file2text_clicked();

    void on_btn_text2file_clicked();

    void on_chk_line_wrap_stateChanged(int arg1);

private:
    Ui::TextEditDialog *ui;

    ~TextEditDialog() override;

    bool readOnly=false;

public:

    void closeEvent(QCloseEvent *event) override;

    bool getReadOnly() const;
    void setReadOnly(bool value);

private:

    std::atomic_bool stop=false;
};

#endif // TEXTEDITDIALOG_H
