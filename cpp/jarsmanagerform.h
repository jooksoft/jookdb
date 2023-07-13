#ifndef JARSMANAGERFORM_H
#define JARSMANAGERFORM_H

#include <QTreeWidget>
#include <QWidget>

namespace Ui {
class JarsManagerForm;
}

class JarsManagerForm : public QWidget
{
    Q_OBJECT

public:
    explicit JarsManagerForm(QWidget *parent = nullptr);
    ~JarsManagerForm();

    void flushLibTree(QString name="");

private:
    Ui::JarsManagerForm *ui;

public:
    void closeEvent(QCloseEvent *event) override;

    QString getType() const;
    void setType(const QString &value);

private slots:
    void on_btn_add_driver_clicked();

    void on_btn_add_jar_clicked();

    void on_btn_delete_clicked();

    void on_btn_edit_clicked();

    void on_btn_lib_path_clicked();

    void on_tree_main_itemDoubleClicked(QTreeWidgetItem *item, int column);

private:
    std::atomic_bool stop=false;

    QString type;
};

#endif // JARSMANAGERFORM_H
