#ifndef DATABASEMANAGERFORM_H
#define DATABASEMANAGERFORM_H

#include "utils.h"

#include <QWidget>

namespace Ui {
class DatabaseManagerForm;
}

class DatabaseManagerForm : public QWidget
{
    Q_OBJECT

public:
    explicit DatabaseManagerForm(QSharedPointer<ConnData> connData,QWidget *parent = nullptr);
    ~DatabaseManagerForm();



    void safeDelete();
    void closeEvent(QCloseEvent *event) override;

    void refresh();
    void refresh_process();
    void refresh_variables();

    QSharedPointer<ConnData> getConnData() const;

    void copyToExcel(QTableWidget * tw,bool isXlsx);

    void updateStatusBar();


private slots:
    void on_tabWidget_currentChanged(int index);

    void on_tw_variables_cellChanged(int row, int column);

    void on_le_twfilter_db_textChanged(const QString &arg1);

    void on_le_twfilter_proc_textChanged(const QString &arg1);

    void on_le_twfilter_var_textChanged(const QString &arg1);

    void on_le_twfilter_db_returnPressed();

    void on_le_twfilter_proc_returnPressed();

    void on_le_twfilter_var_returnPressed();

private:
    Ui::DatabaseManagerForm *ui;

    QMap<QString,QPair<QString,QString>> varMap;

    QSharedPointer<ConnData> connData;


    std::atomic_bool stop=false;

};

#endif // DATABASEMANAGERFORM_H
