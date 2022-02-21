#ifndef ITSINALlPROXY_H
#define ITSINALlPROXY_H

#include <QObject>
#include <QKeySequence>
class ITSignalProxy : public QObject
{
	Q_OBJECT
public:
	ITSignalProxy(QObject *parent);
	~ITSignalProxy();
signals:
	void signal_TaskExecUpdatePlane();
	void signal_updateColor(ulong color,int x,int y);
	void signal_addOneDebugMsg(const QString& sMsg);

private:
	
};
extern ITSignalProxy *g_pSignalProxy;

#endif // ITSINALlPROXY_H
