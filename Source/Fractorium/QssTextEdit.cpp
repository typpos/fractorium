#include "FractoriumPch.h"
#include "QssTextEdit.h"

QssTextEdit::QssTextEdit(QWidget* parent) :
	QTextEdit(parent)
{
	setTabStopDistance(fontMetrics().horizontalAdvance(QLatin1Char(' ')) * 4);
	setAcceptRichText(false);
	new CssHighlighter(document());
}
