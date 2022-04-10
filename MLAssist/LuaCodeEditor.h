#pragma once

#include <QPlainTextEdit>
#include <QObject>
#include <QPaintEvent>
#include <QPainter>
#include <QResizeEvent>
#include <QSize>
#include <QSyntaxHighlighter>
#include <QWidget>

typedef enum
{
	BROWSE,
	EDIT,
} EditorMode;

class LuaCodeEditor : public QPlainTextEdit
{
	Q_OBJECT

public:
	LuaCodeEditor(QWidget *parent = 0);
	~LuaCodeEditor();

	void setMode(EditorMode mode);
	void lineNumberAreaPaintEvent(QPaintEvent *event);
	int lineNumberAreaWidth();

protected:
	void resizeEvent(QResizeEvent *e) override;

private slots:
	void updateLineNumberAreaWidth(int newBlockCount);
	void highlightCurrentLine();
	void updateLineNumberArea(const QRect &rect, int dy);

private:
	QWidget *lineNumberArea;
};
class LineNumberArea : public QWidget
{
public:
	LineNumberArea(LuaCodeEditor *editor) :
			QWidget(editor)
	{
		codeEditor = editor;
	}
	QSize sizeHint() const override
	{
		return QSize(codeEditor->lineNumberAreaWidth(), 0);
	}

protected:
	void paintEvent(QPaintEvent *event) override
	{
		codeEditor->lineNumberAreaPaintEvent(event);
	}

private:
	LuaCodeEditor *codeEditor;
};

