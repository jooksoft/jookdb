#ifndef CONTENTWIDGET_H
#define CONTENTWIDGET_H

#include "databasemanagerform.h"
#include "leftwidgetform.h"
#include "myedit.h"
#include "mytreewidget.h"
#include "objectmanagerform.h"
#include "queryform.h"
#include "tabledataform.h"
#include "tableform.h"
#include <QSplitter>
#include <QTreeWidget>
#include <QWidget>
#include <QMap>


class ContentWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ContentWidget(QWidget *parent = nullptr);

    MyEdit* getCurrTextEdit();
    QueryForm *getCurrQueryForm();
    TableForm *getCurrTableForm();
    TableDataForm *getCurrTableDataForm();
    ObjectManagerForm *getCurrObjectManagerForm();
    DatabaseManagerForm *getCurrDatabaseManagerForm();
    MyTreeWidget* getLeftTree() const;
    QTabWidget *getRightTab() const;

    QSplitter *getSplitter();

    inline int addTabWithToolTip(QWidget *wid,const QString &title, const QString &tip);
    QueryForm * addTab(QString title,QString db="",QString dbName="");
    TableForm * addTableTab(QSharedPointer<ConnData> connData,QString tableName="",QString dbName="",bool readonly=false,bool isView=false);
    TableDataForm * addTableDataTab(QSharedPointer<ConnData> connData,QString tableName="",QString dbName="",bool readonly=false);
    bool closeTab(const int idx);

    ObjectManagerForm * addObjectManager(QSharedPointer<ConnData> connData,QString dbName,bool isDump=false,QString dumpTableName="");

    DatabaseManagerForm *addDatabaseManager(QSharedPointer<ConnData> connData);

    void setTabModified(QWidget *widget,bool modified);
    void setTabTitle(QWidget *widget,const QString &title, const QString &tip,bool keepModify=true);

    void setContentSplitterSizes(const QList<QVariant> &sizes);

signals:

public slots:
    void documentWasModified();
    void openConnDlg(const QString &connType);

private:
    LeftWidgetForm *leftWidgetForm;
    QSplitter *contentSplitter;
    QTabWidget *rightTab;

    int32_t tabChgCount=0;

public:

    void closeEvent(QCloseEvent *event) override;

    QLineEdit *getLeftTree_filter() const;

private:

    std::atomic_bool stop=false;

};

#endif // CONTENTWIDGET_H
