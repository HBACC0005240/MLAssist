#pragma once

#include <QSyntaxHighlighter>
#include <QTextCharFormat>
#include <QTextDocument>
struct HighlightingRule
{
	QRegExp pattern;
	QTextCharFormat format;
};
class LuaCodeHighLighter : public QSyntaxHighlighter
{
	Q_OBJECT

public:
	LuaCodeHighLighter(QTextDocument *parent = 0);
	~LuaCodeHighLighter();

	void appendHighLightingFun(const QString &keywords);
	void initHighLighterRule();
	QVector<HighlightingRule> GetHightLightRules() { return highlightingRules; }

protected:
	void highlightBlock(const QString &text) override;

private:
	QVector<HighlightingRule> highlightingRules;
	QStringList _keywordList;		//防止重复附加
	QRegExp commentStartExpression; //多行注释开始标识符
	QRegExp commentEndExpression;	//多行注释结束标识符

	QTextCharFormat keywordFormat;			 //关键字
	QTextCharFormat classFormat;			 //类
	QTextCharFormat singleLineKey;			 //单行关键字
	QTextCharFormat singleLineValue;		 //单行值
	QTextCharFormat singleLineCommentFormat; //单行注释
	QTextCharFormat multiLineCommentFormat;	 //多行注释
	QTextCharFormat quotationFormat;		 //字符串标识符
	QTextCharFormat functionFormat;			 //方法标识符
	QTextCharFormat customFunFormat;		 //自定义标识符
};
