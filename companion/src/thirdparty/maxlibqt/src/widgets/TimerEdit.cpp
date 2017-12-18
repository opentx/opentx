/*
	TimerEdit

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

#include "TimerEdit.h"
#include <cmath>

TimerEdit::TimerEdit(QWidget *parent) :
  QLineEdit(parent),
  m_showSeconds(true),
  m_minTime(0),
  m_maxTime(24 * 3600 - 1),
  m_singleStep(1),
  m_pageStep(60),
  m_hourDigits(2),
  m_validator(new QRegularExpressionValidator(this))
{
	setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
	setupFormat();
	setToolTip(tr("Use mouse scroll or UP/DOWN arrow keys to change time in small steps.\nCTRL + scroll or PAGE UP/DOWN keys to change time in larger steps."));
	connect(this, &TimerEdit::textEdited, this, &TimerEdit::textEditedHandler);
}

int TimerEdit::timeInSeconds() const
{
	int ret = 0;

	ret += timepart(text(), TimeParts::Hours) * 3600;
	ret += timepart(text(), TimeParts::Minutes) * 60;
	ret += timepart(text(), TimeParts::Seconds);
	ret *= timepart(text(), TimeParts::Polarity);

	return ret;
}

int TimerEdit::timepart(const QString &input, TimerEdit::TimeParts part) const
{
	int ret = 0;
	QRegularExpressionMatch match = m_validator->regularExpression().match(input, 0, QRegularExpression::NormalMatch);

	if (match.hasMatch()) {
		switch (part) {
			case Hours:
				if (!match.captured("hrs").isEmpty())
					ret = match.captured("hrs").toInt();
				break;

			case Minutes:
				if (!match.captured("mins").isEmpty())
					ret = match.captured("mins").toInt();
				break;

			case Seconds:
				if (!match.captured("secs").isEmpty())
					ret = match.captured("secs").toInt();
				break;

			case Polarity:
				if (!match.captured("pol").isEmpty() && !match.captured("pol").compare("-"))
					ret = -1;
				else
					ret = 1;
				break;

			default:
				break;
		}
	}

	return ret;
}


//// Slots ////

void TimerEdit::setTime(int seconds)
{
	if (seconds > m_maxTime)
		seconds = m_maxTime;
	else if (seconds < m_minTime)
		seconds = m_minTime;

	div_t qrM = div(abs(seconds), 60);
	div_t qrH = div(qrM.quot, 60);

	QString val = QString("%1").arg(qrH.rem, 2, 10, QChar('0'));

	if (m_hourDigits)
		val.prepend(QString("%1:").arg(qrH.quot, m_hourDigits, 10, QChar('0')));

	if (m_showSeconds)
		val.append(QString(":%1").arg(qrM.rem, 2, 10, QChar('0')));

	if (m_minTime < 0) {
		if (seconds < 0)
			val.prepend("-");
		else
			val.prepend(" ");
	}

	if (text().compare(val)) {
		setText(val);
	}
}

void TimerEdit::incrDecr(int seconds)
{
	if (!seconds)
		return;

	const int cs = timeInSeconds();
	int s = seconds + cs;
	if (s < m_minTime)
		s = m_minTime;
	if (s > m_maxTime)
		s = m_maxTime;
	if (s != cs) {
		setTime(s);
		emitValueChanged();
	}
}

void TimerEdit::setTimeRange(int minSeconds, int maxSeconds)
{
	if (minSeconds <= maxSeconds && (m_minTime != minSeconds || m_maxTime != maxSeconds)) {
		int digits = 0;
		if (maxSeconds >= 3600)
			digits = floorf(logf(round(maxSeconds / 3600)) / logf(10.0f)) + 1;

		bool mod = (m_hourDigits != digits || std::signbit((float)m_minTime) != std::signbit((float)minSeconds));

		m_minTime = minSeconds;
		m_maxTime = maxSeconds;
		m_hourDigits = digits;

		if (mod)
			setupFormat();
	}
}

void TimerEdit::setMinimumTime(int minSeconds)
{
	setTimeRange(minSeconds, (m_maxTime > minSeconds ? m_maxTime : minSeconds));
}

void TimerEdit::setMaximumTime(int maxSeconds)
{
	setTimeRange((m_minTime < maxSeconds ? m_minTime : maxSeconds), maxSeconds);
}

void TimerEdit::setShowSeconds(bool showSeconds)
{
	if (m_showSeconds != showSeconds) {
		m_showSeconds = showSeconds;
		setupFormat();
	}
}


//// Protected ////

void TimerEdit::textEditedHandler()
{
	int sec = timeInSeconds();
	if (sec < m_minTime)
		setTime(m_minTime);
	else if (sec > m_maxTime)
		setTime(m_maxTime);
	else
		return;

	emitValueChanged();
}

void TimerEdit::setupFormat()
{
	QString inputRe  = "(?<mins>[0-5][0-9])";
	QString inputMsk = "99";
	QString suffx = "\\" % tr("m") % "\\" % tr("m");

	if (m_hourDigits) {
		inputRe.prepend("(?<hrs>[0-9]*[0-9]):");
		inputMsk.prepend(":");
		suffx.prepend(":");
		for (int i=0; i < m_hourDigits; ++i) {
			inputMsk.prepend("9");
			suffx.prepend("\\" % tr("h"));
		}
	}
	if (m_showSeconds) {
		inputRe.append(":(?<secs>[0-5][0-9])");
		inputMsk.append(":99");
		suffx.append(":\\" % tr("s") % "\\" % tr("s"));
	}
	if (m_minTime < 0) {
		inputRe.prepend("(?<pol>-|\\+|\\s)?");
		inputMsk.prepend("#");
	}

	inputRe.prepend("^");
	inputRe.append("\\s?\\[[^\\]]+\\]$");
	inputMsk.append(" \\[" % suffx % "\\]");

	m_validator->setRegularExpression(QRegularExpression(inputRe));

	setInputMask(inputMsk);
	setValidator(m_validator);
}

void TimerEdit::emitValueChanged()
{
	emit QLineEdit::textEdited(text());
}

void TimerEdit::keyPressEvent(QKeyEvent *event)
{
	switch (event->key()) {
		case Qt::Key_Up:
			incrDecr(m_singleStep);
			break;

		case Qt::Key_Down:
			incrDecr(-(int)m_singleStep);
			break;

		case Qt::Key_PageUp:
			incrDecr(m_pageStep);
			break;

		case Qt::Key_PageDown:
			incrDecr(-(int)m_pageStep);
			break;

		case Qt::Key_Plus:
		case Qt::Key_Equal:
		case Qt::Key_Minus:
			if (m_minTime < 0) {
				setTime(-timeInSeconds());
				emitValueChanged();
			}
			break;

		default:
			QLineEdit::keyPressEvent(event);
			break;
	}
}

void TimerEdit::wheelEvent(QWheelEvent *event)
{
	if (event->angleDelta().isNull()) {
		event->ignore();
		return;
	}
	int numSteps = -event->angleDelta().y() / 8 / 15 * -1;  // one step per 15deg
	numSteps *= (event->modifiers() & Qt::ControlModifier) ? m_pageStep : m_singleStep;
	incrDecr(numSteps);
	event->accept();
}
