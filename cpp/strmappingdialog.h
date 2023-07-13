#ifndef STRMAPPINGDIALOG_H
#define STRMAPPINGDIALOG_H

#include <QDialog>
#include <QListWidgetItem>

namespace Ui {
class StrMappingDialog;
}

class StrMappingDialog : public QDialog
{
    Q_OBJECT

public:
    explicit StrMappingDialog(QWidget *parent = nullptr);
    ~StrMappingDialog();

    void setMappingStrs(QStringList srcList,QStringList tgtList,QList<QPair<QString,QString>> mappings);
    QList<QPair<QString,QString>> getMappingList();

private slots:
    void on_btn_ok_clicked();

    void on_btn_cancel_clicked();

    void on_btn_add_clicked();

    void on_btn_delete_clicked();

    void on_btn_auto_clicked();

    void on_btn_clear_clicked();

    void on_lw_src_itemClicked(QListWidgetItem *item);

    void on_lw_tgt_itemClicked(QListWidgetItem *item);

    void on_lw_mapping_itemDoubleClicked(QListWidgetItem *item);

    void on_btn_auto_2_clicked();

private:
    Ui::StrMappingDialog *ui;

    const QString MAP_FGF="  -->  ";

public:

    void closeEvent(QCloseEvent *event) override;

private:

    std::atomic_bool stop=false;
};

#endif // STRMAPPINGDIALOG_H
