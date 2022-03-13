#include "MApplication.h"
#include "stdafx.h"
#include <QDate>
#include <QDesktopServices>
#include <QFileInfo>
#include <QWidget>
#define TIME_OUT 500 /// (ms)
#define Drag_Epsilon 3
#define xstr(a) stringify(a)
#define stringify(a) #a

enum DragDirection
{
	Dir_None = 0,
	Dir_Top = 0x1,
	Dir_Bottom = 0x2,
	Dir_Left = 0x4,
	Dir_Right = 0x8,
	Dir_TopLeft = Dir_Top | Dir_Left,
	Dir_TopRight = Dir_Top | Dir_Right,
	Dir_BottomLeft = Dir_Bottom | Dir_Left,
	Dir_BottomRight = Dir_Bottom | Dir_Right,
};

MApplication::MApplication(int &argc, char **argv, int _internal) :
		QApplication(argc, argv, _internal),
		m_pMainWindow(NULL),
		m_uiDragSizeMode(Dir_None),
		m_bStartDragSize(false)
{
}
void MApplication::setWindowInstance(QWidget *wnd)
{
	m_pMainWindow = wnd;
}

unsigned int MApplication::getDragDirection()
{
	QWidget *activeWidget = qApp->activeWindow();
	unsigned int iFlag = Dir_None;

	if (activeWidget != nullptr && !activeWidget->isMaximized())
	{
		QPoint ptCursor = activeWidget->mapFromGlobal(QCursor::pos());

		int iRightX = ptCursor.x() - activeWidget->width();
		int iBottomY = ptCursor.y() - activeWidget->height();
		if (iBottomY < Drag_Epsilon && iBottomY > -Drag_Epsilon)
		{
			iFlag |= Dir_Bottom;
		}
		else if (ptCursor.y() < Drag_Epsilon && ptCursor.y() > -Drag_Epsilon)
		{
			iFlag |= Dir_Top;
		}

		if (iRightX < Drag_Epsilon && iRightX > -Drag_Epsilon)
		{
			iFlag |= Dir_Right;
		}
		else if (ptCursor.x() < Drag_Epsilon && ptCursor.x() > -Drag_Epsilon)
		{
			iFlag |= Dir_Left;
		}

		if (activeWindow() != NULL)
		{
			switch (iFlag)
			{
				case Dir_Top:
				case Dir_Bottom:
					if (overrideCursor() == NULL)
						setOverrideCursor(Qt::SizeVerCursor);
					else
						changeOverrideCursor(Qt::SizeVerCursor);
					break;
				case Dir_Left:
				case Dir_Right:
					if (overrideCursor() == NULL)
						setOverrideCursor(Qt::SizeHorCursor);
					else
						changeOverrideCursor(Qt::SizeHorCursor);
					break;
				case Dir_TopLeft:
				case Dir_BottomRight:
					if (overrideCursor() == NULL)
						setOverrideCursor(Qt::SizeFDiagCursor);
					else
						changeOverrideCursor(Qt::SizeFDiagCursor);
					break;
				case Dir_TopRight:
				case Dir_BottomLeft:
					if (overrideCursor() == NULL)
						setOverrideCursor(Qt::SizeBDiagCursor);
					else
						changeOverrideCursor(Qt::SizeBDiagCursor);
					break;
				default: restoreOverrideCursor(); break;
			}
		}
	}
	return iFlag;
}

bool MApplication::notify(QObject *pObj, QEvent *ev)
{
	QWidget *activeWidget = qApp->activeWindow();
	if (activeWidget != NULL)
	{
		switch (ev->type())
		{
				//快捷键
			case QEvent::KeyPress:
			{
				QKeyEvent *event = dynamic_cast<QKeyEvent *>(ev);
				if (event->key() == Qt::Key_F8)
				{
				}
				else if (event->key() == Qt::Key_Insert)
				{
					HWND gameHwnd = g_pGameCtrl->getGameHwnd();
					if (gameHwnd)
					{
						long szLng = GetWindowLong(gameHwnd, GWL_STYLE); //    '取的窗口原先的样式
						if (szLng & WS_MINIMIZE && szLng & WS_VISIBLE)
						{
							YunLai::SetWindowVal(gameHwnd, SW_RESTORE);
							SetForegroundWindow((HWND)m_pMainWindow->winId());						
						}						
						else
						{
							YunLai::SetWindowVal(gameHwnd, SW_SHOWMINIMIZED);
							SetForegroundWindow((HWND)m_pMainWindow->winId());
						}
						return true;
					}				
					
				}
				else if (event->key() == Qt::Key_F9)
				{
					/*QString path = qApp->applicationDirPath() + "\\Error\\Sync\\";
					QString errorFile = path + QDate::currentDate().toString(Qt::ISODate) + ".txt";
					if (QFile::exists(errorFile))
					{
						QDesktopServices::openUrl(QUrl::fromLocalFile(errorFile));
						return true;
					}*/
				}
				//保存工程快捷键
				else if ((event->modifiers() == Qt::ControlModifier) && (event->key() == Qt::Key_S))
				{
					/*MainWindow *mv = dynamic_cast<MainWindow *>(activeWidget);
					mv->save();*/
					return true;
				}
			}
			break;
			case QEvent::MouseButtonPress:
			{
				if (!m_bStartDragSize)
				{
					if (m_uiDragSizeMode != Dir_None)
					{
						m_bStartDragSize = true;
						m_StartPt = QCursor::pos();
						QPoint ptPos = activeWidget->pos();
						m_iPosX = ptPos.x();
						m_iPosY = ptPos.y();
						m_iOriginHeight = activeWidget->height();
						m_iOriginWidth = activeWidget->width();
						return true;
					}
				}
			}
			break;
			case QEvent::MouseButtonRelease:
			{
				if (m_bStartDragSize)
				{
					m_bStartDragSize = false;
					return true;
				}
			}
			break;
			case QEvent::MouseMove:
			{
				if (_bEnableDragSize)
				{
					if (m_bStartDragSize)
					{
						QPoint endPt = QCursor::pos();
						int iWidth = m_iOriginWidth, iHeight = m_iOriginHeight;
						int iPosX = m_iPosX, iPosY = m_iPosY;
						if (m_uiDragSizeMode & Dir_Top)
						{
							iHeight -= (endPt.y() - m_StartPt.y());
							if (iHeight >= activeWidget->minimumHeight()) iPosY += (endPt.y() - m_StartPt.y());
						}
						else if (m_uiDragSizeMode & Dir_Bottom)
							iHeight += (endPt.y() - m_StartPt.y());

						if (m_uiDragSizeMode & Dir_Left)
						{
							iWidth -= (endPt.x() - m_StartPt.x());
							if (iWidth >= activeWidget->minimumWidth()) iPosX += (endPt.x() - m_StartPt.x());
						}
						else if (m_uiDragSizeMode & Dir_Right)
							iWidth += (endPt.x() - m_StartPt.x());

						activeWidget->setGeometry(iPosX, iPosY, iWidth, iHeight);
						return true;
					}
					else
					{
						m_uiDragSizeMode = getDragDirection();
					}
				}
			}
			break;
			case QEvent::MouseButtonDblClick:
			{
				m_uiDragSizeMode = Dir_None;
				restoreOverrideCursor();
			}
			break;
			default: break;
		}
	}
	else
	{
		if (m_uiDragSizeMode != Dir_None)
		{
			m_uiDragSizeMode = Dir_None;
			restoreOverrideCursor();
		}
	}

	try
	{
		bool result = QApplication::notify(pObj, ev);
		return result;
	}
	catch (...)
	{
		qCritical("Something really bad happened!!!!!!!!!!!!!");
		return false;
	}
}
