#pragma once

#include <QPainter>
#include <QProxyStyle>
#include <QTabWidget>

class ITTabBarStyle : public QProxyStyle
{
public:
	QSize sizeFromContents(ContentsType type, const QStyleOption *option, const QSize &size, const QWidget *widget) const;
	void drawControl(ControlElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget) const;
};