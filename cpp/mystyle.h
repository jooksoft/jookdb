#ifndef MYSTYLE_H
#define MYSTYLE_H

#include <QProxyStyle>

class MyStyle : public QProxyStyle
{
    Q_OBJECT
private:
    explicit MyStyle(const QString &key="Fusion");

public:
    static MyStyle *instance();

    int pixelMetric(PixelMetric metric, const QStyleOption *option = nullptr, const QWidget *widget = nullptr) const override;

signals:

};

#endif // MYSTYLE_H
