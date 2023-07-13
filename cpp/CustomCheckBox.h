#ifndef CUSTOMCHECKBOX_H
#define CUSTOMCHECKBOX_H

#include <QCheckBox>
#include <QToolButton>
#include <QWidget>

class CustomCheckBox : public QWidget
{
    Q_OBJECT
public:
    explicit CustomCheckBox(QWidget *parent = nullptr);

    void setOldChecked(bool checked);
    bool getOldChecked();
    void setChecked(bool checked);
    bool getChecked();
    void setText(const QString &text);
    QString getText() const;
    void setOldText(const QString &text);
    QString getOldText() const;

    void mouseReleaseEvent(QMouseEvent *event) override;

    bool getReadonly() const;
    void setReadonly(bool value);

    bool hasChanged();

signals:
    void checked(bool checked);
    void mouseRelease(bool checked);

public slots:

private:
    QCheckBox *chkBox=nullptr;
    bool readonly=false;
    bool old_chked=false;
    QString old_text;

    bool ignoreMouseEvent=false;
public:

    void closeEvent(QCloseEvent *event) override;

    bool getIgnoreMouseEvent() const;
    void setIgnoreMouseEvent(bool value);

private:

    std::atomic_bool stop=false;

};

#endif // CUSTOMCHECKBOX_H
