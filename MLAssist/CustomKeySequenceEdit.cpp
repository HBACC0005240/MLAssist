#include "CustomKeySequenceEdit.h"

CustomKeySequenceEdit::CustomKeySequenceEdit(QWidget *parent)
	: QKeySequenceEdit(parent)
{
}

CustomKeySequenceEdit::~CustomKeySequenceEdit()
{
}



void CustomKeySequenceEdit::keyPressEvent(QKeyEvent *pEvent)
{
	QKeySequenceEdit::keyPressEvent(pEvent);

	QKeySequence seq(QKeySequence::fromString(keySequence().toString().split(", ").first()));
	setKeySequence(seq);
}