#ifndef MApplication_H
#define MApplication_H

#include <QApplication>
#include <QKeyEvent>
#include <QWidget>
#include <QtNetwork/QLocalServer>
#include <QtNetwork/QLocalSocket>

class MApplication : public QApplication
{
	Q_OBJECT

public:
	MApplication(int &argc, char **argv, int _internal = ApplicationFlags);
	~MApplication() {}

	bool notify(QObject *pObj, QEvent *ev);
	void setWindowInstance(QWidget *wnd);
	QWidget *getWindowInstance() { return m_pMainWindow; }

private:
	unsigned int getDragDirection();

private:
	int m_iPosX;
	int m_iPosY;
	QPoint m_StartPt;
	int m_iOriginWidth;
	int m_iOriginHeight;
	unsigned int m_uiDragSizeMode;
	bool _bEnableDragSize = false;
	bool m_bStartDragSize;
	QWidget *m_pMainWindow=nullptr; //保存一个你窗体的指针
};

#endif // MApplication_H
