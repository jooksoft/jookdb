#include "objtimecache.h"

#include <QTime>
#include <QDebug>

ObjTimeCache::ObjTimeCache(int defaultExpireMs,int recycleMs):defaultExpireMs(defaultExpireMs),recycleMs(recycleMs)
{

}

void ObjTimeCache::put(const QByteArray &key,const QVariant &value,int expireMs)
{
    if(std::abs(QDateTime::currentMSecsSinceEpoch()-clearTime)>this->recycleMs){
        clearTime=QDateTime::currentMSecsSinceEpoch();

        QHash<QByteArray,ObjTimeCacheData>::iterator iter;
        for (iter=this->cacheMap.begin();iter!=this->cacheMap.end();) {
            ObjTimeCacheData data=iter.value();
            if(data.expireTime<QDateTime::currentMSecsSinceEpoch()){
                iter=this->cacheMap.erase(iter);
            }else{
                iter++;
            }
        }
    }

    if(expireMs<0){
        expireMs=this->defaultExpireMs;
    }
    ObjTimeCacheData data;
    data.expireTime=QDateTime::currentMSecsSinceEpoch()+expireMs;
    data.value=value;
    this->cacheMap.insert(key,data);
}

QVariant ObjTimeCache::get(const QByteArray &key)
{
    ObjTimeCacheData data=this->cacheMap.value(key);

    if(data.expireTime>=QDateTime::currentMSecsSinceEpoch()){
        return data.value;
    }

    if(data.expireTime>0){
        this->cacheMap.remove(key);
    }

    return {};
}

bool ObjTimeCache::contains(const QByteArray &key) const
{
    ObjTimeCacheData data=this->cacheMap.value(key);
    if(data.expireTime>=QDateTime::currentMSecsSinceEpoch()){
        return true;
    }
    return false;
}

void ObjTimeCache::clear()
{
    this->cacheMap.clear();
}
