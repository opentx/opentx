/*
  ExportableTableView
	https://github.com/mpaperno/maxLibQt

  COPYRIGHT: (c)2017 Maxim Paperno; All Right Reserved.
  Contact: http://www.WorldDesign.com/contact

  LICENSE:

  Commercial License Usage
  Licensees holding valid commercial licenses may use this file in
  accordance with the terms contained in a written agreement between
  you and the copyright holder.

  GNU General Public License Usage
  Alternatively, this file may be used under the terms of the GNU
  General Public License as published by the Free Software Foundation,
  either version 3 of the License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  A copy of the GNU General Public License is available at <http://www.gnu.org/licenses/>.
*/

#ifndef EXPORTABLETABLEVIEW_H
#define EXPORTABLETABLEVIEW_H

#include <QAction>
#include <QApplication>
#include <QClipboard>
#include <QFile>
#include <QFileDialog>
#include <QMenu>
#include <QMimeData>
#include <QScrollBar>
#include <QStandardPaths>
#include <QTableView>

/**
	\class ExportableTableView
	\version 1.0.0

	\brief The ExportableTableView class provides a regular QTableView but with features to export the data
	as plain text or HTML.

	Any selection of data can be exported.  The horizontal headings, if any, are included in the export.
	The export functions are available to the user via a custom context menu or keyboard shortcuts.
	3 field delimiter choices are available for plain-text export (tab, comma, or pipe).
	Data can be saved to the clipboard or to a file.

	The export functions can also be accessed programmatically via \c toPlainText(), \c toHtml(),
	and \c saveToFile().

	The overall HTML template can be customized by setting \c setHtmlTemplate().  The data itself is always
	formatted as a basic HTML table and then inserted into the template at the \c %1 placeholder.
	HTML version tries to preserve many data role attributes of the model items:

	\li \c Qt::FontRole
	\li \c Qt::ForegroundRole
	\li \c Qt::BackgroundRole
	\li \c Qt::TextAlignmentRole
	\li \c Qt::ToolTipRole

	Note that \c Qt::EditRole data is not specifically preserved (unless it already matches \c Qt::DisplayRole ).

 */
class ExportableTableView : public QTableView
{
	Q_OBJECT

	public:
		ExportableTableView(QWidget *parent = Q_NULLPTR);

		static QString getDefaultHtmlTemplate();
		void setHtmlTemplate(const QString &value);

		QString toPlainText(const QModelIndexList &indexList, const QString &delim = "\t") const;
		QString toHtml(const QModelIndexList &indexList) const;
		bool saveToFile(const QModelIndexList &indexList, const QString &fileName = QString());

		QSize sizeHint() const Q_DECL_OVERRIDE;

	public slots:
		void copyText(const QString &delim = QString("\t"));
		void copyHtml();
		void copy();
		void save();
		QModelIndexList getSelectedOrAll();

	protected:
		void onCustomContextMenuRequested(const QPoint &pos);

		QString m_htmlTemplate;
};

#endif // EXPORTABLETABLEVIEW_H
