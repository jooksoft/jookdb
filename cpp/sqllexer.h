#ifndef SQLLEXER_H
#define SQLLEXER_H

#include "utils.h"

#include <QObject>
#include <QSet>
#include <QMap>
#include <functional>

#define MAX_SQL_SIZE 10240000

enum class SqlLexType{
    sql,
    all,
};

struct ParsedSql{
    QByteArray sql;
    QByteArray sqlType="unkonw";//unkonw,select,insert,update,delete,drop,alter,create,"rename"
    QByteArray sqlSimpleName;
    int64_t startPos;
    int64_t startPos_indic;
    int64_t len_indic;
    int modifyNum;
    int effectiveChars=0;
    bool isExplainCmd=false;
    QMap<QString,QVariant> props;
};

const static QSet<QByteArray> startKeys={"go","update","delete","insert","create","alter","truncate","exec","dbcc","show","replace","merge",
                                         "grant","revoke","rename","comment","commit","rollback"};

const static QSet<QByteArray> procTypeSqls={"procedure","function","type","package","trigger","event","declare","begin"};

class SqlLexer
{
public:
    explicit SqlLexer(const char* documentBytes,const int64_t length);
    static bool isZS(QByteArray &ba);
    bool isFGF(uint8_t c);
    QByteArray preEntity(bool peek=false);
    QByteArray nextEntity(bool peek=false);
    QByteArray nextDeliEntity();
    //合并括号中内容，如函数、子查询等合并到一个Entity里。compoundStmt:是否支持语句块。periodStmt:是否使用句号连接(对象名、浮点数都有可能是句号连接)
    QByteArray nextCiFaEntity(bool compoundStmt=false,bool periodStmt=false);
    QByteArray nextYXEntity(QByteArray &ba,bool peek=false);
    QByteArray nextSqlEnd(bool peek=false);
    void useFormat();
    QByteArray nextFormatSql();
    QSharedPointer<ParsedSql> nextSqlCiFa();
    QSharedPointer<ParsedSql> nextSql();
    bool testTypeIsScript();
    QByteArray getSqlType();

    void skipSpaceLeft(int64_t &pos);
    static void skipSpaceLeft(const QByteArray &ba,int64_t &pos);
    void findKuoHaoLeft(const QByteArray &ba,int64_t &pos);
    void findKuoHaoLeft(const QString &ba,int64_t &pos);
    void findKuoHaoRight(const QByteArray &ba,int64_t &pos);
    bool inKuoHaoLeft(const QByteArray &ba,int64_t start,int64_t end);
    bool inKuoHaoRight(const QByteArray &ba,int64_t start,int64_t end);
    void findBianJieLeft(const QByteArray &ba_lower,int64_t &pos);
    void findBianJieRight(const QByteArray &ba_lower,int64_t &pos);
    void getWordLeft(QByteArray &ba,int64_t &pos);
    void getWordLeft(const QByteArray &sqlBa,QByteArray &ba,int64_t &pos);
    void skipSpaceRight(int64_t &pos);
    void skipSpaceRight();
    static void skipSpaceRight(const QByteArray &ba,int64_t &pos);
    void getWordRight(QByteArray &ba,int64_t &pos);
    void getWordRight(const QByteArray &sqlBa,QByteArray &ba,int64_t &pos);
    QList<QByteArray> leftWord(bool peek=false);
    QList<QByteArray> rightWord(bool peek=false);

    //dianhao=true表示查找到的key后面不可以有'.'点号，如果有则继续查找
    static void leftKey(const QByteArray &ba,const QByteArray &key,int64_t &pos,bool dianhao=false);
    //dianhao=true表示查找到的key后面不可以有'.'点号，如果有则继续查找
    static void rightKey(const QByteArray &ba,const QByteArray &key,int64_t &pos,bool dianhao=false);

    int64_t nextKeyIdx(const QByteArray &key);

    QByteArray getAliasTables(QByteArray alias);
    QByteArray getCiFaTables(const QByteArray &preKey="");

    void setPos(const int64_t pos);
    int64_t getPos();

    QList<QByteArray> getSqlTables(const QByteArray &sqlBa,const int64_t &khStart, const int64_t &khEnd, const int64_t &start);

    inline static bool leftHasNewline(const QByteArray &ba);

    bool isBianJieStr(const QByteArray &ba) const;

    SqlLexType lexType=SqlLexType::sql;

    const QSet<QByteArray> objPreSet={"from","join","update","table","into","on"};//on仅指创建索引触发器等情况下指定表名

    QSharedPointer<ConnData> getConnData() const;
    void setConnData(const QSharedPointer<ConnData> &value);

    QSet<QByteArray> compoundKeysSet;

    static QStringList getEntityList(QSharedPointer<ConnData> connData,QString sql,bool trim=false);
private:
    const char* documentBytes;
    const int64_t length;
    int64_t docPos=0;

    bool charsEqual(int64_t pos,QByteArray str);
    void nextCreate(QByteArray &sqlBa);
    void nextALL(QByteArray &sqlBa);
    void nextCurrLine(QByteArray &sqlBa);
    void nextFenhao(QByteArray &sqlBa);

    bool isEnd(const QByteArray &sqlBa,const QByteArray &entityBa);

    void removeLastFenhao(QByteArray &sqlBa);

    QMap<QByteArray,std::function<void(QByteArray&,int)>> formatFuncs;
    std::function<void(QByteArray&,int)> getFormatFunc(const QByteArray &entityBa);

    QByteArray delimiter;

    QSharedPointer<ConnData> connData=nullptr;

    int dbType=-1;
};


#endif // SQLLEXER_H
