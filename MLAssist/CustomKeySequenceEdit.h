#pragma once

#include <QKeySequenceEdit>
class QKeyEvent;
class CustomKeySequenceEdit : public QKeySequenceEdit
{
	Q_OBJECT

public:
	explicit CustomKeySequenceEdit(QWidget *parent = 0);
	~CustomKeySequenceEdit();

protected:
	void keyPressEvent(QKeyEvent *pEvent);
};