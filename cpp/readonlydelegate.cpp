#include "readonlydelegate.h"
#include "widgetutils.h"


void ReadOnlyDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    auto opt=option;
    WidgetUtils::paint(painter,opt,index);
}
