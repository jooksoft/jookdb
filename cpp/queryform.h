#ifndef QUERYFORM_H
#define QUERYFORM_H

#include "myedit.h"
#include "sqllexer.h"
#include "sqlstyle.h"

#include <QComboBox>
#include <QLabel>
#include <QTableWidget>
#include <QWidget>
#include <QFileSystemWatcher>

enum class QueryRunType{
    current,
    script,
    one_script,
    explain
};

namespace Ui {
class QueryForm;
}

class QueryForm : public QWidget
{
    Q_OBJECT

public:
    explicit QueryForm(QWidget *parent = nullptr);
    ~QueryForm();

    MyEdit* getMyEdit();
    void clear();
    void closeResults();
    void deleteResultWidget(QWidget* resWidget);
    void addResultSet(QJsonObject res,QSharedPointer<ParsedSql> psql);
    void refreshDB();
    void runQuery(QueryRunType runType=QueryRunType::current);
    bool testRes(QSharedPointer<ConnData> connData,QSharedPointer<ParsedSql> psql,const QJsonObject &res);
    QSharedPointer<ConnData> getCurrConnData();
    QComboBox* getDbCombox();
    QComboBox* getDBNameCombox();
    void setCurrDbComboxItem(QString connName);
    void setCurrDBNameComboxItem(QString dbName);
    void formatSql();
    QTabWidget* getResTabWidget();
    void updateStatusBar();
    void updateSqlStatus();

    void selCurrStatement_v3();
    QPair<int64_t,int64_t> getCurrStatement();

    std::atomic_bool stop=false;
    void safeDelete();
    void closeEvent(QCloseEvent *event) override;

    void loadFile(const QString &fileName,const QByteArray codec);
    void reloadTailFile();

    void stopWatchFileChange();
    void watchFileChange();

    void setLogForm();

    void dragEnterEvent(QDragEnterEvent *ev) override;
    void dropEvent(QDropEvent *ev) override;

    static void refreshDB4All();

    int64_t oldSize;
    QDateTime oldLastModified;

public slots:
    void on_btn_stop_clicked();
    void on_btnCommit_clicked();
    void on_btnRollback_clicked();

    void on_btn_explain_clicked();

private slots:
    void on_btn_run_clicked();

    void on_chkAutoCommit_clicked(bool checked);

    void on_dbComboBox_activated(int index);

    void on_dbNameComboBox_activated(int index);

    void on_btn_clear_clicked();

private:
    Ui::QueryForm *ui;

    QTimer* fileWatchTimer=nullptr;

    std::atomic_bool inQuery=false;//查询命令正在运行

    void setNeedCommit(QSharedPointer<ConnData> connData,bool needCommit);
    void setAutoCommit(QSharedPointer<ConnData> connData,bool autoCommit);
    bool getNeedCommit(QSharedPointer<ConnData> connData);
    bool getAutoCommit(QSharedPointer<ConnData> connData);

};

#endif // QUERYFORM_H
