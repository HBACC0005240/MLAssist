#include "LuaCodeHighLighter.h"
#include <QtDebug>

LuaCodeHighLighter::LuaCodeHighLighter(QTextDocument *parent) :
		QSyntaxHighlighter(parent)
{

	//对于下面正则表达式，标记为紫色，类名称
	classFormat.setFontWeight(QFont::Bold);
	//classFormat.setForeground(Qt::darkMagenta);
	classFormat.setForeground(QColor(33, 110, 144));

	//函数标记为斜体蓝色
	//functionFormat.setFontItalic(true);
	functionFormat.setForeground(Qt::blue);
	customFunFormat.setForeground(Qt::darkCyan);//QColor(140,5,38));
	//customFunFormat.setForeground(Qt::darkBlue);//QColor(140,5,38));
	//customFunFormat.setForeground(Qt::cyan);//QColor(140,5,38));


	//字符串，标记深红色
	quotationFormat.setForeground(Qt::darkRed);
	
		//对于下面关键字部分，标记为深蓝色
	keywordFormat.setForeground(Qt::blue);
	//keywordFormat.setForeground(QColor(143, 8, 196));
	keywordFormat.setFontWeight(QFont::Bold);

		//对于下面正则表达式，单行注释标记为绿色
	singleLineCommentFormat.setForeground(Qt::darkGreen);
	//singleLineCommentFormat.setFontItalic(true);

}

LuaCodeHighLighter::~LuaCodeHighLighter()
{
}

	

void LuaCodeHighLighter::appendHighLightingFun(const QString &keywords)
{
	//单独颜色 170 57 107
	QString pattern = keywords	+"\\(";
	HighlightingRule rule;
	rule.pattern = QRegExp(pattern);
	rule.format = customFunFormat;
	highlightingRules.append(rule);
}

void LuaCodeHighLighter::initHighLighterRule()
{
	HighlightingRule rule;


	rule.pattern = QRegExp("\\b[A-Za-z]+:\\b");
	rule.format = classFormat;
	highlightingRules.append(rule);

	rule.pattern = QRegExp("\\b[A-Za-z]+\\.\\b");
	rule.format = classFormat;
	highlightingRules.append(rule);

	
	rule.pattern = QRegExp("\".*\"");
	rule.format = quotationFormat;
	highlightingRules.append(rule);
	rule.pattern = QRegExp("'.*'");
	rule.format = quotationFormat;
	highlightingRules.append(rule);

	
	//rule.pattern = QRegExp("\\b[A-Za-z0-9_]+(?=\\()");
	//rule.format = functionFormat;
	//highlightingRules.append(rule);

	//关键字
	QStringList keywords = {
		"and", "break", "do", "else", "elseif", "end", "false",
		"for", "function", "if", "in", "local", "nil", "not", "or",
		"repeat", "return", "then", "true", "unitl", "while", "goto"
	};

	QStringList keywordPatterns;
	for (int i = 0; i < keywords.length(); i++)
	{
		QString pattern = "\\b" + keywords[i] + "\\b";
		rule.pattern = QRegExp(pattern);
		rule.format = keywordFormat;
		highlightingRules.append(rule);
	}


	//rule.pattern = QRegExp("//[^\n]*");
	rule.pattern = QRegExp("--[^\n]*");
	rule.format = singleLineCommentFormat;
	highlightingRules.append(rule);

	//对于多行注释
	multiLineCommentFormat.setForeground(Qt::darkGreen);
	//multiLineCommentFormat.setFontItalic(true);
	//commentStartExpression = QRegExp("/\\*");
	//commentEndExpression = QRegExp("\\*/");
	//Lua 多行注释 --[[xx]]
	commentStartExpression = QRegExp("--\\[\\[");
	commentEndExpression = QRegExp("\\]\\]");
}

void LuaCodeHighLighter::highlightBlock(const QString &text)
{
	foreach (const HighlightingRule &rule, highlightingRules)
	{
		QRegExp expression(rule.pattern);
		int index = expression.indexIn(text);
		while (index >= 0)
		{
			int length = expression.matchedLength();
			setFormat(index, length, rule.format);
			index = expression.indexIn(text, index + length);
		}
	}

	setCurrentBlockState(0);

	int startIndex = 0;
	if (previousBlockState() != 1)
		startIndex = commentStartExpression.indexIn(text);

	while (startIndex >= 0)
	{
		int endIndex = commentEndExpression.indexIn(text, startIndex);
		int commentLength;
		if (endIndex == -1)
		{
			setCurrentBlockState(1);
			commentLength = text.length() - startIndex;
		}
		else
		{
			commentLength = endIndex - startIndex + commentEndExpression.matchedLength();
		}
		setFormat(startIndex, commentLength, multiLineCommentFormat);
		startIndex = commentStartExpression.indexIn(text, startIndex + commentLength);
	}
}