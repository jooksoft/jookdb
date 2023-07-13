#ifndef SEARCHREPLACEFORM_H
#define SEARCHREPLACEFORM_H

#include <QWidget>

#define FIND_STATUS_SUCC 1
#define FIND_STATUS_FAIL 2
#define FIND_TYPE_NEXT 3
#define FIND_TYPE_PRE 4

namespace Ui {
class SearchReplaceForm;
}

class SearchReplaceForm : public QWidget
{
    Q_OBJECT

public:
    explicit SearchReplaceForm(QWidget *parent = nullptr);
    ~SearchReplaceForm();

    void useReplace(bool use);

    void saveFindHis(const QString &findBa);
    void saveReplaceHis(const QString &replaceBa);

    void freshFindHis();
    void freshReplaceHis();

    void setCurrSelect();

private slots:
    void on_chb_useReplace_clicked(bool checked);

    void on_btn_close_clicked();

    void on_btn_findNext_clicked();

    void on_btn_findPre_clicked();

    void on_chb_inSelection_clicked(bool checked);

    void on_btn_count_clicked();

    void on_btn_replaceAll_clicked();

    void on_cbox_find_currentTextChanged(const QString &arg1);

    void on_btn_replaceFind_clicked();

    void on_btn_replace_clicked();

    void on_btn_find_reg_clicked();

    void on_btn_replace_reg_clicked();

    void on_chb_useRegular_clicked();

private:
    Ui::SearchReplaceForm *ui;

    int64_t findStart=-1;
    int64_t findEnd=-1;

    int lastType=-1;
    QByteArray lastFindstr;
    int lastStat=-1;
    int64_t lastFindStart=-1;
    int64_t lastFindEnd=-1;
    int64_t lastCursorPos=-1;
    int64_t lastSelStart=-1;
    int64_t lastSelEnd=-1;
    int64_t lastModifyNum=-1;

public:

    void closeEvent(QCloseEvent *event) override;

private:

    std::atomic_bool stop=false;

};

#endif // SEARCHREPLACEFORM_H
