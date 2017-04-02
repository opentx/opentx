/*
 * Copyright (C) OpenTX
 *
 * Based on code named
 *   th9x - http://code.google.com/p/th9x
 *   er9x - http://code.google.com/p/er9x
 *   gruvin9x - http://code.google.com/p/gruvin9x
 *
 * License GPLv2: http://www.gnu.org/licenses/gpl-2.0.html
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include "filteredtextbuffer.h"

#include <QDebug>
#include <QElapsedTimer>

/*
 *
 * FIFOBufferDevice
 *
 */

FIFOBufferDevice::FIFOBufferDevice(QObject * parent) :
  QBuffer(parent),
  m_dataBufferMaxSize(20 * 1024),
  m_hasOverflow(false)
{
}

qint64 FIFOBufferDevice::getDataBufferMaxSize() const
{
  return m_dataBufferMaxSize;
}

void FIFOBufferDevice::setDataBufferMaxSize(qint64 size)
{
  if (m_dataBufferMaxSize > size)
    trimData(m_dataBufferMaxSize - size);

  m_dataBufferMaxSize = size;
}

// Remove data from beginning of storage array.
// NOT thread-safe, lock data before use (this avoids needing a recursive mutex)
qint64 FIFOBufferDevice::trimData(qint64 len)
{
  if (!isWritable())
    return 0;

  qint64 count = 0;
  if (len > 0) {
    count = qMin(len, (qint64)buffer().size());
    buffer().remove(0, count);
    seek(0);
  }
  return count;
}

qint64 FIFOBufferDevice::writeData(const char * data, qint64 len)
{
  if (!isWritable())
    return -1;

  QWriteLocker locker(&m_dataRWLock);

  // Handle overflow
  if (size() + len > m_dataBufferMaxSize) {
    m_hasOverflow = true;
    qint64 tlen = trimData(size() + len - m_dataBufferMaxSize);
    emit bufferOverflow(tlen);
  }
  else if (m_hasOverflow) {
    m_hasOverflow = false;
    emit bufferOverflow(0);
  }

  // Always write to end of stream.
  if (!seek(size()))
    return -1;

  // Save the data
  len = QBuffer::writeData(data, len);
  // Make sure we're always ready for reading, this makes bytesAvailable() (et.al.) return correct result.
  seek(0);

  return len;
}

qint64 FIFOBufferDevice::readData(char * data, qint64 len)
{
  if (!isReadable())
    return 0;

  // Do not block
  if (m_dataRWLock.tryLockForRead()) {
    // Always take data from top
    if (seek(0)) {
      len = QBuffer::readData(data, len);
      trimData(len);
    }
    m_dataRWLock.unlock();
  }

  return len;
}

qint64 FIFOBufferDevice::readLine(char * data, qint64 maxSize)
{
  m_dataRWLock.lockForRead();
  qint64 len = buffer().indexOf('\n', 0);
  m_dataRWLock.unlock();

  if (len < 0 || maxSize <= 0)
    return 0;

  ++len;
  len = qMin(len, maxSize);
  return readData(data, len);
}

QByteArray FIFOBufferDevice::readLine(qint64 maxSize)
{
  QByteArray ba;
  m_dataRWLock.lockForRead();
  qint64 len = buffer().indexOf('\n', 0);
  m_dataRWLock.unlock();

  if (len < 0)
    return ba;

  ++len;
  if (maxSize > 0)
    len = qMin(len, maxSize);

  ba.fill(0, len);
  len = readData(ba.data(), len);

  if (len < 0)
    len = 0;
  if (len < ba.size())
    ba.resize(len);

  return ba;
}


/*
 *
 * FilteredTextBuffer
 *
 */

FilteredTextBuffer::FilteredTextBuffer(QObject * parent) :
  FIFOBufferDevice(parent),
  m_inBuffer(new FIFOBufferDevice(this)),
  m_bufferFlushTimer(new QTimer(this)),
  m_lineFilter(QRegularExpression()),
  m_inBuffMaxSize(5 * 1024),
  m_inBuffFlushTimeout(1500),
  m_lineFilterEnable(false),
  m_lineFilterExclusive(false)
{
  m_bufferFlushTimer->setSingleShot(true);
  setInputBufferMaxSize(m_inBuffMaxSize);
  setInputBufferTimeout(m_inBuffFlushTimeout);

  connect(m_inBuffer, &FIFOBufferDevice::readyRead, this, &FilteredTextBuffer::processInputBuffer, Qt::QueuedConnection);
  connect(m_inBuffer, &FIFOBufferDevice::bytesWritten, this, &FilteredTextBuffer::onInputBufferWrite, Qt::QueuedConnection);
  connect(m_inBuffer, &FIFOBufferDevice::bufferOverflow, this, &FilteredTextBuffer::onInputBufferOverflow, Qt::QueuedConnection);
  connect(m_bufferFlushTimer, &QTimer::timeout, this, &FilteredTextBuffer::processInputBuffer);
  connect(this, &FilteredTextBuffer::timerStart, m_bufferFlushTimer, static_cast<void (QTimer::*)(void)>(&QTimer::start));
  connect(this, &FilteredTextBuffer::timerStop, m_bufferFlushTimer, &QTimer::stop);
}

FilteredTextBuffer::~FilteredTextBuffer()
{
  if (m_bufferFlushTimer) {
    disconnect(m_bufferFlushTimer, 0, this, 0);
    disconnect(this, 0, m_bufferFlushTimer, 0);
    m_bufferFlushTimer->deleteLater();
    m_bufferFlushTimer = Q_NULLPTR;
  }
  if (m_inBuffer) {
    closeInputBuffer();
    disconnect(m_inBuffer, 0, this, 0);
    m_inBuffer->deleteLater();
    m_inBuffer = Q_NULLPTR;
  }
}

qint64 FilteredTextBuffer::getInputBufferMaxSize() const
{
  return m_inBuffMaxSize;
}

quint32 FilteredTextBuffer::getInputBufferTimeout() const
{
  return m_inBuffFlushTimeout;
}

void FilteredTextBuffer::setInputBufferMaxSize(qint64 size)
{
  m_inBuffMaxSize = size;
  if (m_inBuffer)
    m_inBuffer->setDataBufferMaxSize(size);
}

void FilteredTextBuffer::setInputBufferTimeout(quint32 ms)
{
  m_inBuffFlushTimeout = ms;
  if (m_bufferFlushTimer)
    m_bufferFlushTimer->setInterval(ms);
}

void FilteredTextBuffer::setLineFilterExpr(const QRegularExpression & expr)
{
  if (expr.isValid())
    m_lineFilter = expr;
  else
    setLineFilterEnabled(false);
}

void FilteredTextBuffer::setLineFilterEnabled(bool enable)
{
  if (!enable && m_lineFilterEnable)
    closeInputBuffer();

  m_lineFilterEnable = enable;
}

void FilteredTextBuffer::setLineFilterExclusive(bool exclusive)
{
  m_lineFilterExclusive = exclusive;
}

void FilteredTextBuffer::setLineFilter(bool enable, bool exclusive, const QRegularExpression & expr)
{
  setLineFilterEnabled(enable);
  setLineFilterExclusive(exclusive);
  setLineFilterExpr(expr);
}

qint64 FilteredTextBuffer::writeDataSuper(const char * data, qint64 len)
{
  if (len == -1)
    len = qstrlen(data);

  return FIFOBufferDevice::writeData(data, len);
}

qint64 FilteredTextBuffer::writeData(const char * data, qint64 len)
{
  if (!isWritable())
    return -1;

  // if filter is disabled, invalid, or input buffer failure, write directly to output buffer
  if (!m_lineFilterEnable || !m_inBuffer || len > m_inBuffMaxSize || (!m_inBuffer->isOpen() && !openInputBuffer()))
    return writeDataSuper(data, len);

  // check for input buffer overflow
  if (m_inBuffer->bytesAvailable() + len > m_inBuffMaxSize) {
    flushInputBuffer();
    onInputBufferOverflow(m_inBuffer->bytesAvailable() + len - m_inBuffMaxSize);
  }

  return m_inBuffer->write(data, len);
}

void FilteredTextBuffer::flushInputBuffer()
{
  emit timerStop();

  if (m_inBuffer && m_inBuffer->bytesAvailable()) {
    // qDebug() << "Flushing input buffer.";
    writeDataSuper(m_inBuffer->readAll().constData());
  }
}

void FilteredTextBuffer::closeInputBuffer()
{
  if (!m_inBuffer || !m_inBuffer->isOpen())
    return;

  flushInputBuffer();
  m_inBuffer->close();
}

bool FilteredTextBuffer::openInputBuffer()
{
  if (m_inBuffer && m_inBuffer->isOpen())
    closeInputBuffer();

  return m_inBuffer->open(ReadWrite);
}

void FilteredTextBuffer::processInputBuffer()
{
  if (m_lineFilterEnable && m_inBuffer && m_inBuffer->canReadLine()) {
    emit timerStop();
    while (m_inBuffer && m_inBuffer->canReadLine()) {
      QByteArray text = m_inBuffer->readLine();
      bool fltMatch = QString(text).contains(m_lineFilter);
      // check line against filter
      if ((m_lineFilterExclusive && !fltMatch) || (!m_lineFilterExclusive && fltMatch)) {
        // Write line to output buffer
        writeDataSuper(text.constData());
      }
    }
    // restart timer if unread bytes still remain
    if (m_inBuffer && m_inBuffer->bytesAvailable() > 0)
      emit timerStart();
  }
  else if (!m_lineFilterEnable || !m_inBuffer) {
    closeInputBuffer();
  }
  else if (m_bufferFlushTimer && !m_bufferFlushTimer->remainingTime()) {
    flushInputBuffer();
    //qDebug() << "Input buffer timeout.";
  }
}
void FilteredTextBuffer::onInputBufferWrite(qint64)
{
  emit timerStart();
}

void FilteredTextBuffer::onInputBufferOverflow(const qint64 len)
{
  static QElapsedTimer reportTimer;

  if (len <= 0) {
    reportTimer.invalidate();
  }
  else if (!reportTimer.isValid() || reportTimer.elapsed() > 1000 * 15) {
    qWarning("Input data buffer overflow by %lld bytes!", len);
    reportTimer.start();
  }
}
