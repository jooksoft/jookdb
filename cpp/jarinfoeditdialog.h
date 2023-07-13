#ifndef JARINFOEDITDIALOG_H
#define JARINFOEDITDIALOG_H

#include <QDialog>

namespace Ui {
class JarInfoEditDialog;
}

class JarInfoEditDialog : public QDialog
{
    Q_OBJECT

public:
    explicit JarInfoEditDialog(QWidget *parent = nullptr);
    ~JarInfoEditDialog();

    QString getFile() const;
    void setFile(const QString &value);

    QString getType() const;
    void setType(const QString &value);

    QString getClassName() const;
    void setClassName(const QString &value);

    void closeEvent(QCloseEvent *event) override;

private slots:
    void on_btn_ok_clicked();

    void on_btn_cancel_clicked();

private:
    std::atomic_bool stop=false;

    QString file;
    QString type;
    QString className;

private:
    Ui::JarInfoEditDialog *ui;
};

#endif // JARINFOEDITDIALOG_H
