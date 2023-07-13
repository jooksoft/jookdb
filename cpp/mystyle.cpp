#include "mystyle.h"
#include <QDebug>

MyStyle::MyStyle(const QString &key):QProxyStyle(key)
{

}

MyStyle *MyStyle::instance()
{
    static MyStyle* ins=new MyStyle;
    return ins;
}

int MyStyle::pixelMetric(QStyle::PixelMetric metric, const QStyleOption *option, const QWidget *widget) const
{
    switch (metric) {
    case PM_ListViewIconSize:
        return QCommonStyle::pixelMetric(metric,option,widget);
        break;
    default:
        return QProxyStyle::pixelMetric(metric,option,widget);
        break;
    }

}
