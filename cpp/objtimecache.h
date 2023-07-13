#ifndef OBJTIMECACHE_H
#define OBJTIMECACHE_H

#include <QHash>
#include <QVariant>

class ObjTimeCache
{
public:
    ObjTimeCache(int defaultExpireMs=30000,int recycleMs=35000);

    void put(const QByteArray &key,const QVariant &value,int expireMs=-1);
    QVariant get(const QByteArray &key);
    bool contains(const QByteArray &key) const;

    void clear();

    struct ObjTimeCacheData{
        int64_t expireTime=-1;
        QVariant value;
    };
private:
    int defaultExpireMs;
    int recycleMs;
    QHash<QByteArray,ObjTimeCacheData> cacheMap;
    int64_t clearTime=-1;
};

#endif // OBJTIMECACHE_H
