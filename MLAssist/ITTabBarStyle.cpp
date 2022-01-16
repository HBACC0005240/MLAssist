#include "ITTabBarStyle.h"
#include <QStyleOption>

QSize ITTabBarStyle::sizeFromContents(QStyle::ContentsType type, const QStyleOption *option, const QSize &size, const QWidget *widget) const
{
	QSize s = QProxyStyle::sizeFromContents(type, option, size, widget);
	return s;
}

void ITTabBarStyle::drawControl(QStyle::ControlElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget) const
{
	if (element == CE_TabBarTabLabel)
	{
		if (const QStyleOptionTab *tab = qstyleoption_cast<const QStyleOptionTab *>(option))
		{
			QRect allRect = tab->rect;
	/*		if (tab->state & QStyle::State_Selected)
			{
				painter->save();
				painter->setPen(Qt::blue);
				painter->setBrush(QBrush(Qt::transparent));
				painter->drawRect(allRect.adjusted(1, 1, -1, -1));
				painter->restore();
			}
			else if (tab->state)
			{
				painter->save();
				painter->setPen(Qt::black);
				painter->setBrush(QBrush(Qt::transparent));
				painter->drawRect(allRect.adjusted(1, 1, -1, -1));
				painter->restore();
			}*/
			QTextOption option1;
			option1.setAlignment(Qt::AlignCenter);
		/*	if (tab->state & QStyle::State_Selected)
			{
				painter->setPen(Qt::white);
			}
			else
			{
				painter->setPen(Qt::lightGray);
			}*/
			painter->drawText(allRect, tab->text, option1);
			return;
		}
	}	
	QProxyStyle::drawControl(element, option, painter, widget);
}
