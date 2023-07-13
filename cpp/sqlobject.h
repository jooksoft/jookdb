#ifndef SQLOBJECT_H
#define SQLOBJECT_H

#include "utils.h"

#include <QComboBox>
#include <QObject>

//#define DB_TYPE_GROUP_NUMBER 100
//#define DB_TYPE_GROUP_STRING 101
//#define DB_TYPE_GROUP_BINARY 102
//#define DB_TYPE_GROUP_GEOMETRY 103

struct Index
{
    int id;

    QString index_name;
    QString columns;
    QString index_type;
    int seq_in_index;
    QString index_method;
    QString charset;
    QString collation;
    QString cardinality;
    QString comment;
    bool is_unique=false;
    bool is_primary_key=false;
    bool is_unique_constraint=false;
    bool is_descending_key=false;
    bool is_disabled=false;

    bool compress=false;
    bool nologging=false;
    QString prefix_length;
    bool local=false;
    bool reverse=false;
    QString tablespace_name;

    QMap<QString,QVariant> property;

    Index clone();

};
Q_DECLARE_METATYPE(Index)

struct Column
{
    int id=-1;
    QString column_name;
    QString column_type;
    bool column_unsigned=false;
    bool column_zerofill=false;
    QString column_key;
    QString length;
    QString decimal;
    QString identity;
    bool notNull=false;
    bool isGeneratedExpr=false;
    QString storedStr;
    QString defaultExpr;
    QString charset;
    QString collation;
    QString comment;
    QString defaultConstantName;//默认值约束名，MSSQL默认值是通过约束实现的

    QMap<QString,QVariant> property;

};
Q_DECLARE_METATYPE(Column)

struct Check
{
    int id=-1;
    QString check_name;
    QString definition;
    bool is_disabled=false;
    bool is_not_for_replication=false;
    QString comment;

};
Q_DECLARE_METATYPE(Check)

struct Trigger
{
    int id=-1;
    QString trigger_name;
    QString event_object_table;
    QString action_statement;
    QString action_timing;
    QString created;
    QString sql_mode;
    QString columns;
    QString when;
    QString definer;
    QString charset;
    QString collation;
    QString database_collation;
    bool isinsert=false;
    bool isupdate=false;
    bool isdelete=false;
    bool istruncate=false;
    bool is_disabled=false;
    bool is_not_for_replication=false;
    bool is_each_row=false;
    QString comment;

};
Q_DECLARE_METATYPE(Trigger)

struct Foreign
{
    int id=-1;
    QString constraint_schema;
    QString constraint_name;
    QString table_schema;
    QString table_name;
    QString column_name;
    QString referenced_table_schema;
    QString referenced_table_name;
    QString referenced_column_name;
    QString update_rule;
    QString delete_rule;
    QString match_option;
    bool is_disabled=false;
    bool is_not_for_replication=false;
    QString comment;


};
Q_DECLARE_METATYPE(Foreign)

class Table : public QObject
{
    Q_OBJECT
public:
    explicit Table(QObject *parent = nullptr);

    void clear(){
        this->dbName.clear();
        this->tableName.clear();

        this->enging.clear();
        this->charset.clear();
        this->collation.clear();
        this->comment.clear();
        this->table_rows.clear();
        this->row_format.clear();
        this->avg_row_length.clear();
        this->max_rows.clear();
        this->auto_increment.clear();
        this->primaryKeys.clear();
        this->primaryKeyIdxName.clear();
        this->data_free.clear();
        this->data_length.clear();
        this->index_length.clear();
        this->create_time.clear();
        this->update_time.clear();

        this->is_view=false;

        this->lookMode.clear();
        this->curr_identity.clear();
        this->is_change_tracking=false;
        this->is_track_columns_updated=false;

        this->property.clear();

        this->create_sql.clear();

        this->columns.clear();
        this->indexes.clear();
        this->triggers.clear();
        this->foreigns.clear();
        this->checks.clear();
    }

    QString dbName;
    QString tableName;

    QString enging;
    QString charset;
    QString collation;
    QString comment;
    QString table_rows;
    QString row_format;
    QString avg_row_length;
    QString max_rows;
    QString auto_increment;
    QString primaryKeys;
    QString primaryKeyIdxName;
    QString data_free;
    QString data_length;
    QString index_length;
    QString create_time;
    QString update_time;

    bool is_view=false;

    QString lookMode;
    QString curr_identity;
    bool is_change_tracking=false;
    bool is_track_columns_updated=false;

    QString create_sql;

    QList<Column> columns;
    QList<Index> indexes;
    QList<Trigger> triggers;
    QList<Foreign> foreigns;
    QList<Check> checks;

    QMap<QString,QVariant> property;

    static bool loadCharSet(QSharedPointer<ConnData> connData,QComboBox * charSetCBox,QWidget * parent=nullptr);

    static bool loadCollation(QSharedPointer<ConnData> connData,QString charSet,QComboBox * collationCBox,QWidget * parent=nullptr);

    bool updateTableStatus(QSharedPointer<ConnData> connData,bool showErr=true);
    bool updateColumn(QSharedPointer<ConnData> connData,bool showErr=true,bool getExtend=false);
    bool updateIndex(QSharedPointer<ConnData> connData,bool showErr=true);
    bool updateTrigger(QSharedPointer<ConnData> connData,bool showErr=true);
    bool updateForeign(QSharedPointer<ConnData> connData,bool showErr=true);
    bool updateCreateSql(QSharedPointer<ConnData> connData,bool showErr=true);
    bool updateCheck(QSharedPointer<ConnData> connData,bool showErr=true);

    bool updateSelectColumn(QSharedPointer<ConnData> connData,bool showErr=true,bool quoteIdent=false,bool isSubQuery=false);
    bool updateJDBCColumn(QSharedPointer<ConnData> connData,bool showErr=true,bool quoteIdent=false);

    Column getColumn(const QString &column_name);
    Index getIndex(const QString &index_name);
signals:

public slots:

private:

};

class DBObject : public QObject
{
    Q_OBJECT
public:
    static DBObject *instance(){
        static DBObject *dbObject=new DBObject{};
        return dbObject;
    }

    bool hasUnsignedType(const QString &type);
    bool hasZerofillType(const QString &type);

    QMap<QString,QString> typeMap_mysql;

    QMap<QString,QString> typeMap_sqlserver;

    QMap<QString,QString> typeMap_oracle;

    QMap<QString,QString> typeMap_dameng;

    QMap<QString,QString> typeMap_postgresql;

    QMap<QString,QString> typeMap_sqlite;

    QSet<QString> typeKeys;

    static QStringList parseColumnNameList(QString columns);

    static QString findKuohaoStr(const QStringList &arr,const int startIdx,int maxScope=2);

    static bool isIntegerType(QSharedPointer<ConnData> connData,const QString &type);
    static bool isNumberType(QSharedPointer<ConnData> connData,const QString &type);
    static bool isStringType(QSharedPointer<ConnData> connData,const QString &type);
    static bool isBinaryType(QSharedPointer<ConnData> connData,const QString &type);
    static bool isGeometryType(QSharedPointer<ConnData> connData,const QString &type);

    static bool isUnique(QSharedPointer<ConnData> connData,const Index &index);

    static QString genColumnCommentSql(QSharedPointer<ConnData> connData,const QString &tableName,const QString &columnName,const QString &comment);
    static QString genTableCommentSql(QSharedPointer<ConnData> connData,const QString &tableName,const QString &comment);
    static QStringList genTablePrimaryKeysSql(QSharedPointer<ConnData> connData,const QString &tableName,QString primaryKeys, const QString &oldPrimaryKeyName, QString oldPrimaryKeys);
    static QString genDropIndexSql(QSharedPointer<ConnData> connData,const QString &tableName,const QString &indexName);
    static QString genAddIndexSql(QSharedPointer<ConnData> connData,const QString &tableName,const QString &indexName,const bool &isUnique,QString columns);

    static QString genLimitedSql(QSharedPointer<ConnData> connData,QByteArray sql,int page,QByteArray sqlType={});

    static QStringList removeDefaultSchema(QSharedPointer<ConnData> connData,const QStringList &list);
    static QStringList removeSchema(QSharedPointer<ConnData> connData,const QStringList &list);

    static bool setTransformParam_oracle(QSharedPointer<ConnData> connData,bool showErr=true);

signals:

public slots:

private:
    explicit DBObject(QObject *parent = nullptr);
};

#endif // SQLOBJECT_H
