#pragma once

#include "../ThirdPart/libxl/include_cpp/libxl.h"
#include <QObject>
#include <QString>
using namespace libxl;
enum ExportState
{
	EXPORT_SUCCESS,
	EXPORT_NOTWRITING,
	EXPORT_ERRSAVE
};

class ITExcel : public QObject
{
	Q_OBJECT

public:
	ITExcel();
	~ITExcel();

	libxl::Book *createXLSBook(bool isXls);

	Qt::Alignment XLSHorzAlignmentToQtAlignment(libxl::AlignH align);
	Qt::Alignment XLSVertAlignmentToQtAlignment(libxl::AlignV align);
	Qt::Alignment XLSAlignmentToQtAlignment(libxl::AlignH horzAlignment, libxl::AlignV vertAlignment);
	libxl::AlignV QtAlignmentToXLSVertAlignment(Qt::Alignment align);
	libxl::AlignH QtAlignmentToXLSHorzAlignment(Qt::Alignment align);
	/*
 * libxl官方提供的 字体转换像素: px = ([100*{width}-0.5]*{Maximum Digit Width})/100 + 5,
 * {Maximum Digit Width} = 7 for a default font.
*/
	int extraSpace(int maxinumDigitWidth);
	double pixelToDigitWidth(double pixel, int maxinumDigitWidth);
	double digitWidthToPixel(double digitWidth, int maxinumDigitWidth);
	/*
 * libxl官方提供的在Windows下字体宽度的计算方法。
*/
	int calcMaximumDigitWidth(const wchar_t *fontFamily, int fontPointSize);

	Book *loadxlsFile(const QString &fileName);
	bool load(const QString &fileName);
	bool save(const QString &fileName);
	bool setSheetIndex(int index);
	bool exportExcel();

	bool editable() const;
	void setEditable(bool value);
	libxl::Book *book() const;
	libxl::Book *bakBook() const;
	void setXlsBook(libxl::Book *oBook);
	void setXlsBakBook(libxl::Book *oBakBook);
	libxl::Sheet *activeSheet() const;
	QString getSheetName() const;

	/*!
     *@brief releaseBook 给用户提供接口，使其可以释放当前的book.libxl内部有创建book的上限，达到这个上限会崩溃。
     * 适用场景：当需要load多次文件时，用户可以主动去释放前一次load产生的Book.
     *@return void
     */
	void releaseCurBook();
	void slotImporExcelHelp();

private:
	libxl::Book *m_book;
	libxl::Book *m_bakBook; //专门用来解析颜色
	libxl::Sheet *m_sheet;
	QList<libxl::Book *> m_listBook; //统一存储生成的m_book和m_bakBook，在析构时统一释放};
};