#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "contentwidget.h"

#include <QComboBox>
#include <QMainWindow>
#include <QSessionManager>


class MainWindow : public QMainWindow
{
    Q_OBJECT
private:
    MainWindow();

public:

    static MainWindow* instance();

    static ContentWidget* getContent();

    QueryForm* loadFile(QString fileName,const QByteArray codec="",const bool logHis=true);
    void setCurrentTitle(const QString &fileName);
    bool save();

    void setStatus(const QString &text, int timeout = 0);

    QAction *saveAct;
    QMenu *connMenu;

    QLabel codecLabel;
    QLabel statusLabel;
    QLabel sqlLabel;
    void clearStatusLabels();

    bool qf_defaultCalcRowCout;
    bool td_defaultCalcRowCout;

    int defaultPageSize;

    QString defaultFontFamily;
    int defaultTheme;

    int defaultFontSize;

    bool defaultUseCodeCompletion;

    bool defaultShowLineNumber;

    bool defaultUseCodeFloding;

    bool defaultUseSyntaxHighlighting;

    bool defaultUseCurrLineHL;

    int defaultDisableIfFileLargerMib;

    QString defaultQfFontFamily;

    int defaultQfFontSize;

    int defaultQfTabWidth;

    QString defaultQfColorCommon;
    QString defaultQfColorKeyword;
    QString defaultQfColorString;
    QString defaultQfColorNumber;
    QString defaultQfColorComment;
    QString defaultQfColorBackground;

    QString defaultLanguage;

    int getIconThumbSize();

    QString getAppQfFontFamily();

    QSharedPointer<QSettings> loadConfig(bool useDefault=false);
    void loadQSS();

    void loadTranslator();

    QRect getGuiFontRect(const QString &text,const QWidget * wid=nullptr);
    QRect getCharGuiFontRect();

signals:
    void inited();

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    void newFile();
    void open();
    bool saveAs();
    void about();

#ifndef QT_NO_SESSIONMANAGER
    void commitData(QSessionManager &);
#endif

private:
    void createActions();
    void createStatusBar();
    void readSettings(QSharedPointer<QSettings> ini);
    void writeSettings();
    bool maybeSave();
    bool saveFile(const QString &fileName,QByteArray saveCodec="");
    QString strippedName(const QString &fullFileName);

    ContentWidget* content;
};

#endif // MAINWINDOW_H
