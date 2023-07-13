#ifndef SQLSTYLE_H
#define SQLSTYLE_H

#include <QObject>
#include "myedit.h"


const QByteArray keyworks_sql="absolute action add admin after aggregate \
    alias all allocate alter and any are array as asc \
    assertion at authorization \
    before begin binary bit blob body boolean both breadth by \
    call cascade cascaded case cast catalog char character \
    check class clob close collate collation column commit \
    completion connect connection constraint constraints \
    constructor continue corresponding create cross cube current \
    current_date current_path current_role current_time current_timestamp \
    current_user cursor cycle \
    data date datetime day deallocate dec decimal declare default \
    deferrable deferred delete depth deref desc describe descriptor \
    destroy destructor deterministic dictionary diagnostics disconnect \
    distinct domain double dynamic \
    each else end end-exec equals escape every except \
    exception exec execute exists exit external \
    false fetch first float for foreign found from free full \
    function \
    general get global go goto grant group grouping \
    having host hour \
    identity if ignore immediate in indicator initialize initially \
    inner inout input insert int integer intersect interval \
    into is isolation iterate \
    join \
    key \
    language large last lateral leading left less level like \
    limit local localtime localtimestamp locator \
    map match merge minute modifies modify module month \
    names national natural nchar nclob new next no none \
    not null numeric \
    object of off old on only open operation option \
    or order ordinality out outer output \
    package pad parameter parameters partial path postfix precision prefix \
    preorder prepare preserve primary \
    prior privileges procedure public \
    read reads real recursive ref references referencing relative \
    restrict result return returns revoke right \
    role rollback rollup routine row rows \
    savepoint dbName scroll scope search second section select \
    sequence session session_user set sets size smallint some space \
    specific specifictype sql sqlexception sqlstate sqlwarning start \
    state statement static structure system_user schema \
    table temporary terminate than then time timestamp \
    timezone_hour timezone_minute to trailing transaction translation \
    treat trigger true \
    under union unique unknown \
    unnest update usage user using \
    value values variable varying view \
    when whenever where with without work write \
    year \
    zone \
    flush leave load repeat rename show explain replace";
const QByteArray keyworks_plsql="authid between \
    binary_integer \
    bulk char_base cluster collect \
    comment compress constant currval \
    do drop elsif exclusive \
    extends forall \
    heap index \
    interface java \
    limited lock long loop minus mod mode \
    naturaln nextval nocopy nowait number \
    number_base ocirowid opaque operator \
    organization others partition pctfree pls_integer \
    positive positiven pragma private raise \
    range raw record release reverse \
    rowid rownum rowtype separate share \
    sqlcode sqlerrm stddev subtype successful \
    synonym sysdate \
    type uid use validate varchar \
    varchar2 variance while switch break \
    truncate always generated unlock disable enable nvarchar nvarchar2 text ntext";

class SqlStyle : public QObject
{
    Q_OBJECT
public:
    explicit SqlStyle();

    static void setStyle(MyEdit *myEdit);
    static void setMarginLineNumberWidth(MyEdit *myEdit);
    static void setMarginFlod(MyEdit *myEdit);
    static void clearResultIndic(MyEdit *myEdit);
    static void setResultIndic(MyEdit *myEdit,int64_t start,int64_t len);
    static void clearErrorIndic(MyEdit *myEdit);
    static void setErrorIndic(MyEdit *myEdit,int64_t start,int64_t len);
    static void clearFindIndic(MyEdit *myEdit);
    static void setFindIndic(MyEdit *myEdit,int64_t start,int64_t len);
    static void setResultIndicStyle(MyEdit *myEdit);

signals:

public slots:

private:

};

#endif // SQLSTYLE_H
