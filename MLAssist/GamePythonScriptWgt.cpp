#include "GamePythonScriptWgt.h"
//#include "python.h"
#include <QFileDialog>
#include "CGFunction.h"
//#ifdef _DEBUG
//#pragma  comment(lib,"../lib/pythonqt/PythonQt-Qt5-Python39_d.lib")
//#pragma  comment(lib,"../lib/pythonqt/PythonQt_QtAll-Qt5-Python39_d.lib")
//#else
//#pragma  comment(lib,"../lib/pythonqt/PythonQt-Qt5-Python39.lib")
//#pragma  comment(lib,"../lib/pythonqt/PythonQt_QtAll-Qt5-Python39.lib")
//#endif // DEBUG


#include <QHBoxLayout>
GamePythonScriptWgt::GamePythonScriptWgt(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	//Py_Initialize();
	// init PythonQt and Python
	//PythonQt::init();
	//PythonQt_QtAll::init();
	//// get the __main__ python module
	//m_pythonMainModule = PythonQt::self()->getMainModule();
	//m_pPythonConsole = new PythonQtScriptingConsole(nullptr, m_pythonMainModule);
	/*QHBoxLayout* pHLayout = new QHBoxLayout(ui.widget_python);
	pHLayout->addWidget(m_pPythonConsole);
	ui.widget_python->setLayout(pHLayout);
	m_pythonMainModule.addObject("cg", g_pGameFun);*/
}

GamePythonScriptWgt::~GamePythonScriptWgt()
{
//	Py_Finalize();
}

void GamePythonScriptWgt::on_pushButton_load_clicked()
{
	QString szPath = QFileDialog::getOpenFileName(this, QString::fromLocal8Bit("打开"), "./", "*.py");//*.script;
	if (szPath.isEmpty())
		return;
	//PythonQt::cleanup();
	//PythonQt::init();
	//PythonQt_QtAll::init();
	//// get the __main__ python module
	//m_pythonMainModule = PythonQt::self()->getMainModule();
	//m_pPythonConsole = new PythonQtScriptingConsole(ui.widget_python, m_pythonMainModule);
	//m_pythonMainModule.addObject("cg", g_pGameFun);

	//QFile file(szPath);
	//if (file.open(QIODevice::ReadOnly)) 
	//{
	//	int length = file.size();
	//	QTextStream readstream(&file);    //构建文本流
	//	QTextCodec* codec = QTextCodec::codecForName("UTF-8"); //设置编码
	//	readstream.setCodec(codec);
	////	QString strdata = readstream.readAll();  //读取数据进入流

	//	while (!file.atEnd())
	//	{
	//	
	//		QByteArray line = file.readLine();
	//		qDebug() << line;

	//		m_pythonMainModule.evalScript(line);
	//	}
	//	file.close();
	//}
	/*m_scriptCode =	PythonQt::self()->parseFile(szPath);
	if (m_scriptCode)
	{
		qDebug() <<"Number of items in variable part" <<  m_scriptCode->ob_type->ob_base.ob_size << "Name" << m_scriptCode->ob_type->tp_name
			<< m_scriptCode->ob_type->tp_itemsize;
		PythonQt::self()->evalCode(m_pythonMainModule.object(), m_scriptCode);
	}*/
	/*FILE* fp = fopen(szPath.toStdString().c_str(), "r");
	while (!m_bStop && !PyRun_InteractiveOne(fp, szPath.toStdString().c_str()))
	{

	}
	qDebug() << "退出脚本";*/
	//qDebug() << tmpPython->ob_type->tp_str;
	//m_pythonMainModule.evalFile(szPath);
	//auto pPythonObj = m_pythonMainModule.object();
	//pPythonObj->ob_type->;
}

void GamePythonScriptWgt::on_pushButton_stop_clicked()
{
	//m_pythonMainModule.setNewRef(nullptr);
	//PythonQt::cleanup();
	//if (m_scriptCode)
	//	Py_DECREF(m_scriptCode);
	m_bStop = true;
}
