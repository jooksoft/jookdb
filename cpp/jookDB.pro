#-------------------------------------------------
#
# Project created by QtCreator 2019-07-29T11:12:16
#
#-------------------------------------------------

QT       += core gui printsupport network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = jookdb
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++17

!win32{
    QMAKE_CXXFLAGS += -Wno-narrowing
}

CONFIG(release, debug|release) {
}

VERSION = 5.24.0
DEFINES += VERSION_STR=\\\"$$VERSION\\\"

QMAKE_TARGET_PRODUCT = jookdb
QMAKE_TARGET_COMPANY = jooksoft

win32{
    RC_ICONS = $$PWD/images/icon/jookdb.ico
}
macx{
    ICON = $$PWD/images/icon/jookdb.icns
}

CONFIG -= debug_and_release

TRANSLATIONS += language/en.ts \
                language/zh_cn.ts

#include(cryptopp.pri)

SOURCES += \
        aboutdialog.cpp \
        activatedialog.cpp \
        attachdbdialog.cpp \
        autocompleteform.cpp \
        btnfuncdelegate.cpp \
        checkboxdelegate.cpp \
        cmask.cpp \
        comboboxdelegate.cpp \
        conndialog.cpp \
        contentwidget.cpp \
        copytabledialog.cpp \
        customcheckbox.cpp \
        databasemanagerform.cpp \
        datasyncdialog.cpp \
        datetimeeditdelegate.cpp \
        datetimeeditform.cpp \
        dbobjectselectdialog.cpp \
        driverdownloadform.cpp \
        editdatabasedialog.cpp \
        flowlayout.cpp \
        importdialog.cpp \
        jarinfoeditdialog.cpp \
        jarsmanagerform.cpp \
        leftwidgetform.cpp \
        lineeditdelegate.cpp \
        listwidgetdelegate.cpp \
        main.cpp \
        mainwindow.cpp \
        manualactivationdialog.cpp \
        mutichkform.cpp \
        mystyle.cpp \
        objectmanagerform.cpp \
        objtimecache.cpp \
        passwordchgdialog.cpp \
        passwordinputdialog.cpp \
        readonlydelegate.cpp \
        reversecall.cpp \
        runsqldialog.cpp \
        searchreplaceform.cpp \
        spinboxdelegate.cpp \
        sqlstyle.cpp \
        strmappingdialog.cpp \
        test.cpp \
        texteditdialog.cpp \

HEADERS += \
        aboutdialog.h \
        attachdbdialog.h \
        btnfuncdelegate.h \
        checkboxdelegate.h \
        cmask.h \
        comboboxdelegate.h \
        conndialog.h \
        contentwidget.h \
        copytabledialog.h \
        customcheckbox.h \\
        databasemanagerform.h \
        datetimeeditdelegate.h \
        datetimeeditform.h \
        dbobjectselectdialog.h \
        driverdownloadform.h \
        editdatabasedialog.h \
        flowlayout.h \
        fulltextsearchform.h \
        generatedpropeditdialog.h \
        jarinfoeditdialog.h \
        jarsmanagerform.h \
        leftwidgetform.h \
        lineeditdelegate.h \
        listwidgetdelegate.h \
        mainwindow.h \
        manualactivationdialog.h \
        mutichkform.h \
        myedit.h \
        mystyle.h \
        objtimecache.h \
        optionsdialog.h \
        passwordchgdialog.h \
        passwordinputdialog.h \
        readonlydelegate.h \
        reversecall.h \
        runsqldialog.h \
        searchreplaceform.h \
        spinboxdelegate.h \
        sqlstyle.h \
        strmappingdialog.h \
        structsyncdialog.h \
        test.h \
        texteditdialog.h \

RESOURCES = resources.qrc

#INCLUDEPATH += $$(BOOST_HOME)
#!build_pass:message($$(BOOST_HOME))

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES +=

FORMS += \
    aboutdialog.ui \
    activatedialog.ui \
    attachdbdialog.ui \
    autocompleteform.ui \
    conndialog.ui \
    copytabledialog.ui \
    databasedefinitionexoprtdialog.ui \
    databasemanagerform.ui \
    datasyncdialog.ui \
    datetimeeditform.ui \
    dbobjectselectdialog.ui \
    driverdownloadform.ui \
    editdatabasedialog.ui \
    exportdialog.ui \
    importdialog.ui \
    jarinfoeditdialog.ui \
    jarsmanagerform.ui \
    leftwidgetform.ui \
    manualactivationdialog.ui \
    mutichkform.ui \
    optionsdialog.ui \
    passwordchgdialog.ui \
    passwordinputdialog.ui \
    searchreplaceform.ui \
    strmappingdialog.ui \
    structsyncdialog.ui \
    usermanagerdialog.ui

