#ifndef CMASK_H
#define CMASK_H

#include <QLabel>
#include <QToolButton>
#include <QWidget>
#include <functional>

class CMask : public QWidget
{
    Q_OBJECT
public:
    explicit CMask(QWidget *parent);
    ~CMask();

    void setGif(QString strPath);
    void setStopFunction(std::function<void()> stopFunc);

    static void* operator new(std::size_t) = delete;
//    static void operator delete(void*) = delete;

signals:

public slots:

private:
    QLabel *infoLabel;
    QToolButton *btn;
    void paintEvent(QPaintEvent* e);

    std::function<void()> stopFunc=nullptr;
};

#endif // CMASK_H
