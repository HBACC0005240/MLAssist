#include "LuaCodeEditor.h"
#include <QDebug>


LuaCodeEditor::LuaCodeEditor(QWidget *parent) :
		QPlainTextEdit(parent)
{
	lineNumberArea = new LineNumberArea(this);

	//事件绑定
	connect(this, SIGNAL(blockCountChanged(int)), this, SLOT(updateLineNumberAreaWidth(int)));
	connect(this, SIGNAL(updateRequest(QRect, int)), this, SLOT(updateLineNumberArea(QRect, int)));
	connect(this, SIGNAL(cursorPositionChanged()), this, SLOT(highlightCurrentLine()));

	updateLineNumberAreaWidth(0);
	setMode(EditorMode::BROWSE);
	setTabStopDistance(32);
	setFont(QFont(QString::fromUtf8("Source Code Pro"), 12));
}

 LuaCodeEditor::~LuaCodeEditor()
{
}

int LuaCodeEditor::lineNumberAreaWidth()
{
	int digits = 1;
	int max = qMax(1, blockCount());
	while (max >= 10)
	{
		max /= 10;
		++digits;
	}
	QFontMetrics metrics(QFont(QString::fromUtf8("Source Code Pro"), 12, QFont::Weight::Bold));
	int space = 10 + metrics.horizontalAdvance(QChar('9')) * digits;
	return space;
}

void LuaCodeEditor::updateLineNumberAreaWidth(int)
{
	setViewportMargins(lineNumberAreaWidth(), 0, 0, 0);
}

void LuaCodeEditor::updateLineNumberArea(const QRect &rect, int dy)
{
	if (dy)
		lineNumberArea->scroll(0, dy);
	else
		lineNumberArea->update(0, rect.y(), lineNumberArea->width(), rect.height());

	if (rect.contains(viewport()->rect()))
		updateLineNumberAreaWidth(0);
}

void LuaCodeEditor::resizeEvent(QResizeEvent *e)
{
	QPlainTextEdit::resizeEvent(e);

	QRect cr = contentsRect();
	lineNumberArea->setGeometry(QRect(cr.left(), cr.top(), lineNumberAreaWidth(), cr.height()));
}

void LuaCodeEditor::highlightCurrentLine()
{
	QList<QTextEdit::ExtraSelection> extraSelections;

	if (!isReadOnly())
	{
		QTextEdit::ExtraSelection selection;
		QColor lineColor = QColor("whitesmoke");
		selection.format.setBackground(lineColor);
		selection.format.setProperty(QTextFormat::FullWidthSelection, true);
		selection.cursor = textCursor();
		selection.cursor.clearSelection();
		extraSelections.append(selection);
	}
	setExtraSelections(extraSelections);
}

void LuaCodeEditor::lineNumberAreaPaintEvent(QPaintEvent *event)
{
	QPainter painter(lineNumberArea);
	painter.fillRect(event->rect(), Qt::lightGray);

	QTextBlock block = firstVisibleBlock();
	int blockNumber = block.blockNumber();
	int top = (int)blockBoundingGeometry(block).translated(contentOffset()).top();
	int bottom = top + (int)blockBoundingRect(block).height();

	while (block.isValid() && top <= event->rect().bottom())
	{
		if (block.isVisible() && bottom >= event->rect().top())
		{
			QString number = QString::number(blockNumber + 1);
			painter.setPen(Qt::black);
			painter.setFont(QFont(QString::fromUtf8("Source Code Pro"), 12, QFont::Weight::Bold));
			painter.drawText(-5, top, lineNumberArea->width(), fontMetrics().height(),
					Qt::AlignRight, number);
		}

		block = block.next();
		top = bottom;
		bottom = top + (int)blockBoundingRect(block).height();
		++blockNumber;
	}
}

void LuaCodeEditor::setMode(EditorMode mode)
{
	if (mode == EditorMode::BROWSE) //预览
	{
		this->setReadOnly(true);
		this->setStyleSheet("background: #f0f0f0;");
		highlightCurrentLine();
	}
	else if (mode == EditorMode::EDIT) //编辑
	{
		this->setReadOnly(false);
		this->setStyleSheet("background: #fcfcfc;");
		highlightCurrentLine();
	}
}