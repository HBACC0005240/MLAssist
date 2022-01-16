#include "ITExcel.h"
#include <QApplication>
#include <QColor>
#include <QDesktopServices>
#include <QFileInfo>
#include <QFont>
#include <QList>
#include <QSize>
#include <QUrl>
#if defined(WIN32) || defined(WIN64)
#include <windows.h>
#endif
const QString c_LibXlUser = QStringLiteral("wzq");

#pragma comment(lib, "../ThirdPart/libxl/lib/libxl.lib")
ITExcel::ITExcel()
{
}

ITExcel::~ITExcel()
{
	while (m_listBook.count() != NULL)
	{
		libxl::Book *listBook = m_listBook.takeFirst();
		if (listBook != NULL)
		{
			listBook->release();
		}
	}
}

libxl::Book *ITExcel::createXLSBook(bool isXls)
{
	libxl::Book *result;
	if (isXls)
	{
		result = xlCreateBook();
	}
	else
	{
		result = xlCreateXMLBook();
	}
#ifdef _UNICODE
	result->setKey((const wchar_t *)c_LibXlUser.constData(), L"windows-452f2f0705c9e4568db5636vrai7hbj8");
#else
	result->setKey("wzq", "windows-452f2f0705c9e4568db5636vrai7hbj8");
#endif
	return result;
}

Qt::Alignment ITExcel::XLSHorzAlignmentToQtAlignment(libxl::AlignH align)
{
	switch (align)
	{
		case libxl::ALIGNH_GENERAL:
			return 0;
		case libxl::ALIGNH_LEFT:
			return Qt::AlignLeft | Qt::AlignJustify;
		case libxl::ALIGNH_RIGHT:
			return Qt::AlignRight | Qt::AlignJustify;
		case libxl::ALIGNH_CENTER:
		case libxl::ALIGNH_MERGE:
		case libxl::ALIGNH_DISTRIBUTED:
			return Qt::AlignHCenter | Qt::AlignJustify;
		default:
			return 0;
	}
}

Qt::Alignment ITExcel::XLSVertAlignmentToQtAlignment(libxl::AlignV align)
{
	switch (align)
	{
		case libxl::ALIGNV_TOP:
			return Qt::AlignTop | Qt::AlignJustify;
		case libxl::ALIGNV_BOTTOM:
			return Qt::AlignBottom | Qt::AlignJustify;
		case libxl::ALIGNV_CENTER:
		case libxl::ALIGNV_DISTRIBUTED:
			return Qt::AlignVCenter | Qt::AlignJustify;
		default:
			return 0;
	}
}

Qt::Alignment ITExcel::XLSAlignmentToQtAlignment(libxl::AlignH horzAlignment, libxl::AlignV vertAlignment)
{
	return XLSHorzAlignmentToQtAlignment(horzAlignment) | XLSVertAlignmentToQtAlignment(vertAlignment);
}

libxl::AlignV ITExcel::QtAlignmentToXLSVertAlignment(Qt::Alignment align)
{
	if (Qt::AlignTop == (Qt::AlignTop & align))
	{
		return libxl::ALIGNV_TOP;
	}
	else if (Qt::AlignBottom == (Qt::AlignBottom & align))
	{
		return libxl::ALIGNV_BOTTOM;
	}
	else if (Qt::AlignVCenter == (Qt::AlignVCenter & align))
	{
		return libxl::ALIGNV_CENTER;
	}
	else if (Qt::AlignJustify == (Qt::AlignJustify & align))
	{
		return libxl::ALIGNV_JUSTIFY;
	}
	else
		return libxl::ALIGNV_CENTER;
}

libxl::AlignH ITExcel::QtAlignmentToXLSHorzAlignment(Qt::Alignment align)
{
	if (0x00 == (0x0f & align))
	{
		return libxl::ALIGNH_GENERAL;
	}
	else if (Qt::AlignRight == (Qt::AlignRight & align))
	{
		return libxl::ALIGNH_RIGHT;
	}
	else if (Qt::AlignLeft == (Qt::AlignLeft & align))
	{
		return libxl::ALIGNH_LEFT;
	}
	else if (Qt::AlignJustify == (Qt::AlignJustify & align))
	{
		return libxl::ALIGNH_JUSTIFY;
	}
	else
		return libxl::ALIGNH_CENTER;
}

int ITExcel::extraSpace(int maxinumDigitWidth)
{
	return maxinumDigitWidth > 12 ? 16 + 8 * (int)((maxinumDigitWidth - 13) / 16) : 8;
}

double ITExcel::pixelToDigitWidth(double pixel, int maxinumDigitWidth)
{
	int nExtraSpace = extraSpace(maxinumDigitWidth);

	if (pixel < maxinumDigitWidth + nExtraSpace - 3)
	{
		return double(pixel / (double)(maxinumDigitWidth + nExtraSpace - 3) * 100 + 0.5) / 100.0;
	}
	else
	{
		return double((pixel - nExtraSpace + 3) / (double)maxinumDigitWidth * 100 + 0.5) / 100.0;
	}
}

double ITExcel::digitWidthToPixel(double digitWidth, int maxinumDigitWidth)
{
	if (digitWidth > 1)
	{
		return (double)(digitWidth * maxinumDigitWidth + extraSpace(maxinumDigitWidth) - 3 + 0.5);
	}
	else
	{
		return (double)(digitWidth * (maxinumDigitWidth + extraSpace(maxinumDigitWidth) - 3) + 0.5);
	}
}

int ITExcel::calcMaximumDigitWidth(const wchar_t *fontFamily, int fontPointSize)
{
	int maxWidth = 7;
#ifdef Q_OS_WIN
	HDC hdc = CreateDC(L"DISPLAY", NULL, NULL, NULL);
	HFONT font = CreateFont(-MulDiv(fontPointSize, GetDeviceCaps(hdc, LOGPIXELSY), 72), 0, 0, 0,
			0, 0, 0, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
			DEFAULT_PITCH, fontFamily);
	SelectObject(hdc, font);
	ABC abc[10];

	if (!GetCharABCWidths(hdc, 0x30, 0x39, &abc[0]))
	{
		maxWidth = 7;
	}
	for (unsigned i = 0; i < 10; ++i)
	{
		int width = abc[i].abcA + abc[i].abcB + abc[i].abcC;
		if (width > maxWidth)
			maxWidth = width;
	}
	DeleteObject(font);
	DeleteDC(hdc);
#endif
	return maxWidth;
}

void ITExcel::slotImporExcelHelp()
{
	QString sHelpWordFile = QApplication::applicationDirPath() + "\\ImportExcelDetail.docx";
	QFile file(sHelpWordFile);
	if (file.open(QFile::ReadOnly))
	{
		QDesktopServices::openUrl(QUrl::fromLocalFile(sHelpWordFile));
	}
}

bool ITExcel::load(const QString &fileName)
{
	QFileInfo fi(fileName);

	if (fi.suffix() == "xls" || fi.suffix() == "XLS")
	{
		m_book = createXLSBook(true);
	}
	else
	{
		m_book = createXLSBook(false);
	}

	if (!m_listBook.contains(m_book))
	{
		m_listBook.append(m_book);
	}

	m_book->setRgbMode(true);

	m_bakBook = createXLSBook(true);

	if (!m_listBook.contains(m_bakBook))
	{
		m_listBook.append(m_bakBook);
	}

	m_bakBook->setRgbMode(true);
	m_bakBook->load((const wchar_t *)fileName.constData());
	bool bLoadSuccess = m_book->load((const wchar_t *)fileName.constData());
	return bLoadSuccess;
}

bool ITExcel::save(const QString &fileName)
{
	return m_book->save((const wchar_t *)fileName.constData());
}

bool ITExcel::setSheetIndex(int index)
{

	if (m_book && (m_book->sheetCount() > 0) && (index >= 0) && (index < m_book->sheetCount()))
	{
		m_sheet = m_book->getSheet(index);
		return m_sheet != NULL;
	}

	return false;
}

bool ITExcel::exportExcel()
{
	/*ExportState exportState;
	try
	{
		exporter->setGroupCollapsed(isCollapsed);
		exporter->setTableView(tableView);
		exporter->setShowPrompt(showPrompt);
		exporter->setIsToSingleSheet(isToSingleSheet);
		exporter->setIsOverWrite(isOverWrite);
		exporter->load(fileName);
		exporter->doExport(sheetName);
		exportState = exporter->save(fileName);
	}
	catch (...)
	{
		delete exporter;
		exportState = EXPORT_ERRSAVE;
		throw;
	}

	freeAndNil(exporter);
	return exportState;*/
	return false;
}

Book *ITExcel::book() const
{
	return m_book;
}

Book *ITExcel::bakBook() const
{
	return m_bakBook;
}

void ITExcel::setXlsBook(Book *oBook)
{
	m_book = oBook;
}

void ITExcel::setXlsBakBook(Book *oBakBook)
{
	m_bakBook = oBakBook;
}

Sheet *ITExcel::activeSheet() const
{
	return m_sheet;
}

QString ITExcel::getSheetName() const
{
	if (NULL != m_sheet)
	{
		return QString::fromWCharArray(m_sheet->name());
	}

	return QString("");
}

void ITExcel::releaseCurBook()
{
	for (int i = m_listBook.count() - 1; i >= 0; --i)
	{
		Book *pBook = m_listBook.at(i);
		if ((pBook == m_book) || (pBook == m_bakBook))
		{
			m_listBook.removeAt(i);
		}
	}

	if (m_book != NULL)
	{
		m_book->release();
		m_book = NULL;
		m_sheet = NULL;
	}

	if (m_bakBook != NULL)
	{
		m_bakBook->release();
		m_bakBook = NULL;
		m_sheet = NULL;
	}
}

Book *ITExcel::loadxlsFile(const QString &fileName)
{
	QFileInfo fi(fileName);
	libxl::Book *oBook;

	if (fi.suffix() == "xls")
	{
		oBook = createXLSBook(true);
	}
	else
	{
		oBook = createXLSBook(false);
	}

	oBook->setRgbMode(true);
	oBook->load((const wchar_t *)fileName.constData());
	return oBook;
}
