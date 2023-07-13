#include "mainwindow.h"
#include "sqlstyle.h"

SqlStyle::SqlStyle()
{

}

void fontComment(MyEdit *myEdit,int style){
//    myEdit->styleSetFont(style,"Georgia");
//    myEdit->styleSetSize(style,int(MainWindow::instance()->defaultQfFontSize*0.8));
}

void fontMonospace(MyEdit *myEdit,int style){
//    myEdit->styleSetFont(style,"Consolas");
//    myEdit->styleSetSize(style,int(MainWindow::instance()->defaultQfFontSize*0.8));
}

void colourPreproc(MyEdit *myEdit,int style){
    myEdit->styleSetFore(style,0x7F7F00);
}

void setSqlLexer(MyEdit *myEdit){

    myEdit->setLexer(SCLEX_SQL);
//    myEdit->setProperty("lexer.sql.numbersign.comment","1");//1:以'#'开头的行将是注释
//    myEdit->setProperty("sql.backslash.escapes","1");//1:在SQL中启用反斜杠作为转义字符
//    myEdit->setProperty("lexer.sql.allow.dotted.word","1");
    myEdit->setKeyWords(0,keyworks_sql.constData());
    myEdit->setKeyWords(1,keyworks_plsql.constData());
}

void SqlStyle::setStyle(MyEdit *myEdit)
{

    int colorCommon=MainWindow::instance()->defaultQfColorCommon.toInt(nullptr,16);
    int colorKeyword=MainWindow::instance()->defaultQfColorKeyword.toInt(nullptr,16);
    int colorString=MainWindow::instance()->defaultQfColorString.toInt(nullptr,16);
    int colorNumber=MainWindow::instance()->defaultQfColorNumber.toInt(nullptr,16);
    int colorComment=MainWindow::instance()->defaultQfColorComment.toInt(nullptr,16);
    int colorBackground=MainWindow::instance()->defaultQfColorBackground.toInt(nullptr,16);

//    myEdit->setIdleStyling(1);

    myEdit->styleResetDefault();
    myEdit->styleSetSize(STYLE_DEFAULT,MainWindow::instance()->defaultQfFontSize);
    myEdit->styleSetFont(STYLE_DEFAULT,MainWindow::instance()->defaultQfFontFamily.toUtf8());//Courier New,Consolas
    myEdit->setStyleSheet(QString{" MyEdit{font-family:\"%1\";}"}.arg(MainWindow::instance()->defaultQfFontFamily));
    myEdit->styleSetBack(STYLE_DEFAULT,colorBackground);//F7F8F8,F0F0F0
    myEdit->styleSetFore(STYLE_DEFAULT,colorCommon);
    myEdit->styleClearAll();

    myEdit->styleSetFore(SCE_SQL_DEFAULT,colorCommon);
    myEdit->styleSetFore(SCE_SQL_COMMENT,colorComment);//0x929292,0x007F00,0x7F7F7F
    fontComment(myEdit,SCE_SQL_COMMENT);
    myEdit->styleSetFore(SCE_SQL_COMMENTLINE,colorComment);
    fontComment(myEdit,SCE_SQL_COMMENTLINE);
    myEdit->styleSetFore(SCE_SQL_COMMENTDOC,colorComment);
    myEdit->styleSetFore(SCE_SQL_NUMBER,colorNumber);
    myEdit->styleSetFore(SCE_SQL_WORD,colorKeyword);
    myEdit->styleSetBold(SCE_SQL_WORD,true);
    myEdit->styleSetFore(SCE_SQL_STRING,colorString);
    fontMonospace(myEdit,SCE_SQL_STRING);
    myEdit->styleSetFore(SCE_SQL_CHARACTER,colorString);
    fontMonospace(myEdit,SCE_SQL_CHARACTER);

    myEdit->styleSetBold(SCE_SQL_OPERATOR,true);
    myEdit->styleSetFore(SCE_SQL_IDENTIFIER,colorCommon);
    myEdit->styleSetFore(SCE_SQL_SQLPLUS_COMMENT,colorComment);
    fontComment(myEdit,SCE_SQL_SQLPLUS_COMMENT);
    myEdit->styleSetFore(SCE_SQL_COMMENTLINEDOC,colorComment);
    fontComment(myEdit,SCE_SQL_COMMENTLINEDOC);

    myEdit->styleSetFore(SCE_SQL_WORD2,colorKeyword);
    myEdit->styleSetBold(SCE_SQL_WORD2,true);

//    myEdit->styleSetFore(SCE_SQL_USER1,0x4B0082);
//    myEdit->styleSetFore(SCE_SQL_USER2,0xB00040);
//    myEdit->styleSetFore(SCE_SQL_USER3,0x8B0000);
//    myEdit->styleSetFore(SCE_SQL_USER4,0x800080);
//    myEdit->styleSetFore(SCE_SQL_QUOTEDIDENTIFIER,0xDD9900);
//    myEdit->styleSetFore(SCE_SQL_QOPERATOR,0xEE00AA);

    myEdit->setTabWidth(MainWindow::instance()->defaultQfTabWidth);
    myEdit->setUseTabs(false);//使用空格替代Tab
    myEdit->setBackSpaceUnIndents(true);//退格时按照一个Tab对应的空格数回退
    myEdit->setCodePage(SC_CP_UTF8);//设置编辑器使用UTF8编码

    if(MainWindow::instance()->defaultTheme==THEME_DARK){

        myEdit->styleSetBack(STYLE_BRACELIGHT,0xB07000);//括号匹配高亮

        myEdit->setSelBack(true,0x214283);//选中时背景色315293
        myEdit->setCaretFore(0xE0E0E0);//设置光标颜色

        if(MainWindow::instance()->defaultUseCurrLineHL&&myEdit->useCurrLineHL){
            myEdit->setCaretLineVisible(true);//设置当前行高亮
            myEdit->setCaretLineBack(0x333333);
        }else{
            myEdit->setCaretLineVisible(false);
        }

    }else{

        myEdit->styleSetBack(STYLE_BRACELIGHT,0x99FF99);//括号匹配高亮,FFEF0B,99FF99,99CCFF

        myEdit->setSelBack(true,0xBEDCFF);//CFCFCF,3399FF,A6CAF0,99CCFF,BEDCFA
        myEdit->setCaretFore(0x0);//设置光标颜色

        if(MainWindow::instance()->defaultUseCurrLineHL&&myEdit->useCurrLineHL){
            myEdit->setCaretLineVisible(true);//设置当前行高亮
            if(MainWindow::instance()->defaultTheme==THEME_GREEN_LIGHT){
                myEdit->setCaretLineBack(0xE4E5EA);
            }else{
                myEdit->setCaretLineBack(0xFFFAE3);//FFFAE3,E4E5EA
            }
        }else{
            myEdit->setCaretLineVisible(false);
        }

    }
    myEdit->setCaretWidth(2);

    //margin:0用来显示行号,但其宽度默认为0。margin:1用来显示非折叠的符号,默认宽度为16个像素。margin:2用来显示折叠符号,默认情况下宽度也是0(隐藏)。
    myEdit->setMarginWidthN(1,0);//隐藏margin:1默认宽度

//    set MARGIN_LINE_NUMBER
    if(myEdit->useLineNumber&&MainWindow::instance()->defaultShowLineNumber){
        myEdit->setMarginTypeN(MARGIN_LINE_NUMBER_INDEX,SC_MARGIN_NUMBER);
        if(MainWindow::instance()->defaultTheme==THEME_DARK){

            myEdit->styleSetBack(STYLE_LINENUMBER,0x3C3F41);
            myEdit->styleSetFore(STYLE_LINENUMBER,0xBBBBBB);
        }else{

            myEdit->styleSetBack(STYLE_LINENUMBER,0xF0F0F0);//F7F8F8,F0F0F0
            myEdit->styleSetFore(STYLE_LINENUMBER,0x7C7C7C);//ACACAC,7C7C7C,237893
        }
        setMarginLineNumberWidth(myEdit);
    }else{
        myEdit->setMarginWidthN(MARGIN_LINE_NUMBER_INDEX,0);
    }

    if(myEdit->useLexer&&MainWindow::instance()->defaultUseSyntaxHighlighting){

        setSqlLexer(myEdit);

//        set MARGIN_FOLD
        setMarginFlod(myEdit);
    }

    setResultIndicStyle(myEdit);
}

void SqlStyle::setMarginLineNumberWidth(MyEdit *myEdit)
{

    if(myEdit->useLineNumber&&MainWindow::instance()->defaultShowLineNumber){
        int lines=myEdit->lineCount();
        lines=1+std::log10(lines);
        lines = std::max(lines, 3);
        int charWidth=myEdit->textWidth(STYLE_LINENUMBER,"M");
        auto pixelWidth = 12 + lines * charWidth;
        myEdit->setMarginWidthN(MARGIN_LINE_NUMBER_INDEX,pixelWidth);

    }
}

void SqlStyle::setMarginFlod(MyEdit *myEdit)
{
    if(!MainWindow::instance()->defaultUseCodeFloding){
        myEdit->setMarginWidthN(MARGIN_FOLD_INDEX, 0);//页边宽度
        return;
    }

    myEdit->setProperty("fold","1");

    myEdit->setMarginTypeN(MARGIN_FOLD_INDEX, SC_MARGIN_SYMBOL);//页边类型
    myEdit->setMarginMaskN(MARGIN_FOLD_INDEX, SC_MASK_FOLDERS);//页边掩码
    myEdit->setMarginWidthN(MARGIN_FOLD_INDEX, 3 * Utils::getCharGuiFontRect().width());//页边宽度
    myEdit->setMarginSensitiveN(MARGIN_FOLD_INDEX,true);//响应鼠标消息

    // 折叠标签样式
    myEdit->markerDefine(SC_MARKNUM_FOLDER, SC_MARK_BOXPLUS);
    myEdit->markerDefine(SC_MARKNUM_FOLDEROPEN, SC_MARK_BOXMINUS);
    myEdit->markerDefine(SC_MARKNUM_FOLDEREND,  SC_MARK_BOXPLUSCONNECTED);
    myEdit->markerDefine(SC_MARKNUM_FOLDEROPENMID, SC_MARK_BOXMINUSCONNECTED);
    myEdit->markerDefine(SC_MARKNUM_FOLDERMIDTAIL, SC_MARK_TCORNERCURVE);
    myEdit->markerDefine(SC_MARKNUM_FOLDERSUB, SC_MARK_VLINE);
    myEdit->markerDefine(SC_MARKNUM_FOLDERTAIL, SC_MARK_LCORNERCURVE);

    myEdit->setFoldFlags(SC_FOLDFLAG_LINEAFTER_CONTRACTED); //如果折叠就在折叠行的下面画一条横线

    if(MainWindow::instance()->defaultTheme==THEME_DARK){

        myEdit->setFoldMarginColour(true,0x3C3F41);
        myEdit->setFoldMarginHiColour(true,0x3C3F41);

        // 折叠标签颜色
        myEdit->markerSetBack(SC_MARKNUM_FOLDER, 0xE0E0E0);
        myEdit->markerSetBack(SC_MARKNUM_FOLDEROPEN, 0xE0E0E0);
        myEdit->markerSetBack(SC_MARKNUM_FOLDEREND, 0xE0E0E0);
        myEdit->markerSetBack(SC_MARKNUM_FOLDEROPENMID, 0xE0E0E0);

        myEdit->markerSetFore(SC_MARKNUM_FOLDER, 0x3C3F41);
        myEdit->markerSetFore(SC_MARKNUM_FOLDEROPEN, 0x3C3F41);
        myEdit->markerSetFore(SC_MARKNUM_FOLDEREND, 0x3C3F41);
        myEdit->markerSetFore(SC_MARKNUM_FOLDEROPENMID, 0x3C3F41);

        myEdit->markerSetBack(SC_MARKNUM_FOLDERSUB, 0xE0E0E0);
        myEdit->markerSetBack(SC_MARKNUM_FOLDERMIDTAIL, 0xE0E0E0);
        myEdit->markerSetBack(SC_MARKNUM_FOLDERTAIL, 0xE0E0E0);
    }else{

        myEdit->setFoldMarginColour(true,0xF0F0F0);
        myEdit->setFoldMarginHiColour(true,0xF0F0F0);

        // 折叠标签颜色
        myEdit->markerSetBack(SC_MARKNUM_FOLDER, 0x0);
        myEdit->markerSetBack(SC_MARKNUM_FOLDEROPEN, 0x0);
        myEdit->markerSetBack(SC_MARKNUM_FOLDEREND, 0x0);
        myEdit->markerSetBack(SC_MARKNUM_FOLDEROPENMID, 0x0);

        myEdit->markerSetFore(SC_MARKNUM_FOLDER, 0xF0F0F0);
        myEdit->markerSetFore(SC_MARKNUM_FOLDEROPEN, 0xF0F0F0);
        myEdit->markerSetFore(SC_MARKNUM_FOLDEREND, 0xF0F0F0);
        myEdit->markerSetFore(SC_MARKNUM_FOLDEROPENMID, 0xF0F0F0);

        myEdit->markerSetBack(SC_MARKNUM_FOLDERSUB, 0x0);
        myEdit->markerSetBack(SC_MARKNUM_FOLDERMIDTAIL, 0x0);
        myEdit->markerSetBack(SC_MARKNUM_FOLDERTAIL, 0x0);
    }
}

void SqlStyle::clearResultIndic(MyEdit *myEdit)
{
    myEdit->setIndicatorCurrent(INDIC_RESULT_SQL);
    myEdit->indicatorClearRange(0,myEdit->length());

}

void SqlStyle::setResultIndic(MyEdit *myEdit, int64_t start, int64_t len)
{
    int64_t startline=myEdit->lineFromPosition(start);
//    int64_t endline=myEdit->lineFromPosition(start+len);
    int64_t screenstartline=myEdit->firstVisibleLine();
    int64_t screenlines=myEdit->linesOnScreen();
    if(startline<screenstartline||startline>screenstartline+screenlines){
        myEdit->setFirstVisibleLine(startline);
    }

    clearResultIndic(myEdit);
    myEdit->setIndicatorCurrent(INDIC_RESULT_SQL);
    myEdit->indicatorFillRange(start,len);

}

void SqlStyle::clearErrorIndic(MyEdit *myEdit)
{
    myEdit->setIndicatorCurrent(INDIC_ERROR_STR);
    myEdit->indicatorClearRange(0,myEdit->length());
}

void SqlStyle::setErrorIndic(MyEdit *myEdit, int64_t start, int64_t len)
{
    int64_t startline=myEdit->lineFromPosition(start);
//    int64_t endline=myEdit->lineFromPosition(start+len);
    int64_t screenstartline=myEdit->firstVisibleLine();
    int64_t screenlines=myEdit->linesOnScreen();
    if(startline<screenstartline||startline>screenstartline+screenlines){
        myEdit->setFirstVisibleLine(startline);
    }

    clearErrorIndic(myEdit);
    myEdit->setIndicatorCurrent(INDIC_ERROR_STR);
    myEdit->indicatorFillRange(start,len);

}

void SqlStyle::clearFindIndic(MyEdit *myEdit)
{
    myEdit->setIndicatorCurrent(INDIC_FIND_STR);
    myEdit->indicatorClearRange(0,myEdit->length());

}

void SqlStyle::setFindIndic(MyEdit *myEdit, int64_t start, int64_t len)
{
    int64_t startline=myEdit->lineFromPosition(start);
//    int64_t endline=myEdit->lineFromPosition(start+len);
    int64_t screenstartline=myEdit->firstVisibleLine();
    int64_t screenlines=myEdit->linesOnScreen();
    if(startline<screenstartline||startline>screenstartline+screenlines){
        myEdit->setFirstVisibleLine(startline);
    }

    clearFindIndic(myEdit);
    myEdit->setIndicatorCurrent(INDIC_FIND_STR);
    myEdit->indicatorFillRange(start,len);

}

void SqlStyle::setResultIndicStyle(MyEdit *myEdit)
{

    if(MainWindow::instance()->defaultTheme==THEME_DARK){

        myEdit->indicSetStyle(INDIC_RESULT_SQL,INDIC_ROUNDBOX);
        myEdit->indicSetFore(INDIC_RESULT_SQL, 0x7F7F7F);
        myEdit->indicSetAlpha(INDIC_RESULT_SQL,30);
        myEdit->indicSetOutlineAlpha(INDIC_RESULT_SQL,225);

        myEdit->indicSetStyle(INDIC_FIND_STR,INDIC_ROUNDBOX);
        myEdit->indicSetFore(INDIC_FIND_STR, 0x0030A0);//查找到的字符串背景色

    }else{
        myEdit->indicSetStyle(INDIC_RESULT_SQL,INDIC_ROUNDBOX);
        myEdit->indicSetFore(INDIC_RESULT_SQL, 0x77AAEE);//3399FF,A6CAF0,99CCFF,77AAEE
        myEdit->indicSetAlpha(INDIC_RESULT_SQL,10);
        myEdit->indicSetOutlineAlpha(INDIC_RESULT_SQL,205);

        myEdit->indicSetStyle(INDIC_FIND_STR,INDIC_ROUNDBOX);
        myEdit->indicSetFore(INDIC_FIND_STR, 0x40FF40);//查找到的字符串背景色
    }

    myEdit->indicSetStyle(INDIC_ERROR_STR,INDIC_SQUIGGLE);
    myEdit->indicSetFore(INDIC_ERROR_STR, 0xFF0000);//设置出错字符背景色
}
