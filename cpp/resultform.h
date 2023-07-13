#ifndef RESULTFORM_H
#define RESULTFORM_H

#include "queryform.h"
#include "sqllexer.h"
#include "sqlobject.h"

#include <QJsonObject>
#include <QTableWidget>
#include <QWidget>

namespace Ui {
class ResultForm;
}

class ResultForm : public QWidget
{
    Q_OBJECT

public:
    explicit ResultForm(const QJsonObject &res,QSharedPointer<ParsedSql> psql,QWidget *parent=nullptr);
    ~ResultForm();

    QTableWidget *getTableWidget() const;

    QSharedPointer<ParsedSql> getPsql() const;

    void showRes();
    void appendRs(const QJsonObject &resJson);
    void showStatus();
    void updateHasNextStat();
    void setResLabel(const QString &text);
    QTreeWidget *getPlanTree();
    void genPlanTree(QTreeWidget *tree,QTreeWidgetItem *parentItem,QJsonObject json);

    void copyToExcel(bool current=true,bool isXlsx=true,int64_t rows=-1);

    void fetchNext();

    void safeDelete();
    void closeEvent(QCloseEvent *event) override;


    void refresh();

    std::atomic_bool modified=false;

    void saveOldData(QTableWidgetItem *item);

    void itemDataChange(QTableWidgetItem *item);

    void setModified(const bool &value);

    void checkModify();

    void clearOldUpdate(int row);

public:
    std::atomic_bool hasNext=false;

    QSharedPointer<ConnData> getConnData() const;
    void setConnData(const QSharedPointer<ConnData> &value);

    QString getDbName() const;
    void setDbName(const QString &value);

private slots:
    void on_btn_next_clicked();

    void on_btn_nextall_clicked();

    void on_btn_export_clicked();

    void on_le_twfilter_textChanged(const QString &arg1);

    void on_le_twfilter_returnPressed();

    void on_btn_goto_edit_clicked();

    void on_btn_save_clicked();

    void on_btn_close_clicked();

private:
    Ui::ResultForm *ui;

    QSharedPointer<ParsedSql> psql;
    QJsonObject res;
    QSharedPointer<ConnData> connData;
    QString dbName;
    QTableWidget *tw_rs=nullptr;
    QLabel *label=nullptr;
    std::atomic_bool fetchStop=false;
    QSet<int> rightAlign;

    QueryForm *qf=nullptr;

    bool readonly=true;

    QMap<QString,Table*> tableMap;

};

#endif // RESULTFORM_H
