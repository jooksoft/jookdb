#ifndef DATETIMEEDITFORM_H
#define DATETIMEEDITFORM_H

#include <QWidget>

enum class DateTimeEditType{
    time,
    date,
    datetime
};

namespace Ui {
class DateTimeEditForm;
}

class DateTimeEditForm : public QWidget
{
    Q_OBJECT

public:
    explicit DateTimeEditForm(QWidget *parent = nullptr);

    QString getDate() const;
    void setDate(const QString &value);

    QString getTime() const;
    void setTime(const QString &value);

    QString getDateTime() const;
    void setDateTime(const QString &value);

    void waitExit();


signals:

    void editClosed();

private slots:
    void on_btn_cancel_clicked();

    void on_btn_ok_clicked();

    void on_btn_now_clicked();

private:
    Ui::DateTimeEditForm *ui;

    ~DateTimeEditForm();

    QString date;
    QString time;
    QString dateTime;

    bool okExit=false;

    bool readonly=false;

public:

    void closeEvent(QCloseEvent *event) override;

    bool getReadonly() const;
    void setReadonly(bool value);

private:

    std::atomic_bool stop=false;
};

#endif // DATETIMEEDITFORM_H
