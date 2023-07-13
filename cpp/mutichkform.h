#ifndef MUTICHKFORM_H
#define MUTICHKFORM_H

#include <QWidget>
#include <QListWidget>

namespace Ui {
class MutiChkForm;
}

class MutiChkForm : public QWidget
{
    Q_OBJECT

public:
    explicit MutiChkForm(QWidget *parent = nullptr);
    ~MutiChkForm();

    void waitExit();

    QListWidget *getListWidget();

    QListWidget *getListWidgetRight();


signals:

    void editClosed();

private:
    Ui::MutiChkForm *ui;

    bool readonly=false;

public:

    void closeEvent(QCloseEvent *event) override;

    bool getReadonly() const;
    void setReadonly(bool value);

private slots:
    void on_listWidget_itemClicked(QListWidgetItem *item);

    void on_btn_selectAll_clicked();

    void on_btn_deselectAll_clicked();

    void on_btn_ok_clicked();

private:

    std::atomic_bool stop=false;
};

#endif // MUTICHKFORM_H
