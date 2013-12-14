/***************************************************************************
**                                                                        **
**  QCustomPlot, a simple to use, modern plotting widget for Qt           **
**  Copyright (C) 2012 Emanuel Eichhammer                                 **
**                                                                        **
**  This program is free software: you can redistribute it and/or modify  **
**  it under the terms of the GNU General Public License as published by  **
**  the Free Software Foundation, either version 3 of the License, or     **
**  (at your option) any later version.                                   **
**                                                                        **
**  This program is distributed in the hope that it will be useful,       **
**  but WITHOUT ANY WARRANTY; without even the implied warranty of        **
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         **
**  GNU General Public License for more details.                          **
**                                                                        **
**  You should have received a copy of the GNU General Public License     **
**  along with this program.  If not, see http://www.gnu.org/licenses/.   **
**                                                                        **
****************************************************************************
**           Author: Emanuel Eichhammer                                   **
**  Website/Contact: http://www.WorksLikeClockwork.com/                   **
**             Date: 09.06.12                                             **
****************************************************************************/

/*! \mainpage %QCustomPlot Documentation
 
  Below is a brief overview of and guide to the classes and their relations. If you are new to
  QCustomPlot and just want to start using it, it's recommended to look at the examples/tutorials
  at
 
  http://www.WorksLikeClockWork.com/index.php/components/qt-plotting-widget
 
  This documentation is especially helpful when you're familiar with the basic concept of how to use
  %QCustomPlot and you wish to learn more about specific functionality.
 
  \section simpleoverview Simplified Class Overview
  
  \image latex ClassesOverviewSimplified.png "" width=1.2\textwidth
  \image html ClassesOverviewSimplified.png
  <center>Simplified diagram of most important classes, view the \ref classoverview "Class Overview" to see a full overview.</center>
  
  The central widget which displays the plottables and axes on its surface is QCustomPlot. Usually,
  you don't create the axes yourself, but you use the ones already inside every QCustomPlot
  instance (xAxis, yAxis, xAxis2, yAxis2).

  \section plottables Plottables
  
  \a Plottables are classes that display any kind of data inside the QCustomPlot. They all derive
  from QCPAbstractPlottable. For example, the QCPGraph class is a plottable that displays a graph
  inside the plot with different line styles, scatter styles, filling etc.
  
  Since plotting graphs is such a dominant use case, QCustomPlot has a special interface for working
  with QCPGraph plottables, that makes it very easy to handle them:\n
  You create a new graph with QCustomPlot::addGraph and access them with QCustomPlot::graph.
  
  For all other plottables, you need to use the normal plottable interface:\n
  First, you create an instance of the plottable you want, e.g.
  \code
  QCPCurve *newCurve = new QCPCurve(customPlot->xAxis, customPlot->yAxis);\endcode
  add it to the customPlot with QCustomPlot::addPlottable:
  \code
  customPlot->addPlottable(newCurve);\endcode
  and then modify the properties of the newly created plottable via <tt>newCurve</tt>.
  
  Plottables (including graphs) can be retrieved via QCustomPlot::plottable. Since the return type
  of that function is the abstract base class of all plottables, QCPAbstractPlottable, you will
  probably want to qobject_cast (or dynamic_cast) the returned pointer to the respective plottable
  subclass. (As usual, if the cast returns zero, the plottable wasn't of that specific subclass.)
  
  All further interfacing with plottables (e.g how to set data) is specific to the plottable type.
  See the documentations of the subclasses: QCPGraph, QCPCurve, QCPBars, QCPStatisticalBox.

  \section axes Controlling the Axes
  
  As mentioned, QCustomPlot has four axes by default: \a xAxis (bottom), \a yAxis (left), \a xAxis2
  (top), \a yAxis2 (right).
  
  Their range is handled by the simple QCPRange class. You can set the range with the
  QCPAxis::setRange function. By default, the axes represent a linear scale. To set a logarithmic
  scale, set QCPAxis::setScaleType to QCPAxis::stLogarithmic. The logarithm base can be set freely
  with QCPAxis::setScaleLogBase.
  
  By default, an axis automatically creates and labels ticks in a sensible manner, i.e. with a tick
  interval that's pleasing to the viewer. See the following functions for tick manipulation:\n
  QCPAxis::setTicks, QCPAxis::setAutoTicks, QCPAxis::setAutoTickCount, QCPAxis::setAutoTickStep,
  QCPAxis::setTickLabels, QCPAxis::setTickLabelType, QCPAxis::setTickLabelRotation,
  QCPAxis::setTickStep, QCPAxis::setTickLength,...
  
  Each axis can be given an axis label (e.g. "Voltage [mV]") with QCPAxis::setLabel.
  
  The distance of an axis backbone to the respective QCustomPlot widget border is called its margin.
  Normally, the margins are calculated automatically. To change this, set QCustomPlot::setAutoMargin
  to false and set the margins manually with QCustomPlot::setMargin.
  
  \section legend Plot Legend
  
  Every QCustomPlot owns a QCPLegend (as \a legend). That's a small window inside the plot which
  lists the plottables with an icon of the plottable line/symbol and a description. The Description
  is retrieved from the plottable name (QCPAbstractPlottable::setName). Plottables can be added and
  removed from the legend via \ref QCPAbstractPlottable::addToLegend and \ref
  QCPAbstractPlottable::removeFromLegend. By default, adding a plottable to QCustomPlot
  automatically adds it to the legend, too. This behaviour can be modified with the
  QCustomPlot::setAutoAddPlottableToLegend property.
  
  The QCPLegend provides an interface to access, add and remove legend items directly, too. See
  QCPLegend::item, QCPLegend::itemWithPlottable, QCPLegend::addItem, QCPLegend::removeItem for
  example.
  
  \section userinteraction User Interactions
  
  QCustomPlot currently supports dragging axis ranges with the mouse (\ref
  QCustomPlot::setRangeDrag), zooming axis ranges with the mouse wheel (\ref
  QCustomPlot::setRangeZoom) and a complete selection mechanism of most objects.
  
  The availability of these interactions is controlled with \ref QCustomPlot::setInteractions. For
  details about the interaction system, see the documentation there.
  
  Further, QCustomPlot always emits corresponding signals, when objects are clicked or
  doubleClicked. See \ref QCustomPlot::plottableClick, \ref QCustomPlot::plottableDoubleClick
  and \ref QCustomPlot::axisClick for example.
  
  \section items Items
  
  Apart from plottables there is another category of plot objects that are important: Items. The
  base class of all items is QCPAbstractItem. An item sets itself apart from plottables in that
  it's not necessarily bound to any axes. This means it may also be positioned in absolute pixel
  coordinates or placed at a relative position on the axis rect. Further it usually doesn't
  represent data directly but acts as decoration, emphasis, description etc.
  
  Multiple items can be arranged in a parent-child-hierarchy allowing for dynamical behaviour. For
  example, you could place the head of an arrow at a certain plot coordinate, so it always points
  to some important part of your data. The tail of the arrow can be fixed at a text label item
  which always resides in the top center of the axis rect (independent of where the user drags the
  axis ranges).
  
  For a more detailed introduction, see the QCPAbstractItem documentation, and from there the
  documentations of the individual built-in items, to find out how to use them.
  
  \section performancetweaks Performance Tweaks
  
  Although QCustomPlot is quite fast, some features like semi-transparent fills and antialiasing
  can cause a significant slow down. Here are some thoughts on how to increase performance. By far
  the most time is spent in the drawing functions, specifically the drawing of graphs. For maximum
  performance, consider the following (most recommended/effective measures first):
  
  \li use Qt 4.8.0 and up. Performance has doubled or tripled with respect to Qt 4.7.4. However they broke QPainter,
  drawing pixel precise things, e.g. scatters, isn't possible with Qt 4.8.0/1. So it's a performance vs. plot
  quality tradeoff when switching to Qt 4.8.
  \li To increase responsiveness during dragging, consider setting \ref QCustomPlot::setNoAntialiasingOnDrag to true.
  \li On X11 (linux), avoid the (slow) native drawing system, use raster by supplying
  "-graphicssystem raster" as command line argument or calling QApplication::setGraphicsSystem("raster")
  before creating the QApplication object.
  \li On all operating systems, use OpenGL hardware acceleration by supplying "-graphicssystem
  opengl" as command line argument or calling QApplication::setGraphicsSystem("opengl"). If OpenGL
  is available, this will slightly decrease the quality of antialiasing, but extremely increase
  performance especially with alpha (semi-transparent) fills, much antialiasing and a large
  QCustomPlot drawing surface. Note however, that the maximum frame rate might be constrained by
  the vertical sync frequency of your monitor (VSync can be disabled in the graphics card driver
  configuration). So for simple plots (where the potential framerate is far above 60 frames per
  second), OpenGL acceleration might achieve numerically lower frame rates than the other
  graphics systems, because they are not capped at the VSync frequency.
  \li Avoid any kind of alpha (transparency), especially in fills
  \li Avoid any kind of antialiasing, especially in graph lines (see QCustomPlot::setNotAntialiasedElements)
  \li Avoid repeatedly setting the complete data set with QCPGraph::setData. Use QCPGraph::addData instead, if most
  data points stay unchanged, e.g. in a running measurement.
  \li Set the \a copy parameter of the setData functions to false, so only pointers get
  transferred. (Relevant only if preparing data maps with a large number of points, i.e. over 10000)
*/

/*! \page classoverview Class Overview
  
  \image latex ClassesOverview.png "Overview of all classes and their relations" width=1.2\textwidth
  \image html ClassesOverview.png "Overview of all classes and their relations"
  
*/

#include "qcustomplot.h"

// ================================================================================
// =================== QCPData
// ================================================================================

/*! \class QCPData
  \brief Holds the data of one single data point for QCPGraph.
  
  The stored data is:
  \li \a key: coordinate on the key axis of this data point
  \li \a value: coordinate on the value axis of this data point
  \li \a keyErrorMinus: negative error in the key dimension (for error bars)
  \li \a keyErrorPlus: positive error in the key dimension (for error bars)
  \li \a valueErrorMinus: negative error in the value dimension (for error bars)
  \li \a valueErrorPlus: positive error in the value dimension (for error bars)
  
  \see QCPDataMap
*/

/*!
  Constructs a data point with key, value and all errors set to zero.
*/
QCPData::QCPData() :
  key(0),
  value(0),
  keyErrorPlus(0),
  keyErrorMinus(0),
  valueErrorPlus(0),
  valueErrorMinus(0)
{
}

/*!
  Constructs a data point with the specified \a key and \a value. All errors are set to zero.
*/
QCPData::QCPData(double key, double value) :
  key(key),
  value(value),
  keyErrorPlus(0),
  keyErrorMinus(0),
  valueErrorPlus(0),
  valueErrorMinus(0)
{
}

// ================================================================================
// =================== QCPCurveData
// ================================================================================

/*! \class QCPCurveData
  \brief Holds the data of one single data point for QCPCurve.
  
  The stored data is:
  \li \a t: the free parameter of the curve at this curve point (cp. the mathematical vector <em>(x(t), y(t))</em>)
  \li \a key: coordinate on the key axis of this curve point
  \li \a value: coordinate on the value axis of this curve point
  
  \see QCPCurveDataMap
*/

/*!
  Constructs a curve data point with t, key and value set to zero.
*/
QCPCurveData::QCPCurveData() :
  t(0),
  key(0),
  value(0)
{
}

/*!
  Constructs a curve data point with the specified \a t, \a key and \a value.
*/
QCPCurveData::QCPCurveData(double t, double key, double value) :
  t(t),
  key(key),
  value(value)
{
}


// ================================================================================
// =================== QCPBarData
// ================================================================================

/*! \class QCPBarData
  \brief Holds the data of one single data point (one bar) for QCPBars.
  
  The stored data is:
  \li \a key: coordinate on the key axis of this bar
  \li \a value: height coordinate on the value axis of this bar
  
  \see QCPBarDataaMap
*/

/*!
  Constructs a bar data point with key and value set to zero.
*/
QCPBarData::QCPBarData() :
  key(0),
  value(0)
{
}

/*!
  Constructs a bar data point with the specified \a key and \a value.
*/
QCPBarData::QCPBarData(double key, double value) :
  key(key),
  value(value)
{
}

// ================================================================================
// =================== QCPGraph
// ================================================================================

/*! \class QCPGraph
  \brief A plottable representing a graph in a plot.

  Usually QCustomPlot creates it internally via QCustomPlot::addGraph and the resulting instance is
  accessed via QCustomPlot::graph.

  To plot data, assign it with the \ref setData or \ref addData functions.
  
  \section appearance Changing the appearance
  
  The appearance of the graph is mainly determined by the line style, scatter style, brush and pen
  of the graph (\ref setLineStyle, \ref setScatterStyle, \ref setBrush, \ref setPen).
  
  \subsection filling Filling under or between graphs
  
  QCPGraph knows two types of fills: Normal graph fills towards the zero-value-line parallel to
  the key axis of the graph, and fills between two graphs, called channel fills. To enable a fill,
  just set a brush with \ref setBrush which is neither Qt::NoBrush nor fully transparent.
  
  By default, a normal fill towards the zero-value-line will be drawn. To set up a channel fill
  between this graph and another one, call \ref setChannelFillGraph with the other graph as
  parameter.

  \see QCustomPlot::addGraph, QCustomPlot::graph, QCPLegend::addGraph
*/

/*!
  Constructs a graph which uses \a keyAxis as its key axis ("x") and \a valueAxis as its value
  axis ("y"). \a keyAxis and \a valueAxis must reside in the same QCustomPlot instance and not have
  the same orientation. If either of these restrictions is violated, a corresponding message is
  printed to the debug output (qDebug), the construction is not aborted, though.
  
  The constructed QCPGraph can be added to the plot with QCustomPlot::addPlottable, QCustomPlot
  then takes ownership of the graph.
  
  To directly create a graph inside a plot, you can also use the simpler QCustomPlot::addGraph function.
*/
QCPGraph::QCPGraph(QCPAxis *keyAxis, QCPAxis *valueAxis) :
  QCPAbstractPlottable(keyAxis, valueAxis)
{
  mData = new QCPDataMap;
  
  setPen(QPen(Qt::blue));
  setErrorPen(QPen(Qt::black));
  setBrush(Qt::NoBrush);
  setSelectedPen(QPen(QColor(80, 80, 255), 2.5));
  setSelectedBrush(Qt::NoBrush);
  
  setLineStyle(lsLine);
  setScatterStyle(QCP::ssNone);
  setScatterSize(6);
  setErrorType(etNone);
  setErrorBarSize(6);
  setErrorBarSkipSymbol(true);
  setChannelFillGraph(0);
}

QCPGraph::~QCPGraph()
{
  if (mParentPlot)
  {
    // if another graph has a channel fill towards this graph, set it to zero
    for (int i=0; i<mParentPlot->graphCount(); ++i)
    {
      if (mParentPlot->graph(i)->channelFillGraph() == this)
        mParentPlot->graph(i)->setChannelFillGraph(0);
    }
  }
  delete mData;
}

/*!
  Replaces the current data with the provided \a data.
  
  If \a copy is set to true, data points in \a data will only be copied. if false, the graph
  takes ownership of the passed data and replaces the internal data pointer with it. This is
  significantly faster than copying for large datasets.
*/
void QCPGraph::setData(QCPDataMap *data, bool copy)
{
  if (copy)
  {
    *mData = *data;
  } else
  {
    delete mData;
    mData = data;
  }
}

/*! \overload
  
  Replaces the current data with the provided points in \a key and \a value pairs. The provided
  vectors should have equal length. Else, the number of added points will be the size of the
  smallest vector.
*/
void QCPGraph::setData(const QVector<double> &key, const QVector<double> &value)
{
  mData->clear();
  int n = key.size();
  n = qMin(n, value.size());
  QCPData newData;
  for (int i=0; i<n; ++i)
  {
    newData.key = key[i];
    newData.value = value[i];
    mData->insertMulti(newData.key, newData);
  }
}

/*!
  Replaces the current data with the provided points in \a key and \a value pairs. Additionally the
  symmetrical value error of the data points are set to the values in \a valueError.
  For error bars to show appropriately, see \ref setErrorType.
  The provided vectors should have equal length. Else, the number of added points will be the size of the
  smallest vector.
  
  For asymmetrical errors (plus different from minus), see the overloaded version of this function.
*/
void QCPGraph::setDataValueError(const QVector<double> &key, const QVector<double> &value, const QVector<double> &valueError)
{
  mData->clear();
  int n = key.size();
  n = qMin(n, value.size());
  n = qMin(n, valueError.size());
  QCPData newData;
  for (int i=0; i<n; ++i)
  {
    newData.key = key[i];
    newData.value = value[i];
    newData.valueErrorMinus = valueError[i];
    newData.valueErrorPlus = valueError[i];
    mData->insertMulti(key[i], newData);
  }
}

/*!
  \overload
  Replaces the current data with the provided points in \a key and \a value pairs. Additionally the
  negative value error of the data points are set to the values in \a valueErrorMinus, the positive
  value error to \a valueErrorPlus.
  For error bars to show appropriately, see \ref setErrorType.
  The provided vectors should have equal length. Else, the number of added points will be the size of the
  smallest vector.
*/
void QCPGraph::setDataValueError(const QVector<double> &key, const QVector<double> &value, const QVector<double> &valueErrorMinus, const QVector<double> &valueErrorPlus)
{
  mData->clear();
  int n = key.size();
  n = qMin(n, value.size());
  n = qMin(n, valueErrorMinus.size());
  n = qMin(n, valueErrorPlus.size());
  QCPData newData;
  for (int i=0; i<n; ++i)
  {
    newData.key = key[i];
    newData.value = value[i];
    newData.valueErrorMinus = valueErrorMinus[i];
    newData.valueErrorPlus = valueErrorPlus[i];
    mData->insertMulti(key[i], newData);
  }
}

/*!
  Replaces the current data with the provided points in \a key and \a value pairs. Additionally the
  symmetrical key error of the data points are set to the values in \a keyError.
  For error bars to show appropriately, see \ref setErrorType.
  The provided vectors should have equal length. Else, the number of added points will be the size of the
  smallest vector.
  
  For asymmetrical errors (plus different from minus), see the overloaded version of this function.
*/
void QCPGraph::setDataKeyError(const QVector<double> &key, const QVector<double> &value, const QVector<double> &keyError)
{
  mData->clear();
  int n = key.size();
  n = qMin(n, value.size());
  n = qMin(n, keyError.size());
  QCPData newData;
  for (int i=0; i<n; ++i)
  {
    newData.key = key[i];
    newData.value = value[i];
    newData.keyErrorMinus = keyError[i];
    newData.keyErrorPlus = keyError[i];
    mData->insertMulti(key[i], newData);
  }
}

/*!
  \overload
  Replaces the current data with the provided points in \a key and \a value pairs. Additionally the
  negative key error of the data points are set to the values in \a keyErrorMinus, the positive
  key error to \a keyErrorPlus.
  For error bars to show appropriately, see \ref setErrorType.
  The provided vectors should have equal length. Else, the number of added points will be the size of the
  smallest vector.
*/
void QCPGraph::setDataKeyError(const QVector<double> &key, const QVector<double> &value, const QVector<double> &keyErrorMinus, const QVector<double> &keyErrorPlus)
{
  mData->clear();
  int n = key.size();
  n = qMin(n, value.size());
  n = qMin(n, keyErrorMinus.size());
  n = qMin(n, keyErrorPlus.size());
  QCPData newData;
  for (int i=0; i<n; ++i)
  {
    newData.key = key[i];
    newData.value = value[i];
    newData.keyErrorMinus = keyErrorMinus[i];
    newData.keyErrorPlus = keyErrorPlus[i];
    mData->insertMulti(key[i], newData);
  }
}

/*!
  Replaces the current data with the provided points in \a key and \a value pairs. Additionally the
  symmetrical key and value errors of the data points are set to the values in \a keyError and \a valueError.
  For error bars to show appropriately, see \ref setErrorType.
  The provided vectors should have equal length. Else, the number of added points will be the size of the
  smallest vector.
  
  For asymmetrical errors (plus different from minus), see the overloaded version of this function.
*/
void QCPGraph::setDataBothError(const QVector<double> &key, const QVector<double> &value, const QVector<double> &keyError, const QVector<double> &valueError)
{
  mData->clear();
  int n = key.size();
  n = qMin(n, value.size());
  n = qMin(n, valueError.size());
  n = qMin(n, keyError.size());
  QCPData newData;
  for (int i=0; i<n; ++i)
  {
    newData.key = key[i];
    newData.value = value[i];
    newData.keyErrorMinus = keyError[i];
    newData.keyErrorPlus = keyError[i];
    newData.valueErrorMinus = valueError[i];
    newData.valueErrorPlus = valueError[i];
    mData->insertMulti(key[i], newData);
  }
}

/*!
  \overload
  Replaces the current data with the provided points in \a key and \a value pairs. Additionally the
  negative key and value errors of the data points are set to the values in \a keyErrorMinus and \a valueErrorMinus. The positive
  key and value errors are set to the values in \a keyErrorPlus \a valueErrorPlus.
  For error bars to show appropriately, see \ref setErrorType.
  The provided vectors should have equal length. Else, the number of added points will be the size of the
  smallest vector.
*/
void QCPGraph::setDataBothError(const QVector<double> &key, const QVector<double> &value, const QVector<double> &keyErrorMinus, const QVector<double> &keyErrorPlus, const QVector<double> &valueErrorMinus, const QVector<double> &valueErrorPlus)
{
  mData->clear();
  int n = key.size();
  n = qMin(n, value.size());
  n = qMin(n, valueErrorMinus.size());
  n = qMin(n, valueErrorPlus.size());
  n = qMin(n, keyErrorMinus.size());
  n = qMin(n, keyErrorPlus.size());
  QCPData newData;
  for (int i=0; i<n; ++i)
  {
    newData.key = key[i];
    newData.value = value[i];
    newData.keyErrorMinus = keyErrorMinus[i];
    newData.keyErrorPlus = keyErrorPlus[i];
    newData.valueErrorMinus = valueErrorMinus[i];
    newData.valueErrorPlus = valueErrorPlus[i];
    mData->insertMulti(key[i], newData);
  }
}


/*!
  Sets how the single data points are connected in the plot or how they are represented visually
  apart from the scatter symbol. For scatter-only plots, set \a ls to \ref lsNone and \ref
  setScatterStyle to the desired scatter style.
  
  \see setScatterStyle
*/
void QCPGraph::setLineStyle(LineStyle ls)
{
  mLineStyle = ls;
}

/*! 
  Sets the visual appearance of single data points in the plot. If set to \ref QCP::ssNone, no scatter points
  are drawn (e.g. for line-only-plots with appropriate line style).
  
  \see ScatterStyle, setLineStyle
*/
void QCPGraph::setScatterStyle(QCP::ScatterStyle ss)
{
  mScatterStyle = ss;
}

/*! 
  This defines how big (in pixels) single scatters are drawn, if scatter style (\ref
  setScatterStyle) isn't \ref QCP::ssNone, \ref QCP::ssDot or \ref QCP::ssPixmap. Floating point values are
  allowed for fine grained control over optical appearance with antialiased painting.
  
  \see ScatterStyle
*/
void QCPGraph::setScatterSize(double size)
{
  mScatterSize = size;
}

/*! 
  If the scatter style (\ref setScatterStyle) is set to ssPixmap, this function defines the QPixmap
  that will be drawn centered on the data point coordinate.
  
  \see ScatterStyle
*/
void QCPGraph::setScatterPixmap(const QPixmap &pixmap)
{
  mScatterPixmap = pixmap;
}

/*!
  Sets which kind of error bars (Key Error, Value Error or both) should be drawn on each data
  point. If you set \a errorType to something other than \ref etNone, make sure to actually pass
  error data via the specific setData functions along with the data points (e.g. \ref
  setDataValueError, \ref setDataKeyError, \ref setDataBothError).

  \see ErrorType
*/
void QCPGraph::setErrorType(ErrorType errorType)
{
  mErrorType = errorType;
}

/*!
  Sets the pen with which the error bars will be drawn.
  \see setErrorBarSize, setErrorType
*/
void QCPGraph::setErrorPen(const QPen &pen)
{
  mErrorPen = pen;
}

/*! 
  Sets the width of the handles at both ends of an error bar in pixels.
*/
void QCPGraph::setErrorBarSize(double size)
{
  mErrorBarSize = size;
}

/*! 
  If \a enabled is set to true, the error bar will not be drawn as a solid line under the scatter symbol but
  leave some free space around the symbol.
  
  This feature uses the current scatter size (\ref setScatterSize) to determine the size of the
  area to leave blank. So when drawing Pixmaps as scatter points (\ref QCP::ssPixmap), the scatter size
  must be set manually to a value corresponding to the size of the Pixmap, if the error bars should
  leave gaps to its boundaries.
*/
void QCPGraph::setErrorBarSkipSymbol(bool enabled)
{
  mErrorBarSkipSymbol = enabled;
}

/*! 
  Sets the target graph for filling the area between this graph and \a targetGraph with the current
  brush (\ref setBrush).
  
  When \a targetGraph is set to 0, a normal graph fill will be produced. This means, when the brush
  is not Qt::NoBrush or fully transparent, a fill all the way to the zero-value-line parallel to
  the key axis of this graph will be drawn. To disable any filling, set the brush to Qt::NoBrush.
  \see setBrush
*/
void QCPGraph::setChannelFillGraph(QCPGraph *targetGraph)
{
  // prevent setting channel target to this graph itself:
  if (targetGraph == this)
  {
    qDebug() << Q_FUNC_INFO << "targetGraph is this graph itself";
    mChannelFillGraph = 0;
    return;
  }
  // prevent setting channel target to a graph not in the plot:
  if (targetGraph && targetGraph->mParentPlot != mParentPlot)
  {
    qDebug() << Q_FUNC_INFO << "targetGraph not in same plot";
    mChannelFillGraph = 0;
    return;
  }
  
  mChannelFillGraph = targetGraph;
}

/*!
  Adds the provided data points in \a dataMap to the current data.
  \see removeData
*/
void QCPGraph::addData(const QCPDataMap &dataMap)
{
  mData->unite(dataMap);
}

/*! \overload
  Adds the provided single data point in \a data to the current data.
  \see removeData
*/
void QCPGraph::addData(const QCPData &data)
{
  mData->insertMulti(data.key, data);
}

/*! \overload
  Adds the provided single data point as \a key and \a value pair to the current data.
  \see removeData
*/
void QCPGraph::addData(double key, double value)
{
  QCPData newData;
  newData.key = key;
  newData.value = value;
  mData->insertMulti(newData.key, newData);
}

/*! \overload
  Adds the provided data points as \a key and \a value pairs to the current data.
  \see removeData
*/
void QCPGraph::addData(const QVector<double> &keys, const QVector<double> &values)
{
  int n = qMin(keys.size(), values.size());
  QCPData newData;
  for (int i=0; i<n; ++i)
  {
    newData.key = keys[i];
    newData.value = values[i];
    mData->insertMulti(newData.key, newData);
  }
}

/*!
  Removes all data points with keys smaller than \a key.
  \see addData, clearData
*/
void QCPGraph::removeDataBefore(double key)
{
  QCPDataMap::iterator it = mData->begin();
  while (it != mData->end() && it.key() < key)
    it = mData->erase(it);
}

/*!
  Removes all data points with keys greater than \a key.
  \see addData, clearData
*/
void QCPGraph::removeDataAfter(double key)
{
  if (mData->isEmpty()) return;
  QCPDataMap::iterator it = mData->upperBound(key);
  while (it != mData->end())
    it = mData->erase(it);
}

/*!
  Removes all data points with keys between \a fromKey and \a toKey.
  if \a fromKey is greater or equal to \a toKey, the function does nothing. To remove
  a single data point with known key, use \ref removeData(double key).
  
  \see addData, clearData
*/
void QCPGraph::removeData(double fromKey, double toKey)
{
  if (fromKey >= toKey || mData->isEmpty()) return;
  QCPDataMap::iterator it = mData->upperBound(fromKey);
  QCPDataMap::iterator itEnd = mData->upperBound(toKey);
  while (it != itEnd)
    it = mData->erase(it);
}

/*! \overload
  
  Removes a single data point at \a key. If the position is not known with absolute precision,
  consider using \ref removeData(double fromKey, double toKey) with a small fuzziness interval around
  the suspected position, depeding on the precision with which the key is known.

  \see addData, clearData
*/
void QCPGraph::removeData(double key)
{
  mData->remove(key);
}

/*!
  Removes all data points.
  \see removeData, removeDataAfter, removeDataBefore
*/
void QCPGraph::clearData()
{
  mData->clear();
}

/* inherits documentation from base class */
double QCPGraph::selectTest(const QPointF &pos) const
{
  if (mData->isEmpty() || !mVisible)
    return -1;
  
  return pointDistance(pos);
}

/*! \overload
  
  Allows to define whether error bars are taken into consideration when determining the new axis
  range.
*/
void QCPGraph::rescaleAxes(bool onlyEnlarge, bool includeErrorBars) const
{
  rescaleKeyAxis(onlyEnlarge, includeErrorBars);
  rescaleValueAxis(onlyEnlarge, includeErrorBars);
}

/*! \overload
  
  Allows to define whether error bars (of kind \ref QCPGraph::etKey) are taken into consideration
  when determining the new axis range.
*/
void QCPGraph::rescaleKeyAxis(bool onlyEnlarge, bool includeErrorBars) const
{
  // this code is a copy of QCPAbstractPlottable::rescaleKeyAxis with the only change
  // that getKeyRange is passed the includeErrorBars value.
  if (mData->isEmpty()) return;

  SignDomain signDomain = sdBoth;
  if (mKeyAxis->scaleType() == QCPAxis::stLogarithmic)
    signDomain = (mKeyAxis->range().upper < 0 ? sdNegative : sdPositive);
  
  bool validRange;
  QCPRange newRange = getKeyRange(validRange, signDomain, includeErrorBars);
  
  if (validRange)
  {
    if (onlyEnlarge)
    {
      if (mKeyAxis->range().lower < newRange.lower)
        newRange.lower = mKeyAxis->range().lower;
      if (mKeyAxis->range().upper > newRange.upper)
        newRange.upper = mKeyAxis->range().upper;
    }
    mKeyAxis->setRange(newRange);
  }
}

/*! \overload
  
  Allows to define whether error bars (of kind \ref QCPGraph::etValue) are taken into consideration
  when determining the new axis range.
*/
void QCPGraph::rescaleValueAxis(bool onlyEnlarge, bool includeErrorBars) const
{
  // this code is a copy of QCPAbstractPlottable::rescaleValueAxis with the only change
  // is that getValueRange is passed the includeErrorBars value.
  if (mData->isEmpty()) return;

  SignDomain signDomain = sdBoth;
  if (mValueAxis->scaleType() == QCPAxis::stLogarithmic)
    signDomain = (mValueAxis->range().upper < 0 ? sdNegative : sdPositive);
  
  bool validRange;
  QCPRange newRange = getValueRange(validRange, signDomain, includeErrorBars);
  
  if (validRange)
  {
    if (onlyEnlarge)
    {
      if (mValueAxis->range().lower < newRange.lower)
        newRange.lower = mValueAxis->range().lower;
      if (mValueAxis->range().upper > newRange.upper)
        newRange.upper = mValueAxis->range().upper;
    }
    mValueAxis->setRange(newRange);
  }
}

/* inherits documentation from base class */
void QCPGraph::draw(QCPPainter *painter)
{
  if (mKeyAxis->range().size() <= 0 || mData->isEmpty()) return;
  if (mLineStyle == lsNone && mScatterStyle == QCP::ssNone) return;
  
  // allocate line and (if necessary) point vectors:
  QVector<QPointF> *lineData = new QVector<QPointF>;
  QVector<QCPData> *pointData = 0;
  if (mScatterStyle != QCP::ssNone)
    pointData = new QVector<QCPData>;
  
  // fill vectors with data appropriate to plot style:
  getPlotData(lineData, pointData);

  // draw fill of graph:
  drawFill(painter, lineData);
  
  // draw line:
  if (mLineStyle == lsImpulse)
    drawImpulsePlot(painter, lineData);
  else if (mLineStyle != lsNone)
    drawLinePlot(painter, lineData); // also step plots can be drawn as a line plot
  
  // draw scatters:
  if (pointData)
    drawScatterPlot(painter, pointData);
  
  // free allocated line and point vectors:
  delete lineData;
  if (pointData)
    delete pointData;
}

/* inherits documentation from base class */
void QCPGraph::drawLegendIcon(QCPPainter *painter, const QRect &rect) const
{
  // draw fill:
  if (mBrush.style() != Qt::NoBrush)
  {
    applyFillAntialiasingHint(painter);
    painter->fillRect(QRectF(rect.left(), rect.top()+rect.height()/2.0, rect.width(), rect.height()/3.0), mBrush);
  }
  // draw line vertically centered:
  if (mLineStyle != lsNone)
  {
    applyDefaultAntialiasingHint(painter);
    painter->setPen(mPen);
    painter->drawLine(QLineF(rect.left(), rect.top()+rect.height()/2.0, rect.right()+5, rect.top()+rect.height()/2.0)); // +5 on x2 else last segment is missing from dashed/dotted pens
  }
  // draw scatter symbol:
  if (mScatterStyle != QCP::ssNone)
  {
    if (mScatterStyle == QCP::ssPixmap && (mScatterPixmap.size().width() > rect.width() || mScatterPixmap.size().height() > rect.height()))
    {
      // handle pixmap scatters that are larger than legend icon rect separately.
      // We resize them and draw them manually, instead of calling drawScatter:
      QSize newSize = mScatterPixmap.size();
      newSize.scale(rect.size(), Qt::KeepAspectRatio);
      QRect targetRect;
      targetRect.setSize(newSize);
      targetRect.moveCenter(rect.center());
      bool smoothBackup = painter->testRenderHint(QPainter::SmoothPixmapTransform);
      painter->setRenderHint(QPainter::SmoothPixmapTransform, true);
      painter->drawPixmap(targetRect, mScatterPixmap);
      painter->setRenderHint(QPainter::SmoothPixmapTransform, smoothBackup);
    } else
    {
      applyScattersAntialiasingHint(painter);
      painter->setPen(mPen);
      painter->drawScatter(QRectF(rect).center().x(), QRectF(rect).center().y(), mScatterSize, mScatterStyle);
    }
  }
}

/*! 
  \internal
  This function branches out to the line style specific "get(...)PlotData" functions, according to the
  line style of the graph.
  \param lineData will be filled with raw points that will be drawn with the according draw functions, e.g. \ref drawLinePlot and \ref drawImpulsePlot.
  These aren't necessarily the original data points, since for step plots for example, additional points are needed for drawing lines that make up steps.
  If the line style of the graph is \ref lsNone, the \a lineData vector will be left untouched.
  \param pointData will be filled with the original data points so \ref drawScatterPlot can draw the scatter symbols accordingly. If no scatters need to be
  drawn, i.e. scatter style is \ref QCP::ssNone, pass 0 as \a pointData, and this step will be skipped.
  
  \see getScatterPlotData, getLinePlotData, getStepLeftPlotData, getStepRightPlotData, getStepCenterPlotData, getImpulsePlotData
*/
void QCPGraph::getPlotData(QVector<QPointF> *lineData, QVector<QCPData> *pointData) const
{
  switch(mLineStyle)
  {
    case lsNone: getScatterPlotData(pointData); break;
    case lsLine: getLinePlotData(lineData, pointData); break;
    case lsStepLeft: getStepLeftPlotData(lineData, pointData); break;
    case lsStepRight: getStepRightPlotData(lineData, pointData); break;
    case lsStepCenter: getStepCenterPlotData(lineData, pointData); break;
    case lsImpulse: getImpulsePlotData(lineData, pointData); break;
  }
}

/*! 
  \internal
  If line style is \ref lsNone and scatter style is not \ref QCP::ssNone, this function serves at providing the
  visible data points in \a pointData, so the \ref drawScatterPlot function can draw the scatter points
  accordingly.
  
  If line style is not \ref lsNone, this function is not called and the data for the scatter points
  are (if needed) calculated inside the corresponding other "get(...)PlotData" functions.
  \see drawScatterPlot
*/
void QCPGraph::getScatterPlotData(QVector<QCPData> *pointData) const
{
  if (!pointData) return;
  
  // get visible data range:
  QCPDataMap::const_iterator lower, upper;
  int dataCount;
  getVisibleDataBounds(lower, upper, dataCount);
  // prepare vectors:
  if (pointData)
    pointData->resize(dataCount);

  // position data points:
  QCPDataMap::const_iterator it = lower;
  QCPDataMap::const_iterator upperEnd = upper+1;
  int i = 0;
  if (mKeyAxis->orientation() == Qt::Vertical)
  {
    while (it != upperEnd)
    {
      (*pointData)[i] = it.value();
      ++i;
      ++it;
    }
  } else // key axis is horizontal
  {
    while (it != upperEnd)
    {
      (*pointData)[i] = it.value();
      ++i;
      ++it;
    }
  }
}

/*! 
  \internal
  Places the raw data points needed for a normal linearly connected plot in \a lineData.

  As for all plot data retrieval functions, \a pointData just contains all unaltered data (scatter)
  points that are visible, for drawing scatter points, if necessary. If drawing scatter points is
  disabled (i.e. scatter style \ref QCP::ssNone), pass 0 as \a pointData, and the function will skip
  filling the vector.
  \see drawLinePlot
*/
void QCPGraph::getLinePlotData(QVector<QPointF> *lineData, QVector<QCPData> *pointData) const
{
  // get visible data range:
  QCPDataMap::const_iterator lower, upper;
  int dataCount;
  getVisibleDataBounds(lower, upper, dataCount);
  // prepare vectors:
  if (lineData)
  { 
    // added 2 to reserve memory for lower/upper fill base points that might be needed for fill
    lineData->reserve(dataCount+2);
    lineData->resize(dataCount);
  }
  if (pointData)
    pointData->resize(dataCount);

  // position data points:
  QCPDataMap::const_iterator it = lower;
  QCPDataMap::const_iterator upperEnd = upper+1;
  int i = 0;
  if (mKeyAxis->orientation() == Qt::Vertical)
  {
    while (it != upperEnd)
    {
      if (pointData)
        (*pointData)[i] = it.value();
      (*lineData)[i].setX(mValueAxis->coordToPixel(it.value().value));
      (*lineData)[i].setY(mKeyAxis->coordToPixel(it.key()));
      ++i;
      ++it;
    }
  } else // key axis is horizontal
  {
    while (it != upperEnd)
    {
      if (pointData)
        (*pointData)[i] = it.value();
      (*lineData)[i].setX(mKeyAxis->coordToPixel(it.key()));
      (*lineData)[i].setY(mValueAxis->coordToPixel(it.value().value));
      ++i;
      ++it;
    }
  }
}

/*! 
  \internal
  Places the raw data points needed for a step plot with left oriented steps in \a lineData.

  As for all plot data retrieval functions, \a pointData just contains all unaltered data (scatter)
  points that are visible, for drawing scatter points, if necessary. If drawing scatter points is
  disabled (i.e. scatter style \ref QCP::ssNone), pass 0 as \a pointData, and the function will skip
  filling the vector.
  \see drawLinePlot
*/
void QCPGraph::getStepLeftPlotData(QVector<QPointF> *lineData, QVector<QCPData> *pointData) const
{
  // get visible data range:
  QCPDataMap::const_iterator lower, upper;
  int dataCount;
  getVisibleDataBounds(lower, upper, dataCount);
  // prepare vectors:
  if (lineData)
  {
    // added 2 to reserve memory for lower/upper fill base points that might be needed for fill
    // multiplied by 2 because step plot needs two polyline points per one actual data point
    lineData->reserve(dataCount*2+2);
    lineData->resize(dataCount*2);
  }
  if (pointData)
    pointData->resize(dataCount);
  
  // position data points:
  QCPDataMap::const_iterator it = lower;
  QCPDataMap::const_iterator upperEnd = upper+1;
  int i = 0;
  int ipoint = 0;
  if (mKeyAxis->orientation() == Qt::Vertical)
  {
    double lastValue = mValueAxis->coordToPixel(it.value().value);
    double key;
    while (it != upperEnd)
    {
      if (pointData)
      {
        (*pointData)[ipoint] = it.value();
        ++ipoint;
      }
      key = mKeyAxis->coordToPixel(it.key());
      (*lineData)[i].setX(lastValue);
      (*lineData)[i].setY(key);
      ++i;
      lastValue = mValueAxis->coordToPixel(it.value().value);
      (*lineData)[i].setX(lastValue);
      (*lineData)[i].setY(key);
      ++i;
      ++it;
    }
  } else // key axis is horizontal
  {
    double lastValue = mValueAxis->coordToPixel(it.value().value);
    double key;
    while (it != upperEnd)
    {
      if (pointData)
      {
        (*pointData)[ipoint] = it.value();
        ++ipoint;
      }
      key = mKeyAxis->coordToPixel(it.key());
      (*lineData)[i].setX(key);
      (*lineData)[i].setY(lastValue);
      ++i;
      lastValue = mValueAxis->coordToPixel(it.value().value);
      (*lineData)[i].setX(key);
      (*lineData)[i].setY(lastValue);
      ++i;
      ++it;
    }
  }
}

/*! 
  \internal
  Places the raw data points needed for a step plot with right oriented steps in \a lineData.

  As for all plot data retrieval functions, \a pointData just contains all unaltered data (scatter)
  points that are visible, for drawing scatter points, if necessary. If drawing scatter points is
  disabled (i.e. scatter style \ref QCP::ssNone), pass 0 as \a pointData, and the function will skip
  filling the vector.
  \see drawLinePlot
*/
void QCPGraph::getStepRightPlotData(QVector<QPointF> *lineData, QVector<QCPData> *pointData) const
{
  // get visible data range:
  QCPDataMap::const_iterator lower, upper;
  int dataCount;
  getVisibleDataBounds(lower, upper, dataCount);
  // prepare vectors:
  if (lineData)
  {
    // added 2 to reserve memory for lower/upper fill base points that might be needed for fill
    // multiplied by 2 because step plot needs two polyline points per one actual data point
    lineData->reserve(dataCount*2+2);
    lineData->resize(dataCount*2);
  }
  if (pointData)
    pointData->resize(dataCount);
  
  // position points:
  QCPDataMap::const_iterator it = lower;
  QCPDataMap::const_iterator upperEnd = upper+1;
  int i = 0;
  int ipoint = 0;
  if (mKeyAxis->orientation() == Qt::Vertical)
  {
    double lastKey = mKeyAxis->coordToPixel(it.key());
    double value;
    while (it != upperEnd)
    {
      if (pointData)
      {
        (*pointData)[ipoint] = it.value();
        ++ipoint;
      }
      value = mValueAxis->coordToPixel(it.value().value);
      (*lineData)[i].setX(value);
      (*lineData)[i].setY(lastKey);
      ++i;
      lastKey = mKeyAxis->coordToPixel(it.key());
      (*lineData)[i].setX(value);
      (*lineData)[i].setY(lastKey);
      ++i;
      ++it;
    }
  } else // key axis is horizontal
  {
    double lastKey = mKeyAxis->coordToPixel(it.key());
    double value;
    while (it != upperEnd)
    {
      if (pointData)
      {
        (*pointData)[ipoint] = it.value();
        ++ipoint;
      }
      value = mValueAxis->coordToPixel(it.value().value);
      (*lineData)[i].setX(lastKey);
      (*lineData)[i].setY(value);
      ++i;
      lastKey = mKeyAxis->coordToPixel(it.key());
      (*lineData)[i].setX(lastKey);
      (*lineData)[i].setY(value);
      ++i;
      ++it;
    }
  }
}

/*! 
  \internal
  Places the raw data points needed for a step plot with centered steps in \a lineData.

  As for all plot data retrieval functions, \a pointData just contains all unaltered data (scatter)
  points that are visible, for drawing scatter points, if necessary. If drawing scatter points is
  disabled (i.e. scatter style \ref QCP::ssNone), pass 0 as \a pointData, and the function will skip
  filling the vector.
  \see drawLinePlot
*/
void QCPGraph::getStepCenterPlotData(QVector<QPointF> *lineData, QVector<QCPData> *pointData) const
{
  // get visible data range:
  QCPDataMap::const_iterator lower, upper;
  int dataCount;
  getVisibleDataBounds(lower, upper, dataCount);
  // prepare vectors:
  if (lineData)
  {
    // added 2 to reserve memory for lower/upper fill base points that might be needed for base fill
    // multiplied by 2 because step plot needs two polyline points per one actual data point
    lineData->reserve(dataCount*2+2);
    lineData->resize(dataCount*2);
  }
  if (pointData)
    pointData->resize(dataCount);
  
  // position points:
  QCPDataMap::const_iterator it = lower;
  QCPDataMap::const_iterator upperEnd = upper+1;
  int i = 0;
  int ipoint = 0;
  if (mKeyAxis->orientation() == Qt::Vertical)
  {
    double lastKey = mKeyAxis->coordToPixel(it.key());
    double lastValue = mValueAxis->coordToPixel(it.value().value);
    double key;
    if (pointData)
    {
      (*pointData)[ipoint] = it.value();
      ++ipoint;
    }
    (*lineData)[i].setX(lastValue);
    (*lineData)[i].setY(lastKey);
    ++it;
    ++i;
    while (it != upperEnd)
    {
      if (pointData)
      {
        (*pointData)[ipoint] = it.value();
        ++ipoint;
      }
      key = (mKeyAxis->coordToPixel(it.key())-lastKey)*0.5 + lastKey;
      (*lineData)[i].setX(lastValue);
      (*lineData)[i].setY(key);
      ++i;
      lastValue = mValueAxis->coordToPixel(it.value().value);
      lastKey = mKeyAxis->coordToPixel(it.key());
      (*lineData)[i].setX(lastValue);
      (*lineData)[i].setY(key);
      ++it;
      ++i;
    }
    (*lineData)[i].setX(lastValue);
    (*lineData)[i].setY(lastKey);
  } else // key axis is horizontal
  {
    double lastKey = mKeyAxis->coordToPixel(it.key());
    double lastValue = mValueAxis->coordToPixel(it.value().value);
    double key;
    if (pointData)
    {
      (*pointData)[ipoint] = it.value();
      ++ipoint;
    }
    (*lineData)[i].setX(lastKey);
    (*lineData)[i].setY(lastValue);
    ++it;
    ++i;
    while (it != upperEnd)
    {
      if (pointData)
      {
        (*pointData)[ipoint] = it.value();
        ++ipoint;
      }
      key = (mKeyAxis->coordToPixel(it.key())-lastKey)*0.5 + lastKey;
      (*lineData)[i].setX(key);
      (*lineData)[i].setY(lastValue);
      ++i;
      lastValue = mValueAxis->coordToPixel(it.value().value);
      lastKey = mKeyAxis->coordToPixel(it.key());
      (*lineData)[i].setX(key);
      (*lineData)[i].setY(lastValue);
      ++it;
      ++i;
    }
    (*lineData)[i].setX(lastKey);
    (*lineData)[i].setY(lastValue);
  }
}

/*! 
  \internal
  Places the raw data points needed for an impulse plot in \a lineData.

  As for all plot data retrieval functions, \a pointData just contains all unaltered data (scatter)
  points that are visible, for drawing scatter points, if necessary. If drawing scatter points is
  disabled (i.e. scatter style \ref QCP::ssNone), pass 0 as \a pointData, and the function will skip
  filling the vector.
  \see drawImpulsePlot
*/
void QCPGraph::getImpulsePlotData(QVector<QPointF> *lineData, QVector<QCPData> *pointData) const
{
  // get visible data range:
  QCPDataMap::const_iterator lower, upper;
  int dataCount;
  getVisibleDataBounds(lower, upper, dataCount);
  // prepare vectors:
  if (lineData)
  {
    // no need to reserve 2 extra points, because there is no fill for impulse plot
    lineData->resize(dataCount*2);
  }
  if (pointData)
    pointData->resize(dataCount);
  
  // position data points:
  QCPDataMap::const_iterator it = lower;
  QCPDataMap::const_iterator upperEnd = upper+1;
  int i = 0;
  int ipoint = 0;
  if (mKeyAxis->orientation() == Qt::Vertical)
  {
    double zeroPointX = mValueAxis->coordToPixel(0);
    double key;
    while (it != upperEnd)
    {
      if (pointData)
      {
        (*pointData)[ipoint] = it.value();
        ++ipoint;
      }
      key = mKeyAxis->coordToPixel(it.key());
      (*lineData)[i].setX(zeroPointX);
      (*lineData)[i].setY(key);
      ++i;
      (*lineData)[i].setX(mValueAxis->coordToPixel(it.value().value));
      (*lineData)[i].setY(key);
      ++i;
      ++it;
    }
  } else // key axis is horizontal
  {
    double zeroPointY = mValueAxis->coordToPixel(0);
    double key;
    while (it != upperEnd)
    {
      if (pointData)
      {
        (*pointData)[ipoint] = it.value();
        ++ipoint;
      }
      key = mKeyAxis->coordToPixel(it.key());
      (*lineData)[i].setX(key);
      (*lineData)[i].setY(zeroPointY);
      ++i;
      (*lineData)[i].setX(key);
      (*lineData)[i].setY(mValueAxis->coordToPixel(it.value().value));
      ++i;
      ++it;
    }
  }
}

/*! 
  \internal
  Draws the fill of the graph with the specified brush. If the fill is a normal "base" fill, i.e.
  under the graph toward the zero-value-line, only the \a lineData is required (and two extra points
  at the zero-value-line, which are added by \ref addFillBasePoints and removed by \ref removeFillBasePoints
  after the fill drawing is done).
  
  If the fill is a channel fill between this graph and another graph (mChannelFillGraph), the more complex
  polygon is calculated with the \ref getChannelFillPolygon function.
  \see drawLinePlot
*/
void QCPGraph::drawFill(QCPPainter *painter, QVector<QPointF> *lineData) const
{
  if (mLineStyle == lsImpulse) return; // fill doesn't make sense for impulse plot
  if (mainBrush().style() == Qt::NoBrush || mainBrush().color().alpha() == 0) return;
  
  applyFillAntialiasingHint(painter);
  if (!mChannelFillGraph)
  {
    // draw base fill under graph, fill goes all the way to the zero-value-line:
    addFillBasePoints(lineData);
    painter->setPen(Qt::NoPen);
    painter->setBrush(mainBrush());
    painter->drawPolygon(QPolygonF(*lineData));
    removeFillBasePoints(lineData);
  } else
  {
    // draw channel fill between this graph and mChannelFillGraph:
    painter->setPen(Qt::NoPen);
    painter->setBrush(mainBrush());
    painter->drawPolygon(getChannelFillPolygon(lineData));
  }
}

/*! \internal
  
  Draws scatter symbols at every data point passed in \a pointData. scatter symbols are independent of
  the line style and are always drawn if scatter style is not \ref QCP::ssNone. Hence, the \a pointData vector
  is outputted by all "get(...)PlotData" functions, together with the (line style dependent) line data.
  \see drawLinePlot, drawImpulsePlot
*/
void QCPGraph::drawScatterPlot(QCPPainter *painter, QVector<QCPData> *pointData) const
{
  // draw error bars:
  if (mErrorType != etNone)
  {
    applyErrorBarsAntialiasingHint(painter);
    painter->setPen(mErrorPen);
    if (mKeyAxis->orientation() == Qt::Vertical)
    {
      for (int i=0; i<pointData->size(); ++i)
        drawError(painter, mValueAxis->coordToPixel(pointData->at(i).value), mKeyAxis->coordToPixel(pointData->at(i).key), pointData->at(i));
    } else
    {
      for (int i=0; i<pointData->size(); ++i)
        drawError(painter, mKeyAxis->coordToPixel(pointData->at(i).key), mValueAxis->coordToPixel(pointData->at(i).value), pointData->at(i));
    }
  }
  
  // draw scatter point symbols:
  applyScattersAntialiasingHint(painter);
  painter->setPen(mainPen());
  painter->setBrush(mainBrush());
  painter->setScatterPixmap(mScatterPixmap);
  if (mKeyAxis->orientation() == Qt::Vertical)
  {
    for (int i=0; i<pointData->size(); ++i)
      painter->drawScatter(mValueAxis->coordToPixel(pointData->at(i).value), mKeyAxis->coordToPixel(pointData->at(i).key), mScatterSize, mScatterStyle);
  } else
  {
    for (int i=0; i<pointData->size(); ++i)
      painter->drawScatter(mKeyAxis->coordToPixel(pointData->at(i).key), mValueAxis->coordToPixel(pointData->at(i).value), mScatterSize, mScatterStyle);
  }
}

/*! 
  \internal
  Draws line graphs from the provided data. It connects all points in \a lineData, which
  was created by one of the "get(...)PlotData" functions for line styles that require simple line
  connections between the point vector they create. These are for example \ref getLinePlotData, \ref
  getStepLeftPlotData, \ref getStepRightPlotData and \ref getStepCenterPlotData.
  \see drawScatterPlot, drawImpulsePlot
*/
void QCPGraph::drawLinePlot(QCPPainter *painter, QVector<QPointF> *lineData) const
{
  // draw line of graph:
  if (mainPen().style() != Qt::NoPen && mainPen().color().alpha() != 0)
  {
    applyDefaultAntialiasingHint(painter);
    painter->setPen(mainPen());
    painter->setBrush(Qt::NoBrush);
    
    /* Draws polyline in batches, currently not used:
    int p = 0;
    while (p < lineData->size())
    {
      int batch = qMin(25, lineData->size()-p);
      if (p != 0)
      {
        ++batch;
        --p; // to draw the connection lines between two batches
      }
      painter->drawPolyline(lineData->constData()+p, batch);
      p += batch;
    }
    */
    
    // if drawing solid line and not in PDF, use much faster line drawing instead of polyline:
    if (mParentPlot->plottingHints().testFlag(QCP::phFastPolylines) &&
        painter->pen().style() == Qt::SolidLine &&
        !painter->pdfExportMode())
    {
      for (int i=1; i<lineData->size(); ++i)
        painter->drawLine(lineData->at(i-1), lineData->at(i));
    } else
    {  
      painter->drawPolyline(QPolygonF(*lineData));
    }
  }
}

/*! 
  \internal
  Draws impulses graphs from the provided data, i.e. it connects all line pairs in \a lineData, which was
  created by \ref getImpulsePlotData.
  \see drawScatterPlot, drawLinePlot
*/
void QCPGraph::drawImpulsePlot(QCPPainter *painter, QVector<QPointF> *lineData) const
{
  // draw impulses:
  if (mainPen().style() != Qt::NoPen && mainPen().color().alpha() != 0)
  {
    applyDefaultAntialiasingHint(painter);
    QPen pen = mainPen();
    pen.setCapStyle(Qt::FlatCap); // so impulse line doesn't reach beyond zero-line
    painter->setPen(pen);
    painter->setBrush(Qt::NoBrush);
    painter->drawLines(*lineData);
  }
}

/*! 
  \internal
  called by the scatter drawing function (\ref drawScatterPlot) to draw the error bars on one data
  point. \a x and \a y pixel positions of the data point are passed since they are already known in
  pixel coordinates in the drawing function, so we save some extra coordToPixel transforms here. \a
  data is therefore only used for the errors, not key and value.
*/
void QCPGraph::drawError(QCPPainter *painter, double x, double y, const QCPData &data) const
{
  double a, b; // positions of error bar bounds in pixels
  double barWidthHalf = mErrorBarSize*0.5;
  double skipSymbolMargin = mScatterSize*1.25; // pixels left blank per side, when mErrorBarSkipSymbol is true

  if (mKeyAxis->orientation() == Qt::Vertical)
  {
    // draw key error vertically and value error horizontally
    if (mErrorType == etKey || mErrorType == etBoth)
    {
      a = mKeyAxis->coordToPixel(data.key-data.keyErrorMinus);
      b = mKeyAxis->coordToPixel(data.key+data.keyErrorPlus);
      if (mKeyAxis->rangeReversed())
        qSwap(a,b);
      // draw spine:
      if (mErrorBarSkipSymbol)
      {
        if (a-y > skipSymbolMargin) // don't draw spine if error is so small it's within skipSymbolmargin
          painter->drawLine(QLineF(x, a, x, y+skipSymbolMargin));
        if (y-b > skipSymbolMargin) 
          painter->drawLine(QLineF(x, y-skipSymbolMargin, x, b));
      } else
        painter->drawLine(QLineF(x, a, x, b));
      // draw handles:
      painter->drawLine(QLineF(x-barWidthHalf, a, x+barWidthHalf, a));
      painter->drawLine(QLineF(x-barWidthHalf, b, x+barWidthHalf, b));
    }
    if (mErrorType == etValue || mErrorType == etBoth)
    {
      a = mValueAxis->coordToPixel(data.value-data.valueErrorMinus);
      b = mValueAxis->coordToPixel(data.value+data.valueErrorPlus);
      if (mValueAxis->rangeReversed())
        qSwap(a,b);
      // draw spine:
      if (mErrorBarSkipSymbol)
      {
        if (x-a > skipSymbolMargin) // don't draw spine if error is so small it's within skipSymbolmargin
          painter->drawLine(QLineF(a, y, x-skipSymbolMargin, y));
        if (b-x > skipSymbolMargin)
          painter->drawLine(QLineF(x+skipSymbolMargin, y, b, y));
      } else
        painter->drawLine(QLineF(a, y, b, y));
      // draw handles:
      painter->drawLine(QLineF(a, y-barWidthHalf, a, y+barWidthHalf));
      painter->drawLine(QLineF(b, y-barWidthHalf, b, y+barWidthHalf));
    }
  } else // mKeyAxis->orientation() is Qt::Horizontal
  {
    // draw value error vertically and key error horizontally
    if (mErrorType == etKey || mErrorType == etBoth)
    {
      a = mKeyAxis->coordToPixel(data.key-data.keyErrorMinus);
      b = mKeyAxis->coordToPixel(data.key+data.keyErrorPlus);
      if (mKeyAxis->rangeReversed())
        qSwap(a,b);
      // draw spine:
      if (mErrorBarSkipSymbol)
      {
        if (x-a > skipSymbolMargin) // don't draw spine if error is so small it's within skipSymbolmargin
          painter->drawLine(QLineF(a, y, x-skipSymbolMargin, y));
        if (b-x > skipSymbolMargin)
          painter->drawLine(QLineF(x+skipSymbolMargin, y, b, y));
      } else
        painter->drawLine(QLineF(a, y, b, y));
      // draw handles:
      painter->drawLine(QLineF(a, y-barWidthHalf, a, y+barWidthHalf));
      painter->drawLine(QLineF(b, y-barWidthHalf, b, y+barWidthHalf));
    }
    if (mErrorType == etValue || mErrorType == etBoth)
    {
      a = mValueAxis->coordToPixel(data.value-data.valueErrorMinus);
      b = mValueAxis->coordToPixel(data.value+data.valueErrorPlus);
      if (mValueAxis->rangeReversed())
        qSwap(a,b);
      // draw spine:
      if (mErrorBarSkipSymbol)
      {
        if (a-y > skipSymbolMargin) // don't draw spine if error is so small it's within skipSymbolmargin
          painter->drawLine(QLineF(x, a, x, y+skipSymbolMargin));
        if (y-b > skipSymbolMargin)
          painter->drawLine(QLineF(x, y-skipSymbolMargin, x, b));
      } else
        painter->drawLine(QLineF(x, a, x, b));
      // draw handles:
      painter->drawLine(QLineF(x-barWidthHalf, a, x+barWidthHalf, a));
      painter->drawLine(QLineF(x-barWidthHalf, b, x+barWidthHalf, b));
    }
  }
}

/*! 
  \internal
  called by the specific plot data generating functions "get(...)PlotData" to determine
  which data range is visible, so only that needs to be processed.
  
  \param[out] lower returns an iterator to the lowest data point that needs to be taken into account
  when plotting. Note that in order to get a clean plot all the way to the edge of the axes, \a lower
  may still be outside the visible range.
  \param[out] upper returns an iterator to the highest data point. Same as before, \a upper may also
  lie outside of the visible range.
  \param[out] count number of data points that need plotting, i.e. points between \a lower and \a upper,
  including them. This is useful for allocating the array of QPointFs in the specific drawing functions.
*/
void QCPGraph::getVisibleDataBounds(QCPDataMap::const_iterator &lower, QCPDataMap::const_iterator &upper, int &count) const
{
  // get visible data range as QMap iterators
  QCPDataMap::const_iterator lbound = mData->lowerBound(mKeyAxis->range().lower);
  QCPDataMap::const_iterator ubound = mData->upperBound(mKeyAxis->range().upper)-1;
  bool lowoutlier = lbound != mData->constBegin(); // indicates whether there exist points below axis range
  bool highoutlier = ubound+1 != mData->constEnd(); // indicates whether there exist points above axis range
  lower = (lowoutlier ? lbound-1 : lbound); // data pointrange that will be actually drawn
  upper = (highoutlier ? ubound+1 : ubound); // data pointrange that will be actually drawn
  
  // count number of points in range lower to upper (including them), so we can allocate array for them in draw functions:
  QCPDataMap::const_iterator it = lower;
  count = 1;
  while (it != upper)
  {
    ++it;
    ++count;
  }
}

/*! 
  \internal
  The line data vector generated by e.g. getLinePlotData contains only the line
  that connects the data points. If the graph needs to be filled, two additional points
  need to be added at the value-zero-line in the lower and upper key positions, the graph
  reaches. This function calculates these points and adds them to the end of \a lineData.
  Since the fill is typically drawn before the line stroke, these added points need to
  be removed again after the fill is done, with the removeFillBasePoints function.
  
  The expanding of \a lineData by two points will not cause unnecessary memory reallocations,
  because the data vector generation functions (getLinePlotData etc.) reserve two extra points
  when they allocate memory for \a lineData.
  \see removeFillBasePoints, lowerFillBasePoint, upperFillBasePoint
*/
void QCPGraph::addFillBasePoints(QVector<QPointF> *lineData) const
{
  // append points that close the polygon fill at the key axis:
  if (mKeyAxis->orientation() == Qt::Vertical)
  {
    *lineData << upperFillBasePoint(lineData->last().y());
    *lineData << lowerFillBasePoint(lineData->first().y());
  } else
  {
    *lineData << upperFillBasePoint(lineData->last().x());
    *lineData << lowerFillBasePoint(lineData->first().x());
  }
}

/*! 
  \internal
  removes the two points from \a lineData that were added by addFillBasePoints.
  \see addFillBasePoints, lowerFillBasePoint, upperFillBasePoint
*/
void QCPGraph::removeFillBasePoints(QVector<QPointF> *lineData) const
{
  lineData->remove(lineData->size()-2, 2);
}

/*! 
  \internal
  called by addFillBasePoints to conveniently assign the point which closes the fill
  polygon on the lower side of the zero-value-line parallel to the key axis.
  The logarithmic axis scale case is a bit special, since the zero-value-line in pixel coordinates
  is in positive or negative infinity. So this case is handled separately by just closing the
  fill polygon on the axis which lies in the direction towards the zero value.
  
  \param lowerKey pixel position of the lower key of the point. Depending on whether the key axis
  is horizontal or vertical, \a lowerKey will end up as the x or y value of the returned point,
  respectively.
  \see upperFillBasePoint, addFillBasePoints
*/
QPointF QCPGraph::lowerFillBasePoint(double lowerKey) const
{
  QPointF point;
  if (mValueAxis->scaleType() == QCPAxis::stLinear)
  {
    if (mKeyAxis->axisType() == QCPAxis::atLeft)
    {
      point.setX(mValueAxis->coordToPixel(0));
      point.setY(lowerKey);
    } else if (mKeyAxis->axisType() == QCPAxis::atRight)
    {
      point.setX(mValueAxis->coordToPixel(0));
      point.setY(lowerKey);
    } else if (mKeyAxis->axisType() == QCPAxis::atTop)
    {
      point.setX(lowerKey);
      point.setY(mValueAxis->coordToPixel(0));
    } else if (mKeyAxis->axisType() == QCPAxis::atBottom)
    {
      point.setX(lowerKey);
      point.setY(mValueAxis->coordToPixel(0));
    }
  } else // mValueAxis->mScaleType == QCPAxis::stLogarithmic
  {
    // In logarithmic scaling we can't just draw to value zero so we just fill all the way
    // to the axis which is in the direction towards zero
    if (mKeyAxis->orientation() == Qt::Vertical)
    {
      if ((mValueAxis->range().upper < 0 && !mValueAxis->rangeReversed()) ||
          (mValueAxis->range().upper > 0 && mValueAxis->rangeReversed())) // if range is negative, zero is on opposite side of key axis
        point.setX(mKeyAxis->axisRect().right());
      else
        point.setX(mKeyAxis->axisRect().left());
      point.setY(lowerKey);
    } else if (mKeyAxis->axisType() == QCPAxis::atTop || mKeyAxis->axisType() == QCPAxis::atBottom)
    {
      point.setX(lowerKey);
      if ((mValueAxis->range().upper < 0 && !mValueAxis->rangeReversed()) ||
          (mValueAxis->range().upper > 0 && mValueAxis->rangeReversed())) // if range is negative, zero is on opposite side of key axis
        point.setY(mKeyAxis->axisRect().top());
      else
        point.setY(mKeyAxis->axisRect().bottom());
    }
  }
  return point;
}

/*! 
  \internal
  called by addFillBasePoints to conveniently assign the point which closes the fill
  polygon on the upper side of the zero-value-line parallel to the key axis. The logarithmic axis
  scale case is a bit special, since the zero-value-line in pixel coordinates is in positive or
  negative infinity. So this case is handled separately by just closing the fill polygon on the
  axis which lies in the direction towards the zero value.
  
  \param upperKey pixel position of the upper key of the point. Depending on whether the key axis
  is horizontal or vertical, \a upperKey will end up as the x or y value of the returned point,
  respectively.
  \see lowerFillBasePoint, addFillBasePoints
*/
QPointF QCPGraph::upperFillBasePoint(double upperKey) const
{
  QPointF point;
  if (mValueAxis->scaleType() == QCPAxis::stLinear)
  {
    if (mKeyAxis->axisType() == QCPAxis::atLeft)
    {
      point.setX(mValueAxis->coordToPixel(0));
      point.setY(upperKey);
    } else if (mKeyAxis->axisType() == QCPAxis::atRight)
    {
      point.setX(mValueAxis->coordToPixel(0));
      point.setY(upperKey);
    } else if (mKeyAxis->axisType() == QCPAxis::atTop)
    {
      point.setX(upperKey);
      point.setY(mValueAxis->coordToPixel(0));
    } else if (mKeyAxis->axisType() == QCPAxis::atBottom)
    {
      point.setX(upperKey);
      point.setY(mValueAxis->coordToPixel(0));
    }
  } else // mValueAxis->mScaleType == QCPAxis::stLogarithmic
  {
    // In logarithmic scaling we can't just draw to value 0 so we just fill all the way
    // to the axis which is in the direction towards 0
    if (mKeyAxis->orientation() == Qt::Vertical)
    {
      if ((mValueAxis->range().upper < 0 && !mValueAxis->rangeReversed()) ||
          (mValueAxis->range().upper > 0 && mValueAxis->rangeReversed())) // if range is negative, zero is on opposite side of key axis
        point.setX(mKeyAxis->axisRect().right());
      else
        point.setX(mKeyAxis->axisRect().left());
      point.setY(upperKey);
    } else if (mKeyAxis->axisType() == QCPAxis::atTop || mKeyAxis->axisType() == QCPAxis::atBottom)
    {
      point.setX(upperKey);
      if ((mValueAxis->range().upper < 0 && !mValueAxis->rangeReversed()) ||
          (mValueAxis->range().upper > 0 && mValueAxis->rangeReversed())) // if range is negative, zero is on opposite side of key axis
        point.setY(mKeyAxis->axisRect().top());
      else
        point.setY(mKeyAxis->axisRect().bottom());
    }
  }
  return point;
}

/*! \internal
  
  Generates the polygon needed for drawing channel fills between this graph (data passed via \a
  lineData) and the graph specified by mChannelFillGraph (data generated by calling its \ref
  getPlotData function). May return an empty polygon if the key ranges have no overlap or fill
  target graph and this graph don't have same orientation (i.e. both key axes horizontal or both
  key axes vertical). For increased performance (due to implicit sharing), keep the returned QPolygonF
  const.
*/
const QPolygonF QCPGraph::getChannelFillPolygon(const QVector<QPointF> *lineData) const
{
  if (mChannelFillGraph->mKeyAxis->orientation() != mKeyAxis->orientation())
    return QPolygonF(); // don't have same axis orientation, can't fill that (Note: if keyAxis fits, valueAxis will fit too, because it's always orthogonal to keyAxis)
  
  if (lineData->isEmpty()) return QPolygonF();
  QVector<QPointF> otherData;
  mChannelFillGraph->getPlotData(&otherData, 0);
  if (otherData.isEmpty()) return QPolygonF();
  QVector<QPointF> thisData;
  thisData.reserve(lineData->size()+otherData.size()); // because we will join both vectors at end of this function
  for (int i=0; i<lineData->size(); ++i) // don't use the vector<<(vector),  it squeezes internally, which ruins the performance tuning with reserve()
    thisData << lineData->at(i);
  
  // pointers to be able to swap them, depending which data range needs cropping:
  QVector<QPointF> *staticData = &thisData;
  QVector<QPointF> *croppedData = &otherData;
  
  // crop both vectors to ranges in which the keys overlap (which coord is key, depends on axisType):
  if (mKeyAxis->orientation() == Qt::Horizontal)
  {
    // x is key
    // if an axis range is reversed, the data point keys will be descending. Reverse them, since following algorithm assumes ascending keys:
    if (staticData->first().x() > staticData->last().x())
    {
      int size = staticData->size();
      for (int i=0; i<size/2; ++i)
        qSwap((*staticData)[i], (*staticData)[size-1-i]);
    }
    if (croppedData->first().x() > croppedData->last().x())
    {
      int size = croppedData->size();
      for (int i=0; i<size/2; ++i)
        qSwap((*croppedData)[i], (*croppedData)[size-1-i]);
    }
    // crop lower bound:
    if (staticData->first().x() < croppedData->first().x()) // other one must be cropped
      qSwap(staticData, croppedData);
    int lowBound = findIndexBelowX(croppedData, staticData->first().x());
    if (lowBound == -1) return QPolygonF(); // key ranges have no overlap
    croppedData->remove(0, lowBound);
    // set lowest point of cropped data to fit exactly key position of first static data
    // point via linear interpolation:
    if (croppedData->size() < 2) return QPolygonF(); // need at least two points for interpolation
    double slope;
    if (croppedData->at(1).x()-croppedData->at(0).x() != 0)
      slope = (croppedData->at(1).y()-croppedData->at(0).y())/(croppedData->at(1).x()-croppedData->at(0).x());
    else
      slope = 0;
    (*croppedData)[0].setY(croppedData->at(0).y()+slope*(staticData->first().x()-croppedData->at(0).x()));
    (*croppedData)[0].setX(staticData->first().x());
    
    // crop upper bound:
    if (staticData->last().x() > croppedData->last().x()) // other one must be cropped
      qSwap(staticData, croppedData);
    int highBound = findIndexAboveX(croppedData, staticData->last().x());
    if (highBound == -1) return QPolygonF(); // key ranges have no overlap
    croppedData->remove(highBound+1, croppedData->size()-(highBound+1));
    // set highest point of cropped data to fit exactly key position of last static data
    // point via linear interpolation:
    if (croppedData->size() < 2) return QPolygonF(); // need at least two points for interpolation
    int li = croppedData->size()-1; // last index
    if (croppedData->at(li).x()-croppedData->at(li-1).x() != 0)
      slope = (croppedData->at(li).y()-croppedData->at(li-1).y())/(croppedData->at(li).x()-croppedData->at(li-1).x());
    else
      slope = 0;
    (*croppedData)[li].setY(croppedData->at(li-1).y()+slope*(staticData->last().x()-croppedData->at(li-1).x()));
    (*croppedData)[li].setX(staticData->last().x());
  } else // mKeyAxis->orientation() == Qt::Vertical
  {
    // y is key
    // similar to "x is key" but switched x,y. Further, lower/upper meaning is inverted compared to x,
    // because in pixel coordinates, y increases from top to bottom, not bottom to top like data coordinate.
    // if an axis range is reversed, the data point keys will be descending. Reverse them, since following algorithm assumes ascending keys:
    if (staticData->first().y() < staticData->last().y())
    {
      int size = staticData->size();
      for (int i=0; i<size/2; ++i)
        qSwap((*staticData)[i], (*staticData)[size-1-i]);
    }
    if (croppedData->first().y() < croppedData->last().y())
    {
      int size = croppedData->size();
      for (int i=0; i<size/2; ++i)
        qSwap((*croppedData)[i], (*croppedData)[size-1-i]);
    }
    // crop lower bound:
    if (staticData->first().y() > croppedData->first().y()) // other one must be cropped
      qSwap(staticData, croppedData);
    int lowBound = findIndexAboveY(croppedData, staticData->first().y());
    if (lowBound == -1) return QPolygonF(); // key ranges have no overlap
    croppedData->remove(0, lowBound);
    // set lowest point of cropped data to fit exactly key position of first static data
    // point via linear interpolation:
    if (croppedData->size() < 2) return QPolygonF(); // need at least two points for interpolation
    double slope;
    if (croppedData->at(1).y()-croppedData->at(0).y() != 0) // avoid division by zero in step plots
      slope = (croppedData->at(1).x()-croppedData->at(0).x())/(croppedData->at(1).y()-croppedData->at(0).y());
    else
      slope = 0;
    (*croppedData)[0].setX(croppedData->at(0).x()+slope*(staticData->first().y()-croppedData->at(0).y()));
    (*croppedData)[0].setY(staticData->first().y());
    
    // crop upper bound:
    if (staticData->last().y() < croppedData->last().y()) // other one must be cropped
      qSwap(staticData, croppedData);
    int highBound = findIndexBelowY(croppedData, staticData->last().y());
    if (highBound == -1) return QPolygonF(); // key ranges have no overlap
    croppedData->remove(highBound+1, croppedData->size()-(highBound+1));
    // set highest point of cropped data to fit exactly key position of last static data
    // point via linear interpolation:
    if (croppedData->size() < 2) return QPolygonF(); // need at least two points for interpolation
    int li = croppedData->size()-1; // last index
    if (croppedData->at(li).y()-croppedData->at(li-1).y() != 0) // avoid division by zero in step plots
      slope = (croppedData->at(li).x()-croppedData->at(li-1).x())/(croppedData->at(li).y()-croppedData->at(li-1).y());
    else
      slope = 0;
    (*croppedData)[li].setX(croppedData->at(li-1).x()+slope*(staticData->last().y()-croppedData->at(li-1).y()));
    (*croppedData)[li].setY(staticData->last().y());
  }
  
  // return joined:
  for (int i=otherData.size()-1; i>=0; --i) // insert reversed, otherwise the polygon will be twisted
    thisData << otherData.at(i);
  return QPolygonF(thisData);
}

/*! \internal
  
  Finds the smallest index of \a data, whose points x value is just above \a x.
  Assumes x values in \a data points are ordered ascending, as is the case
  when plotting with horizontal key axis.
  Used to calculate the channel fill polygon, see \ref getChannelFillPolygon.
*/
int QCPGraph::findIndexAboveX(const QVector<QPointF> *data, double x) const
{
  for (int i=data->size()-1; i>=0; --i)
  {
    if (data->at(i).x() < x)
    {
      if (i<data->size()-1)
        return i+1;
      else
        return data->size()-1;
    }
  }
  return -1;
}

/*! \internal
  
  Finds the greatest index of \a data, whose points x value is just below \a x.
  Assumes x values in \a data points are ordered ascending, as is the case
  when plotting with horizontal key axis.
  Used to calculate the channel fill polygon, see \ref getChannelFillPolygon.
*/
int QCPGraph::findIndexBelowX(const QVector<QPointF> *data, double x) const
{
  for (int i=0; i<data->size(); ++i)
  {
    if (data->at(i).x() > x)
    {
      if (i>0)
        return i-1;
      else
        return 0;
    }
  }
  return -1;
}

/*! \internal
  
  Finds the smallest index of \a data, whose points y value is just above \a y.
  Assumes y values in \a data points are ordered descending, as is the case
  when plotting with vertical key axis.
  Used to calculate the channel fill polygon, see \ref getChannelFillPolygon.
*/
int QCPGraph::findIndexAboveY(const QVector<QPointF> *data, double y) const
{
  for (int i=0; i<data->size(); ++i)
  {
    if (data->at(i).y() < y)
    {
      if (i>0)
        return i-1;
      else
        return 0;
    }
  }
  return -1;
}

/*! \internal 
  
  Calculates the (minimum) distance (in pixels) the graph's representation has from the given \a
  pixelPoint in pixels. This is used to determine whether the graph was clicked or not, e.g. in
  \ref selectTest.
*/
double QCPGraph::pointDistance(const QPointF &pixelPoint) const
{
  if (mData->isEmpty())
  {
    qDebug() << Q_FUNC_INFO << "requested point distance on graph" << mName << "without data";
    return 500;
  }
  if (mData->size() == 1)
  {
    QPointF dataPoint = coordsToPixels(mData->constBegin().key(), mData->constBegin().value().value);
    return QVector2D(dataPoint-pixelPoint).length();
  }
  
  if (mLineStyle == lsNone && mScatterStyle == QCP::ssNone)
    return 500;
  
  // calculate minimum distances to graph representation:
  if (mLineStyle == lsNone)
  {
    // no line displayed, only calculate distance to scatter points:
    QVector<QCPData> *pointData = new QVector<QCPData>;
    getScatterPlotData(pointData);
    double minDistSqr = std::numeric_limits<double>::max();
    QPointF ptA;
    QPointF ptB = coordsToPixels(pointData->at(0).key, pointData->at(0).value); // getScatterPlotData returns in plot coordinates, so transform to pixels
    for (int i=1; i<pointData->size(); ++i)
    {
      ptA = ptB;
      ptB = coordsToPixels(pointData->at(i).key, pointData->at(i).value);
      double currentDistSqr = distSqrToLine(ptA, ptB, pixelPoint);
      if (currentDistSqr < minDistSqr)
        minDistSqr = currentDistSqr;
    }
    delete pointData;
    return sqrt(minDistSqr);
  } else
  {
    // line displayed calculate distance to line segments:
    QVector<QPointF> *lineData = new QVector<QPointF>;
    getPlotData(lineData, 0); // unlike with getScatterPlotData we get pixel coordinates here
    double minDistSqr = std::numeric_limits<double>::max();
    if (mLineStyle == lsImpulse)
    {
      // impulse plot differs from other line styles in that the lineData points are only pairwise connected:
      for (int i=0; i<lineData->size()-1; i+=2) // iterate pairs
      {
        double currentDistSqr = distSqrToLine(lineData->at(i), lineData->at(i+1), pixelPoint);
        if (currentDistSqr < minDistSqr)
          minDistSqr = currentDistSqr;
      }
    } else 
    {
      // all other line plots (line and step) connect points directly:
      for (int i=0; i<lineData->size()-1; ++i)
      {
        double currentDistSqr = distSqrToLine(lineData->at(i), lineData->at(i+1), pixelPoint);
        if (currentDistSqr < minDistSqr)
          minDistSqr = currentDistSqr;
      }
    }
    delete lineData;
    return sqrt(minDistSqr);
  }
}

/*! \internal
  
  Finds the greatest index of \a data, whose points y value is just below \a y.
  Assumes y values in \a data points are ordered descending, as is the case
  when plotting with vertical key axis (since keys are ordered ascending).
  Used to calculate the channel fill polygon, see \ref getChannelFillPolygon.
*/
int QCPGraph::findIndexBelowY(const QVector<QPointF> *data, double y) const
{
  for (int i=data->size()-1; i>=0; --i)
  {
    if (data->at(i).y() > y)
    {
      if (i<data->size()-1)
        return i+1;
      else
        return data->size()-1;
    }
  }
  return -1;
}

/* inherits documentation from base class */
QCPRange QCPGraph::getKeyRange(bool &validRange, SignDomain inSignDomain) const
{
  // just call the specialized version which takes an additional argument whether error bars
  // should also be taken into consideration for range calculation. We set this to true here.
  return getKeyRange(validRange, inSignDomain, true);
}

/* inherits documentation from base class */
QCPRange QCPGraph::getValueRange(bool &validRange, SignDomain inSignDomain) const
{
  // just call the specialized version which takes an additional argument whether error bars
  // should also be taken into consideration for range calculation. We set this to true here.
  return getValueRange(validRange, inSignDomain, true);
}

/*! \overload
  Allows to specify whether the error bars should be included in the range calculation.
  
  \see getKeyRange(bool &validRange, SignDomain inSignDomain)
*/
QCPRange QCPGraph::getKeyRange(bool &validRange, SignDomain inSignDomain, bool includeErrors) const
{
  QCPRange range;
  bool haveLower = false;
  bool haveUpper = false;
  
  double current, currentErrorMinus, currentErrorPlus;
  
  if (inSignDomain == sdBoth) // range may be anywhere
  {
    QCPDataMap::const_iterator it = mData->constBegin();
    while (it != mData->constEnd())
    {
      current = it.value().key;
      currentErrorMinus = (includeErrors ? it.value().keyErrorMinus : 0);
      currentErrorPlus = (includeErrors ? it.value().keyErrorPlus : 0);
      if (current-currentErrorMinus < range.lower || !haveLower)
      {
        range.lower = current-currentErrorMinus;
        haveLower = true;
      }
      if (current+currentErrorPlus > range.upper || !haveUpper)
      {
        range.upper = current+currentErrorPlus;
        haveUpper = true;
      }
      ++it;
    }
  } else if (inSignDomain == sdNegative) // range may only be in the negative sign domain
  {
    QCPDataMap::const_iterator it = mData->constBegin();
    while (it != mData->constEnd())
    {
      current = it.value().key;
      currentErrorMinus = (includeErrors ? it.value().keyErrorMinus : 0);
      currentErrorPlus = (includeErrors ? it.value().keyErrorPlus : 0);
      if ((current-currentErrorMinus < range.lower || !haveLower) && current-currentErrorMinus < 0)
      {
        range.lower = current-currentErrorMinus;
        haveLower = true;
      }
      if ((current+currentErrorPlus > range.upper || !haveUpper) && current+currentErrorPlus < 0)
      {
        range.upper = current+currentErrorPlus;
        haveUpper = true;
      }
      if (includeErrors) // in case point is in valid sign domain but errobars stretch beyond it, we still want to geht that point.
      {
        if ((current < range.lower || !haveLower) && current < 0)
        {
          range.lower = current;
          haveLower = true;
        }
        if ((current > range.upper || !haveUpper) && current < 0)
        {
          range.upper = current;
          haveUpper = true;
        }
      }
      ++it;
    }
  } else if (inSignDomain == sdPositive) // range may only be in the positive sign domain
  {
    QCPDataMap::const_iterator it = mData->constBegin();
    while (it != mData->constEnd())
    {
      current = it.value().key;
      currentErrorMinus = (includeErrors ? it.value().keyErrorMinus : 0);
      currentErrorPlus = (includeErrors ? it.value().keyErrorPlus : 0);
      if ((current-currentErrorMinus < range.lower || !haveLower) && current-currentErrorMinus > 0)
      {
        range.lower = current-currentErrorMinus;
        haveLower = true;
      }
      if ((current+currentErrorPlus > range.upper || !haveUpper) && current+currentErrorPlus > 0)
      {
        range.upper = current+currentErrorPlus;
        haveUpper = true;
      }
      if (includeErrors) // in case point is in valid sign domain but errobars stretch beyond it, we still want to get that point.
      {
        if ((current < range.lower || !haveLower) && current > 0)
        {
          range.lower = current;
          haveLower = true;
        }
        if ((current > range.upper || !haveUpper) && current > 0)
        {
          range.upper = current;
          haveUpper = true;
        }
      }
      ++it;
    }
  }
  
  validRange = haveLower && haveUpper;
  return range;
}

/*! \overload
  Allows to specify whether the error bars should be included in the range calculation.
  
  \see getValueRange(bool &validRange, SignDomain inSignDomain)
*/
QCPRange QCPGraph::getValueRange(bool &validRange, SignDomain inSignDomain, bool includeErrors) const
{
  QCPRange range;
  bool haveLower = false;
  bool haveUpper = false;
  
  double current, currentErrorMinus, currentErrorPlus;
  
  if (inSignDomain == sdBoth) // range may be anywhere
  {
    QCPDataMap::const_iterator it = mData->constBegin();
    while (it != mData->constEnd())
    {
      current = it.value().value;
      currentErrorMinus = (includeErrors ? it.value().valueErrorMinus : 0);
      currentErrorPlus = (includeErrors ? it.value().valueErrorPlus : 0);
      if (current-currentErrorMinus < range.lower || !haveLower)
      {
        range.lower = current-currentErrorMinus;
        haveLower = true;
      }
      if (current+currentErrorPlus > range.upper || !haveUpper)
      {
        range.upper = current+currentErrorPlus;
        haveUpper = true;
      }
      ++it;
    }
  } else if (inSignDomain == sdNegative) // range may only be in the negative sign domain
  {
    QCPDataMap::const_iterator it = mData->constBegin();
    while (it != mData->constEnd())
    {
      current = it.value().value;
      currentErrorMinus = (includeErrors ? it.value().valueErrorMinus : 0);
      currentErrorPlus = (includeErrors ? it.value().valueErrorPlus : 0);
      if ((current-currentErrorMinus < range.lower || !haveLower) && current-currentErrorMinus < 0)
      {
        range.lower = current-currentErrorMinus;
        haveLower = true;
      }
      if ((current+currentErrorPlus > range.upper || !haveUpper) && current+currentErrorPlus < 0)
      {
        range.upper = current+currentErrorPlus;
        haveUpper = true;
      }
      if (includeErrors) // in case point is in valid sign domain but errobars stretch beyond it, we still want to get that point.
      {
        if ((current < range.lower || !haveLower) && current < 0)
        {
          range.lower = current;
          haveLower = true;
        }
        if ((current > range.upper || !haveUpper) && current < 0)
        {
          range.upper = current;
          haveUpper = true;
        }
      }
      ++it;
    }
  } else if (inSignDomain == sdPositive) // range may only be in the positive sign domain
  {
    QCPDataMap::const_iterator it = mData->constBegin();
    while (it != mData->constEnd())
    {
      current = it.value().value;
      currentErrorMinus = (includeErrors ? it.value().valueErrorMinus : 0);
      currentErrorPlus = (includeErrors ? it.value().valueErrorPlus : 0);
      if ((current-currentErrorMinus < range.lower || !haveLower) && current-currentErrorMinus > 0)
      {
        range.lower = current-currentErrorMinus;
        haveLower = true;
      }
      if ((current+currentErrorPlus > range.upper || !haveUpper) && current+currentErrorPlus > 0)
      {
        range.upper = current+currentErrorPlus;
        haveUpper = true;
      }
      if (includeErrors) // in case point is in valid sign domain but errobars stretch beyond it, we still want to geht that point.
      {
        if ((current < range.lower || !haveLower) && current > 0)
        {
          range.lower = current;
          haveLower = true;
        }
        if ((current > range.upper || !haveUpper) && current > 0)
        {
          range.upper = current;
          haveUpper = true;
        }
      }
      ++it;
    }
  }
  
  validRange = haveLower && haveUpper;
  return range;
}


// ================================================================================
// =================== QCPRange
// ================================================================================
/*! \class QCPRange
  \brief Represents the range an axis is encompassing.
  
  contains a \a lower and \a upper double value and provides convenience input, output and
  modification functions.
  
  \see QCPAxis::setRange
*/

/*! 
  Minimum range size (\a upper - \a lower) the range changing functions will accept. Smaller
  intervals would cause errors due to the 11-bit exponent of double precision numbers,
  corresponding to a minimum magnitude of roughly 1e-308.
  \see validRange, maxRange
*/
const double QCPRange::minRange = 1e-280;

/*! 
  Maximum values (negative and positive) the range will accept in range-changing functions.
  Larger absolute values would cause errors due to the 11-bit exponent of double precision numbers,
  corresponding to a maximum magnitude of roughly 1e308.
  Since the number of planck-volumes in the entire visible universe is only ~1e183, this should
  be enough.
  \see validRange, minRange
*/
const double QCPRange::maxRange = 1e250;

/*! 
  Constructs a range with \a lower and \a upper set to zero.
*/
QCPRange::QCPRange() :
  lower(0),
  upper(0)
{
}

/*! \overload
  Constructs a range with the specified \a lower and \a upper values.
*/
QCPRange::QCPRange(double lower, double upper) :
  lower(lower),
  upper(upper)
{
  normalize();
}

/*! 
  Returns the size of the range, i.e. \a upper-\a lower
*/
double QCPRange::size() const
{
  return upper-lower;
}

/*! 
  Returns the center of the range, i.e. (\a upper+\a lower)*0.5
*/
double QCPRange::center() const
{
  return (upper+lower)*0.5;
}

/*! 
  Makes sure \a lower is numerically smaller than \a upper. If this is not the case, the values
  are swapped.
*/
void QCPRange::normalize()
{
  if (lower > upper)
    qSwap(lower, upper);
}

/*! 
  Returns a sanitized version of the range. Sanitized means for logarithmic scales, that
  the range won't span the positive and negative sign domain, i.e. contain zero. Further
  \a lower will always be numerically smaller (or equal) to \a upper.
  
  If the original range does span positive and negative sign domains or contains zero,
  the returned range will try to approximate the original range as good as possible.
  If the positive interval of the original range is wider than the negative interval, the
  returned range will only contain the positive interval, with lower bound set to \a rangeFac or
  \a rangeFac *\a upper, whichever is closer to zero. Same procedure is used if the negative interval
  is wider than the positive interval, this time by changing the \a upper bound.
*/
QCPRange QCPRange::sanitizedForLogScale() const
{
  double rangeFac = 1e-3;
  QCPRange sanitizedRange(lower, upper);
  sanitizedRange.normalize();
  // can't have range spanning negative and positive values in log plot, so change range to fix it
  //if (qFuzzyCompare(sanitizedRange.lower+1, 1) && !qFuzzyCompare(sanitizedRange.upper+1, 1))
  if (sanitizedRange.lower == 0.0 && sanitizedRange.upper != 0.0)
  {
    // case lower is 0
    if (rangeFac < sanitizedRange.upper*rangeFac)
      sanitizedRange.lower = rangeFac;
    else
      sanitizedRange.lower = sanitizedRange.upper*rangeFac;
  } //else if (!qFuzzyCompare(lower+1, 1) && qFuzzyCompare(upper+1, 1))
  else if (sanitizedRange.lower != 0.0 && sanitizedRange.upper == 0.0)
  {
    // case upper is 0
    if (-rangeFac > sanitizedRange.lower*rangeFac)
      sanitizedRange.upper = -rangeFac;
    else
      sanitizedRange.upper = sanitizedRange.lower*rangeFac;
  } else if (sanitizedRange.lower < 0 && sanitizedRange.upper > 0)
  {
    // find out whether negative or positive interval is wider to decide which sign domain will be chosen
    if (-sanitizedRange.lower > sanitizedRange.upper)
    {
      // negative is wider, do same as in case upper is 0
      if (-rangeFac > sanitizedRange.lower*rangeFac)
        sanitizedRange.upper = -rangeFac;
      else
        sanitizedRange.upper = sanitizedRange.lower*rangeFac;
    } else
    {
      // positive is wider, do same as in case lower is 0
      if (rangeFac < sanitizedRange.upper*rangeFac)
        sanitizedRange.lower = rangeFac;
      else
        sanitizedRange.lower = sanitizedRange.upper*rangeFac;
    }
  }
  // due to normalization, case lower>0 && upper<0 should never occur, because that implies upper<lower
  return sanitizedRange;
}

/*! 
  Returns a sanitized version of the range. Sanitized means for linear scales, that
  \a lower will always be numerically smaller (or equal) to \a upper.
*/
QCPRange QCPRange::sanitizedForLinScale() const
{
  QCPRange sanitizedRange(lower, upper);
  sanitizedRange.normalize();
  return sanitizedRange;
}

/*! 
  Returns true when \a value lies within or exactly on the borders of the range.
*/
bool QCPRange::contains(double value) const
{
  return value >= lower && value <= upper;
}

/*! 
  Checks, whether the specified range is within valid bounds, which are defined
  as QCPRange::maxRange and QCPRange::minRange.
  A valid range means:
  \li range bounds within -maxRange and maxRange
  \li range size above minRange
  \li range size below maxRange
*/
bool QCPRange::validRange(double lower, double upper)
{
  /*
  return (lower > -maxRange &&
          upper < maxRange &&
          qAbs(lower-upper) > minRange &&
          (lower < -minRange || lower > minRange) &&
          (upper < -minRange || upper > minRange));
          */
  return (lower > -maxRange &&
          upper < maxRange &&
          qAbs(lower-upper) > minRange &&
          qAbs(lower-upper) < maxRange);
}

/*! 
  \overload
  Checks, whether the specified range is within valid bounds, which are defined
  as QCPRange::maxRange and QCPRange::minRange.
  A valid range means:
  \li range bounds within -maxRange and maxRange
  \li range size above minRange
  \li range size below maxRange
*/
bool QCPRange::validRange(const QCPRange &range)
{
  /*
  return (range.lower > -maxRange &&
          range.upper < maxRange &&
          qAbs(range.lower-range.upper) > minRange &&
          qAbs(range.lower-range.upper) < maxRange &&
          (range.lower < -minRange || range.lower > minRange) &&
          (range.upper < -minRange || range.upper > minRange));
          */
  return (range.lower > -maxRange &&
          range.upper < maxRange &&
          qAbs(range.lower-range.upper) > minRange &&
          qAbs(range.lower-range.upper) < maxRange);
}


// ================================================================================
// =================== QCPLegend
// ================================================================================

/*! \class QCPLegend
  \brief Manages a legend inside a QCustomPlot.

  Doesn't need to be instantiated externally, rather access QCustomPlot::legend
*/

/* start of documentation of signals */

/*! \fn void QCPLegend::selectionChanged(QCPLegend::SelectableParts selection);

  This signal is emitted when the selection state of this legend has changed.
  
  \see setSelected, setSelectable
*/

/* end of documentation of signals */

/*!
  Constructs a new QCPLegend instance with \a parentPlot as the containing plot and default
  values. Under normal usage, QCPLegend needn't be instantiated outside of QCustomPlot.
  Access QCustomPlot::legend to modify the legend (set to invisible by default, see \ref
  setVisible).
*/
QCPLegend::QCPLegend(QCustomPlot *parentPlot) :
  QCPLayerable(parentPlot)
{
  setAntialiased(false);
  setPositionStyle(psTopRight);
  setSize(100, 28);
  setMinimumSize(100, 0);
  setIconSize(32, 18);
  setAutoSize(true);
  
  setMargin(12, 12, 12, 12);
  setPadding(8, 8, 3, 3);
  setItemSpacing(3);
  setIconTextPadding(7);
  
  setSelectable(spLegendBox | spItems);
  setSelected(spNone);
  
  setBorderPen(QPen(Qt::black));
  setSelectedBorderPen(QPen(Qt::blue, 2));
  setIconBorderPen(Qt::NoPen);
  setSelectedIconBorderPen(QPen(Qt::blue, 2));
  setBrush(Qt::white);
  setSelectedBrush(Qt::white);
  setFont(parentPlot->font());
  setSelectedFont(parentPlot->font());
  setTextColor(Qt::black);
  setSelectedTextColor(Qt::blue);
}

QCPLegend::~QCPLegend()
{
  clearItems();
}

/*!
  Sets the pen, the border of the entire legend is drawn with.
*/
void QCPLegend::setBorderPen(const QPen &pen)
{
  mBorderPen = pen;
}

/*!
  Sets the brush of the legend background.
*/
void QCPLegend::setBrush(const QBrush &brush)
{
  mBrush = brush;
}

/*!
  Sets the default font of legend text. Legend items that draw text (e.g. the name of a graph) will
  use this font by default. However, a different font can be specified on a per-item-basis by
  accessing the specific legend item.
  
  This function will also set \a font on all already existing legend items.
  
  \see QCPAbstractLegendItem::setFont
*/
void QCPLegend::setFont(const QFont &font)
{
  mFont = font;
  for (int i=0; i<mItems.size(); ++i)
    mItems.at(i)->setFont(mFont);
}

/*!
  Sets the default color of legend text. Legend items that draw text (e.g. the name of a graph)
  will use this color by default. However, a different colors can be specified on a per-item-basis
  by accessing the specific legend item.
  
  This function will also set \a color on all already existing legend items.
  
  \see QCPAbstractLegendItem::setTextColor
*/
void QCPLegend::setTextColor(const QColor &color)
{
  mTextColor = color;
  for (int i=0; i<mItems.size(); ++i)
    mItems.at(i)->setTextColor(color);
}

/*!
  Sets the position style of the legend. If the \a legendPositionStyle is not \ref psManual, the
  position is found automatically depending on the specific \a legendPositionStyle and the
  legend margins. If \a legendPositionStyle is \ref psManual, the exact pixel position of the
  legend must be specified via \ref setPosition. Margins have no effect in that case.
  \see setMargin
*/
void QCPLegend::setPositionStyle(PositionStyle legendPositionStyle)
{
  mPositionStyle = legendPositionStyle;
}

/*!
  Sets the exact pixel Position of the legend inside the QCustomPlot widget, if \ref
  setPositionStyle is set to \ref psManual. Margins have no effect in that case.
*/
void QCPLegend::setPosition(const QPoint &pixelPosition)
{
  mPosition = pixelPosition;
}

/*!
  Sets whether the size of the legend should be calculated automatically to fit all the content
  (plus padding), or whether the size must be specified manually with \ref setSize.
  
  If the autoSize mechanism is enabled, the legend will have the smallest possible size to still
  display all its content. For items with text wrapping (QCPPlottableLegendItem::setTextWrap) this
  means, they would become very compressed, i.e. wrapped at every word. To prevent this, set a
  reasonable \ref setMinimumSize width.
*/
void QCPLegend::setAutoSize(bool on)
{
  mAutoSize = on;
}

/*!
  Sets the size of the legend. Setting the size manually with this function only has an effect, if
  \ref setAutoSize is set to false.
  
  If you want to control the minimum size (or the text-wrapping width) while still leaving the
  autoSize mechanism enabled, consider using \ref setMinimumSize.
  
  \see setAutoSize, setMinimumSize
*/
void QCPLegend::setSize(const QSize &size)
{
  mSize = size;
}

/*! \overload
*/
void QCPLegend::setSize(int width, int height)
{
  mSize = QSize(width, height);
}

/*!
  Sets the minimum size of the legend when \ref setAutoSize is enabled.
  
  If text wrapping is enabled in the legend items (e.g. \ref QCPPlottableLegendItem::setTextWrap), this minimum \a size defines the width
  at which the wrapping will occur. Note that the wrapping will happen only at word boundaries, so the actual size might
  still be bigger than the \a size given here, but not smaller.
  
  If \ref setAutoSize is not enabled, the minimum \a size is ignored. Setting a smaller legend size with \ref setSize manually, is not prevented.
  
  \see setAutoSize, setSize, QCPPlottableLegendItem::setTextWrap
*/
void QCPLegend::setMinimumSize(const QSize &size)
{
  mMinimumSize = size;
}

/*! \overload
*/
void QCPLegend::setMinimumSize(int width, int height)
{
  mMinimumSize = QSize(width, height);
}

/*!
  Sets the left padding of the legend. Padding is the space by what the legend box is made larger
  than minimally needed for the content to fit. I.e. it's the space left blank on each side inside
  the legend.
*/
void QCPLegend::setPaddingLeft(int padding)
{
  mPaddingLeft = padding;
}

/*!
  Sets the right padding of the legend. Padding is the space by what the legend box is made larger
  than minimally needed for the content to fit. I.e. it's the space left blank on each side inside
  the legend.
*/
void QCPLegend::setPaddingRight(int padding)
{
  mPaddingRight = padding;
}

/*!
  Sets the top padding of the legend. Padding is the space by what the legend box is made larger
  than minimally needed for the content to fit. I.e. it's the space left blank on each side inside
  the legend.
*/
void QCPLegend::setPaddingTop(int padding)
{
  mPaddingTop = padding;
}

/*!
  Sets the bottom padding of the legend. Padding is the space by what the legend box is made larger
  than minimally needed for the content to fit. I.e. it's the space left blank on each side inside
  the legend.
*/
void QCPLegend::setPaddingBottom(int padding)
{
  mPaddingBottom = padding;
}

/*!
  Sets the padding of the legend. Padding is the space by what the legend box is made larger than
  minimally needed for the content to fit. I.e. it's the space left blank on each side inside the
  legend.
*/
void QCPLegend::setPadding(int left, int right, int top, int bottom)
{
  mPaddingLeft = left;
  mPaddingRight = right;
  mPaddingTop = top;
  mPaddingBottom = bottom;
}

/*!
  Sets the left margin of the legend. Margins are the distances the legend will keep to the axis
  rect, when \ref setPositionStyle is not \ref psManual.
*/
void QCPLegend::setMarginLeft(int margin)
{
  mMarginLeft = margin;
}

/*!
  Sets the right margin of the legend. Margins are the distances the legend will keep to the axis
  rect, when \ref setPositionStyle is not \ref psManual.
*/
void QCPLegend::setMarginRight(int margin)
{
  mMarginRight = margin;
}

/*!
  Sets the top margin of the legend. Margins are the distances the legend will keep to the axis
  rect, when \ref setPositionStyle is not \ref psManual.
*/
void QCPLegend::setMarginTop(int margin)
{
  mMarginTop = margin;
}

/*!
  Sets the bottom margin of the legend. Margins are the distances the legend will keep to the axis
  rect, when \ref setPositionStyle is not \ref psManual.
*/
void QCPLegend::setMarginBottom(int margin)
{
  mMarginBottom = margin;
}

/*!
  Sets the margin of the legend. Margins are the distances the legend will keep to the axis rect,
  when \ref setPositionStyle is not \ref psManual.
*/
void QCPLegend::setMargin(int left, int right, int top, int bottom)
{
  mMarginLeft = left;
  mMarginRight = right;
  mMarginTop = top;
  mMarginBottom = bottom;
}

/*!
  Sets the vertical space between two legend items in the legend.
  
  \see setIconTextPadding, setPadding
*/
void QCPLegend::setItemSpacing(int spacing)
{
  mItemSpacing = spacing;
}

/*!
  Sets the size of legend icons. Legend items that draw an icon (e.g. a visual
  representation of the graph) will use this size by default.
*/
void QCPLegend::setIconSize(const QSize &size)
{
  mIconSize = size;
}

/*! \overload
*/
void QCPLegend::setIconSize(int width, int height)
{
  mIconSize.setWidth(width);
  mIconSize.setHeight(height);
}

/*!
  Sets the horizontal space in pixels between the legend icon and the text next to it.
  Legend items that draw an icon (e.g. a visual representation of the graph) and text (e.g. the
  name of the graph) will use this space by default.
  
  \see setItemSpacing
*/
void QCPLegend::setIconTextPadding(int padding)
{
  mIconTextPadding = padding;
}

/*!
  Sets the pen used to draw a border around each legend icon. Legend items that draw an
  icon (e.g. a visual representation of the graph) will use this pen by default.
  
  If no border is wanted, set this to \a Qt::NoPen.
*/
void QCPLegend::setIconBorderPen(const QPen &pen)
{
  mIconBorderPen = pen;
}

/*!
  Sets whether the user can (de-)select the parts in \a selectable by clicking on the QCustomPlot surface.
  (When \ref QCustomPlot::setInteractions contains iSelectLegend.)
  
  However, even when \a selectable is set to a value not allowing the selection of a specific part,
  it is still possible to set the selection of this part manually, by calling \ref setSelected
  directly.
  
  \see SelectablePart, setSelected
*/
void QCPLegend::setSelectable(const SelectableParts &selectable)
{
  mSelectable = selectable;
}

/*!
  Sets the selected state of the respective legend parts described by \ref SelectablePart. When a part
  is selected, it uses a different pen/font and brush. If some legend items are selected and \a selected
  doesn't contain \ref spItems, those items become deselected.
  
  The entire selection mechanism is handled automatically when \ref QCustomPlot::setInteractions
  contains iSelectLegend. You only need to call this function when you wish to change the selection
  state manually.
  
  This function can change the selection state of a part even when \ref setSelectable was set to a
  value that actually excludes the part.
  
  emits the \ref selectionChanged signal when \a selected is different from the previous selection state.
  
  Note that it doesn't make sense to set the selected state \ref spItems here when it wasn't set
  before, because there's no way to specify which exact items to newly select. Do this by calling
  \ref QCPAbstractLegendItem::setSelected directly on the legend item you wish to select.
  
  \see SelectablePart, setSelectable, selectTest, setSelectedBorderPen, setSelectedIconBorderPen, setSelectedBrush,
  setSelectedFont
*/
void QCPLegend::setSelected(const SelectableParts &selected)
{
  if (mSelected != selected)
  {
    if (!selected.testFlag(spItems) && mSelected.testFlag(spItems)) // some items are selected, but new selection state doesn't contain spItems, so deselect them
    {
      for (int i=0; i<mItems.size(); ++i)
        mItems.at(i)->setSelected(false);
      mSelected = selected;
      // not necessary to emit selectionChanged here because this will have happened for the last setSelected(false) on mItems already, via updateSelectionState()
    } else
    {
      mSelected = selected;
      emit selectionChanged(mSelected);
    }
  }
}

/*!
  When the legend box is selected, this pen is used to draw the border instead of the normal pen
  set via \ref setBorderPen.

  \see setSelected, setSelectable, setSelectedBrush
*/
void QCPLegend::setSelectedBorderPen(const QPen &pen)
{
  mSelectedBorderPen = pen;
}

/*!
  Sets the pen legend items will use to draw their icon borders, when they are selected.

  \see setSelected, setSelectable, setSelectedFont
*/
void QCPLegend::setSelectedIconBorderPen(const QPen &pen)
{
  mSelectedIconBorderPen = pen;
}

/*!
  When the legend box is selected, this brush is used to draw the legend background instead of the normal brush
  set via \ref setBrush.

  \see setSelected, setSelectable, setSelectedBorderPen
*/
void QCPLegend::setSelectedBrush(const QBrush &brush)
{
  mSelectedBrush = brush;
}

/*!
  Sets the default font that is used by legend items when they are selected.
  
  This function will also set \a font on all already existing legend items.

  \see setFont, QCPAbstractLegendItem::setSelectedFont
*/
void QCPLegend::setSelectedFont(const QFont &font)
{
  mSelectedFont = font;
  for (int i=0; i<mItems.size(); ++i)
    mItems.at(i)->setSelectedFont(font);
}

/*!
  Sets the default text color that is used by legend items when they are selected.
  
  This function will also set \a color on all already existing legend items.

  \see setTextColor, QCPAbstractLegendItem::setSelectedTextColor
*/
void QCPLegend::setSelectedTextColor(const QColor &color)
{
  mSelectedTextColor = color;
  for (int i=0; i<mItems.size(); ++i)
    mItems.at(i)->setSelectedTextColor(color);
}

/*!
  Returns the item with index \a i.
  
  \see itemCount
*/
QCPAbstractLegendItem *QCPLegend::item(int index) const
{
  if (index >= 0 && index < mItems.size())
    return mItems[index];
  else
    return 0;
}

/*!
  Returns the QCPPlottableLegendItem which is associated with \a plottable (e.g. a \ref QCPGraph*).
  If such an item isn't in the legend, returns 0.
  
  \see hasItemWithPlottable
*/
QCPPlottableLegendItem *QCPLegend::itemWithPlottable(const QCPAbstractPlottable *plottable) const
{
  for (int i=0; i<mItems.size(); ++i)
  {
    if (QCPPlottableLegendItem *pli = qobject_cast<QCPPlottableLegendItem*>(mItems.at(i)))
    {
      if (pli->plottable() == plottable)
        return pli;
    }
  }
  return 0;
}

/*!
  Returns the number of items currently in the legend.
  \see item
*/
int QCPLegend::itemCount() const
{
  return mItems.size();
}

/*!
  Returns whether the legend contains \a item.
*/
bool QCPLegend::hasItem(QCPAbstractLegendItem *item) const
{
  return mItems.contains(item);
}

/*!
  Returns whether the legend contains a QCPPlottableLegendItem which is associated with \a plottable (e.g. a \ref QCPGraph*).
  If such an item isn't in the legend, returns false.
  
  \see itemWithPlottable
*/
bool QCPLegend::hasItemWithPlottable(const QCPAbstractPlottable *plottable) const
{
  return itemWithPlottable(plottable);
}

/*!
  Adds \a item to the legend, if it's not present already.
  
  Returns true on sucess, i.e. if the item wasn't in the list already and has been successfuly added.
  
  The legend takes ownership of the item.
*/
bool QCPLegend::addItem(QCPAbstractLegendItem *item)
{
  if (!mItems.contains(item))
  {
    mItems.append(item);
    return true;
  } else
    return false;
}

/*!
  Removes the item with index \a index from the legend.

  Returns true, if successful.
  
  \see itemCount, clearItems
*/
bool QCPLegend::removeItem(int index)
{
  if (index >= 0 && index < mItems.size())
  {
    mItemBoundingBoxes.remove(mItems.at(index));
    delete mItems.at(index);
    mItems.removeAt(index);
    return true;
  } else
    return false;
}

/*! \overload
  
  Removes \a item from the legend.

  Returns true, if successful.
  
  \see clearItems
*/
bool QCPLegend::removeItem(QCPAbstractLegendItem *item)
{
  return removeItem(mItems.indexOf(item));
}

/*!
  Removes all items from the legend.
*/
void QCPLegend::clearItems()
{
  qDeleteAll(mItems);
  mItems.clear();
  mItemBoundingBoxes.clear();
}


/*!
  Returns the legend items that are currently selected. If no items are selected,
  the list is empty.
  
  \see QCPAbstractLegendItem::setSelected, setSelectable
*/
QList<QCPAbstractLegendItem *> QCPLegend::selectedItems() const
{
  QList<QCPAbstractLegendItem*> result;
  for (int i=0; i<mItems.size(); ++i)
  {
    if (mItems.at(i)->selected())
      result.append(mItems.at(i));
  }
  return result;
}

/*!
  If \ref setAutoSize is true, the size needed to fit all legend contents is calculated and applied.
  Finally, the automatic positioning of the legend is performed, depending on the \ref
  setPositionStyle setting.
*/
void  QCPLegend::reArrange()
{
  if (mAutoSize)
  {
    calculateAutoSize();
  }
  calculateAutoPosition();
}

/*!
  Returns whether the point \a pos in pixels hits the legend rect.
  
  \see selectTestItem
*/
bool QCPLegend::selectTestLegend(const QPointF &pos) const
{
  return QRect(mPosition, mSize).contains(pos.toPoint());
}

/*!
  When the point \a pos in pixels hits a legend item, the item is returned. If no item is hit, 0 is
  returned.
  
  \see selectTestLegend
*/
QCPAbstractLegendItem *QCPLegend::selectTestItem(const QPoint pos) const
{
  QMap<QCPAbstractLegendItem*, QRect>::const_iterator it;
  for (it = mItemBoundingBoxes.constBegin(); it != mItemBoundingBoxes.constEnd(); ++it)
  {
    if (it.value().contains(pos) && mItems.contains(it.key()))
      return it.key();
  }
  return 0;
}

/*! \internal
  
  Updates the spItems part of the selection state of this legend by going through all child items
  and checking their selected state.
  
  If no items are selected and the current selected state contains spItems, it is removed and the
  \ref selectionChanged signal is emitted. If at least one item is selected and the current selection
  state does not contain spItems, it is added and the signal is emitted, too.
  
  This function is called in the QCPAbstractLegendItem::setSelected functions to propagate their
  change to the parent legend.
*/
void QCPLegend::updateSelectionState()
{
  bool hasSelections = false;
  for (int i=0; i<mItems.size(); ++i)
  {
    if (mItems.at(i)->selected())
    {
      hasSelections = true;
      break;
    }
  }
  
  // in the following we don't use setSelected because it would cause unnecessary
  // logic looping through items if spItems isn't set in the new state. (look at setSelected and you'll understand)
  if (hasSelections && !mSelected.testFlag(spItems))
  {
    mSelected |= spItems;
    emit selectionChanged(mSelected);
  } else if (!hasSelections && mSelected.testFlag(spItems))
  {
    mSelected &= ~spItems;
    emit selectionChanged(mSelected);
  }
}

/*! \internal
  
  Handles the selection \a event and returns true when the selection event hit any parts of the
  legend. If the selection state of any parts of the legend was changed, the output parameter \a
  modified is set to true.
  
  When \a additiveSelecton is true, any new selections become selected in addition to the recent
  selections. The recent selections are not cleared. Further, clicking on one object multiple times
  in additive selection mode, toggles the selection of that object on and off.
  
  To indicate that an event deselects the legend (i.e. the parts that are deselectable by the user,
  see \ref setSelectable), pass 0 as \a event.
*/
bool QCPLegend::handleLegendSelection(QMouseEvent *event, bool additiveSelection, bool &modified)
{
  modified = false;
  bool selectionFound = false;
  
  if (event && selectTestLegend(event->pos())) // clicked inside legend somewhere
  {
    QCPAbstractLegendItem *ali = selectTestItem(event->pos());
    if (selectable().testFlag(QCPLegend::spItems) && ali && ali->selectable()) // items shall be selectable and item ali was clicked 
    {
      selectionFound = true;
      // deselect legend box:
      if (!additiveSelection && selected().testFlag(QCPLegend::spLegendBox) && selectable().testFlag(QCPLegend::spLegendBox))
        setSelected(selected() & ~QCPLegend::spLegendBox);
      // first select clicked item:
      if (!ali->selected() || additiveSelection) // if additive selection, we toggle selection on and off per click
      {
        modified = true;
        ali->setSelected(!ali->selected());
      }
      // finally, deselect all other items (if we had deselected all first, the selectionChanged signal of QCPLegend might have been emitted twice):
      if (!additiveSelection)
      {
        for (int i=0; i<itemCount(); ++i)
        {
          if (item(i) != ali && item(i)->selected() && item(i)->selectable())
          {
            modified = true;
            item(i)->setSelected(false);
          }
        }
      }
    } else // no specific item clicked or items not selectable
    {
      // if items actually were selectable, this means none were clicked, deselect them:
      if (selectable().testFlag(QCPLegend::spItems) && selected().testFlag(QCPLegend::spItems) && !additiveSelection)
      {
        for (int i=0; i<itemCount(); ++i)
        {
          if (item(i)->selectable())
            item(i)->setSelected(false);
        }
        modified = true;
      }
      // if legend box is selectable, select it:
      if (selectable().testFlag(QCPLegend::spLegendBox))
      {
        if (!selected().testFlag(QCPLegend::spLegendBox) || additiveSelection)
        {
          selectionFound = true;
          setSelected(selected() ^ QCPLegend::spLegendBox); // xor because we always toggle
          modified = true;
        }
      }
    }
  } else if (selected() != QCPLegend::spNone && selectable() != QCPLegend::spNone && !additiveSelection) // legend not clicked, deselect it if selectable allows that (and all child items)
  {
    // only deselect parts that are allowed to be changed by user according to selectable()
    // deselect child items (and automatically removes spItems from selected state of legend, if last item gets deselected):
    if (selectable().testFlag(spItems)) 
    {
      for (int i=0; i<itemCount(); ++i)
      {
        if (item(i)->selected() && item(i)->selectable())
        {
          item(i)->setSelected(false);
          modified = true;
        }
      }
    }
    // only deselect parts that are allowed to be changed (are selectable). Don't forcibly remove
    // spItems, because some selected items might not be selectable, i.e. allowed to be deselected
    // by user interaction. If that's not the case, spItems will have been removed from selected()
    // state in previous loop by individual setSelected(false) calls on the items anyway.
    QCPLegend::SelectableParts newState = selected() & ~(selectable()&~spItems);
    if (newState != selected())
    {
      setSelected(newState);
      modified = true;
    }
  }
  
  return selectionFound;
}

/*! \internal

  A convenience function to easily set the QPainter::Antialiased hint on the provided \a painter
  before drawing main legend elements.

  This is the antialiasing state the painter passed to the \ref draw method is in by default.
  
  This function takes into account the local setting of the antialiasing flag as well as
  the overrides set e.g. with \ref QCustomPlot::setNotAntialiasedElements.
  
  \see setAntialiased
*/
void QCPLegend::applyDefaultAntialiasingHint(QCPPainter *painter) const
{
  applyAntialiasingHint(painter, mAntialiased, QCP::aeLegend);
}

/*! \internal
  
  Returns the pen used to paint the border of the legend, taking into account the selection state
  of the legend box.
*/
QPen QCPLegend::getBorderPen() const
{
  return mSelected.testFlag(spLegendBox) ? mSelectedBorderPen : mBorderPen;
}

/*! \internal
  
  Returns the brush used to paint the background of the legend, taking into account the selection
  state of the legend box.
*/
QBrush QCPLegend::getBrush() const
{
  return mSelected.testFlag(spLegendBox) ? mSelectedBrush : mBrush;
}

/*! \internal
  
  Draws the legend with the provided \a painter.
*/
void QCPLegend::draw(QCPPainter *painter)
{
  painter->setBrush(getBrush());
  painter->setPen(getBorderPen());
  // draw background rect:
  painter->drawRect(QRect(mPosition, mSize));
  // draw legend items:
  painter->setClipRect(QRect(mPosition, mSize).adjusted(1, 1, 0, 0));
  painter->setPen(QPen());
  painter->setBrush(Qt::NoBrush);
  int currentTop = mPosition.y()+mPaddingTop;
  for (int i=0; i<mItems.size(); ++i)
  {
    QSize itemSize = mItems.at(i)->size(QSize(mSize.width(), 0));
    QRect itemRect = QRect(QPoint(mPosition.x()+mPaddingLeft, currentTop), itemSize);
    mItemBoundingBoxes.insert(mItems.at(i), itemRect);
    painter->save();
    mItems.at(i)->applyAntialiasingHint(painter);
    mItems.at(i)->draw(painter, itemRect);
    painter->restore();
    currentTop += itemSize.height()+mItemSpacing;
  }
}

/*! \internal 
  
  Goes through similar steps as \ref draw and calculates the width and height needed to
  fit all items and padding in the legend. The new calculated size is then applied to the mSize of
  this legend.
*/
void QCPLegend::calculateAutoSize()
{
  int width = mMinimumSize.width()-mPaddingLeft-mPaddingRight; // start with minimum width and only expand from there
  int currentTop;
  bool repeat = true;
  int repeatCount = 0;
  while (repeat && repeatCount < 3) // repeat until we find self-consistent width (usually 2 runs)
  {
    repeat = false;
    currentTop = mPaddingTop;
    for (int i=0; i<mItems.size(); ++i)
    {
      QSize s = mItems.at(i)->size(QSize(width, 0));
      currentTop += s.height();
      if (i < mItems.size()-1) // vertical spacer for all but last item
        currentTop += mItemSpacing;
      if (width < s.width())
      {
        width = s.width();
        repeat = true; // changed width, so need a new run with new width to let other items adapt their height to that new width
      }
    }
    repeatCount++;
  }
  if (repeat)
    qDebug() << Q_FUNC_INFO << "hit repeat limit for iterative width calculation";
  currentTop += mPaddingBottom;
  width += mPaddingLeft+mPaddingRight;
  
  mSize.setWidth(width);
  if (currentTop > mMinimumSize.height())
    mSize.setHeight(currentTop);
  else
    mSize.setHeight(mMinimumSize.height());
}

/*! \internal
  
  Sets the position dependant on the \ref setPositionStyle setting and the margins.
*/
void QCPLegend::calculateAutoPosition()
{
  switch (mPositionStyle)
  {
    case psTopLeft:
      mPosition = mParentPlot->mAxisRect.topLeft() + QPoint(mMarginLeft, mMarginTop); break;
    case psTop:
      mPosition = mParentPlot->mAxisRect.topLeft() + QPoint(mParentPlot->mAxisRect.width()/2.0-mSize.width()/2.0, mMarginTop); break;
    case psTopRight:
      mPosition = mParentPlot->mAxisRect.topRight() + QPoint(-mMarginRight-mSize.width(), mMarginTop); break;
    case psRight:
      mPosition = mParentPlot->mAxisRect.topRight() + QPoint(-mMarginRight-mSize.width(), mParentPlot->mAxisRect.height()/2.0-mSize.height()/2.0); break;
    case psBottomRight:
      mPosition = mParentPlot->mAxisRect.bottomRight() + QPoint(-mMarginRight-mSize.width(), -mMarginBottom-mSize.height()); break;
    case psBottom:
      mPosition = mParentPlot->mAxisRect.bottomLeft() + QPoint(mParentPlot->mAxisRect.width()/2.0-mSize.width()/2.0, -mMarginBottom-mSize.height()); break;
    case psBottomLeft:
      mPosition = mParentPlot->mAxisRect.bottomLeft() + QPoint(mMarginLeft, -mMarginBottom-mSize.height()); break;
    case psLeft:
      mPosition = mParentPlot->mAxisRect.topLeft() + QPoint(mMarginLeft, mParentPlot->mAxisRect.height()/2.0-mSize.height()/2.0); break;
    case psManual: break;
  }
}


// ================================================================================
// =================== QCPAxis
// ================================================================================

/*! \class QCPAxis
  \brief Manages a single axis inside a QCustomPlot.

  Usually doesn't need to be instantiated externally. Access %QCustomPlot's axes via
  QCustomPlot::xAxis (bottom), QCustomPlot::yAxis (left), QCustomPlot::xAxis2 (top) and
  QCustomPlot::yAxis2 (right).
*/

/* start of documentation of inline functions */

/*! \fn Qt::Orientation QCPAxis::orientation() const
  
  Returns the orientation of the axis. The axis orientation (horizontal or vertical) is deduced
  from the axis type (left, top, right or bottom).
*/

/* end of documentation of inline functions */
/* start of documentation of signals */

/*! \fn void QCPAxis::ticksRequest()
  
  This signal is emitted when \ref setAutoTicks is false and the axis is about to generate tick
  labels and replot itself.
  
  Modifying the tick positions can be done with \ref setTickVector. If you also want to control the
  tick labels, set \ref setAutoTickLabels to false and also provide the labels with \ref
  setTickVectorLabels.
  
  If you only want static ticks you probably don't need this signal, since you can just set the
  tick vector (and possibly tick label vector) once. However, if you want to provide ticks (and
  maybe labels) dynamically, e.g. depending on the current axis range, connect a slot to this
  signal and set the vector/vectors there.
*/

/*! \fn void QCPAxis::rangeChanged(const QCPRange &newRange)

  This signal is emitted when the range of this axis has changed. You can connect it to the \ref
  setRange slot of another axis to communicate the new range to the other axis, in order for it to
  be synchronized.
*/

/*! \fn void QCPAxis::selectionChanged(QCPAxis::SelectableParts selection)
  
  This signal is emitted when the selection state of this axis has changed, either by user interaction
  or by a direct call to \ref setSelected.
*/

/* end of documentation of signals */

/*!
  Constructs an Axis instance of Type \a type inside \a parentPlot.
*/
QCPAxis::QCPAxis(QCustomPlot *parentPlot, AxisType type) :
  QCPLayerable(parentPlot)
{
  mLowestVisibleTick = 0;
  mHighestVisibleTick = -1;
  mGrid = new QCPGrid(this);
  setAxisType(type);
  setAxisRect(parentPlot->axisRect()); 
  setScaleType(stLinear);
  setScaleLogBase(10);
  
  setAntialiased(false);
  setRange(0, 5);
  setRangeReversed(false);
  
  setTicks(true);
  setTickStep(1);
  setAutoTickCount(6);
  setAutoTicks(true);
  setAutoTickLabels(true);
  setAutoTickStep(true);
  setTickLabelFont(parentPlot->font());
  setTickLabelColor(Qt::black);
  setTickLength(5);
  setTickPen(QPen(Qt::black, 0, Qt::SolidLine, Qt::SquareCap));
  setTickLabels(true);
  setTickLabelType(ltNumber);
  setTickLabelRotation(0);
  setDateTimeFormat("hh:mm:ss\ndd.MM.yy");
  setNumberFormat("gbd");
  setNumberPrecision(6);
  setLabel("");
  setLabelFont(parentPlot->font());
  setLabelColor(Qt::black);
  
  setAutoSubTicks(true);
  setSubTickCount(4);
  setSubTickLength(2);
  setSubTickPen(QPen(Qt::black, 0, Qt::SolidLine, Qt::SquareCap));
  setBasePen(QPen(Qt::black, 0, Qt::SolidLine, Qt::SquareCap));
  
  setSelected(spNone);
  setSelectable(spAxis | spTickLabels | spAxisLabel);
  QFont selTickLabelFont = tickLabelFont();
  selTickLabelFont.setBold(true);
  setSelectedTickLabelFont(selTickLabelFont);
  QFont selLabelFont = labelFont();
  selLabelFont.setBold(true);
  setSelectedLabelFont(selLabelFont);
  setSelectedTickLabelColor(Qt::blue);
  setSelectedLabelColor(Qt::blue);
  QPen blueThickPen(Qt::blue, 2);
  setSelectedBasePen(blueThickPen);
  setSelectedTickPen(blueThickPen);
  setSelectedSubTickPen(blueThickPen);
  
  setPadding(0);
  if (type == atTop)
  {
    setTickLabelPadding(3);
    setLabelPadding(6);
  } else if (type == atRight)
  {
    setTickLabelPadding(7);
    setLabelPadding(12);
  } else if (type == atBottom)
  {
    setTickLabelPadding(3);
    setLabelPadding(3);
  } else if (type == atLeft)
  {
    setTickLabelPadding(5);
    setLabelPadding(10);
  }
}

QCPAxis::~QCPAxis()
{
  delete mGrid;
}

/* No documentation as it is a property getter */
QString QCPAxis::numberFormat() const
{
  QString result;
  result.append(mNumberFormatChar);
  if (mNumberBeautifulPowers)
  {
    result.append("b");
    if (mNumberMultiplyCross)
      result.append("c");
  }
  return result;
}

/*! \internal
  
  Sets the axis type. This determines the \ref orientation and together with the current axis rect
  (see \ref setAxisRect), the position of the axis. Depending on \a type, ticks, tick labels, and
  label are drawn on corresponding sides of the axis base line.
*/
void QCPAxis::setAxisType(AxisType type)
{
  mAxisType = type;
  mOrientation = (type == atBottom || type == atTop) ? Qt::Horizontal : Qt::Vertical;
}

/*! \internal
  
  Sets the axis rect. The axis uses this rect to position itself within the plot,
  together with the information of its type (\ref setAxisType). Theoretically it's possible to give
  a plot's axes different axis rects (e.g. for gaps between them), however, they are currently all
  synchronized by the QCustomPlot::setAxisRect function.
*/
void QCPAxis::setAxisRect(const QRect &rect)
{
  mAxisRect = rect;
}

/*!
  Sets whether the axis uses a linear scale or a logarithmic scale. If \a type is set to \ref
  stLogarithmic, the logarithm base can be set with \ref setScaleLogBase. In logarithmic axis
  scaling, major tick marks appear at all powers of the logarithm base. Properties like tick step
  (\ref setTickStep) don't apply in logarithmic scaling. If you wish a decimal base but less major
  ticks, consider choosing a logarithm base of 100, 1000 or even higher.
  
  If \a type is \ref stLogarithmic and the number format (\ref setNumberFormat) uses the 'b' option
  (beautifully typeset decimal powers), the display usually is "1 [multiplication sign] 10
  [superscript] n", which looks unnatural for logarithmic scaling (the "1 [multiplication sign]"
  part). To only display the decimal power, set the number precision to zero with
  \ref setNumberPrecision.
*/
void QCPAxis::setScaleType(ScaleType type)
{
  mScaleType = type;
  if (mScaleType == stLogarithmic)
    mRange = mRange.sanitizedForLogScale();
}

/*!
  If \ref setScaleType is set to \ref stLogarithmic, \a base will be the logarithm base of the
  scaling. In logarithmic axis scaling, major tick marks appear at all powers of \a base.
  
  Properties like tick step (\ref setTickStep) don't apply in logarithmic scaling. If you wish a decimal base but
  less major ticks, consider choosing \a base 100, 1000 or even higher.
*/
void QCPAxis::setScaleLogBase(double base)
{
  if (base > 1)
  {
    mScaleLogBase = base;
    mScaleLogBaseLogInv = 1.0/qLn(mScaleLogBase); // buffer for faster baseLog() calculation
  } else
    qDebug() << Q_FUNC_INFO << "Invalid logarithmic scale base (must be greater 1):" << base;
}

/*!
  Sets the range of the axis.
  
  This slot may be connected with the \ref rangeChanged signal of another axis so this axis
  is always synchronized with the other axis range, when it changes.
  
  To invert the direction of an axis range, use \ref setRangeReversed.
*/
void QCPAxis::setRange(const QCPRange &range)
{
  if (range.lower == mRange.lower && range.upper == mRange.upper)
    return;
  
  if (!QCPRange::validRange(range)) return;
  if (mScaleType == stLogarithmic)
  {
    mRange = range.sanitizedForLogScale();
  } else
  {
    mRange = range.sanitizedForLinScale();
  }
  emit rangeChanged(mRange);
}

/*!
  Sets whether the user can (de-)select the parts in \a selectable by clicking on the QCustomPlot surface.
  (When \ref QCustomPlot::setInteractions contains iSelectAxes.)
  
  However, even when \a selectable is set to a value not allowing the selection of a specific part,
  it is still possible to set the selection of this part manually, by calling \ref setSelected
  directly.
  
  \see SelectablePart, setSelected
*/
void QCPAxis::setSelectable(const SelectableParts &selectable)
{
  mSelectable = selectable;
}

/*!
  Sets the selected state of the respective axis parts described by \ref SelectablePart. When a part
  is selected, it uses a different pen/font.
  
  The entire selection mechanism for axes is handled automatically when \ref
  QCustomPlot::setInteractions contains iSelectAxes. You only need to call this function when you
  wish to change the selection state manually.
  
  This function can change the selection state of a part even when \ref setSelectable was set to a
  value that actually excludes the part.
  
  emits the \ref selectionChanged signal when \a selected is different from the previous selection state.
  
  \see SelectablePart, setSelectable, selectTest, setSelectedBasePen, setSelectedTickPen, setSelectedSubTickPen,
  setSelectedTickLabelFont, setSelectedLabelFont, setSelectedTickLabelColor, setSelectedLabelColor
*/
void QCPAxis::setSelected(const SelectableParts &selected)
{
  if (mSelected != selected)
  {
    mSelected = selected;
    emit selectionChanged(mSelected);
  }
}

/*!
  \overload
  Sets the lower and upper bound of the axis range.
  
  To invert the direction of an axis range, use \ref setRangeReversed.
  
  There is also a slot to set a range, see \ref setRange(const QCPRange &range).
*/
void QCPAxis::setRange(double lower, double upper)
{
  if (lower == mRange.lower && upper == mRange.upper)
    return;
  
  if (!QCPRange::validRange(lower, upper)) return;
  mRange.lower = lower;
  mRange.upper = upper;
  if (mScaleType == stLogarithmic)
  {
    mRange = mRange.sanitizedForLogScale();
  } else
  {
    mRange = mRange.sanitizedForLinScale();
  }
  emit rangeChanged(mRange);
}

/*!
  \overload
  Sets the range of the axis.

  \param position the \a position coordinate indicates together with the \a alignment parameter, where
  the new range will be positioned.
  \param size defines the size (upper-lower) of the new axis range.
  \param alignment determines how \a position is to be interpreted.\n
  If \a alignment is Qt::AlignLeft, \a position will be the lower bound of the range.\n
  If \a alignment is Qt::AlignRight, \a position will be the upper bound of the range.\n
  If \a alignment is Qt::AlignCenter, the new range will be centered around \a position.\n
  Any other values for \a alignment will default to Qt::AlignCenter.
*/
void QCPAxis::setRange(double position, double size, Qt::AlignmentFlag alignment)
{
  if (alignment == Qt::AlignLeft)
    setRange(position, position+size);
  else if (alignment == Qt::AlignRight)
    setRange(position-size, position);
  else // alignment == Qt::AlignCenter
    setRange(position-size/2.0, position+size/2.0);
}

/*!
  Sets the lower bound of the axis range, independently of the upper bound.
  \see setRange
*/
void QCPAxis::setRangeLower(double lower)
{
  if (mRange.lower == lower)
    return;
  
  mRange.lower = lower;
  if (mScaleType == stLogarithmic)
  {
    mRange = mRange.sanitizedForLogScale();
  } else
  {
    mRange = mRange.sanitizedForLinScale();
  }
  emit rangeChanged(mRange);
}

/*!
  Sets the upper bound of the axis range, independently of the lower bound.
  \see setRange
*/
void QCPAxis::setRangeUpper(double upper)
{
  if (mRange.upper == upper)
    return;
  
  mRange.upper = upper;
  if (mScaleType == stLogarithmic)
  {
    mRange = mRange.sanitizedForLogScale();
  } else
  {
    mRange = mRange.sanitizedForLinScale();
  }
  emit rangeChanged(mRange);
}

/*!
  Sets whether the axis range (direction) is displayed reversed. Normally, the values on horizontal
  axes increase left to right, on vertical axes bottom to top. When \a reversed is set to true, the
  direction of increasing values is inverted. Note that the range and data interface stays the same
  for reversed axes, e.g. the \a lower part of the \ref setRange interface will still reference the
  mathematically smaller number than the \a upper part.
*/
void QCPAxis::setRangeReversed(bool reversed)
{
  mRangeReversed = reversed;
}

/*!
  Sets whether the grid of this axis is drawn antialiased or not.
  
  Note that this setting may be overridden by \ref QCustomPlot::setAntialiasedElements and \ref
  QCustomPlot::setNotAntialiasedElements.
*/
void QCPAxis::setAntialiasedGrid(bool enabled)
{
  mGrid->setAntialiased(enabled);
}

/*!
  Sets whether the sub grid of this axis is drawn antialiased or not.
  
  Note that this setting may be overridden by \ref QCustomPlot::setAntialiasedElements and \ref
  QCustomPlot::setNotAntialiasedElements.
*/
void QCPAxis::setAntialiasedSubGrid(bool enabled)
{
  mGrid->setAntialiasedSubGrid(enabled);
}

/*!
  Sets whether the zero line of this axis is drawn antialiased or not.
  
  Note that this setting may be overridden by \ref QCustomPlot::setAntialiasedElements and \ref
  QCustomPlot::setNotAntialiasedElements.
*/
void QCPAxis::setAntialiasedZeroLine(bool enabled)
{
  mGrid->setAntialiasedZeroLine(enabled);
}

/*!
  Sets whether the grid lines are visible.
  \see setSubGrid, setGridPen, setZeroLinePen
*/
void QCPAxis::setGrid(bool show)
{
  mGrid->setVisible(show);
}

/*!
  Sets whether the sub grid lines are visible.
  \see setGrid, setSubGridPen, setZeroLinePen
*/
void QCPAxis::setSubGrid(bool show)
{
  mGrid->setSubGridVisible(show);
}

/*!
  Sets whether the tick positions should be calculated automatically (either from an automatically
  generated tick step or a tick step provided manually via \ref setTickStep, see \ref setAutoTickStep).
  
  If \a on is set to false, you must provide the tick positions manually via \ref setTickVector.
  For these manual ticks you may let QCPAxis generate the appropriate labels automatically
  by setting/leaving \ref setAutoTickLabels true. If you also wish to control the displayed labels
  manually, set \ref setAutoTickLabels to false and provide the label strings with \ref setTickVectorLabels.
  
  If you need dynamically calculated tick vectors (and possibly tick label vectors), set the
  vectors in a slot connected to the \ref ticksRequest signal.
*/
void QCPAxis::setAutoTicks(bool on)
{
  mAutoTicks = on;
}

/*!
  When \ref setAutoTickStep is true, \a approximateCount determines how many ticks should be generated
  in the visible range approximately.
*/
void QCPAxis::setAutoTickCount(int approximateCount)
{
  mAutoTickCount = approximateCount;
}

/*!
  Sets whether the tick labels are generated automatically depending on the tick label type
  (\ref ltNumber or \ref ltDateTime).
  
  If \a on is set to false, you should provide the tick labels via \ref setTickVectorLabels. This
  is usually used in a combination with \ref setAutoTicks set to false for complete control over
  tick positions and labels, e.g. when the ticks should be at multiples of pi and show "2pi", "3pi"
  etc. as tick labels.
  
  If you need dynamically calculated tick vectors (and possibly tick label vectors), set the
  vectors in a slot connected to the \ref ticksRequest signal.
*/
void QCPAxis::setAutoTickLabels(bool on)
{
  mAutoTickLabels = on;
}

/*!
  Sets whether the tick step, i.e. the interval between two (major) ticks, is calculated
  automatically. If \a on is set to true, the axis finds a tick step that is reasonable for human
  readable plots. This means the tick step mantissa is chosen such that it's either a multiple of
  two or ends in 0.5. The number of ticks the algorithm aims for within the visible range can be
  set with \ref setAutoTickCount. It's not guaranteed that this number of ticks is met exactly, but
  approximately within a tolerance of two or three.
  
  If \a on is set to false, you may set the tick step manually with \ref setTickStep.
*/
void QCPAxis::setAutoTickStep(bool on)
{
  mAutoTickStep = on;
}

/*!
  Sets whether the number of sub ticks in one tick interval is determined automatically.
  This works, as long as the tick step mantissa is a multiple of 0.5 (which it is, when
  \ref setAutoTickStep is enabled).\n
  When \a on is set to false, you may set the sub tick count with \ref setSubTickCount manually.
*/
void QCPAxis::setAutoSubTicks(bool on)
{
  mAutoSubTicks = on;
}

/*!
  Sets whether tick marks are displayed. Setting \a show to false does not imply, that tick labels
  are invisible, too. To achieve that, see \ref setTickLabels.
*/
void QCPAxis::setTicks(bool show)
{
  mTicks = show;
}

/*!
  Sets whether tick labels are displayed.
*/
void QCPAxis::setTickLabels(bool show)
{
  mTickLabels = show;
}

/*!
  Sets the distance between the axis base line (or any tick marks pointing outward) and the tick labels.
  \see setLabelPadding, setPadding
*/
void QCPAxis::setTickLabelPadding(int padding)
{
  mTickLabelPadding = padding;
}

/*!
  Sets whether the tick labels display numbers or dates/times.\n
  If \a type is set to \ref ltNumber, the format specifications of \ref setNumberFormat apply.\n
  If \a type is set to \ref ltDateTime, the format specifications of \ref setDateTimeFormat apply.\n
  In QCustomPlot, date/time coordinates are double numbers representing the seconds since 1970-01-01T00:00:00 UTC.
  This format can be retrieved from QDateTime objects with the QDateTime::toTime_t() function. Since this
  only gives a resolution of one second, there is also the QDateTime::toMSecsSinceEpoch() function which
  returns the timespan described above in milliseconds. Divide its return value by 1000.0 to get a value with
  the format needed for date/time plotting, this time with a resolution of one millisecond.
*/
void QCPAxis::setTickLabelType(LabelType type)
{
  mTickLabelType = type;
}

/*!
  Sets the font of the tick labels, i.e. the numbers drawn next to tick marks.
  
  \see setTickLabelColor
*/
void QCPAxis::setTickLabelFont(const QFont &font)
{
  mTickLabelFont = font;
}

/*!
  Sets the color of the tick labels, i.e. the numbers drawn next to tick marks.
  
  \see setTickLabelFont
*/
void QCPAxis::setTickLabelColor(const QColor &color)
{
  mTickLabelColor = color;
}

/*!
  Sets the rotation of the tick labels, i.e. the numbers drawn next to tick marks. If \a degrees
  is zero, the labels are drawn normally. Else, the tick labels are drawn rotated by \a degrees
  clockwise. The specified angle is bound to values from -90 to 90 degrees.
*/
void QCPAxis::setTickLabelRotation(double degrees)
{
  mTickLabelRotation = qBound(-90.0, degrees, 90.0);
}

/*!
  Sets the format in which dates and times are displayed as tick labels, if \ref setTickLabelType is \ref ltDateTime.
  for details about the \a format string, see the documentation of QDateTime::toString().
  Newlines can be inserted with "\n".
*/
void QCPAxis::setDateTimeFormat(const QString &format)
{
  mDateTimeFormat = format;
}

/*!
  Sets the number format for the numbers drawn as tick labels (if tick label type is \ref
  ltNumber). This \a formatCode is an extended version of the format code used e.g. by
  QString::number() and QLocale::toString(). For reference about that, see the "Argument Formats"
  section in the detailed description of the QString class. \a formatCode is a string of one, two
  or three characters. The first character is identical to the normal format code used by Qt. In
  short, this means: 'e'/'E' scientific format, 'f' fixed format, 'g'/'G' scientific or fixed,
  whichever is shorter.
  
  The second and third characters are optional and specific to QCustomPlot:\n
  If the first char was 'e' or 'g', numbers are/might be displayed in the scientific format, e.g.
  "5.5e9", which is ugly in a plot. So when the second char of \a formatCode is set to 'b' (for
  "beautiful"), those exponential numbers are formatted in a more natural way, i.e. "5.5
  [multiplication sign] 10 [superscript] 9". By default, the multiplication sign is a centered dot.
  If instead a cross should be shown (as is usual in the USA), the third char of \a formatCode can
  be set to 'c'. The inserted multiplication signs are the UTF-8 characters 215 (0xD7) for the
  cross and 183 (0xB7) for the dot.
  
  If the scale type (\ref setScaleType) is \ref stLogarithmic and the \a formatCode uses the 'b'
  option (beautifully typeset decimal powers), the display usually is "1 [multiplication sign] 10
  [superscript] n", which looks unnatural for logarithmic scaling (the "1 [multiplication sign]"
  part). To only display the decimal power, set the number precision to zero with \ref
  setNumberPrecision.
  
  Examples for \a formatCode:
  \li \c g normal format code behaviour. If number is small, fixed format is used, if number is large,
  normal scientific format is used
  \li \c gb If number is small, fixed format is used, if number is large, scientific format is used with
  beautifully typeset decimal powers and a dot as multiplication sign
  \li \c ebc All numbers are in scientific format with beautifully typeset decimal power and a cross as
  multiplication sign
  \li \c fb illegal format code, since fixed format doesn't support (or need) beautifully typeset decimal
  powers. Format code will be reduced to 'f'.
  \li \c hello illegal format code, since first char is not 'e', 'E', 'f', 'g' or 'G'. Current format
  code will not be changed.
*/
void QCPAxis::setNumberFormat(const QString &formatCode)
{
  if (formatCode.length() < 1) return;
  
  // interpret first char as number format char:
  QString allowedFormatChars = "eEfgG";
  if (allowedFormatChars.contains(formatCode.at(0)))
  {
    mNumberFormatChar = formatCode.at(0).toAscii();
  } else
  {
    qDebug() << Q_FUNC_INFO << "Invalid number format code (first char not in 'eEfgG'):" << formatCode;
    return;
  }
  if (formatCode.length() < 2)
  {
    mNumberBeautifulPowers = false;
    mNumberMultiplyCross = false;
    return;
  }
  
  // interpret second char as indicator for beautiful decimal powers:
  if (formatCode.at(1) == 'b' && (mNumberFormatChar == 'e' || mNumberFormatChar == 'g'))
  {
    mNumberBeautifulPowers = true;
  } else
  {
    qDebug() << Q_FUNC_INFO << "Invalid number format code (second char not 'b' or first char neither 'e' nor 'g'):" << formatCode;
    return;
  }
  if (formatCode.length() < 3)
  {
    mNumberMultiplyCross = false;
    return;
  }
  
  // interpret third char as indicator for dot or cross multiplication symbol:
  if (formatCode.at(2) == 'c')
  {
    mNumberMultiplyCross = true;
  } else if (formatCode.at(2) == 'd')
  {
    mNumberMultiplyCross = false;
  } else
  {
    qDebug() << Q_FUNC_INFO << "Invalid number format code (third char neither 'c' nor 'd'):" << formatCode;
    return;
  }
}

/*!
  Sets the precision of the numbers drawn as tick labels. See QLocale::toString(double i, char f,
  int prec) for details. The effect of precisions are most notably for number Formats starting with
  'e', see \ref setNumberFormat

  If the scale type (\ref setScaleType) is \ref stLogarithmic and the number format (\ref
  setNumberFormat) uses the 'b' format code (beautifully typeset decimal powers), the display
  usually is "1 [multiplication sign] 10 [superscript] n", which looks unnatural for logarithmic
  scaling (the "1 [multiplication sign]" part). To only display the decimal power, set \a precision
  to zero.
*/
void QCPAxis::setNumberPrecision(int precision)
{
  mNumberPrecision = precision;
}

/*!
  If \ref setAutoTickStep is set to false, use this function to set the tick step manually.
  The tick step is the interval between (major) ticks, in plot coordinates.
  \see setSubTickCount
*/
void QCPAxis::setTickStep(double step)
{
  mTickStep = step;
}

/*!
  If you want full control over what ticks (and possibly labels) the axes show, this function is
  used to set the coordinates at which ticks will appear.\ref setAutoTicks must be disabled, else
  the provided tick vector will be overwritten with automatically generated tick coordinates. The
  labels of the ticks can either be generated automatically when \ref setAutoTickLabels is left
  enabled, or be set manually with \ref setTickVectorLabels, when \ref setAutoTickLabels is
  disabled.
  
  \a vec is a vector containing the positions of the ticks.

  \see setTickVectorLabels
*/
void QCPAxis::setTickVector(const QVector<double> &vec)
{
  mTickVector = vec;
}

/*!
  If you want full control over what ticks and labels the axes show, this function is used to set a
  number of QStrings that will be displayed at the tick positions which you need to provide with
  \ref setTickVector. These two vectors should have the same size. (Note that you need to disable
  \ref setAutoTicks and \ref setAutoTickLabels first.)
  
  \a vec is a vector containing the labels of the ticks.
  
  \see setTickVector
*/
void QCPAxis::setTickVectorLabels(const QVector<QString> &vec)
{
  mTickVectorLabels = vec;
}

/*!
  Sets the length of the ticks in pixels. \a inside is the length the ticks will reach inside the
  plot and \a outside is the length they will reach outside the plot. If \a outside is greater than
  zero, the tick labels will increase their distance to the axis accordingly, so they won't collide
  with the ticks.
  \see setSubTickLength
*/
void QCPAxis::setTickLength(int inside, int outside)
{
  mTickLengthIn = inside;
  mTickLengthOut = outside;
}

/*!
  Sets the number of sub ticks in one (major) tick step. A sub tick count of three for example,
  divides the tick intervals in four sub intervals.
  
  By default, the number of sub ticks is chosen automatically in a reasonable manner as long as
  the mantissa of the tick step is a multiple of 0.5 (which it is, when \ref setAutoTickStep is enabled).
  If you want to disable automatic sub ticks and use this function to set the count manually, see
  \ref setAutoSubTicks.
*/
void QCPAxis::setSubTickCount(int count)
{
  mSubTickCount = count;
}

/*!
  Sets the length of the subticks in pixels. \a inside is the length the subticks will reach inside the
  plot and \a outside is the length they will reach outside the plot. If \a outside is greater than
  zero, the tick labels will increase their distance to the axis accordingly, so they won't collide
  with the ticks.
  \see setTickLength
*/
void QCPAxis::setSubTickLength(int inside, int outside)
{
  mSubTickLengthIn = inside;
  mSubTickLengthOut = outside;
}

/*!
  Sets the pen, the axis base line is drawn with.
  
  \see setTickPen, setSubTickPen
*/
void QCPAxis::setBasePen(const QPen &pen)
{
  mBasePen = pen;
}

/*!
  Sets the pen, grid lines are drawn with.
  \see setSubGridPen, setZeroLinePen
*/
void QCPAxis::setGridPen(const QPen &pen)
{
  mGrid->setPen(pen);
}

/*!
  Sets the pen, the sub grid lines are drawn with.
  (By default, subgrid drawing needs to be enabled first with \ref setSubGrid.)
  \see setGridPen, setZeroLinePen
*/
void QCPAxis::setSubGridPen(const QPen &pen)
{
  mGrid->setSubGridPen(pen);
}

/*!
  Sets the pen with which a single grid-like line will be drawn at value position zero. The line
  will be drawn instead of a grid line at that position, and not on top. To disable the drawing of
  a zero-line, set \a pen to Qt::NoPen. Then, if \ref setGrid is enabled, a grid line will be
  drawn instead.
  \see setGrid, setGridPen
*/
void QCPAxis::setZeroLinePen(const QPen &pen)
{
  mGrid->setZeroLinePen(pen);
}

/*!
  Sets the pen, tick marks will be drawn with.
  \see setTickLength, setBasePen
*/
void QCPAxis::setTickPen(const QPen &pen)
{
  mTickPen = pen;
}

/*!
  Sets the pen, subtick marks will be drawn with.
  \see setSubTickCount, setSubTickLength, setBasePen
*/
void QCPAxis::setSubTickPen(const QPen &pen)
{
  mSubTickPen = pen;
}

/*!
  Sets the font of the axis label.
  
  \see setLabelColor
*/
void QCPAxis::setLabelFont(const QFont &font)
{
  mLabelFont = font;
}

/*!
  Sets the color of the axis label.
  
  \see setLabelFont
*/
void QCPAxis::setLabelColor(const QColor &color)
{
  mLabelColor = color;
}

/*!
  Sets the axis label that will be shown below/above or next to the axis, depending on its orientation.
*/
void QCPAxis::setLabel(const QString &str)
{
  mLabel = str;
}

/*!
  Sets the distance between the tick labels and the axis label.
  \see setTickLabelPadding, setPadding
*/
void QCPAxis::setLabelPadding(int padding)
{
  mLabelPadding = padding;
}

/*!
  Sets the padding of the axis.

  When \ref QCustomPlot::setAutoMargin is enabled, the padding is the additional distance to the
  respective widget border, that is left blank. If \a padding is zero (default), the auto margin
  mechanism will find a margin that the axis label (or tick label, if no axis label is set) barely
  fits inside the QCustomPlot widget. To give the label closest to the border some freedom,
  increase \a padding.
  
  The axis padding has no meaning if \ref QCustomPlot::setAutoMargin is disabled.
  
  \see setLabelPadding, setTickLabelPadding
*/
void QCPAxis::setPadding(int padding)
{
  mPadding = padding;
}

/*!
  Sets the font that is used for tick labels when they are selected.
  
  \see setTickLabelFont, setSelectable, setSelected, QCustomPlot::setInteractions
*/
void QCPAxis::setSelectedTickLabelFont(const QFont &font)
{
  mSelectedTickLabelFont = font;
}

/*!
  Sets the font that is used for the axis label when it is selected.
  
  \see setLabelFont, setSelectable, setSelected, QCustomPlot::setInteractions
*/
void QCPAxis::setSelectedLabelFont(const QFont &font)
{
  mSelectedLabelFont = font;
}

/*!
  Sets the color that is used for tick labels when they are selected.
  
  \see setTickLabelColor, setSelectable, setSelected, QCustomPlot::setInteractions
*/
void QCPAxis::setSelectedTickLabelColor(const QColor &color)
{
  mSelectedTickLabelColor = color;
}

/*!
  Sets the color that is used for the axis label when it is selected.
  
  \see setLabelColor, setSelectable, setSelected, QCustomPlot::setInteractions
*/
void QCPAxis::setSelectedLabelColor(const QColor &color)
{
  mSelectedLabelColor = color;
}

/*!
  Sets the pen that is used to draw the axis base line when selected.
  
  \see setBasePen, setSelectable, setSelected, QCustomPlot::setInteractions
*/
void QCPAxis::setSelectedBasePen(const QPen &pen)
{
  mSelectedBasePen = pen;
}

/*!
  Sets the pen that is used to draw the (major) ticks when selected.
  
  \see setTickPen, setSelectable, setSelected, QCustomPlot::setInteractions
*/
void QCPAxis::setSelectedTickPen(const QPen &pen)
{
  mSelectedTickPen = pen;
}

/*!
  Sets the pen that is used to draw the subticks when selected.
  
  \see setSubTickPen, setSelectable, setSelected, QCustomPlot::setInteractions
*/
void QCPAxis::setSelectedSubTickPen(const QPen &pen)
{
  mSelectedSubTickPen = pen;
}

/*!
  If the scale type (\ref setScaleType) is \ref stLinear, \a diff is added to the lower and upper
  bounds of the range. The range is simply moved by \a diff.
  
  If the scale type is \ref stLogarithmic, the range bounds are multiplied by \a diff. This
  corresponds to an apparent "linear" move in logarithmic scaling by a distance of log(diff).
*/
void QCPAxis::moveRange(double diff)
{
  if (mScaleType == stLinear)
  {
    mRange.lower += diff;
    mRange.upper += diff;
  } else // mScaleType == stLogarithmic
  {
    mRange.lower *= diff;
    mRange.upper *= diff;
  }
  emit rangeChanged(mRange);
}

/*!
  Scales the range of this axis by \a factor around the coordinate \a center. For example, if \a
  factor is 2.0, \a center is 1.0, then the axis range will double its size, and the point at
  coordinate 1.0 won't have changed its position in the QCustomPlot widget (i.e. coordinates
  around 1.0 will have moved symmetrically closer to 1.0).
*/
void QCPAxis::scaleRange(double factor, double center)
{
  
  if (mScaleType == stLinear)
  {
    QCPRange newRange;
    newRange.lower = (mRange.lower-center)*factor + center;
    newRange.upper = (mRange.upper-center)*factor + center;
    if (QCPRange::validRange(newRange))
      mRange = newRange.sanitizedForLinScale();
  } else // mScaleType == stLogarithmic
  {
    if ((mRange.upper < 0 && center < 0) || (mRange.upper > 0 && center > 0)) // make sure center has same sign as range
    {
      QCPRange newRange;
      newRange.lower = pow(mRange.lower/center, factor)*center;
      newRange.upper = pow(mRange.upper/center, factor)*center;
      if (QCPRange::validRange(newRange))
        mRange = newRange.sanitizedForLogScale();
    } else
      qDebug() << Q_FUNC_INFO << "center of scaling operation doesn't lie in same logarithmic sign domain as range:" << center;
  }
  emit rangeChanged(mRange);
}

/*!
  Sets the range of this axis to have a certain scale \a ratio to \a otherAxis. For example, if \a
  ratio is 1, this axis is the \a yAxis and \a otherAxis is \a xAxis, graphs plotted with those
  axes will appear in a 1:1 ratio, independent of the aspect ratio the axis rect has. This is an
  operation that changes the range of this axis once, it doesn't fix the scale ratio indefinitely.
  Consequently calling this function in the constructor won't have the desired effect, since the
  widget's dimensions aren't defined yet, and a resizeEvent will follow.
*/
void QCPAxis::setScaleRatio(const QCPAxis *otherAxis, double ratio)
{
  int otherPixelSize, ownPixelSize;
  
  if (otherAxis->orientation() == Qt::Horizontal)
    otherPixelSize = otherAxis->mAxisRect.width();
  else
    otherPixelSize = otherAxis->mAxisRect.height();
  
  if (orientation() == Qt::Horizontal)
    ownPixelSize = mAxisRect.width();
  else
    ownPixelSize = mAxisRect.height();
  
  double newRangeSize = ratio*otherAxis->mRange.size()*ownPixelSize/(double)otherPixelSize;
  setRange(range().center(), newRangeSize, Qt::AlignCenter);
}

/*!
  Transforms \a value (in pixel coordinates of the QCustomPlot widget) to axis coordinates.
*/
double QCPAxis::pixelToCoord(double value) const
{
  if (orientation() == Qt::Horizontal)
  {
    if (mScaleType == stLinear)
    {
      if (!mRangeReversed)
        return (value-mAxisRect.left())/(double)mAxisRect.width()*mRange.size()+mRange.lower;
      else
        return -(value-mAxisRect.left())/(double)mAxisRect.width()*mRange.size()+mRange.upper;
    } else // mScaleType == stLogarithmic
    {
      if (!mRangeReversed)
        return pow(mRange.upper/mRange.lower, (value-mAxisRect.left())/(double)mAxisRect.width())*mRange.lower;
      else
        return pow(mRange.upper/mRange.lower, (mAxisRect.left()-value)/(double)mAxisRect.width())*mRange.upper;
    }
  } else // orientation() == Qt::Vertical
  {
    if (mScaleType == stLinear)
    {
      if (!mRangeReversed)
        return (mAxisRect.bottom()-value)/(double)mAxisRect.height()*mRange.size()+mRange.lower;
      else
        return -(mAxisRect.bottom()-value)/(double)mAxisRect.height()*mRange.size()+mRange.upper;
    } else // mScaleType == stLogarithmic
    {
      if (!mRangeReversed)
        return pow(mRange.upper/mRange.lower, (mAxisRect.bottom()-value)/(double)mAxisRect.height())*mRange.lower;
      else
        return pow(mRange.upper/mRange.lower, (value-mAxisRect.bottom())/(double)mAxisRect.height())*mRange.upper;
    }
  }
}

/*!
  Transforms \a value (in coordinates of the axis) to pixel coordinates of the QCustomPlot widget.
*/
double QCPAxis::coordToPixel(double value) const
{
  if (orientation() == Qt::Horizontal)
  {
    if (mScaleType == stLinear)
    {
      if (!mRangeReversed)
        return (value-mRange.lower)/mRange.size()*mAxisRect.width()+mAxisRect.left();
      else
        return (mRange.upper-value)/mRange.size()*mAxisRect.width()+mAxisRect.left();
    } else // mScaleType == stLogarithmic
    {
      if (value >= 0 && mRange.upper < 0) // invalid value for logarithmic scale, just draw it outside visible range
        return !mRangeReversed ? mAxisRect.right()+200 : mAxisRect.left()-200;
      else if (value <= 0 && mRange.upper > 0) // invalid value for logarithmic scale, just draw it outside visible range
        return !mRangeReversed ? mAxisRect.left()-200 : mAxisRect.right()+200;
      else
      {
        if (!mRangeReversed)
          return baseLog(value/mRange.lower)/baseLog(mRange.upper/mRange.lower)*mAxisRect.width()+mAxisRect.left();
        else
          return baseLog(mRange.upper/value)/baseLog(mRange.upper/mRange.lower)*mAxisRect.width()+mAxisRect.left();
      }
    }
  } else // orientation() == Qt::Vertical
  {
    if (mScaleType == stLinear)
    {
      if (!mRangeReversed)
        return mAxisRect.bottom()-(value-mRange.lower)/mRange.size()*mAxisRect.height();
      else
        return mAxisRect.bottom()-(mRange.upper-value)/mRange.size()*mAxisRect.height();
    } else // mScaleType == stLogarithmic
    {     
      if (value >= 0 && mRange.upper < 0) // invalid value for logarithmic scale, just draw it outside visible range
        return !mRangeReversed ? mAxisRect.top()-200 : mAxisRect.bottom()+200;
      else if (value <= 0 && mRange.upper > 0) // invalid value for logarithmic scale, just draw it outside visible range
        return !mRangeReversed ? mAxisRect.bottom()+200 : mAxisRect.top()-200;
      else
      {
        if (!mRangeReversed)
          return mAxisRect.bottom()-baseLog(value/mRange.lower)/baseLog(mRange.upper/mRange.lower)*mAxisRect.height();
        else
          return mAxisRect.bottom()-baseLog(mRange.upper/value)/baseLog(mRange.upper/mRange.lower)*mAxisRect.height();
      }
    }
  }
}

/*!
  Returns the part of the axis that is hit by \a pos (in pixels). The return value of this function
  is independent of the user-selectable parts defined with \ref setSelectable. Further, this
  function does not change the current selection state of the axis.
  
  If the axis is not visible (\ref setVisible), this function always returns \ref spNone.
  
  \see setSelected, setSelectable, QCustomPlot::setInteractions
*/
QCPAxis::SelectablePart QCPAxis::selectTest(const QPointF &pos) const
{
  if (!mVisible)
    return spNone;
  
  if (mAxisSelectionBox.contains(pos.toPoint()))
    return spAxis;
  else if (mTickLabelsSelectionBox.contains(pos.toPoint()))
    return spTickLabels;
  else if (mLabelSelectionBox.contains(pos.toPoint()))
    return spAxisLabel;
  else
    return spNone;
}

/*! \internal
  
  This function is called before the grid and axis is drawn, in order to prepare the tick vector,
  sub tick vector and tick label vector. If \ref setAutoTicks is set to true, appropriate tick
  values are determined automatically via \ref generateAutoTicks. If it's set to false, the signal
  ticksRequest is emitted, which can be used to provide external tick positions. Then the sub tick
  vectors and tick label vectors are created.
*/
void QCPAxis::setupTickVectors()
{
  if ((!mTicks && !mTickLabels && !mGrid->visible()) || mRange.size() <= 0) return;
  
  // fill tick vectors, either by auto generating or by notifying user to fill the vectors himself
  if (mAutoTicks)
  {
    generateAutoTicks();
  } else
  {
    emit ticksRequest();
  }
  
  visibleTickBounds(mLowestVisibleTick, mHighestVisibleTick);
  if (mTickVector.isEmpty())
  {
    mSubTickVector.clear();
    return;
  }
  
  // generate subticks between ticks:
  mSubTickVector.resize((mTickVector.size()-1)*mSubTickCount);
  if (mSubTickCount > 0)
  {
    double subTickStep = 0;
    double subTickPosition = 0;
    int subTickIndex = 0;
    bool done = false;
    for (int i=1; i<mTickVector.size(); ++i)
    {
      subTickStep = (mTickVector.at(i)-mTickVector.at(i-1))/(double)(mSubTickCount+1);
      for (int k=1; k<=mSubTickCount; ++k)
      {
        subTickPosition = mTickVector.at(i-1) + k*subTickStep;
        if (subTickPosition < mRange.lower)
          continue;
        if (subTickPosition > mRange.upper)
        {
          done = true;
          break;
        }
        mSubTickVector[subTickIndex] = subTickPosition;
        subTickIndex++;
      }
      if (done) break;
    }
    mSubTickVector.resize(subTickIndex);
  }

  // generate tick labels according to tick positions:
  mExponentialChar = mParentPlot->locale().exponential();   // will be needed when drawing the numbers generated here, in drawTickLabel()
  mPositiveSignChar = mParentPlot->locale().positiveSign(); // will be needed when drawing the numbers generated here, in drawTickLabel()
  if (mAutoTickLabels)
  {
    int vecsize = mTickVector.size();
    mTickVectorLabels.resize(vecsize);
    if (mTickLabelType == ltNumber)
    {
      for (int i=0; i<vecsize; ++i)
        mTickVectorLabels[i] = mParentPlot->locale().toString(mTickVector.at(i), mNumberFormatChar, mNumberPrecision);
    } else if (mTickLabelType == ltDateTime)
    {
      for (int i=0; i<vecsize; ++i)
      {
#if QT_VERSION < QT_VERSION_CHECK(4, 7, 0) // use fromMSecsSinceEpoch function if available, to gain sub-second accuracy on tick labels (e.g. for format "hh:mm:ss:zzz")
        mTickVectorLabels[i] = mParentPlot->locale().toString(QDateTime::fromTime_t(mTickVector.at(i)), mDateTimeFormat);
#else
        mTickVectorLabels[i] = mParentPlot->locale().toString(QDateTime::fromMSecsSinceEpoch(mTickVector.at(i)*1000), mDateTimeFormat);
#endif
      }
    }
  } else // mAutoTickLabels == false
  {
    if (mAutoTicks) // ticks generated automatically, but not ticklabels, so emit ticksRequest here for labels
    {
      emit ticksRequest();
    }
    // make sure provided tick label vector has correct (minimal) length:
    if (mTickVectorLabels.size() < mTickVector.size())
      mTickVectorLabels.resize(mTickVector.size());
  }
}

/*! \internal
  
  If \ref setAutoTicks is set to true, this function is called by \ref setupTickVectors to
  generate reasonable tick positions (and subtick count). The algorithm tries to create
  approximately <tt>mAutoTickCount</tt> ticks (set via \ref setAutoTickCount), taking into account,
  that tick mantissas that are divisable by two or end in .5 are nice to look at and practical in
  linear scales. If the scale is logarithmic, one tick is generated at every power of the current
  logarithm base, set via \ref setScaleLogBase.
*/
void QCPAxis::generateAutoTicks()
{
  if (mScaleType == stLinear)
  {
    if (mAutoTickStep)
    {
      // Generate tick positions according to linear scaling:
      mTickStep = mRange.size()/(double)(mAutoTickCount+1e-10); // mAutoTickCount ticks on average, the small addition is to prevent jitter on exact integers
      double magnitudeFactor = qPow(10.0, qFloor(qLn(mTickStep)/qLn(10.0))); // get magnitude factor e.g. 0.01, 1, 10, 1000 etc.
      double tickStepMantissa = mTickStep/magnitudeFactor;
      if (tickStepMantissa < 5)
      {
        // round digit after decimal point to 0.5
        mTickStep = (int)(tickStepMantissa*2)/2.0*magnitudeFactor;
      } else
      {
        // round to first digit in multiples of 2
        mTickStep = (int)((tickStepMantissa/10.0)*5)/5.0*10*magnitudeFactor;
      }
    }
    if (mAutoSubTicks)
      mSubTickCount = calculateAutoSubTickCount(mTickStep);
    // Generate tick positions according to mTickStep:
    int firstStep = floor(mRange.lower/mTickStep);
    int lastStep = ceil(mRange.upper/mTickStep);
    int tickcount = lastStep-firstStep+1;
    if (tickcount < 0) tickcount = 0;
    mTickVector.resize(tickcount);
    for (int i=0; i<tickcount; ++i)
    {
      mTickVector[i] = (firstStep+i)*mTickStep;
    }
  } else // mScaleType == stLogarithmic
  {
    // Generate tick positions according to logbase scaling:
    if (mRange.lower > 0 && mRange.upper > 0) // positive range
    {
      double lowerMag = basePow((int)floor(baseLog(mRange.lower)));
      double currentMag = lowerMag;
      mTickVector.clear();
      mTickVector.append(currentMag);
      while (currentMag < mRange.upper && currentMag > 0) // currentMag might be zero for ranges ~1e-300, just cancel in that case
      {
        currentMag *= mScaleLogBase;
        mTickVector.append(currentMag);
      }
    } else if (mRange.lower < 0 && mRange.upper < 0) // negative range
    {
      double lowerMag = -basePow((int)ceil(baseLog(-mRange.lower)));
      double currentMag = lowerMag;
      mTickVector.clear();
      mTickVector.append(currentMag);
      while (currentMag < mRange.upper && currentMag < 0) // currentMag might be zero for ranges ~1e-300, just cancel in that case
      {
        currentMag /= mScaleLogBase;
        mTickVector.append(currentMag);
      }
    } else // invalid range for logarithmic scale, because lower and upper have different sign
    {
      mTickVector.clear();
      qDebug() << Q_FUNC_INFO << "Invalid range for logarithmic plot: " << mRange.lower << "-" << mRange.upper;
    }
  }
}

/*! \internal
  
  Called by generateAutoTicks when \ref setAutoSubTicks is set to true. Depending on the \a
  tickStep between two major ticks on the axis, a different number of sub ticks is appropriate. For
  Example taking 4 sub ticks for a \a tickStep of 1 makes more sense than taking 5 sub ticks,
  because this corresponds to a sub tick step of 0.2, instead of the less intuitive 0.16666. Note
  that a subtick count of 4 means dividing the major tick step into 5 sections.
  
  This is implemented by a hand made lookup for integer tick steps as well as fractional tick steps
  with a fractional part of (approximately) 0.5. If a tick step is different (i.e. has no
  fractional part close to 0.5), the currently set sub tick count (\ref setSubTickCount) is
  returned.
*/
int QCPAxis::calculateAutoSubTickCount(double tickStep) const
{
  int result = mSubTickCount; // default to current setting, if no proper value can be found
  
  // get mantissa of tickstep:
  double magnitudeFactor = qPow(10.0, qFloor(qLn(tickStep)/qLn(10.0))); // get magnitude factor e.g. 0.01, 1, 10, 1000 etc.
  double tickStepMantissa = tickStep/magnitudeFactor;
  
  // separate integer and fractional part of mantissa:
  double epsilon = 0.01;
  double intPartf;
  int intPart;
  double fracPart = modf(tickStepMantissa, &intPartf);
  intPart = intPartf;
  
  // handle cases with (almost) integer mantissa:
  if (fracPart < epsilon || 1.0-fracPart < epsilon)
  {
    if (1.0-fracPart < epsilon)
      intPart++;
    switch (intPart)
    {
      case 1: result = 4; break; // 1.0 -> 0.2 substep
      case 2: result = 3; break; // 2.0 -> 0.5 substep
      case 3: result = 2; break; // 3.0 -> 1.0 substep
      case 4: result = 3; break; // 4.0 -> 1.0 substep
      case 5: result = 4; break; // 5.0 -> 1.0 substep
      case 6: result = 2; break; // 6.0 -> 2.0 substep
      case 7: result = 6; break; // 7.0 -> 1.0 substep
      case 8: result = 3; break; // 8.0 -> 2.0 substep
      case 9: result = 2; break; // 9.0 -> 3.0 substep
    }
  } else
  {
    // handle cases with significantly fractional mantissa:
    if (qAbs(fracPart-0.5) < epsilon) // *.5 mantissa
    {
      switch (intPart)
      {
        case 1: result = 2; break; // 1.5 -> 0.5 substep
        case 2: result = 4; break; // 2.5 -> 0.5 substep
        case 3: result = 4; break; // 3.5 -> 0.7 substep
        case 4: result = 2; break; // 4.5 -> 1.5 substep
        case 5: result = 4; break; // 5.5 -> 1.1 substep (won't occur with autoTickStep from here on)
        case 6: result = 4; break; // 6.5 -> 1.3 substep
        case 7: result = 2; break; // 7.5 -> 2.5 substep
        case 8: result = 4; break; // 8.5 -> 1.7 substep
        case 9: result = 4; break; // 9.5 -> 1.9 substep
      }
    }
    // if mantissa fraction isnt 0.0 or 0.5, don't bother finding good sub tick marks, leave default
  }
  
  return result;
}

/*! \internal
  
  The main draw function of an axis, called by QCustomPlot::draw for each axis. Draws axis
  baseline, major ticks, subticks, tick labels and axis label.
  
  The selection boxes (mAxisSelectionBox, mTickLabelsSelectionBox, mLabelSelectionBox) are set
  here, too.
*/
void QCPAxis::draw(QCPPainter *painter)
{
  QPoint origin;
  if (mAxisType == atLeft)
    origin = mAxisRect.bottomLeft();
  else if (mAxisType == atRight)
    origin = mAxisRect.bottomRight();
  else if (mAxisType == atTop)
    origin = mAxisRect.topLeft();
  else if (mAxisType == atBottom)
    origin = mAxisRect.bottomLeft();
  
  double xCor = 0, yCor = 0; // paint system correction, for pixel exact matches (affects baselines and ticks of top/right axes)
  switch (mAxisType)
  {
    case atTop: yCor = -1; break;
    case atRight: xCor = 1; break;
    default: break;
  }
  
  int margin = 0;
  int lowTick = mLowestVisibleTick;
  int highTick = mHighestVisibleTick;
  double t; // helper variable, result of coordinate-to-pixel transforms

  // draw baseline:
  painter->setPen(getBasePen());
  if (orientation() == Qt::Horizontal)
    painter->drawLine(QLineF(origin+QPointF(xCor, yCor), origin+QPointF(mAxisRect.width()+xCor, yCor)));
  else
    painter->drawLine(QLineF(origin+QPointF(xCor, yCor), origin+QPointF(xCor, -mAxisRect.height()+yCor)));
  
  // draw ticks:
  if (mTicks)
  {
    painter->setPen(getTickPen());
    // direction of ticks ("inward" is right for left axis and left for right axis)
    int tickDir = (mAxisType == atBottom || mAxisType == atRight) ? -1 : 1;
    if (orientation() == Qt::Horizontal)
    {
      for (int i=lowTick; i <= highTick; ++i)
      {
        t = coordToPixel(mTickVector.at(i)); // x
        painter->drawLine(QLineF(t+xCor, origin.y()-mTickLengthOut*tickDir+yCor, t+xCor, origin.y()+mTickLengthIn*tickDir+yCor));
      }
    } else
    {
      for (int i=lowTick; i <= highTick; ++i)
      {
        t = coordToPixel(mTickVector.at(i)); // y
        painter->drawLine(QLineF(origin.x()-mTickLengthOut*tickDir+xCor, t+yCor, origin.x()+mTickLengthIn*tickDir+xCor, t+yCor));
      }
    }
  }
  
  // draw subticks:
  if (mTicks && mSubTickCount > 0)
  {
    painter->setPen(getSubTickPen());
    // direction of ticks ("inward" is right for left axis and left for right axis)
    int tickDir = (mAxisType == atBottom || mAxisType == atRight) ? -1 : 1;
    if (orientation() == Qt::Horizontal)
    {
      for (int i=0; i<mSubTickVector.size(); ++i) // no need to check bounds because subticks are always only created inside current mRange
      {
        t = coordToPixel(mSubTickVector.at(i));
        painter->drawLine(QLineF(t+xCor, origin.y()-mSubTickLengthOut*tickDir+yCor, t+xCor, origin.y()+mSubTickLengthIn*tickDir+yCor));
      }
    } else
    {
      for (int i=0; i<mSubTickVector.size(); ++i)
      {
        t = coordToPixel(mSubTickVector.at(i));
        painter->drawLine(QLineF(origin.x()-mSubTickLengthOut*tickDir+xCor, t+yCor, origin.x()+mSubTickLengthIn*tickDir+xCor, t+yCor));
      }
    }
  }
  margin += qMax(0, qMax(mTickLengthOut, mSubTickLengthOut));
  
  // tick labels:
  QSize tickLabelsSize(0, 0); // size of largest tick label, for offset calculation of axis label
  if (mTickLabels)
  {
    margin += mTickLabelPadding;
    painter->setFont(getTickLabelFont());
    painter->setPen(QPen(getTickLabelColor()));
    for (int i=lowTick; i <= highTick; ++i)
    {
      t = coordToPixel(mTickVector.at(i));
      drawTickLabel(painter, t, margin, mTickVectorLabels.at(i), &tickLabelsSize);
    }
  }
  if (orientation() == Qt::Horizontal)
    margin += tickLabelsSize.height();
  else
    margin += tickLabelsSize.width();

  // axis label:
  QRect labelBounds;
  if (!mLabel.isEmpty())
  {
    margin += mLabelPadding;
    painter->setFont(getLabelFont());
    painter->setPen(QPen(getLabelColor()));
    labelBounds = painter->fontMetrics().boundingRect(0, 0, 0, 0, Qt::TextDontClip, mLabel);
    if (mAxisType == atLeft)
    {
      QTransform oldTransform = painter->transform();
      painter->translate((origin.x()-margin-labelBounds.height()), origin.y());
      painter->rotate(-90);
      painter->drawText(0, 0, mAxisRect.height(), labelBounds.height(), Qt::TextDontClip | Qt::AlignCenter, mLabel);
      painter->setTransform(oldTransform);
    }
    else if (mAxisType == atRight)
    {
      QTransform oldTransform = painter->transform();
      painter->translate((origin.x()+margin+labelBounds.height()), origin.y()-mAxisRect.height());
      painter->rotate(90);
      painter->drawText(0, 0, mAxisRect.height(), labelBounds.height(), Qt::TextDontClip | Qt::AlignCenter, mLabel);
      painter->setTransform(oldTransform);
    }
    else if (mAxisType == atTop)
      painter->drawText(origin.x(), origin.y()-margin-labelBounds.height(), mAxisRect.width(), labelBounds.height(), Qt::TextDontClip | Qt::AlignCenter, mLabel);
    else if (mAxisType == atBottom)
      painter->drawText(origin.x(), origin.y()+margin, mAxisRect.width(), labelBounds.height(), Qt::TextDontClip | Qt::AlignCenter, mLabel);
  }
  
  // set selection boxes:
  int selAxisOutSize = qMax(qMax(mTickLengthOut, mSubTickLengthOut), mParentPlot->selectionTolerance());
  int selAxisInSize = mParentPlot->selectionTolerance();
  int selTickLabelSize = (orientation()==Qt::Horizontal ? tickLabelsSize.height() : tickLabelsSize.width());
  int selTickLabelOffset = qMax(mTickLengthOut, mSubTickLengthOut)+mTickLabelPadding;
  int selLabelSize = labelBounds.height();
  int selLabelOffset = selTickLabelOffset+selTickLabelSize+mLabelPadding;
  if (mAxisType == atLeft)
  {
    mAxisSelectionBox.setCoords(mAxisRect.left()-selAxisOutSize, mAxisRect.top(), mAxisRect.left()+selAxisInSize, mAxisRect.bottom());
    mTickLabelsSelectionBox.setCoords(mAxisRect.left()-selTickLabelOffset-selTickLabelSize, mAxisRect.top(), mAxisRect.left()-selTickLabelOffset, mAxisRect.bottom());
    mLabelSelectionBox.setCoords(mAxisRect.left()-selLabelOffset-selLabelSize, mAxisRect.top(), mAxisRect.left()-selLabelOffset, mAxisRect.bottom());
  } else if (mAxisType == atRight)
  {
    mAxisSelectionBox.setCoords(mAxisRect.right()-selAxisInSize, mAxisRect.top(), mAxisRect.right()+selAxisOutSize, mAxisRect.bottom());
    mTickLabelsSelectionBox.setCoords(mAxisRect.right()+selTickLabelOffset+selTickLabelSize, mAxisRect.top(), mAxisRect.right()+selTickLabelOffset, mAxisRect.bottom());
    mLabelSelectionBox.setCoords(mAxisRect.right()+selLabelOffset+selLabelSize, mAxisRect.top(), mAxisRect.right()+selLabelOffset, mAxisRect.bottom());
  } else if (mAxisType == atTop)
  {
    mAxisSelectionBox.setCoords(mAxisRect.left(), mAxisRect.top()-selAxisOutSize, mAxisRect.right(), mAxisRect.top()+selAxisInSize);
    mTickLabelsSelectionBox.setCoords(mAxisRect.left(), mAxisRect.top()-selTickLabelOffset-selTickLabelSize, mAxisRect.right(), mAxisRect.top()-selTickLabelOffset);
    mLabelSelectionBox.setCoords(mAxisRect.left(), mAxisRect.top()-selLabelOffset-selLabelSize, mAxisRect.right(), mAxisRect.top()-selLabelOffset);
  } else if (mAxisType == atBottom)
  {
    mAxisSelectionBox.setCoords(mAxisRect.left(), mAxisRect.bottom()-selAxisInSize, mAxisRect.right(), mAxisRect.bottom()+selAxisOutSize);
    mTickLabelsSelectionBox.setCoords(mAxisRect.left(), mAxisRect.bottom()+selTickLabelOffset+selTickLabelSize, mAxisRect.right(), mAxisRect.bottom()+selTickLabelOffset);
    mLabelSelectionBox.setCoords(mAxisRect.left(), mAxisRect.bottom()+selLabelOffset+selLabelSize, mAxisRect.right(), mAxisRect.bottom()+selLabelOffset);
  }
  // draw hitboxes for debug purposes:
  //painter->drawRects(QVector<QRect>() << mAxisSelectionBox << mTickLabelsSelectionBox << mLabelSelectionBox);
}

/*! \internal
  
  Draws a single tick label with the provided \a painter. The tick label is always bound to an axis
  in one direction (distance to axis in that direction is however controllable via \a
  distanceToAxis in pixels). The position in the other direction is passed in the \a position
  parameter. Hence for the bottom axis, \a position would indicate the horizontal pixel position
  (not coordinate!), at which the label should be drawn.
  
  In order to draw the axis label after all the tick labels in a position, that doesn't overlap
  with the tick labels, we need to know the largest tick label size. This is done by passing a \a
  tickLabelsSize to all \ref drawTickLabel calls during the process of drawing all tick labels of
  one axis. \a tickLabelSize is only expanded, if the drawn label exceeds the value \a
  tickLabelsSize currently holds.
  
  This function is also responsible for turning ugly exponential numbers "5.5e9" into a more
  beautifully typeset format "5.5 [multiplication sign] 10 [superscript] 9". This feature is
  controlled with \ref setNumberFormat.
  
  The label is drawn with the font and pen that are currently set on the \a painter. To draw
  superscripted powers, the font is temporarily made smaller by a fixed factor.
*/
void QCPAxis::drawTickLabel(QCPPainter *painter, double position, int distanceToAxis, const QString &text, QSize *tickLabelsSize)
{
  // warning: if you change anything here, also adapt getMaxTickLabelSize() accordingly!
  
  // determine whether beautiful decimal powers should be used
  bool useBeautifulPowers = false;
  int ePos = -1;
  if (mAutoTickLabels && mNumberBeautifulPowers && mTickLabelType == ltNumber)
  {
    ePos = text.indexOf('e');
    if (ePos > -1)
      useBeautifulPowers = true;
  }
  
  // calculate text bounding rects and do string preparation for beautiful decimal powers:
  QRect bounds, baseBounds, expBounds;
  QString basePart, expPart;
  QFont bugFixFont(painter->font());
  bugFixFont.setPointSizeF(bugFixFont.pointSizeF()+0.05); // QFontMetrics.boundingRect has a bug for exact point sizes that make the results oscillate due to internal rounding 
  QFont expFont;
  if (useBeautifulPowers)
  {
    // split string parts for part of number/symbol that will be drawn normally and part that will be drawn as exponent:
    basePart = text.left(ePos);
    // in log scaling, we want to turn "1*10^n" into "10^n", else add multiplication sign and decimal base:
    if (mScaleType == stLogarithmic && basePart == "1")
      basePart = "10";
    else
      basePart += (mNumberMultiplyCross ? QString(QChar(215)) : QString(QChar(183))) + "10";
    expPart = text.mid(ePos+1);
    // clip "+" and leading zeros off expPart:
    while (expPart.at(1) == '0' && expPart.length() > 2) // length > 2 so we leave one zero when numberFormatChar is 'e'
      expPart.remove(1, 1);
    if (expPart.at(0) == mPositiveSignChar)
      expPart.remove(0, 1);
    // prepare smaller font for exponent:
    expFont = painter->font();
    expFont.setPointSize(expFont.pointSize()*0.75);
    // calculate bounding rects of base part, exponent part and total one:
    QFontMetrics fontMetrics(bugFixFont);
    baseBounds = fontMetrics.boundingRect(0, 0, 0, 0, Qt::TextDontClip, basePart);
    QFontMetrics expFontMetrics(expFont);
    expBounds = expFontMetrics.boundingRect(0, 0, 0, 0, Qt::TextDontClip, expPart);
    bounds = baseBounds.adjusted(0, 0, expBounds.width(), 0);
  } else // useBeautifulPowers == false
  {
    QFontMetrics fontMetrics(bugFixFont);
    bounds = fontMetrics.boundingRect(0, 0, 0, 0, Qt::TextDontClip | Qt::AlignHCenter, text);
  }
  
  // if using rotated tick labels, transform bounding rect, too:
  QRect rotatedBounds = bounds;
  if (!qFuzzyIsNull(mTickLabelRotation))
  {
    QTransform transform;
    transform.rotate(mTickLabelRotation);
    rotatedBounds = transform.mapRect(bounds);
  }
  // expand passed tickLabelsSize if current tick label is larger:
  if (rotatedBounds.width() > tickLabelsSize->width()) 
    tickLabelsSize->setWidth(rotatedBounds.width());
  if (rotatedBounds.height() > tickLabelsSize->height())
    tickLabelsSize->setHeight(rotatedBounds.height());
  
  /*
    calculate coordinates (non-trivial, for best visual appearance): short explanation for bottom
    axis: The anchor, i.e. the point in the label that is placed horizontally under the
    corresponding tick is always on the label side that is closer to the axis (e.g. the left side
    of the text when we're rotating clockwise). On that side, the height is halved and the
    resulting point is defined the anchor. This way, a 90 degree rotated text will be centered
    under the tick (i.e. displaced horizontally by half its height). At the same time, a 45 degree
    rotated text will "point toward" its tick, as is typical for rotated tick labels.
  */
  bool doRotation = !qFuzzyIsNull(mTickLabelRotation);
  double radians = mTickLabelRotation/180.0*M_PI;
  int x=0,y=0;
  if (mAxisType == atLeft)
  {
    if (doRotation)
    {
      if (mTickLabelRotation > 0)
      {
        x = mAxisRect.left()-qCos(radians)*bounds.width()-distanceToAxis;
        y = position-qSin(radians)*bounds.width()-qCos(radians)*bounds.height()/2.0;
      } else
      {
        x = mAxisRect.left()-qCos(-radians)*bounds.width()-qSin(-radians)*bounds.height()-distanceToAxis;
        y = position+qSin(-radians)*bounds.width()-qCos(-radians)*bounds.height()/2.0;
      }
    } else
    {
      x = mAxisRect.left()-bounds.width()-distanceToAxis;
      y = position-bounds.height()/2.0;
    }
  } else if (mAxisType == atRight)
  {
    if (doRotation)
    {
      if (mTickLabelRotation > 0)
      {
        x = mAxisRect.right()+qSin(radians)*bounds.height()+distanceToAxis;
        y = position-qCos(radians)*bounds.height()/2.0;
      } else
      {
        x = mAxisRect.right()+distanceToAxis;
        y = position-qCos(-radians)*bounds.height()/2.0;
      }
    } else
    {
      x = mAxisRect.right()+distanceToAxis;
      y = position-bounds.height()/2.0;
    }
  } else if (mAxisType == atTop)
  {
    if (doRotation)
    {
      if (mTickLabelRotation > 0)
      {
        x = position-qCos(radians)*bounds.width()+qSin(radians)*bounds.height()/2.0;
        y = mAxisRect.top()-qSin(radians)*bounds.width()-qCos(radians)*bounds.height()-distanceToAxis;
      } else
      {
        x = position-qSin(-radians)*bounds.height()/2.0;
        y = mAxisRect.top()-qCos(-radians)*bounds.height()-distanceToAxis;
      }
    } else
    {
      x = position-bounds.width()/2.0;
      y = mAxisRect.top()-bounds.height()-distanceToAxis;
    }
  } else if (mAxisType == atBottom)
  {
    if (doRotation)
    {
      if (mTickLabelRotation > 0)
      {
        x = position+qSin(radians)*bounds.height()/2.0;
        y = mAxisRect.bottom()+distanceToAxis;
      } else
      {
        x = position-qCos(-radians)*bounds.width()-qSin(-radians)*bounds.height()/2.0;
        y = mAxisRect.bottom()+qSin(-radians)*bounds.width()+distanceToAxis;
      }
    } else
    {
      x = position-bounds.width()/2.0;
      y = mAxisRect.bottom()+distanceToAxis;
    }
  }
  
  // if label would be partly clipped by widget border on sides, don't draw it:
  if (orientation() == Qt::Horizontal)
  {
    if (x+bounds.width() > mParentPlot->mViewport.right() ||
        x < mParentPlot->mViewport.left())
      return;
  } else
  {
    if (y+bounds.height() > mParentPlot->mViewport.bottom() ||
        y < mParentPlot->mViewport.top())
      return;
  }
  
  // transform painter to position/rotation:
  QTransform oldTransform = painter->transform();
  painter->translate(x, y);
  if (doRotation)
    painter->rotate(mTickLabelRotation);
  // draw text:
  if (useBeautifulPowers)
  {
    // draw base:
    painter->drawText(0, 0, 0, 0, Qt::TextDontClip, basePart);
    // draw exponent:
    QFont normalFont = painter->font();
    painter->setFont(expFont);
    painter->drawText(baseBounds.width()+1, 0, expBounds.width(), expBounds.height(), Qt::TextDontClip,  expPart);
    painter->setFont(normalFont);
  } else // useBeautifulPowers == false
  {
    painter->drawText(0, 0, bounds.width(), bounds.height(), Qt::TextDontClip | Qt::AlignHCenter, text);
  }
  
  // reset rotation/translation transform to what it was before:
  painter->setTransform(oldTransform);
}

/*! \internal
  
  Simulates the steps done by \ref drawTickLabel by calculating bounding boxes of the text label to
  be drawn, depending on number format etc. Since we only want the largest tick label for the
  margin calculation, the passed \a tickLabelsSize isn't overridden with the calculated label size,
  but it's only expanded, if it's currently set to a smaller width/height.
*/
void QCPAxis::getMaxTickLabelSize(const QFont &font, const QString &text,  QSize *tickLabelsSize) const
{
  // This function does the same as drawTickLabel but omits the actual drawing
  // changes involve creating extra QFontMetrics instances for font, since painter->fontMetrics() isn't available
  
  // determine whether beautiful powers should be used
  bool useBeautifulPowers = false;
  int ePos=-1;
  if (mAutoTickLabels && mNumberBeautifulPowers && mTickLabelType == ltNumber)
  {
    ePos = text.indexOf(mExponentialChar);
    if (ePos > -1)
      useBeautifulPowers = true;
  }
  
  // calculate and draw text, depending on whether beautiful powers are applicable or not:
  QRect bounds, baseBounds, expBounds;
  QString basePart, expPart;
  QFont bugFixFont(font);
  bugFixFont.setPointSizeF(bugFixFont.pointSizeF()+0.05); // QFontMetrics.boundingRect has a bug for exact point sizes that make the results oscillate due to internal rounding 
  QFont expFont;
  if (useBeautifulPowers)
  {
    // split string parts for part of number/symbol that will be drawn normally and part that will be drawn as exponent:
    basePart = text.left(ePos);
    // in log scaling, we want to turn "1*10^n" into "10^n", else add multiplication sign and decimal base:
    if (mScaleType == stLogarithmic && basePart == "1")
      basePart = "10";
    else
      basePart += (mNumberMultiplyCross ? QString(QChar(215)) : QString(QChar(183))) + "10";
    expPart = text.mid(ePos+1);
    // clip "+" and leading zeros off expPart:
    while (expPart.at(1) == '0' && expPart.length() > 2) // length > 2 so we leave one zero when numberFormatChar is 'e'
      expPart.remove(1, 1);
    if (expPart.at(0) == mPositiveSignChar)
      expPart.remove(0, 1);
    // prepare smaller font for exponent:
    expFont = font;
    expFont.setPointSize(expFont.pointSize()*0.75);
    // calculate bounding rects of base part, exponent part and total one:
    QFontMetrics baseFontMetrics(bugFixFont);
    baseBounds = baseFontMetrics.boundingRect(0, 0, 0, 0, Qt::TextDontClip, basePart);
    QFontMetrics expFontMetrics(expFont);
    expBounds = expFontMetrics.boundingRect(0, 0, 0, 0, Qt::TextDontClip, expPart);
    bounds = baseBounds.adjusted(0, 0, expBounds.width(), 0); 
  } else // useBeautifulPowers == false
  {
    QFontMetrics fontMetrics(bugFixFont);
    bounds = fontMetrics.boundingRect(0, 0, 0, 0, Qt::TextDontClip | Qt::AlignHCenter, text);
  }
  
  // if rotated tick labels, transform bounding rect, too:
  QRect rotatedBounds = bounds;
  if (!qFuzzyIsNull(mTickLabelRotation))
  {
    QTransform transform;
    transform.rotate(mTickLabelRotation);
    rotatedBounds = transform.mapRect(bounds);
  }
  
  // expand passed tickLabelsSize if current tick label is larger:
  if (rotatedBounds.width() > tickLabelsSize->width()) 
    tickLabelsSize->setWidth(rotatedBounds.width());
  if (rotatedBounds.height() > tickLabelsSize->height())
    tickLabelsSize->setHeight(rotatedBounds.height());
}

/*! \internal
  
  Handles the selection \a event and returns true when the selection event hit any parts of the
  axis. If the selection state of any parts of the axis was changed, the output parameter \a
  modified is set to true.
  
  When \a additiveSelecton is true, any new selections become selected in addition to the recent
  selections. The recent selections are not cleared. Further, clicking on one object multiple times
  in additive selection mode, toggles the selection of that object on and off.
  
  To indicate that an event deselects the axis (i.e. the parts that are deselectable by the user,
  see \ref setSelectable), pass 0 as \a event.
*/
bool QCPAxis::handleAxisSelection(QMouseEvent *event, bool additiveSelection, bool &modified)
{
  bool selectionFound = false;
  if (event)
  {
    SelectablePart selectedAxisPart = selectTest(event->pos());
    if (selectedAxisPart == spNone || !selectable().testFlag(selectedAxisPart))
    {
      // deselect parts that are changeable (selectable):
      SelectableParts newState = selected() & ~selectable();
      if (newState != selected() && !additiveSelection)
      {
        modified = true;
        setSelected(newState);
      }
    } else
    {
      selectionFound = true;
      if (additiveSelection)
      {
        // additive selection, so toggle selected part:
        setSelected(selected() ^ selectedAxisPart);
        modified = true;
      } else
      {
        // not additive selection, so select part and deselect all others that are changeable (selectable):
        SelectableParts newState = (selected() & ~selectable()) | selectedAxisPart;
        if (newState != selected())
        {
          modified = true;
          setSelected(newState);
        }
      }
    }
  } else // event == 0, so deselect all changeable parts
  {
    SelectableParts newState = selected() & ~selectable();
    if (newState != selected())
    {
      modified = true;
      setSelected(newState);
    }
  }
  return selectionFound;
}

/*! \internal

  A convenience function to easily set the QPainter::Antialiased hint on the provided \a painter
  before drawing axis lines.

  This is the antialiasing state the painter passed to the \ref draw method is in by default.
  
  This function takes into account the local setting of the antialiasing flag as well as
  the overrides set e.g. with \ref QCustomPlot::setNotAntialiasedElements.
  
  \see setAntialiased
*/
void QCPAxis::applyDefaultAntialiasingHint(QCPPainter *painter) const
{
  applyAntialiasingHint(painter, mAntialiased, QCP::aeAxes);
}

/*! \internal
  
  Returns via \a lowIndex and \a highIndex, which ticks in the current tick vector are visible in
  the current range. The return values are indices of the tick vector, not the positions of the
  ticks themselves.
  
  The actual use of this function is when we have an externally provided tick vector, which might
  exceed far beyond the currently displayed range, and would cause unnecessary calculations e.g. of
  subticks.
*/
void QCPAxis::visibleTickBounds(int &lowIndex, int &highIndex) const
{
  lowIndex = 0;
  highIndex = -1;
  // make sure only ticks that are in visible range are returned
  for (int i=0; i < mTickVector.size(); ++i)
  {
    lowIndex = i;
    if (mTickVector.at(i) >= mRange.lower) break;
  }
  for (int i=mTickVector.size()-1; i >= 0; --i)
  {
    highIndex = i;
    if (mTickVector.at(i) <= mRange.upper) break;
  }
}

/*! \internal
  
  A log function with the base mScaleLogBase, used mostly for coordinate transforms in logarithmic
  scales with arbitrary log base. Uses the buffered mScaleLogBaseLogInv for faster calculation.
  This is set to <tt>1.0/qLn(mScaleLogBase)</tt> in \ref setScaleLogBase.
  
  \see basePow, setScaleLogBase, setScaleType
*/
double QCPAxis::baseLog(double value) const
{
  return qLn(value)*mScaleLogBaseLogInv;
}

/*! \internal
  
  A power function with the base mScaleLogBase, used mostly for coordinate transforms in
  logarithmic scales with arbitrary log base.
  
  \see baseLog, setScaleLogBase, setScaleType
*/
double QCPAxis::basePow(double value) const
{
  return qPow(mScaleLogBase, value);
}

/*! \internal
  
  Returns the pen that is used to draw the axis base line. Depending on the selection state, this
  is either mSelectedBasePen or mBasePen.
*/
QPen QCPAxis::getBasePen() const
{
  return mSelected.testFlag(spAxis) ? mSelectedBasePen : mBasePen;
}

/*! \internal
  
  Returns the pen that is used to draw the (major) ticks. Depending on the selection state, this
  is either mSelectedTickPen or mTickPen.
*/
QPen QCPAxis::getTickPen() const
{
  return mSelected.testFlag(spAxis) ? mSelectedTickPen : mTickPen;
}

/*! \internal
  
  Returns the pen that is used to draw the subticks. Depending on the selection state, this
  is either mSelectedSubTickPen or mSubTickPen.
*/
QPen QCPAxis::getSubTickPen() const
{
  return mSelected.testFlag(spAxis) ? mSelectedSubTickPen : mSubTickPen;
}

/*! \internal
  
  Returns the font that is used to draw the tick labels. Depending on the selection state, this
  is either mSelectedTickLabelFont or mTickLabelFont.
*/
QFont QCPAxis::getTickLabelFont() const
{
  return mSelected.testFlag(spTickLabels) ? mSelectedTickLabelFont : mTickLabelFont;
}

/*! \internal
  
  Returns the font that is used to draw the axis label. Depending on the selection state, this
  is either mSelectedLabelFont or mLabelFont.
*/
QFont QCPAxis::getLabelFont() const
{
  return mSelected.testFlag(spAxisLabel) ? mSelectedLabelFont : mLabelFont;
}

/*! \internal
  
  Returns the color that is used to draw the tick labels. Depending on the selection state, this
  is either mSelectedTickLabelColor or mTickLabelColor.
*/
QColor QCPAxis::getTickLabelColor() const
{
  return mSelected.testFlag(spTickLabels) ? mSelectedTickLabelColor : mTickLabelColor;
}

/*! \internal
  
  Returns the color that is used to draw the axis label. Depending on the selection state, this
  is either mSelectedLabelColor or mLabelColor.
*/
QColor QCPAxis::getLabelColor() const
{
  return mSelected.testFlag(spAxisLabel) ? mSelectedLabelColor : mLabelColor;
}

/*! \internal
  
  Simulates the steps of \ref draw by calculating all appearing text bounding boxes. From this
  information, the appropriate margin for this axis is determined, so nothing is drawn beyond the
  widget border in the actual \ref draw function (if \ref QCustomPlot::setAutoMargin is set to
  true).
  
  The margin consists of: tick label padding, tick label size, label padding, label size. The
  return value is the calculated margin for this axis. Thus, an axis with axis type \ref atLeft
  will return an appropriate left margin, \ref atBottom will return an appropriate bottom margin
  and so forth.
  
  \warning if anything is changed in this function, make sure it's synchronized with the actual
  drawing function \ref draw.
*/
int QCPAxis::calculateMargin() const
{
  // run through similar steps as QCPAxis::draw, and caluclate margin needed to fit axis and its labels
  int margin = 0;
  
  if (mVisible)
  {
    int lowTick, highTick;
    visibleTickBounds(lowTick, highTick);
    // get length of tick marks reaching outside axis rect:
    margin += qMax(0, qMax(mTickLengthOut, mSubTickLengthOut));
    // calculate size of tick labels:
    QSize tickLabelsSize(0, 0);
    if (mTickLabels)
    {
      for (int i=lowTick; i <= highTick; ++i)
      {
        getMaxTickLabelSize(mTickLabelFont, mTickVectorLabels.at(i), &tickLabelsSize); // don't use getTickLabelFont() because we don't want margin to possibly change on selection
      }
      if (orientation() == Qt::Horizontal)
        margin += tickLabelsSize.height() + mTickLabelPadding;
      else
        margin += tickLabelsSize.width() + mTickLabelPadding;
    }
    // calculate size of axis label (only height needed, because left/right labels are rotated by 90 degrees):
    if (!mLabel.isEmpty())
    {
      QFontMetrics fontMetrics(mLabelFont); // don't use getLabelFont() because we don't want margin to possibly change on selection
      QRect bounds;
      bounds = fontMetrics.boundingRect(0, 0, 0, 0, Qt::TextDontClip | Qt::AlignHCenter | Qt::AlignVCenter, mLabel);
      margin += bounds.height() + mLabelPadding;
    }
  }
  margin += mPadding;
  
  if (margin < 15) // need a bit of margin if no axis text is shown at all (i.e. only baseline and tick lines, or no axis at all)
    margin = 15;
  return margin;
}


// ================================================================================
// =================== QCustomPlot
// ================================================================================

/*! \class QCustomPlot
  \brief The central class of the library, the QWidget which displays the plot and interacts with the user.
  
  For tutorials on how to use QCustomPlot, see the website\n
  http://www.WorksLikeClockWork.com/index.php/components/qt-plotting-widget
*/

/* start of documentation of inline functions */

/*! \fn QRect QCustomPlot::viewport() const
  
  Returns the viewport rect of this QCustomPlot instance. The viewport is the area the plot is
  drawn in, all mechanisms, e.g. margin caluclation take the viewport to be the outer border of the
  plot. The viewport normally is the rect() of the QCustomPlot widget, i.e. a rect with top left
  (0, 0) and size of the QCustomPlot widget.
  
  Don't confuse the viewport with the axisRect. An axisRect is the rect defined by two axes, where
  the graphs/plottables are drawn in. The viewport is larger and contains also the axes themselves, their
  tick numbers, their labels, the plot title etc.
  
  Only when saving to a file (see \ref savePng, savePdf etc.) the viewport is temporarily modified
  to allow saving plots with sizes independent of the current widget size.
*/

/* end of documentation of inline functions */
/* start of documentation of signals */

/*! \fn void QCustomPlot::mouseDoubleClick(QMouseEvent *event)

  This signal is emitted when the QCustomPlot receives a mouse double click event.
*/

/*! \fn void QCustomPlot::mousePress(QMouseEvent *event)

  This signal is emitted when the QCustomPlot receives a mouse press event.
  
  It is emitted before the QCustomPlot handles its range dragging mechanism, so a slot connected to
  this signal can still influence the behaviour e.g. with \ref setRangeDrag or \ref
  setRangeDragAxes.
*/

/*! \fn void QCustomPlot::mouseMove(QMouseEvent *event)

  This signal is emitted when the QCustomPlot receives a mouse move event.
  
  It is emitted before the QCustomPlot handles its range dragging mechanism, so a slot connected to
  this signal can still influence the behaviour e.g. with \ref setRangeDrag.
  
  \warning It is discouraged to change the drag-axes with \ref setRangeDragAxes here, because the
  dragging starting point was saved the moment the mouse was pressed. Thus it only has a sensible
  meaning for the range drag axes that were set at that moment. If you want to change the drag
  axes, consider doing this in the \ref mousePress signal instead.
*/

/*! \fn void QCustomPlot::mouseRelease(QMouseEvent *event)

  This signal is emitted when the QCustomPlot receives a mouse release event.
  
  It is emitted before the QCustomPlot handles its selection mechanism, so a slot connected to this
  signal can still influence the behaviour e.g. with \ref setInteractions or \ref
  QCPAbstractPlottable::setSelectable.
*/

/*! \fn void QCustomPlot::mouseWheel(QMouseEvent *event)

  This signal is emitted when the QCustomPlot receives a mouse wheel event.
  
  It is emitted before the QCustomPlot handles its range zooming mechanism, so a slot connected to
  this signal can still influence the behaviour e.g. with \ref setRangeZoom, \ref setRangeZoomAxes
  or \ref setRangeZoomFactor.
*/

/*! \fn void QCustomPlot::plottableClick(QCPAbstractPlottable *plottable, QMouseEvent *event)
  
  This signal is emitted when a plottable is clicked.

  \a event is the mouse event that caused the click and \a plottable is the plottable that received
  the click.
  
  \see plottableDoubleClick
*/

/*! \fn void QCustomPlot::plottableDoubleClick(QCPAbstractPlottable *plottable, QMouseEvent *event)
  
  This signal is emitted when a plottable is double clicked.
  
  \a event is the mouse event that caused the click and \a plottable is the plottable that received
  the click.
  
  \see plottableClick
*/

/*! \fn void QCustomPlot::itemClick(QCPAbstractItem *item, QMouseEvent *event)
  
  This signal is emitted when an item is clicked.

  \a event is the mouse event that caused the click and \a item is the item that received the
  click.
  
  \see itemDoubleClick
*/

/*! \fn void QCustomPlot::itemDoubleClick(QCPAbstractItem *item, QMouseEvent *event)
  
  This signal is emitted when an item is double clicked.
  
  \a event is the mouse event that caused the click and \a item is the item that received the
  click.
  
  \see itemClick
*/

/*! \fn void QCustomPlot::axisClick(QCPAxis *axis, QCPAxis::SelectablePart part, QMouseEvent *event)
  
  This signal is emitted when an axis is clicked.
  
  \a event is the mouse event that caused the click, \a axis is the axis that received the click and
  \a part indicates the part of the axis that was clicked.
  
  \see axisDoubleClick
*/

/*! \fn void QCustomPlot::axisDoubleClick(QCPAxis *axis, QCPAxis::SelectablePart part, QMouseEvent *event)

  This signal is emitted when an axis is double clicked.
  
  \a event is the mouse event that caused the click, \a axis is the axis that received the click and
  \a part indicates the part of the axis that was clicked.
  
  \see axisClick
*/

/*! \fn void QCustomPlot::legendClick(QCPLegend *legend, QCPAbstractLegendItem *item, QMouseEvent *event)

  This signal is emitted when a legend (item) is clicked.
  
  \a event is the mouse event that caused the click, \a legend is the legend that received the
  click and \a item is the legend item that received the click. If only the legend and no item is
  clicked, \a item is 0 (e.g. a click inside the legend padding, which is not part of any item).
  
  \see legendDoubleClick
*/

/*! \fn void QCustomPlot::legendDoubleClick(QCPLegend *legend,  QCPAbstractLegendItem *item, QMouseEvent *event)

  This signal is emitted when a legend (item) is double clicked.
  
  \a event is the mouse event that caused the click, \a legend is the legend that received the
  click and \a item is the legend item that received the click. If only the legend and no item is
  clicked, \a item is 0 (e.g. a click inside the legend padding, which is not part of any item).
  
  \see legendClick
*/

/*! \fn void QCustomPlot:: titleClick(QMouseEvent *event)

  This signal is emitted when the plot title is clicked.
  
  \a event is the mouse event that caused the click.
  
  \see titleDoubleClick
*/

/*! \fn void QCustomPlot::titleDoubleClick(QMouseEvent *event)

  This signal is emitted when the plot title is double clicked.
  
  \a event is the mouse event that caused the click.
  
  \see titleClick
*/

/*! \fn void QCustomPlot::selectionChangedByUser()
  
  This signal is emitted after the user has changed the selection in the QCustomPlot, e.g. by
  clicking. It is not emitted, when the selection state of an object has changed programmatically,
  e.g. by a direct call to setSelected() on a plottable or by calling \ref deselectAll.
  
  See the documentation of \ref setInteractions for how to find out which objects are currently
  selected.
  
  \see setInteractions, QCPAbstractPlottable::selectionChanged, QCPAxis::selectionChanged
*/

/*! \fn void QCustomPlot::beforeReplot()
  
  This signal is emitted immediately before a replot takes place (caused by a call to the slot \ref
  replot).
  
  It is safe to mutually connect the replot slot with this signal on two QCustomPlots to make them
  replot synchronously (i.e. it won't cause an infinite recursion).
  
  \see replot, afterReplot
*/

/*! \fn void QCustomPlot::afterReplot()
  
  This signal is emitted immediately after a replot has taken place (caused by a call to the slot \ref
  replot).
  
  It is safe to mutually connect the replot slot with this signal on two QCustomPlots to make them
  replot synchronously (i.e. it won't cause an infinite recursion).
  
  \see replot, beforeReplot
*/

/* end of documentation of signals */

/*!
  Constructs a QCustomPlot and sets reasonable default values.
  Four axes are created at the bottom, left, top and right sides (xAxis, yAxis, xAxis2, yAxis2),
  however, only the bottom and left axes are set to be visible.
  The legend is also set to be invisible initially.
*/
QCustomPlot::QCustomPlot(QWidget *parent) :
  QWidget(parent),
  mDragging(false),
  mReplotting(false),
  mPlottingHints(QCP::phNone)
{
  setAttribute(Qt::WA_NoMousePropagation);
  setAttribute(Qt::WA_OpaquePaintEvent);
  setMouseTracking(true);
  QLocale currentLocale = locale();
  currentLocale.setNumberOptions(QLocale::OmitGroupSeparator);
  setLocale(currentLocale);
  
  // create very first layers:
  QCPLayer *gridLayer = new QCPLayer(this, "grid");
  QCPLayer *mainLayer = new QCPLayer(this, "main");
  QCPLayer *axesLayer = new QCPLayer(this, "axes");
  mLayers.append(gridLayer);
  mLayers.append(mainLayer);
  mLayers.append(axesLayer);
  setCurrentLayer(mainLayer);

  mPaintBuffer = QPixmap(size());
  legend = new QCPLegend(this);
  legend->setVisible(false);
  legend->setLayer(axesLayer);
  xAxis = new QCPAxis(this, QCPAxis::atBottom);
  yAxis = new QCPAxis(this, QCPAxis::atLeft);
  xAxis2 = new QCPAxis(this, QCPAxis::atTop);
  yAxis2 = new QCPAxis(this, QCPAxis::atRight);
  xAxis2->setGrid(false);
  yAxis2->setGrid(false);
  xAxis2->setZeroLinePen(Qt::NoPen);
  yAxis2->setZeroLinePen(Qt::NoPen);
  xAxis2->setVisible(false);
  yAxis2->setVisible(false);
  xAxis->setLayer(axesLayer);
  yAxis->setLayer(axesLayer);
  xAxis2->setLayer(axesLayer);
  yAxis2->setLayer(axesLayer);
  xAxis->mGrid->setLayer(gridLayer);
  yAxis->mGrid->setLayer(gridLayer);
  xAxis2->mGrid->setLayer(gridLayer);
  yAxis2->mGrid->setLayer(gridLayer);
  mViewport = rect();
  
  setNoAntialiasingOnDrag(false);
  setAutoAddPlottableToLegend(true);
  setAxisBackgroundScaled(true);
  setAxisBackgroundScaledMode(Qt::KeepAspectRatioByExpanding);
  setTitleFont(QFont(font().family(), 14, QFont::Bold));
  setTitleColor(Qt::black);
  setSelectedTitleFont(QFont(font().family(), 14, QFont::Bold));
  setSelectedTitleColor(Qt::blue);
  setTitleSelected(false);
  setTitle("");
  setColor(Qt::white);
  
#ifdef Q_WS_WIN
  setPlottingHint(QCP::phForceRepaint);
#endif
  setAntialiasedElements(QCP::aeNone);
  setNotAntialiasedElements(QCP::aeNone);
  setInteractions(iRangeDrag|iRangeZoom);
  setMultiSelectModifier(Qt::ControlModifier);
  setRangeDragAxes(xAxis, yAxis);
  setRangeZoomAxes(xAxis, yAxis);
  setRangeDrag(0);
  setRangeZoom(0);
  setRangeZoomFactor(0.85);
  setSelectionTolerance(8);
  
  setMargin(0, 0, 0, 0); // also initializes the mAxisRect
  setAutoMargin(true);
  replot();
}

QCustomPlot::~QCustomPlot()
{
  clearPlottables();
  clearItems();
  delete legend;
  delete xAxis;
  delete yAxis;
  delete xAxis2;
  delete yAxis2;
  qDeleteAll(mLayers);
  mLayers.clear();
}

/*!
  Returns the range drag axis of the \a orientation provided
  \see setRangeDragAxes
*/
QCPAxis *QCustomPlot::rangeDragAxis(Qt::Orientation orientation)
{
  return (orientation == Qt::Horizontal ? mRangeDragHorzAxis : mRangeDragVertAxis);
}

/*!
  Returns the range zoom axis of the \a orientation provided
  \see setRangeZoomAxes
*/
QCPAxis *QCustomPlot::rangeZoomAxis(Qt::Orientation orientation)
{
  return (orientation == Qt::Horizontal ? mRangeZoomHorzAxis : mRangeZoomVertAxis);
}

/*!
  Returns the range zoom factor of the \a orientation provided
  \see setRangeZoomFactor
*/
double QCustomPlot::rangeZoomFactor(Qt::Orientation orientation)
{
  return (orientation == Qt::Horizontal ? mRangeZoomFactorHorz : mRangeZoomFactorVert);
}

/*!
  Sets the plot title which will be drawn centered at the top of the widget.
  The title position is not dependant on the actual position of the axes. However, if
  \ref setAutoMargin is set to true, the top margin will be adjusted appropriately,
  so the top axis labels/tick labels will not overlap with the title.
  
  \see setTitleFont, setTitleColor
*/
void QCustomPlot::setTitle(const QString &title)
{
  mTitle = title;
}

/*!
  Sets the font of the plot title
  \see setTitleColor, setTitle
*/
void QCustomPlot::setTitleFont(const QFont &font)
{
  mTitleFont = font;
}

/*!
  Sets the text color of the plot title
  \see setTitleFont, setTitle
*/
void QCustomPlot::setTitleColor(const QColor &color)
{
  mTitleColor = color;
}

/*!
  An alternative way to set the margins, by directly setting the wanted axis rect. The rect
  will be translated into appropriate margin values.
  
  \warning Setting the axis rect with this function does not guarantee that the axis rect will stay
  like this indefinitely. In QCustomPlot, margins are the fixed values (if \ref setAutoMargin is
  false). Hence the axis rect is automatically changed when the widget size changes, but the
  margins (distances between axis rect sides and widget/viewport rect sides) stay the same.

  \see setMargin
*/
void QCustomPlot::setAxisRect(const QRect &arect)
{
  mMarginLeft = arect.left()-mViewport.left();
  mMarginRight = mViewport.right()-arect.right();
  mMarginTop = arect.top()-mViewport.top();
  mMarginBottom = mViewport.bottom()-arect.bottom();
  updateAxisRect();
}

/*!
  Sets the left margin manually. Will only have effect, if \ref setAutoMargin is set to false.
  see \ref setMargin for an explanation of what margins mean in QCustomPlot.
*/
void QCustomPlot::setMarginLeft(int margin)
{
  mMarginLeft = margin;
  updateAxisRect();
}

/*!
  Sets the right margin manually. Will only have effect, if \ref setAutoMargin is set to false.
  see \ref setMargin for an explanation of what margins mean in QCustomPlot.
*/
void QCustomPlot::setMarginRight(int margin)
{
  mMarginRight = margin;
  updateAxisRect();
}

/*!
  Sets the top margin manually. Will only have effect, if \ref setAutoMargin is set to false.
  see \ref setMargin for an explanation of what margins mean in QCustomPlot.
*/
void QCustomPlot::setMarginTop(int margin)
{
  mMarginTop = margin;
  updateAxisRect();
}

/*!
  Sets the bottom margin manually. Will only have effect, if \ref setAutoMargin is set to false.
  see \ref setMargin for an explanation of what margins mean in QCustomPlot.
*/
void QCustomPlot::setMarginBottom(int margin)
{
  mMarginBottom = margin;
  updateAxisRect();
}

/*!
  Sets the margins manually. Will only have effect, if \ref setAutoMargin is set to false.
  The margins are the distances in pixels between the axes box and the viewport box.
  The viewport box normally is the entire QCustomPlot widget or the entire image, if
  using one of the export functions. Positive margin values always mean the axes box
  is shrinked, going inward from the sides of the viewport box.
*/
void QCustomPlot::setMargin(int left, int right, int top, int bottom)
{
  mMarginLeft = left;
  mMarginRight = right;
  mMarginTop = top;
  mMarginBottom = bottom;
  updateAxisRect();
}

/*!
  Sets whether the margins are calculated automatically depeding on the sizes
  of the tick labels, axis labels, paddings etc.
  If disabled, the margins must be set manually with the \a setMargin functions.
  \see setMargin, QCPAxis::setLabelPadding, QCPAxis::setTickLabelPadding
*/
void QCustomPlot::setAutoMargin(bool enabled)
{
  mAutoMargin = enabled;
}

/*!
  Sets the background color of the QCustomPlot widget.
*/
void QCustomPlot::setColor(const QColor &color)
{
  mColor = color;
}

/*!
  Sets which axis orientation may be range dragged by the user with mouse interaction.
  What orientation corresponds to which specific axis can be set with
  \ref setRangeDragAxes(QCPAxis *horizontal, QCPAxis *vertical). By
  default, the horizontal axis is the bottom axis (xAxis) and the vertical axis
  is the left axis (yAxis).
  
  To disable range dragging entirely, pass 0 as \a orientations or remove \ref iRangeDrag from \ref
  setInteractions. To enable range dragging for both directions, pass <tt>Qt::Horizontal |
  Qt::Vertical</tt> as \a orientations.
  
  In addition to setting \a orientations to a non-zero value, make sure \ref setInteractions
  contains \ref iRangeDrag to enable the range dragging interaction.
  
  \see setRangeZoom, setRangeDragAxes, setNoAntialiasingOnDrag
*/
void QCustomPlot::setRangeDrag(Qt::Orientations orientations)
{
  mRangeDrag = orientations;
}

/*!
  Sets which axis orientation may be zoomed by the user with the mouse wheel. What orientation
  corresponds to which specific axis can be set with \ref setRangeZoomAxes(QCPAxis *horizontal,
  QCPAxis *vertical). By default, the horizontal axis is the bottom axis (xAxis) and the vertical
  axis is the left axis (yAxis).

  To disable range zooming entirely, pass 0 as \a orientations or remove \ref iRangeZoom from \ref
  setInteractions. To enable range zooming for both directions, pass <tt>Qt::Horizontal |
  Qt::Vertical</tt> as \a orientations.
  
  In addition to setting \a orientations to a non-zero value, make sure \ref setInteractions
  contains \ref iRangeZoom to enable the range zooming interaction.
  
  \see setRangeZoomFactor, setRangeZoomAxes, setRangeDrag
*/
void QCustomPlot::setRangeZoom(Qt::Orientations orientations)
{
  mRangeZoom = orientations;
}

/*!
  Sets the axes whose range will be dragged when \ref setRangeDrag enables mouse range dragging
  on the QCustomPlot widget.
  
  \see setRangeZoomAxes
*/
void QCustomPlot::setRangeDragAxes(QCPAxis *horizontal, QCPAxis *vertical)
{
  if (horizontal)
    mRangeDragHorzAxis = horizontal;
  if (vertical)
    mRangeDragVertAxis = vertical;
}

/*!
  Sets the axes whose range will be zoomed when \ref setRangeZoom enables mouse wheel zooming on the
  QCustomPlot widget. The two axes can be zoomed with different strengths, when different factors
  are passed to \ref setRangeZoomFactor(double horizontalFactor, double verticalFactor).
  
  \see setRangeDragAxes
*/
void QCustomPlot::setRangeZoomAxes(QCPAxis *horizontal, QCPAxis *vertical)
{
  if (horizontal)
    mRangeZoomHorzAxis = horizontal;
  if (vertical)
    mRangeZoomVertAxis = vertical;
}

/*!
  Sets how strong one rotation step of the mouse wheel zooms, when range zoom was activated with
  \ref setRangeZoom. The two parameters \a horizontalFactor and \a verticalFactor provide a way to
  let the horizontal axis zoom at different rates than the vertical axis. Which axis is horizontal
  and which is vertical, can be set with \ref setRangeZoomAxes.

  When the zoom factor is greater than one, scrolling the mouse wheel backwards (towards the user)
  will zoom in (make the currently visible range smaller). For zoom factors smaller than one, the
  same scrolling direction will zoom out.
*/
void QCustomPlot::setRangeZoomFactor(double horizontalFactor, double verticalFactor)
{
  mRangeZoomFactorHorz = horizontalFactor;
  mRangeZoomFactorVert = verticalFactor;
}

/*! \overload
  
  Sets both the horizontal and vertical zoom \a factor.
*/
void QCustomPlot::setRangeZoomFactor(double factor)
{
  mRangeZoomFactorHorz = factor;
  mRangeZoomFactorVert = factor;
}

/*!
  Sets which elements are forcibly drawn antialiased as an or combination of QCP::AntialiasedElement.
  
  This overrides the antialiasing settings for whole element groups, normally controlled with the
  \a setAntialiasing function on the individual elements. If an element is neither specified in
  \ref setAntialiasedElements nor in \ref setNotAntialiasedElements, the antialiasing setting on
  each individual element instance is used.
  
  For example, if \a antialiasedElements contains \ref QCP::aePlottables, all plottables will be
  drawn antialiased, no matter what the specific QCPAbstractPlottable::setAntialiased value was set
  to.
  
  \see setNotAntialiasedElements
*/
void QCustomPlot::setAntialiasedElements(const QCP::AntialiasedElements &antialiasedElements)
{
  mAntialiasedElements = antialiasedElements;
  
  // make sure elements aren't in mNotAntialiasedElements and mAntialiasedElements simultaneously:
  if ((mNotAntialiasedElements & mAntialiasedElements) != 0)
    mNotAntialiasedElements |= ~mAntialiasedElements;
}

/*!
  Sets whether the specified \a antialiasedElement is forcibly drawn antialiased.
  
  This overrides the antialiasing settings for whole element groups, normally controlled with the
  \a setAntialiasing function on the individual elements. If an element is neither specified in
  \ref setAntialiasedElements nor in \ref setNotAntialiasedElements, the antialiasing setting on
  each individual element instance is used.
  
  For example, if \a enabled is true and \a antialiasedElement is \ref QCP::aePlottables, all
  plottables will be drawn antialiased, no matter what the specific
  QCPAbstractPlottable::setAntialiased value was set to.
  
  \see setNotAntialiasedElement
*/
void QCustomPlot::setAntialiasedElement(QCP::AntialiasedElement antialiasedElement, bool enabled)
{
  if (!enabled && mAntialiasedElements.testFlag(antialiasedElement))
    mAntialiasedElements &= ~antialiasedElement;
  else if (enabled && !mAntialiasedElements.testFlag(antialiasedElement))
    mAntialiasedElements |= antialiasedElement;
  
  // make sure elements aren't in mNotAntialiasedElements and mAntialiasedElements simultaneously:
  if ((mNotAntialiasedElements & mAntialiasedElements) != 0)
    mNotAntialiasedElements |= ~mAntialiasedElements;
}

/*!
  Sets which elements are forcibly drawn not antialiased as an or combination of
  QCP::AntialiasedElement.
  
  This overrides the antialiasing settings for whole element groups, normally controlled with the
  \a setAntialiasing function on the individual elements. If an element is neither specified in
  \ref setAntialiasedElements nor in \ref setNotAntialiasedElements, the antialiasing setting on
  each individual element instance is used.
  
  For example, if \a notAntialiasedElements contains \ref QCP::aePlottables, no plottables will be
  drawn antialiased, no matter what the specific QCPAbstractPlottable::setAntialiased value was set
  to.
  
  if an element in \a notAntialiasedElements is already set in \ref setAntialiasedElements, it is
  removed from there.
  
  \see setAntialiasedElements
*/
void QCustomPlot::setNotAntialiasedElements(const QCP::AntialiasedElements &notAntialiasedElements)
{
  mNotAntialiasedElements = notAntialiasedElements;
  
  // make sure elements aren't in mNotAntialiasedElements and mAntialiasedElements simultaneously:
  if ((mNotAntialiasedElements & mAntialiasedElements) != 0)
    mAntialiasedElements |= ~mNotAntialiasedElements;
}

/*!
  Sets whether the specified \a notAntialiasedElement is forcibly drawn not antialiased.
  
  This overrides the antialiasing settings for whole element groups, normally controlled with the
  \a setAntialiasing function on the individual elements. If an element is neither specified in
  \ref setAntialiasedElements nor in \ref setNotAntialiasedElements, the antialiasing setting on
  each individual element instance is used.
  
  For example, if \a enabled is true and \a notAntialiasedElement is \ref QCP::aePlottables, no
  plottables will be drawn antialiased, no matter what the specific
  QCPAbstractPlottable::setAntialiased value was set to.
  
  if \a enabled is true and \a notAntialiasedElement is already set with \ref
  setAntialiasedElement, it is removed from there.
  
  \see setAntialiasedElement
*/
void QCustomPlot::setNotAntialiasedElement(QCP::AntialiasedElement notAntialiasedElement, bool enabled)
{
  if (!enabled && mNotAntialiasedElements.testFlag(notAntialiasedElement))
    mNotAntialiasedElements &= ~notAntialiasedElement;
  else if (enabled && !mNotAntialiasedElements.testFlag(notAntialiasedElement))
    mNotAntialiasedElements |= notAntialiasedElement;
  
  // make sure elements aren't in mNotAntialiasedElements and mAntialiasedElements simultaneously:
  if ((mNotAntialiasedElements & mAntialiasedElements) != 0)
    mAntialiasedElements |= ~mNotAntialiasedElements;
}

/*!
  If set to true, adding a plottable (e.g. a graph) to the QCustomPlot automatically also adds the
  newly created plottable to the legend.
  
  \see addPlottable, addGraph, QCPLegend::addItem
*/
void QCustomPlot::setAutoAddPlottableToLegend(bool on)
{
  mAutoAddPlottableToLegend = on;
}

/*!
  Sets \a pm as the axis background pixmap. The axis background pixmap will be drawn inside the current
  axis rect, before anything else (e.g. the axes themselves, grids, graphs, etc.) is drawn.
  If the provided pixmap doesn't have the same size as the axis rect, scaling can be enabled with \ref setAxisBackgroundScaled
  and the scaling mode (i.e. whether and how the aspect ratio is preserved) can be set with \ref setAxisBackgroundScaledMode.
  To set all these options in one call, consider using the overloaded version of this function.
  \see setAxisBackgroundScaled, setAxisBackgroundScaledMode
*/
void QCustomPlot::setAxisBackground(const QPixmap &pm)
{
  mAxisBackground = pm;
  mScaledAxisBackground = QPixmap();
}

/*!
  \overload
  Allows setting the background pixmap, whether it shall be scaled and how it shall be scaled in one call.
  \see setAxisBackground(const QPixmap &pm), setAxisBackgroundScaled, setAxisBackgroundScaledMode
*/
void QCustomPlot::setAxisBackground(const QPixmap &pm, bool scaled, Qt::AspectRatioMode mode)
{
  mAxisBackground = pm;
  mScaledAxisBackground = QPixmap();
  mAxisBackgroundScaled = scaled;
  mAxisBackgroundScaledMode = mode;
}

/*!
  Sets whether the axis background pixmap shall be scaled to fit the current axis rect or not. If
  \a scaled is set to true, you may control whether and how the aspect ratio of the original pixmap is
  preserved with \ref setAxisBackgroundScaledMode.
  
  Note that the scaled version of the original pixmap is buffered, so there is no performance penalty
  on replots, when enabling the scaling. (Except of course, the axis rect is continuously
  changed, but that's not very likely.)
  
  \see setAxisBackground, setAxisBackgroundScaledMode
*/
void QCustomPlot::setAxisBackgroundScaled(bool scaled)
{
  mAxisBackgroundScaled = scaled;
}

/*!
  If scaling of the axis background pixmap is enabled (\ref setAxisBackgroundScaled), use this function to
  define whether and how the aspect ratio of the original pixmap passed to \ref setAxisBackground is preserved.
  \see setAxisBackground, setAxisBackgroundScaled
*/
void QCustomPlot::setAxisBackgroundScaledMode(Qt::AspectRatioMode mode)
{
  mAxisBackgroundScaledMode = mode;
}

/*!
  Sets the possible interactions of this QCustomPlot as an or-combination of \ref Interaction
  enums. There are the following types of interactions:
  
  <b>Axis range manipulation</b> is controlled via \ref iRangeDrag and \ref iRangeZoom. When the
  respective interaction is enabled, the user may drag axes ranges and zoom with the mouse wheel.
  For details how to control which axes the user may drag/zoom and in what orientations, see \ref
  setRangeDrag, \ref setRangeZoom, \ref setRangeDragAxes, \ref setRangeZoomAxes.
  
  <b>Plottable selection</b> is controlled by \ref iSelectPlottables. If \ref iSelectPlottables is
  set, the user may select plottables (e.g. graphs, curves, bars,...) by clicking on them or in
  their vicinity, see \ref setSelectionTolerance. Whether the user can actually select a plottable
  can further be restricted with the \ref QCPAbstractPlottable::setSelectable function on the
  specific plottable. To find out whether a specific plottable is selected, call
  QCPAbstractPlottable::selected(). To retrieve a list of all currently selected plottables, call
  \ref selectedPlottables. If you're only interested in QCPGraphs, you may use the convenience
  function \ref selectedGraphs.
  
  <b>Item selection</b> is controlled by \ref iSelectItems. If \ref iSelectItems is set, the user
  may select items (e.g. QCPItemLine, QCPItemText,...) by clicking on them or in their vicinity. To
  find out whether a specific item is selected, call QCPAbstractItem::selected(). To retrieve a
  list of all currently selected items, call \ref selectedItems.
  
  <b>Axis selection</b> is controlled with \ref iSelectAxes. If \ref iSelectAxes is set, the user
  may select parts of the axes by clicking on them. What parts exactly (e.g. Axis base line, tick
  labels, axis label) are selectable can be controlled via \ref QCPAxis::setSelectable for each
  axis. To retrieve a list of all axes that currently contain selected parts, call \ref
  selectedAxes. Which parts of an axis are selected, can be retrieved with QCPAxis::selected().
  
  <b>Legend selection</b> is controlled with \ref iSelectLegend. If this is set, the user may
  select the legend itself or individual items by clicking on them. What parts exactly are
  selectable can be controlled via \ref QCPLegend::setSelectable. To find out whether the legend or
  any child items are selected, check the value of QCPLegend::selected. To find out which child
  items are selected, call \ref QCPLegend::selectedItems.
  
  <b>Plot title selection</b> is controlled with \ref iSelectTitle. If set, the user may select the
  plot title by clicking on it. To find out whether the title is currently selected, call
  QCustomPlot::titleSelected().
  
  If the selection state has changed by user interaction, the \ref selectionChangedByUser signal is
  emitted. Each selectable object additionally emits an individual selectionChanged signal whenever
  their selection state has changed, i.e. not only by user interaction.
  
  To allow multiple objects to be selected by holding the modifier set with \ref
  setMultiSelectModifier, set the flag \ref iMultiSelect.
  
  \note In addition to the selection mechanism presented here, QCustomPlot always emits
  corresponding signals, when an object is clicked or double clicked. see \ref plottableClick and
  \ref plottableDoubleClick for example.
  
  \see setInteraction, setSelectionTolerance
*/
void QCustomPlot::setInteractions(const Interactions &interactions)
{
  mInteractions = interactions;
}

/*!
  Sets the single \a interaction of this QCustomPlot to \a enabled.
  
  For details about the interaction system, see \ref setInteractions.
  
  \see setInteractions
*/
void QCustomPlot::setInteraction(const QCustomPlot::Interaction &interaction, bool enabled)
{
  if (!enabled && mInteractions.testFlag(interaction))
    mInteractions &= ~interaction;
  else if (enabled && !mInteractions.testFlag(interaction))
    mInteractions |= interaction;
}

/*!
  Sets the tolerance that is used when deciding whether a click on the QCustomPlot surface selects
  an object (e.g. a plottable) or not.
  
  If for example the user clicks in the vicinity of the line of a QCPGraph, it's only regarded as a
  potential selection when the minimum distance between the click position and the graph line is
  smaller than \a pixels. Objects that are defined by an area (e.g. QCPBars) only react to clicks
  directly inside the area and ignore this selection tolerance. In other words it only has meaning
  for parts of objects that are too thin to exactly hit with a click and thus need such a
  tolerance.
  
  \see setInteractions, QCPAbstractPlottable::selectTest
*/
void QCustomPlot::setSelectionTolerance(int pixels)
{
  mSelectionTolerance = pixels;
}

/*!
  This \a font is used to draw the title, when it is selected.
  
  \see setTitleSelected, setTitleFont
*/
void QCustomPlot::setSelectedTitleFont(const QFont &font)
{
  mSelectedTitleFont = font;
}

/*!
  This \a color is used to draw the title, when it is selected.
  
  \see setTitleSelected, setTitleColor
*/
void QCustomPlot::setSelectedTitleColor(const QColor &color)
{
  mSelectedTitleColor = color;
}

/*!
  Sets whether the plot title is selected.
  
  \see setInteractions, setSelectedTitleFont, setSelectedTitleColor, setTitle
*/
void QCustomPlot::setTitleSelected(bool selected)
{
  mTitleSelected = selected;
}

/*!
  Sets whether antialiasing is disabled for all elements while the user is dragging axes ranges. If
  many objects, especially plottables, are normally drawn antialiased, this greatly improves
  performance during dragging. Thus it creates a more responsive user experience. As soon as the
  user stops dragging, the last replot is done with normal antialiasing, to restore high image
  quality.
  
  \see setAntialiasedElements, setNotAntialiasedElements
*/
void QCustomPlot::setNoAntialiasingOnDrag(bool enabled)
{
  mNoAntialiasingOnDrag = enabled;
}

/*!
  Sets the plotting hints for this QCustomPlot instance.
  \see setPlottingHint
*/
void QCustomPlot::setPlottingHints(const QCP::PlottingHints &hints)
{
  mPlottingHints = hints;
}

/*!
  Sets the specified plotting \a hint to \a enabled.
  \see setPlottingHints
*/
void QCustomPlot::setPlottingHint(QCP::PlottingHint hint, bool enabled)
{
  QCP::PlottingHints newHints = mPlottingHints;
  if (!enabled)
    newHints &= ~hint;
  else
    newHints |= hint;
  
  if (newHints != mPlottingHints)
    setPlottingHints(newHints);
}

/*!
  Sets the keyboard modifier that will be recognized as multi-select-modifier.
  
  If \ref iMultiSelect is specified in \ref setInteractions, the user may select multiple objects
  by clicking on them one after the other while holding down \a modifier.
  
  By default the multi-select-modifier is set to Qt::ControlModifier.
  
  \see setInteractions
*/
void QCustomPlot::setMultiSelectModifier(Qt::KeyboardModifier modifier)
{
  mMultiSelectModifier = modifier;
}

/*!
  Returns the plottable with \a index. If the index is invalid, returns 0.
  
  There is an overloaded version of this function with no parameter which returns the last added
  plottable, see QCustomPlot::plottable()
  
  \see plottableCount, addPlottable
*/
QCPAbstractPlottable *QCustomPlot::plottable(int index)
{
  if (index >= 0 && index < mPlottables.size())
  {
    return mPlottables.at(index);
  } else
  {
    qDebug() << Q_FUNC_INFO << "index out of bounds:" << index;
    return 0;
  }
}

/*! \overload
  
  Returns the last plottable, that was added with \ref addPlottable. If there are no plottables in the plot,
  returns 0.
  
  \see plottableCount, addPlottable
*/
QCPAbstractPlottable *QCustomPlot::plottable()
{
  if (!mPlottables.isEmpty())
  {
    return mPlottables.last();
  } else
    return 0;
}

/*!
  Adds the specified plottable to the plot and, if \ref setAutoAddPlottableToLegend is enabled, to the legend.
  QCustomPlot takes ownership of the plottable.
  
  Returns true on success, i.e. when \a plottable wasn't already added to the plot and
  the parent plot of \a plottable is this QCustomPlot (the latter is controlled by what
  axes the plottable was passed in the constructor).
  
  \see plottable, plottableCount, removePlottable, clearPlottables
*/
bool QCustomPlot::addPlottable(QCPAbstractPlottable *plottable)
{
  if (mPlottables.contains(plottable))
  {
    qDebug() << Q_FUNC_INFO << "plottable already added to this QCustomPlot:" << reinterpret_cast<quintptr>(plottable);
    return false;
  }
  if (plottable->parentPlot() != this)
  {
    qDebug() << Q_FUNC_INFO << "plottable not created with this QCustomPlot as parent:" << reinterpret_cast<quintptr>(plottable);
    return false;
  }
  
  mPlottables.append(plottable);
  // possibly add plottable to legend:
  if (mAutoAddPlottableToLegend)
    plottable->addToLegend();
  // special handling for QCPGraphs to maintain the simple graph interface:
  if (QCPGraph *graph = qobject_cast<QCPGraph*>(plottable))
    mGraphs.append(graph);
  if (!plottable->layer()) // usually the layer is already set in the constructor of the plottable (via QCPLayerable constructor)
    plottable->setLayer(currentLayer());
  return true;
}

/*!
  Removes the specified plottable from the plot and, if necessary, from the legend.
  
  Returns true on success.
  
  \see addPlottable, clearPlottables
*/
bool QCustomPlot::removePlottable(QCPAbstractPlottable *plottable)
{
  if (!mPlottables.contains(plottable))
  {
    qDebug() << Q_FUNC_INFO << "plottable not in list:" << reinterpret_cast<quintptr>(plottable);
    return false;
  }
  
  // remove plottable from legend:
  plottable->removeFromLegend();
  // special handling for QCPGraphs to maintain the simple graph interface:
  if (QCPGraph *graph = qobject_cast<QCPGraph*>(plottable))
    mGraphs.removeOne(graph);
  // remove plottable:
  delete plottable;
  mPlottables.removeOne(plottable);
  return true;
}

/*! \overload
  
  Removes the plottable by its \a index.
*/
bool QCustomPlot::removePlottable(int index)
{
  if (index >= 0 && index < mPlottables.size())
    return removePlottable(mPlottables[index]);
  else
  {
    qDebug() << Q_FUNC_INFO << "index out of bounds:" << index;
    return false;
  }
}

/*!
  Removes all plottables from the plot (and the legend, if necessary).
  
  Returns the number of plottables removed.
  
  \see removePlottable
*/
int QCustomPlot::clearPlottables()
{
  int c = mPlottables.size();
  for (int i=c-1; i >= 0; --i)
    removePlottable(mPlottables[i]);
  return c;
}

/*!
  Returns the number of currently existing plottables in the plot
  
  \see plottable, addPlottable
*/
int QCustomPlot::plottableCount() const
{
  return mPlottables.size();
}

/*!
  Returns a list of the selected plottables. If no plottables are currently selected, the list is empty.
  
  There is a convenience function if you're only interested in selected graphs, see \ref selectedGraphs.
  
  \see setInteractions, QCPAbstractPlottable::setSelectable, QCPAbstractPlottable::setSelected, selectedGraphs
*/
QList<QCPAbstractPlottable*> QCustomPlot::selectedPlottables() const
{
  QList<QCPAbstractPlottable*> result;
  for (int i=0; i<mPlottables.size(); ++i)
  {
    if (mPlottables.at(i)->selected())
      result.append(mPlottables.at(i));
  }
  return result;
}

/*!
  Returns the plottable at the pixel position \a pos. Plottables that only consist of single lines
  (e.g. graphs) have a tolerance band around them, see \ref setSelectionTolerance.
  If multiple plottables come into consideration, the one closest to \a pos is returned.
  
  If \a onlySelectable is true, only plottables that are selectable
  (QCPAbstractPlottable::setSelectable) are considered.
  
  If there is no plottable at \a pos, the return value is 0.
*/
QCPAbstractPlottable *QCustomPlot::plottableAt(const QPointF &pos, bool onlySelectable) const
{
  QCPAbstractPlottable *resultPlottable = 0;
  double resultDistance = mSelectionTolerance; // only regard clicks with distances smaller than mSelectionTolerance as selections, so initialize with that value
  
  for (int i=0; i<mPlottables.size(); ++i)
  {
    QCPAbstractPlottable *currentPlottable = mPlottables[i];
    if (onlySelectable && !currentPlottable->selectable())
      continue;
    if ((currentPlottable->keyAxis()->axisRect() | currentPlottable->valueAxis()->axisRect()).contains(pos.toPoint())) // only consider clicks inside the rect that is spanned by the plottable's key/value axes
    {
      double currentDistance = currentPlottable->selectTest(pos);
      if (currentDistance >= 0 && currentDistance < resultDistance)
      {
        resultPlottable = currentPlottable;
        resultDistance = currentDistance;
      }
    }
  }
  
  return resultPlottable;
}

/*!
  Returns whether this QCustomPlot instance contains the \a plottable.
  
  \see addPlottable
*/
bool QCustomPlot::hasPlottable(QCPAbstractPlottable *plottable) const
{
  return mPlottables.contains(plottable);
}

/*!
  Returns the graph with \a index. If the index is invalid, returns 0.
  
  There is an overloaded version of this function with no parameter which returns the last created
  graph, see QCustomPlot::graph()
  
  \see graphCount, addGraph
*/
QCPGraph *QCustomPlot::graph(int index) const
{
  if (index >= 0 && index < mGraphs.size())
  {
    return mGraphs.at(index);
  } else
  {
    qDebug() << Q_FUNC_INFO << "index out of bounds:" << index;
    return 0;
  }
}

/*! \overload
  
  Returns the last graph, that was created with \ref addGraph. If there are no graphs in the plot,
  returns 0.
  
  \see graphCount, addGraph
*/
QCPGraph *QCustomPlot::graph() const
{
  if (!mGraphs.isEmpty())
  {
    return mGraphs.last();
  } else
    return 0;
}

/*!
  Creates a new graph inside the plot. If \a keyAxis and \a valueAxis are left unspecified (0), the
  bottom (xAxis) is used as key and the left (yAxis) is used as value. If specified, \a keyAxis and
  \a valueAxis must reside in this QCustomPlot.
  
  \a keyAxis will be used as key axis (typically "x") and \a valueAxis as value axis (typically
  "y") for the graph.
  
  Returns a pointer to the newly created graph.
  
  \see graph, graphCount, removeGraph, clearGraphs
*/
QCPGraph *QCustomPlot::addGraph(QCPAxis *keyAxis, QCPAxis *valueAxis)
{
  if (!keyAxis) keyAxis = xAxis;
  if (!valueAxis) valueAxis = yAxis;
  if (keyAxis->parentPlot() != this || valueAxis->parentPlot() != this)
  {
    qDebug() << Q_FUNC_INFO << "passed keyAxis or valueAxis doesn't have this QCustomPlot as parent";
    return 0;
  }
  
  QCPGraph *newGraph = new QCPGraph(keyAxis, valueAxis);
  if (addPlottable(newGraph))
  {
    newGraph->setName("Graph "+QString::number(mGraphs.size()));
    return newGraph;
  } else
  {
    delete newGraph;
    return 0;
  }
}

/*!
  Removes the specified \a graph from the plot and, if necessary, from the legend. If
  any other graphs in the plot have a channel fill set towards the removed graph, the channel fill
  property of those graphs is reset to zero (no channel fill).
  
  Returns true on success.
  
  \see clearGraphs
*/
bool QCustomPlot::removeGraph(QCPGraph *graph)
{
  return removePlottable(graph);
}

/*! \overload
  
  Removes the graph by its \a index.
*/
bool QCustomPlot::removeGraph(int index)
{
  if (index >= 0 && index < mGraphs.size())
    return removeGraph(mGraphs[index]);
  else
    return false;
}

/*!
  Removes all graphs from the plot (and the legend, if necessary).
  Returns the number of graphs removed.
  \see removeGraph
*/
int QCustomPlot::clearGraphs()
{
  int c = mGraphs.size();
  for (int i=c-1; i >= 0; --i)
    removeGraph(mGraphs[i]);
  return c;
}

/*!
  Returns the number of currently existing graphs in the plot
  
  \see graph, addGraph
*/
int QCustomPlot::graphCount() const
{
  return mGraphs.size();
}

/*!
  Returns a list of the selected graphs. If no graphs are currently selected, the list is empty.
  
  \note Even if the returned list is empty, it might still be, that there are selected plottables
  in the plot that are not of type QCPGraph (e.g. QCPCurve, QCPBars, etc.), see \ref
  selectedPlottables. Of course, this only applies, if you actually add non-QCPGraph plottables.
  
  \see setInteractions, selectedPlottables, QCPAbstractPlottable::setSelectable, QCPAbstractPlottable::setSelected
*/
QList<QCPGraph*> QCustomPlot::selectedGraphs() const
{
  QList<QCPGraph*> result;
  for (int i=0; i<mGraphs.size(); ++i)
  {
    if (mGraphs.at(i)->selected())
      result.append(mGraphs.at(i));
  }
  return result;
}

/*!
  Returns the item with \a index. If the index is invalid, returns 0.
  
  There is an overloaded version of this function with no parameter which returns the last added
  item, see QCustomPlot::item()
  
  \see itemCount, addItem
*/
QCPAbstractItem *QCustomPlot::item(int index) const
{
  if (index >= 0 && index < mItems.size())
  {
    return mItems.at(index);
  } else
  {
    qDebug() << Q_FUNC_INFO << "index out of bounds:" << index;
    return 0;
  }
}

/*! \overload
  
  Returns the last item, that was added with \ref addItem. If there are no items in the plot,
  returns 0.
  
  \see itemCount, addItem
*/
QCPAbstractItem *QCustomPlot::item() const
{
  if (!mItems.isEmpty())
  {
    return mItems.last();
  } else
    return 0;
}

/*!
  Adds the specified item to the plot. QCustomPlot takes ownership of the item.
  
  Returns true on success, i.e. when \a item wasn't already added to the plot and the parent plot
  of \a item is this QCustomPlot.
  
  \see item, itemCount, removeItem, clearItems
*/
bool QCustomPlot::addItem(QCPAbstractItem *item)
{
  if (!mItems.contains(item) && item->parentPlot() == this)
  {
    mItems.append(item);
    return true;
  } else
  {
    qDebug() << Q_FUNC_INFO << "item either already in list or not created with this QCustomPlot as parent:" << reinterpret_cast<quintptr>(item);
    return false;
  }
}

/*!
  Removes the specified item from the plot.
  
  Returns true on success.
  
  \see addItem, clearItems
*/
bool QCustomPlot::removeItem(QCPAbstractItem *item)
{
  if (mItems.contains(item))
  {
    delete item;
    mItems.removeOne(item);
    return true;
  } else
  {
    qDebug() << Q_FUNC_INFO << "item not in list:" << reinterpret_cast<quintptr>(item);
    return false;
  }
}

/*! \overload
  
  Removes the item by its \a index.
*/
bool QCustomPlot::removeItem(int index)
{
  if (index >= 0 && index < mItems.size())
    return removeItem(mItems[index]);
  else
  {
    qDebug() << Q_FUNC_INFO << "index out of bounds:" << index;
    return false;
  }
}

/*!
  Removes all items from the plot.
  
  Returns the number of items removed.
  
  \see removeItem
*/
int QCustomPlot::clearItems()
{
  int c = mItems.size();
  for (int i=c-1; i >= 0; --i)
    removeItem(mItems[i]);
  return c;
}

/*!
  Returns the number of currently existing items in the plot
  
  \see item, addItem
*/
int QCustomPlot::itemCount() const
{
  return mItems.size();
}

/*!
  Returns a list of the selected items. If no items are currently selected, the list is empty.
  
  \see setInteractions, QCPAbstractItem::setSelectable, QCPAbstractItem::setSelected
*/
QList<QCPAbstractItem*> QCustomPlot::selectedItems() const
{
  QList<QCPAbstractItem*> result;
  for (int i=0; i<mItems.size(); ++i)
  {
    if (mItems.at(i)->selected())
      result.append(mItems.at(i));
  }
  return result;
}

/*!
  Returns the item at the pixel position \a pos. Items that only consist of single lines (e.g. \ref
  QCPItemLine or \ref QCPItemCurve) have a tolerance band around them, see \ref
  setSelectionTolerance. If multiple items come into consideration, the one closest to \a pos is
  returned.
  
  If \a onlySelectable is true, only items that are selectable (QCPAbstractItem::setSelectable) are
  considered.
  
  If there is no item at \a pos, the return value is 0.
*/
QCPAbstractItem *QCustomPlot::itemAt(const QPointF &pos, bool onlySelectable) const
{
  QCPAbstractItem *resultItem = 0;
  double resultDistance = mSelectionTolerance; // only regard clicks with distances smaller than mSelectionTolerance as selections, so initialize with that value
  
  for (int i=0; i<mItems.size(); ++i)
  {
    QCPAbstractItem *currentItem = mItems[i];
    if (onlySelectable && !currentItem->selectable())
      continue;
    if (!currentItem->clipToAxisRect() || currentItem->clipRect().contains(pos.toPoint())) // only consider clicks inside axis cliprect of the item if actually clipped to it
    {
      double currentDistance = currentItem->selectTest(pos);
      if (currentDistance >= 0 && currentDistance < resultDistance)
      {
        resultItem = currentItem;
        resultDistance = currentDistance;
      }
    }
  }
  
  return resultItem;
}

/*!
  Returns the layer with the specified \a name.
  
  \see addLayer, moveLayer, removeLayer
*/
QCPLayer *QCustomPlot::layer(const QString &name) const
{
  for (int i=0; i<mLayers.size(); ++i)
  {
    if (mLayers.at(i)->name() == name)
      return mLayers.at(i);
  }
  return 0;
}

/*! \overload
  
  Returns the layer by index.
  
  \see addLayer, moveLayer, removeLayer
*/
QCPLayer *QCustomPlot::layer(int index) const
{
  if (index >= 0 && index < mLayers.size())
  {
    return mLayers.at(index);
  } else
  {
    qDebug() << Q_FUNC_INFO << "index out of bounds:" << index;
    return 0;
  }
}

/*!
  Returns the layer that is set as current layer (see \ref setCurrentLayer).
*/
QCPLayer *QCustomPlot::currentLayer() const
{
  return mCurrentLayer; 
}

/*!
  Sets the layer with the specified \a name to be the current layer. All newly created/added
  layerables (\ref QCPLayerable), e.g. plottables and items, are initially placed on the current
  layer.
  
  Returns true on success, i.e. if there is a layer with the specified \a name in the QCustomPlot.
  
  \see addLayer, moveLayer, removeLayer
*/
bool QCustomPlot::setCurrentLayer(const QString &name)
{
  if (QCPLayer *newCurrentLayer = layer(name))
  {
    return setCurrentLayer(newCurrentLayer);
  } else
  {
    qDebug() << Q_FUNC_INFO << "layer with name doesn't exist:" << name;
    return false;
  }
}

/*! \overload
  
  Sets the provided \a layer to be the current layer.
  
  Returns true on success, i.e. when \a layer is a valid layer in the QCustomPlot.
  
  \see addLayer, moveLayer, removeLayer
*/
bool QCustomPlot::setCurrentLayer(QCPLayer *layer)
{
  if (!mLayers.contains(layer))
  {
    qDebug() << Q_FUNC_INFO << "layer not a layer of this QCustomPlot:" << reinterpret_cast<quintptr>(layer);
    return false;
  }
  
  mCurrentLayer = layer;
  return true;
}

/*!
  Returns the number of currently existing layers in the plot
  
  \see layer, addLayer
*/
int QCustomPlot::layerCount() const
{
  return mLayers.size();
}

/*!
  Adds a new layer to this QCustomPlot instance. The new layer will have the name \a name, which must
  be unique. It is positioned either below or above \a otherLayer, which can be controlled with \a insertMode.
  
  Returns true on success, i.e. if there is no other layer named \a name and \a otherLayer is a
  valid layer inside this QCustomPlot.
  
  If \a otherLayer is 0, the highest layer in the QCustomPlot will be used.
  
  For an explanation of what layers are in QCustomPlot, see the documentation of \ref QCPLayer.
  
  \see layer, moveLayer, removeLayer
*/
bool QCustomPlot::addLayer(const QString &name, QCPLayer *otherLayer, QCustomPlot::LayerInsertMode insertMode)
{
  if (!otherLayer)
    otherLayer = mLayers.last();
  if (!mLayers.contains(otherLayer))
  {
    qDebug() << Q_FUNC_INFO << "otherLayer not a layer of this QCustomPlot:" << reinterpret_cast<quintptr>(otherLayer);
    return false;
  }
  if (layer(name))
  {
    qDebug() << Q_FUNC_INFO << "A layer exists already with the name" << name;
    return false;
  }
    
  QCPLayer *newLayer = new QCPLayer(this, name);
  mLayers.insert(otherLayer->index() + (insertMode==limAbove ? 1:0), newLayer);
  return true;
}

/*!
  Removes the specified \a layer and returns true on success.
  
  All layerables (e.g. plottables and items) on the removed layer will be moved to the layer below
  \a layer. If \a layer is the bottom layer, the layerables are moved to the layer above. In both
  cases, the total rendering order of all layerables in the QCustomPlot is preserved.
  
  If \a layer is the current layer (\ref setCurrentLayer), the layer below (or above, if bottom
  layer) becomes the new current layer.
  
  Note that it is not possible to remove the last layer.
  
  \see layer, addLayer, moveLayer
*/
bool QCustomPlot::removeLayer(QCPLayer *layer)
{
  if (!mLayers.contains(layer))
  {
    qDebug() << Q_FUNC_INFO << "layer not a layer of this QCustomPlot:" << reinterpret_cast<quintptr>(layer);
    return false;
  }
  if (mLayers.size() <= 1)
  {
    qDebug() << Q_FUNC_INFO << "can't remove last layer";
    return false;
  }
  
  // append all children of this layer to layer below (if this is lowest layer, prepend to layer above)
  int removedIndex = layer->index();
  bool isFirstLayer = removedIndex==0;
  QCPLayer *targetLayer = isFirstLayer ? mLayers.at(removedIndex+1) : mLayers.at(removedIndex-1);
  QList<QCPLayerable*> children = layer->children();
  if (isFirstLayer) // prepend in reverse order (so order relative to each other stays the same)
  {
    for (int i=children.size()-1; i>=0; --i)
      children.at(i)->moveToLayer(targetLayer, true);
  } else  // append normally
  {
    for (int i=0; i<children.size(); ++i)
      children.at(i)->moveToLayer(targetLayer, false);
  }
  // if removed layer is current layer, change current layer to layer below/above:
  if (layer == mCurrentLayer)
    setCurrentLayer(targetLayer);
  // remove layer:
  delete layer;
  mLayers.removeOne(layer);
  return true;
}

/*!
  Moves the specified \a layer to the position relative to \a otherLayer. Whether \a layer is
  placed above or below \a otherLayer can be controlled with \a insertMode.
  
  Returns true on success, i.e. when both \a layer and \a otherLayer are valid layers in the
  QCustomPlot.
  
  \see layer, addLayer, moveLayer
*/
bool QCustomPlot::moveLayer(QCPLayer *layer, QCPLayer *otherLayer, QCustomPlot::LayerInsertMode insertMode)
{
  if (!mLayers.contains(layer))
  {
    qDebug() << Q_FUNC_INFO << "layer not a layer of this QCustomPlot:" << reinterpret_cast<quintptr>(layer);
    return false;
  }
  if (!mLayers.contains(otherLayer))
  {
    qDebug() << Q_FUNC_INFO << "otherLayer not a layer of this QCustomPlot:" << reinterpret_cast<quintptr>(otherLayer);
    return false;
  }
  
  mLayers.move(layer->index(), otherLayer->index() + (insertMode==limAbove ? 1:0));
  return true;
}

/*!
  Returns the axes that currently have selected parts, i.e. whose selection is not \ref QCPAxis::spNone.
  
  \see selectedPlottables, selectedLegends, setInteractions, QCPAxis::setSelected, QCPAxis::setSelectable
*/
QList<QCPAxis*> QCustomPlot::selectedAxes() const
{
  QList<QCPAxis*> result = QList<QCPAxis*>() << xAxis << yAxis << xAxis2 << yAxis2;
  for (int i=result.size()-1; i>=0; --i)
  {
    if (result.at(i)->selected() == QCPAxis::spNone)
      result.removeAt(i);
  }
  return result;
}

/*!
  Returns the legends (typically one or zero) that currently have selected parts, i.e. whose
  selection is not \ref QCPLegend::spNone.
  
  \see selectedPlottables, selectedAxes, setInteractions, QCPLegend::setSelected, QCPLegend::setSelectable, QCPLegend::selectedItems
*/
QList<QCPLegend*> QCustomPlot::selectedLegends() const
{
  /* for now, we only have the one legend. Maybe later, there will be a mechanism to have more. */
  QList<QCPLegend*> result;
  if (legend->selected() != QCPLegend::spNone)
    result.append(legend);
  return result;
}

/*!
  Deselects everything in the QCustomPlot (plottables, items, axes, legend and title).
  
  Since calling this function is not a user interaction, this does not emit the \ref
  selectionChangedByUser signal. The individual selectionChanged signals are emitted though, if the
  objects were previously selected.
  
  \see setInteractions, selectedPlottables, selectedItems, selectedAxes, selectedLegends
*/
void QCustomPlot::deselectAll()
{
  // deselect plottables:
  QList<QCPAbstractPlottable*> selPlottables = selectedPlottables();
  for (int i=0; i<selPlottables.size(); ++i)
    selPlottables.at(i)->setSelected(false);
  
  // deselect items:
  QList<QCPAbstractItem*> selItems = selectedItems();
  for (int i=0; i<selItems.size(); ++i)
    selItems.at(i)->setSelected(false);
  
  // deselect axes:
  QList<QCPAxis*> selAxes = selectedAxes();
  for (int i=0; i<selAxes.size(); ++i)
    selAxes.at(i)->setSelected(QCPAxis::spNone);
  
  // deselect legend (and legend items):
  legend->setSelected(QCPLegend::spNone);
  
  // deselect title:
  setTitleSelected(false);
}

/*!
  Causes a complete replot (axes, labels, graphs, etc.) into the internal buffer. Finally, update()
  is called, to redraw the buffer on the QCustomPlot widget surface.
  
  Before the replot happens, the signal \ref beforeReplot is emitted. After the replot, \ref afterReplot is
  emitted. It is safe to mutually connect the replot slot with any of those two signals on two QCustomPlots
  to make them replot synchronously (i.e. it won't cause an infinite recursion).
*/
void QCustomPlot::replot()
{
  if (mReplotting) // incase signals loop back to replot slot
    return;
  mReplotting = true;
  emit beforeReplot();
  mPaintBuffer.fill(mColor);
  QCPPainter painter;
  painter.begin(&mPaintBuffer);
  if (painter.isActive()) 
  {
    painter.setRenderHint(QPainter::HighQualityAntialiasing);
    draw(&painter);
    if (mPlottingHints.testFlag(QCP::phForceRepaint))
      repaint();
    else
      update();
    painter.end();
  } else // might happen if QCustomPlot has width or height zero
    qDebug() << Q_FUNC_INFO << "Couldn't activate painter on buffer";
  emit afterReplot();
  mReplotting = false;
}

/*!
  Convenience function to make the top and right axes visible and assign them the following
  properties from their corresponding bottom/left axes:
  
  \li range (\ref QCPAxis::setRange)
  \li range reversed (\ref QCPAxis::setRangeReversed)
  \li scale type (\ref QCPAxis::setScaleType)
  \li scale log base  (\ref QCPAxis::setScaleLogBase)
  \li ticks (\ref QCPAxis::setTicks)
  \li auto (major) tick count (\ref QCPAxis::setAutoTickCount)
  \li sub tick count (\ref QCPAxis::setSubTickCount)
  \li auto sub ticks (\ref QCPAxis::setAutoSubTicks)
  \li tick step (\ref QCPAxis::setTickStep)
  \li auto tick step (\ref QCPAxis::setAutoTickStep)
  
  Tick labels (\ref QCPAxis::setTickLabels) however, is always set to false.

  This function does \a not connect the rangeChanged signals of the bottom and left axes to the \ref
  QCPAxis::setRange slots of the top and right axes in order to synchronize the ranges permanently.
*/
void QCustomPlot::setupFullAxesBox()
{
  xAxis2->setVisible(true);
  yAxis2->setVisible(true);
  
  xAxis2->setTickLabels(false);
  yAxis2->setTickLabels(false);
  
  xAxis2->setAutoSubTicks(xAxis->autoSubTicks());
  yAxis2->setAutoSubTicks(yAxis->autoSubTicks());
  
  xAxis2->setAutoTickCount(xAxis->autoTickCount());
  yAxis2->setAutoTickCount(yAxis->autoTickCount());
  
  xAxis2->setAutoTickStep(xAxis->autoTickStep());
  yAxis2->setAutoTickStep(yAxis->autoTickStep());
  
  xAxis2->setScaleType(xAxis->scaleType());
  yAxis2->setScaleType(yAxis->scaleType());
  
  xAxis2->setScaleLogBase(xAxis->scaleLogBase());
  yAxis2->setScaleLogBase(yAxis->scaleLogBase());
  
  xAxis2->setTicks(xAxis->ticks());
  yAxis2->setTicks(yAxis->ticks());
  
  xAxis2->setSubTickCount(xAxis->subTickCount());
  yAxis2->setSubTickCount(yAxis->subTickCount());
  
  xAxis2->setTickStep(xAxis->tickStep());
  yAxis2->setTickStep(yAxis->tickStep());
  
  xAxis2->setRange(xAxis->range());
  yAxis2->setRange(yAxis->range());
  
  xAxis2->setRangeReversed(xAxis->rangeReversed());
  yAxis2->setRangeReversed(yAxis->rangeReversed());
}

/*!
  Rescales the axes such that all plottables (e.g. graphs) in the plot are fully visible.
  It does this by calling \ref QCPAbstractPlottable::rescaleAxes on all plottables.
  
  \see QCPAbstractPlottable::rescaleAxes
*/
void QCustomPlot::rescaleAxes()
{
  if (mPlottables.isEmpty()) return;
  
  mPlottables.at(0)->rescaleAxes(false); // onlyEnlarge disabled on first plottable
  for (int i=1; i<mPlottables.size(); ++i)
    mPlottables.at(i)->rescaleAxes(true);  // onlyEnlarge enabled on all other plottables
}

/*!
  Saves a PDF with the vectorized plot to the file \a fileName. The axis ratio as well as the scale
  of texts and lines will be derived from the specified \a width and \a height. This means, the
  output will look like the normal on-screen output of a QCustomPlot widget with the corresponding
  pixel width and height. If either \a width or \a height is zero, the exported image will have
  the same dimensions as the QCustomPlot widget currently has.

  \a noCosmeticPen disables the use of cosmetic pens when drawing to the PDF file. Cosmetic pens
  are pens with numerical width 0, which are always drawn as a one pixel wide line, no matter what
  zoom factor is set in the PDF-Viewer. For more information about cosmetic pens, see QPainter and
  QPen documentation.
  
  The objects of the plot will appear in the current selection state. So when you don't want e.g.
  selected axes to be painted in their selected look, deselect everything with \ref deselectAll
  before calling this function.

  Returns true on success.
  
  \warning
  \li If you plan on editing the exported PDF file with a vector graphics editor like
  Inkscape, it is advised to set \a noCosmeticPen to true to avoid losing those cosmetic lines
  (which might be quite many, because cosmetic pens are the default for e.g. axes and tick marks).
  \li If calling this function inside the constructor of the parent of the QCustomPlot widget
  (i.e. the MainWindow constructor, if QCustomPlot is inside the MainWindow), always provide
  explicit non-zero widths and heights. If you leave \a width or \a height as 0 (default), this
  function uses the current width and height of the QCustomPlot widget. However, in Qt, these
  aren't defined yet inside the constructor, so you would get an image that has strange
  widths/heights.
  
  \see savePng, saveBmp, saveJpg, saveRastered
*/
bool QCustomPlot::savePdf(const QString &fileName, bool noCosmeticPen, int width, int height)
{
  bool success = false;
  int newWidth, newHeight;
  if (width == 0 || height == 0)
  {
    newWidth = this->width();
    newHeight = this->height();
  } else
  {
    newWidth = width;
    newHeight = height;
  }
  
  QPrinter printer(QPrinter::ScreenResolution);
  printer.setOutputFileName(fileName);
  printer.setFullPage(true);
  QRect oldViewport = mViewport;
  mViewport = QRect(0, 0, newWidth, newHeight);
  updateAxisRect();
  printer.setPaperSize(mViewport.size(), QPrinter::DevicePixel);
  QCPPainter printpainter;
  if (printpainter.begin(&printer))
  {
    printpainter.setPdfExportMode(true);
    printpainter.setWindow(mViewport);
    printpainter.setRenderHint(QPainter::NonCosmeticDefaultPen, noCosmeticPen);
    if (mColor != Qt::white && mColor != Qt::transparent && mColor.alpha() > 0) // draw pdf background color if not white/transparent
      printpainter.fillRect(mViewport, mColor);
    draw(&printpainter);
    printpainter.end();
    success = true;
  }
  mViewport = oldViewport;
  updateAxisRect();
  return success;
}

/*!
  Saves a PNG image file to \a fileName on disc. The output plot will have the dimensions \a width
  and \a height in pixels. If either \a width or \a height is zero, the exported image will have
  the same dimensions as the QCustomPlot widget currently has. Line widths and texts etc. are not
  scaled up when larger widths/heights are used. If you want that effect, use the \a scale parameter.

  For example, if you set both \a width and \a height to 100 and \a scale to 2, you will end up with an
  image file of size 200*200 in which all graphical elements are scaled up by factor 2 (line widths,
  texts, etc.). This scaling is not done by stretching a 100*100 image, the result will have full
  200*200 pixel resolution.

  \warning If calling this function inside the constructor of the parent of the QCustomPlot widget
  (i.e. the MainWindow constructor, if QCustomPlot is inside the MainWindow), always provide
  explicit non-zero widths and heights. If you leave \a width or \a height as 0 (default), this
  function uses the current width and height of the QCustomPlot widget. However, in Qt, these
  aren't defined yet inside the constructor, so you would get an image that has strange
  widths/heights.
  
  The objects of the plot will appear in the current selection state. If you don't want any selected
  objects to be painted in their selected look, deselect everything with \ref deselectAll before calling
  this function.

  If you want the plot to be painted in a PNG with transparent background, call \ref setColor with a
  transparent color, e.g. Qt::transparent, before saving.

  PNG compression can be controlled with the \a quality parameter which must be between 0 and 100 or
  -1 to use the default setting.
  
  Returns true on success. If this function fails, most likely the PNG format isn't supported by
  the system, see Qt docs about QImageWriter::supportedImageFormats().

  \see savePdf, saveBmp, saveJpg, saveRastered
*/
bool QCustomPlot::savePng(const QString &fileName, int width, int height, double scale, int quality)
{  
  return saveRastered(fileName, width, height, scale, "PNG", quality);
}

/*!
  Saves a JPG image file to \a fileName on disc. The output plot will have the dimensions \a width
  and \a height in pixels. If either \a width or \a height is zero, the exported image will have
  the same dimensions as the QCustomPlot widget currently has. Line widths and texts etc. are not
  scaled up when larger widths/heights are used. If you want that effect, use the \a scale parameter.

  For example, if you set both \a width and \a height to 100 and \a scale to 2, you will end up with an
  image file of size 200*200 in which all graphical elements are scaled up by factor 2 (line widths,
  texts, etc.). This scaling is not done by stretching a 100*100 image, the result will have full
  200*200 pixel resolution.

  \warning If calling this function inside the constructor of the parent of the QCustomPlot widget
  (i.e. the MainWindow constructor, if QCustomPlot is inside the MainWindow), always provide
  explicit non-zero widths and heights. If you leave \a width or \a height as 0 (default), this
  function uses the current width and height of the QCustomPlot widget. However, in Qt, these
  aren't defined yet inside the constructor, so you would get an image that has strange
  widths/heights.

  The objects of the plot will appear in the current selection state. If you don't want any selected
  objects to be painted in their selected look, deselect everything with \ref deselectAll before calling
  this function.

  JPG compression can be controlled with the \a quality parameter which must be between 0 and 100 or
  -1 to use the default setting.
  
  Returns true on success. If this function fails, most likely the JPG format isn't supported by
  the system, see Qt docs about QImageWriter::supportedImageFormats().

  \see savePdf, savePng, saveBmp, saveRastered
*/
bool QCustomPlot::saveJpg(const QString &fileName, int width, int height, double scale, int quality)
{
  return saveRastered(fileName, width, height, scale, "JPG", quality);
}

/*!
  Saves a BMP image file to \a fileName on disc. The output plot will have the dimensions \a width
  and \a height in pixels. If either \a width or \a height is zero, the exported image will have
  the same dimensions as the QCustomPlot widget currently has. Line widths and texts etc. are not
  scaled up when larger widths/heights are used. If you want that effect, use the \a scale parameter.

  For example, if you set both \a width and \a height to 100 and \a scale to 2, you will end up with an
  image file of size 200*200 in which all graphical elements are scaled up by factor 2 (line widths,
  texts, etc.). This scaling is not done by stretching a 100*100 image, the result will have full
  200*200 pixel resolution.

  \warning If calling this function inside the constructor of the parent of the QCustomPlot widget
  (i.e. the MainWindow constructor, if QCustomPlot is inside the MainWindow), always provide
  explicit non-zero widths and heights. If you leave \a width or \a height as 0 (default), this
  function uses the current width and height of the QCustomPlot widget. However, in Qt, these
  aren't defined yet inside the constructor, so you would get an image that has strange
  widths/heights.

  The objects of the plot will appear in the current selection state. If you don't want any selected
  objects to be painted in their selected look, deselect everything with \ref deselectAll before calling
  this function.
  
  Returns true on success. If this function fails, most likely the BMP format isn't supported by
  the system, see Qt docs about QImageWriter::supportedImageFormats().

  \see savePdf, savePng, saveJpg, saveRastered
*/
bool QCustomPlot::saveBmp(const QString &fileName, int width, int height, double scale)
{
  return saveRastered(fileName, width, height, scale, "BMP");
}

/*! \internal
  
  Returns a minimum size hint of QSize(50, 50). This prevents QCustomPlot from being collapsed to
  size/width zero when placed in a layout where other components try to take in as much space as
  possible (e.g. QMdiArea).

  (To overwrite this minimum size hint of QCustomPlot, simply call QWidget::setMinimumSize in the
  QCustomPlot widget.)
*/
QSize QCustomPlot::minimumSizeHint() const
{
  return QSize(50, 50);
}

/*! \internal
  
  Event handler for when the QCustomPlot widget needs repainting. This does not cause a replot, but
  draws the internal buffer on the widget surface.
*/
void QCustomPlot::paintEvent(QPaintEvent *event)
{
  Q_UNUSED(event);
  QPainter painter(this);
  painter.drawPixmap(0, 0, mPaintBuffer);
}

/*! \internal
  
  Event handler for a resize of the QCustomPlot widget. Causes the internal buffer to be resized to
  the new size. The viewport and the axis rect are resized appropriately. Finally a replot is
  performed.
*/
void QCustomPlot::resizeEvent(QResizeEvent *event)
{
  // resize and repaint the buffer:
  mPaintBuffer = QPixmap(event->size());
  mViewport = rect();
  updateAxisRect();
  replot();
}

/*! \internal
  
  Event handler for when a double click occurs.
*/
void QCustomPlot::mouseDoubleClickEvent(QMouseEvent *event)
{
  emit mouseDoubleClick(event);
  
  // emit specialized object double click signals:
  bool foundHit = false;
  // for legend:
  if (receivers(SIGNAL(legendDoubleClick(QCPLegend*,QCPAbstractLegendItem*,QMouseEvent*))) > 0)
  {
    if (legend->selectTestLegend(event->pos()))
    {
      emit legendDoubleClick(legend, legend->selectTestItem(event->pos()), event);
      foundHit = true;
    }
  }
  // for plottables:
  if (!foundHit && receivers(SIGNAL(plottableDoubleClick(QCPAbstractPlottable*,QMouseEvent*))) > 0)
  {
    if (QCPAbstractPlottable *ap = plottableAt(event->pos(), false))
    {
      emit plottableDoubleClick(ap, event);
      foundHit = true;
    }
  }
  // for items:
  if (!foundHit && receivers(SIGNAL(itemDoubleClick(QCPAbstractItem*,QMouseEvent*))) > 0)
  {
    if (QCPAbstractItem *ai = itemAt(event->pos(), false))
    {
      emit itemDoubleClick(ai, event);
      foundHit = true;
    }
  }
  // for axes:
  if (!foundHit && receivers(SIGNAL(axisDoubleClick(QCPAxis*,QCPAxis::SelectablePart,QMouseEvent*))) > 0)
  {
    QVector<QCPAxis*> axes = QVector<QCPAxis*>() << xAxis << yAxis << xAxis2 << yAxis2;
    for (int i=0; i<axes.size(); ++i)
    {
      QCPAxis::SelectablePart part = axes.at(i)->selectTest(event->pos());
      if (part != QCPAxis::spNone)
      {
        foundHit = true;
        emit axisDoubleClick(axes.at(i), part, event);
        break;
      }
    }
  }
  // for title:
  if (!foundHit && receivers(SIGNAL(titleDoubleClick(QMouseEvent*))) > 0)
  {
    if (selectTestTitle(event->pos()))
    {
      emit titleDoubleClick(event);
      foundHit = true;
    }
  }
}

/*! \internal
  
  Event handler for when a mouse button is pressed. If the left mouse button is pressed, the range
  dragging interaction is initialized (the actual range manipulation happens in the \ref
  mouseMoveEvent).

  The mDragging flag is set to true and some anchor points are set that are needed to determine the
  distance the mouse was dragged in the mouse move/release events later.
  
  \see mouseMoveEvent, mouseReleaseEvent
*/
void QCustomPlot::mousePressEvent(QMouseEvent *event)
{
  emit mousePress(event);
  mDragStart = event->pos(); // need this even when not LeftButton is pressed, to determine in releaseEvent whether it was a full click (no position change between press and release)
  if (event->buttons() & Qt::LeftButton)
  {
    mDragging = true;
    // initialize antialiasing backup in case we start dragging:
    if (mNoAntialiasingOnDrag)
    {
      mAADragBackup = antialiasedElements();
      mNotAADragBackup = notAntialiasedElements();
    }
    // Mouse range dragging interaction:
    if (mInteractions.testFlag(iRangeDrag))
    {
      mDragStartHorzRange = mRangeDragHorzAxis->range();
      mDragStartVertRange = mRangeDragVertAxis->range();
    }
  }
  
  QWidget::mousePressEvent(event);
}

/*! \internal
  
  Event handler for when the cursor is moved. This is where the built-in range dragging mechanism
  is handled.
  
  \see mousePressEvent, mouseReleaseEvent
*/
void QCustomPlot::mouseMoveEvent(QMouseEvent *event)
{
  emit mouseMove(event);

  // Mouse range dragging interaction:
  if (mInteractions.testFlag(iRangeDrag))
  {
    if (mDragging)
    {
      if (mRangeDrag.testFlag(Qt::Horizontal))
      {
        if (mRangeDragHorzAxis->mScaleType == QCPAxis::stLinear)
        {
          double diff = mRangeDragHorzAxis->pixelToCoord(mDragStart.x()) - mRangeDragHorzAxis->pixelToCoord(event->pos().x());
          mRangeDragHorzAxis->setRange(mDragStartHorzRange.lower+diff, mDragStartHorzRange.upper+diff);
        } else if (mRangeDragHorzAxis->mScaleType == QCPAxis::stLogarithmic)
        {
          double diff = mRangeDragHorzAxis->pixelToCoord(mDragStart.x()) / mRangeDragHorzAxis->pixelToCoord(event->pos().x());
          mRangeDragHorzAxis->setRange(mDragStartHorzRange.lower*diff, mDragStartHorzRange.upper*diff);
        }
      }
      if (mRangeDrag.testFlag(Qt::Vertical))
      {
        if (mRangeDragVertAxis->mScaleType == QCPAxis::stLinear)
        {
          double diff = mRangeDragVertAxis->pixelToCoord(mDragStart.y()) - mRangeDragVertAxis->pixelToCoord(event->pos().y());
          mRangeDragVertAxis->setRange(mDragStartVertRange.lower+diff, mDragStartVertRange.upper+diff);
        } else if (mRangeDragVertAxis->mScaleType == QCPAxis::stLogarithmic)
        {
          double diff = mRangeDragVertAxis->pixelToCoord(mDragStart.y()) / mRangeDragVertAxis->pixelToCoord(event->pos().y());
          mRangeDragVertAxis->setRange(mDragStartVertRange.lower*diff, mDragStartVertRange.upper*diff);
        }
      }
      if (mRangeDrag != 0) // if either vertical or horizontal drag was enabled, do a replot
      {
        if (mNoAntialiasingOnDrag)
          setNotAntialiasedElements(QCP::aeAll);
        replot();
      }
    }
  }
  
  QWidget::mouseMoveEvent(event);
}

/*! \internal
  
  Event handler for when a mouse button is released. This is where the selection mechanism is
  handled.
  
  \see mousePressEvent, mouseMoveEvent
*/
void QCustomPlot::mouseReleaseEvent(QMouseEvent *event)
{
  emit mouseRelease(event);
  mDragging = false;
  bool doReplot = false;
  if (mNoAntialiasingOnDrag)
  {
    setAntialiasedElements(mAADragBackup);
    setNotAntialiasedElements(mNotAADragBackup);
    doReplot = true;
  }
  
  // determine whether it was a drag or click operation:
  if ((mDragStart-event->pos()).manhattanLength() < 5) // was a click
  {
    // Mouse selection interaction:
    if ((mInteractions & (iSelectPlottables|iSelectItems|iSelectAxes|iSelectLegend|iSelectTitle)) > 0 
        && event->button() == Qt::LeftButton)
    {
      bool selectionFound = false;
      bool emitChangedSignal = false;
      bool additiveSelection = mInteractions.testFlag(iMultiSelect) && event->modifiers().testFlag(mMultiSelectModifier);
      // Mouse selection of legend:
      if (mInteractions.testFlag(iSelectLegend))
        selectionFound |= legend->handleLegendSelection(event, additiveSelection, emitChangedSignal);
      // Mouse selection of plottables:
      if (mInteractions.testFlag(iSelectPlottables))
        selectionFound |= handlePlottableSelection((!selectionFound || additiveSelection) ? event : 0, additiveSelection, emitChangedSignal);
      // Mouse selection of items:
      if (mInteractions.testFlag(iSelectItems))
        selectionFound |= handleItemSelection((!selectionFound || additiveSelection) ? event : 0, additiveSelection, emitChangedSignal);
      // Mouse selection of axes:
      if (mInteractions.testFlag(iSelectAxes))
        selectionFound |= handleAxisSelection((!selectionFound || additiveSelection) ? event : 0, additiveSelection, emitChangedSignal);
      // Mouse selection of title:
      if (mInteractions.testFlag(iSelectTitle))
        selectionFound |= handleTitleSelection((!selectionFound || additiveSelection) ? event : 0, additiveSelection, emitChangedSignal);
      
      if (emitChangedSignal)
        emit selectionChangedByUser();
      doReplot = true;
    }
    
    // emit specialized object click signals:
    bool foundHit = false;
    // for legend:
    if (receivers(SIGNAL(legendClick(QCPLegend*,QCPAbstractLegendItem*,QMouseEvent*))) > 0)
    {
      if (legend->selectTestLegend(event->pos()))
      {
        emit legendClick(legend, legend->selectTestItem(event->pos()), event);
        foundHit = true;
      }
    }
    // for plottables:
    if (!foundHit && receivers(SIGNAL(plottableClick(QCPAbstractPlottable*,QMouseEvent*))) > 0)
    {
      if (QCPAbstractPlottable *ap = plottableAt(event->pos(), false))
      {
        emit plottableClick(ap, event);
        foundHit = true;
      }
    }
    // for items:
    if (!foundHit && receivers(SIGNAL(itemClick(QCPAbstractItem*,QMouseEvent*))) > 0)
    {
      if (QCPAbstractItem *ai = itemAt(event->pos(), false))
      {
        emit itemClick(ai, event);
        foundHit = true;
      }
    }
    // for axes:
    if (!foundHit && receivers(SIGNAL(axisClick(QCPAxis*,QCPAxis::SelectablePart,QMouseEvent*))) > 0)
    {
      QVector<QCPAxis*> axes = QVector<QCPAxis*>() << xAxis << yAxis << xAxis2 << yAxis2;
      for (int i=0; i<axes.size(); ++i)
      {
        QCPAxis::SelectablePart part = axes.at(i)->selectTest(event->pos());
        if (part != QCPAxis::spNone)
        {
          foundHit = true;
          emit axisClick(axes.at(i), part, event);
          break;
        }
      }
    }
    // for title:
    if (!foundHit && receivers(SIGNAL(titleClick(QMouseEvent*))) > 0)
    {
      if (selectTestTitle(event->pos()))
      {
        emit titleClick(event);
        foundHit = true;
      }
    }
  } // was a click end
  
  if (doReplot)
    replot();
  
  QWidget::mouseReleaseEvent(event);
}

/*! \internal
  
  Event handler for mouse wheel events. First, the mouseWheel signal is emitted.
  If rangeZoom is Qt::Horizontal, Qt::Vertical or both, the ranges of the axes defined as
  rangeZoomHorzAxis and rangeZoomVertAxis are scaled. The center of the scaling
  operation is the current cursor position inside the plot. The scaling factor
  is dependant on the mouse wheel delta (which direction the wheel was rotated)
  to provide a natural zooming feel. The Strength of the zoom can be controlled via
  \ref setRangeZoomFactor.
  
  Note, that event->delta() is usually +/-120 for single rotation steps. However, if the mouse
  wheel is turned rapidly, many steps may bunch up to one event, so the event->delta() may then be
  multiples of 120. This is taken into account here, by calculating \a wheelSteps and using it as
  exponent of the range zoom factor. This takes care of the wheel direction automatically, by
  inverting the factor, when the wheel step is negative (f^-1 = 1/f).
*/
void QCustomPlot::wheelEvent(QWheelEvent *event)
{
  emit mouseWheel(event);
  
  // Mouse range zooming interaction:
  if (mInteractions.testFlag(iRangeZoom))
  {
    if (mRangeZoom != 0)
    {
      double factor;
      double wheelSteps = event->delta()/120.0; // a single step delta is +/-120 usually
      if (mRangeZoom.testFlag(Qt::Horizontal))
      {
        factor = pow(mRangeZoomFactorHorz, wheelSteps);
        mRangeZoomHorzAxis->scaleRange(factor, mRangeZoomHorzAxis->pixelToCoord(event->pos().x()));
      }
      if (mRangeZoom.testFlag(Qt::Vertical))
      {
        factor = pow(mRangeZoomFactorVert, wheelSteps);
        mRangeZoomVertAxis->scaleRange(factor, mRangeZoomVertAxis->pixelToCoord(event->pos().y()));
      }
      replot();
    }
  }
  
  QWidget::wheelEvent(event);
}

/*! \internal
  
  Handles a mouse \a event for the plottable selection interaction. Returns true, when a selectable
  plottable was hit by the mouse event. The output variable \a modified is set to true when the
  selection state of a plottable has changed.
  
  When \a additiveSelecton is true, any new selections become selected in addition to the recent
  selections. The recent selections are not cleared. Further, clicking on one object multiple times
  in additive selection mode, toggles the selection of that object on and off.
  
  To indicate that all plottables that are selectable shall be deselected, pass 0 as \a event.
  
  Unlike for axis and legend selection, this function can't be exported to the respective class
  itself (i.e. QCPAbstractPlottable). The function needs to know the distance of the mouse event to
  all plottables in the plot, in order to choose the plottable with the smallest distance. This
  wouldn't work if it were local to a single plottable.
*/
bool QCustomPlot::handlePlottableSelection(QMouseEvent *event, bool additiveSelection, bool &modified)
{
  // Note: This code is basically identical to handleItemSelection, only for plottables
  
  bool selectionFound = false;
  if (event)
  {
    QCPAbstractPlottable *plottableSelection = plottableAt(event->pos(), true);
    // handle selection of found plottable:
    if (plottableSelection)
    {
      selectionFound = true;
      if (!plottableSelection->selected() || additiveSelection)
      {
        plottableSelection->setSelected(!plottableSelection->selected());
        modified = true;
      }
    }
    // deselect all others (if plottableSelection is 0, all plottables are deselected):
    if (!additiveSelection)
    {
      for (int i=0; i<mPlottables.size(); ++i)
      {
        if (mPlottables.at(i) != plottableSelection && mPlottables.at(i)->selected() && mPlottables.at(i)->selectable())
        {
          mPlottables.at(i)->setSelected(false);
          modified = true;
        }
      }
    }
  } else // event == 0, so deselect selectable plottables
  {
    for (int i=0; i<mPlottables.size(); ++i)
    {
      if (mPlottables.at(i)->selected() && mPlottables.at(i)->selectable())
      {
        mPlottables.at(i)->setSelected(false);
        modified = true;
      }
    }
  }
  return selectionFound;
}

/*! \internal
  
  Handles a mouse \a event for the item selection interaction. Returns true, when a selectable
  item was hit by the mouse event. The output variable \a modified is set to true when the
  selection state of an item has changed.
  
  When \a additiveSelecton is true, any new selections become selected in addition to the recent
  selections. The recent selections are not cleared. Further, clicking on one object multiple times
  in additive selection mode, toggles the selection of that object on and off.
  
  To indicate that all items that are selectable shall be deselected, pass 0 as \a event.
  
  Unlike for axis and legend selection, this function can't be exported to the respective class
  itself (i.e. QCPAbstractItem). The function needs to know the distance of the mouse event to
  all items in the plot, in order to choose the item with the smallest distance. This
  wouldn't work if it were local to a single item.
*/
bool QCustomPlot::handleItemSelection(QMouseEvent *event, bool additiveSelection, bool &modified)
{
  // Note: This code is basically identical to handlePlottableSelection, only for items
  
  bool selectionFound = false;
  if (event)
  {
    QCPAbstractItem *itemSelection = itemAt(event->pos(), true);
    // handle selection of found plottable:
    if (itemSelection)
    {
      selectionFound = true;
      if (!itemSelection->selected() || additiveSelection)
      {
        itemSelection->setSelected(!itemSelection->selected());
        modified = true;
      }
    }
    // deselect all others (if itemSelection is 0, all items are deselected):
    if (!additiveSelection)
    {
      for (int i=0; i<mItems.size(); ++i)
      {
        if (mItems.at(i) != itemSelection && mItems.at(i)->selected() && mItems.at(i)->selectable())
        {
          mItems.at(i)->setSelected(false);
          modified = true;
        }
      }
    }
  } else // event == 0, so deselect selectable items
  {
    for (int i=0; i<mItems.size(); ++i)
    {
      if (mItems.at(i)->selected() && mItems.at(i)->selectable())
      {
        mItems.at(i)->setSelected(false);
        modified = true;
      }
    }
  }
  return selectionFound;
}

/*! \internal
  
  Handles a mouse \a event for the axis selection interaction. Returns true, when a selectable axis
  part was hit by the mouse event. The output variable \a modified is set to true when the
  selection state of an axis has changed.
  
  When \a additiveSelecton is true, any new selections become selected in addition to the recent
  selections. The recent selections are not cleared. Further, clicking on one object multiple times
  in additive selection mode, toggles the selection of that object on and off.
  
  To indicate that all axes shall be deselected, pass 0 as \a event.
*/
bool QCustomPlot::handleAxisSelection(QMouseEvent *event, bool additiveSelection, bool &modified)
{
  bool selectionFound = false;
  QVector<QCPAxis*> axes = QVector<QCPAxis*>() << xAxis << yAxis << xAxis2 << yAxis2;
  for (int i=0; i<axes.size(); ++i)
    selectionFound |= axes.at(i)->handleAxisSelection((!selectionFound || additiveSelection) ? event : 0, additiveSelection, modified);
  return selectionFound;
}

/*! \internal
  
  Handles a mouse \a event for the title selection interaction. Returns true, when the title was
  hit by the mouse event. The output variable \a modified is set to true when the selection state
  of the title has changed.
  
  When \a additiveSelecton is true, any new selections become selected in addition to the recent
  selections. The recent selections are not cleared. Further, clicking on one object multiple times
  in additive selection mode, toggles the selection of that object on and off.
  
  To indicate that the title shall be deselected, pass 0 as \a event.
*/
bool QCustomPlot::handleTitleSelection(QMouseEvent *event, bool additiveSelection, bool &modified)
{
  bool selectionFound = false;
  if (event && selectTestTitle(event->pos())) // hit, select title
  {
    selectionFound = true;
    if (!titleSelected() || additiveSelection)
    {
      setTitleSelected(!titleSelected());
      modified = true;
    }
  } else // no hit or event == 0, deselect title
  {
    if (titleSelected() && !additiveSelection)
    {
      setTitleSelected(false);
      modified = true;
    }
  }
  return selectionFound;
}

/*! \internal
  
  This is the main draw function which first generates the tick vectors of all axes,
  calculates and applies appropriate margins if autoMargin is true and finally draws
  all elements with the passed \a painter. (axis background, title, subgrid, grid, axes, plottables)
*/
void QCustomPlot::draw(QCPPainter *painter)
{
  // calculate title bounding box:
  if (!mTitle.isEmpty())
  {
    painter->setFont(titleSelected() ? mSelectedTitleFont : mTitleFont);
    mTitleBoundingBox = painter->fontMetrics().boundingRect(mViewport, Qt::TextDontClip | Qt::AlignHCenter, mTitle);
  } else
    mTitleBoundingBox = QRect();
  
  // prepare values of ticks and tick strings:
  xAxis->setupTickVectors();
  yAxis->setupTickVectors();
  xAxis2->setupTickVectors();
  yAxis2->setupTickVectors();
  // set auto margin such that tick/axis labels etc. are not clipped:
  if (mAutoMargin)
  {
    setMargin(yAxis->calculateMargin(),
              yAxis2->calculateMargin(),
              xAxis2->calculateMargin()+mTitleBoundingBox.height(),
              xAxis->calculateMargin());
  }
  // position legend:
  legend->reArrange();
  
  // draw axis background:
  drawAxisBackground(painter);
  
  // draw all layered objects (grid, axes, plottables, items, legend,...):
  for (int layerIndex=0; layerIndex < mLayers.size(); ++layerIndex)
  {
    QList<QCPLayerable*> layerChildren = mLayers.at(layerIndex)->children();
    for (int k=0; k < layerChildren.size(); ++k)
    {
      QCPLayerable *child = layerChildren.at(k);
      if (child->visible())
      {
        painter->save();
        painter->setClipRect(child->clipRect().translated(0, -1));
        child->applyDefaultAntialiasingHint(painter);
        child->draw(painter);
        painter->restore();
      }
    }
  }
  
  // draw title:
  if (!mTitle.isEmpty())
  {
    painter->setFont(titleSelected() ? mSelectedTitleFont : mTitleFont);
    painter->setPen(QPen(titleSelected() ? mSelectedTitleColor : mTitleColor));
    painter->drawText(mTitleBoundingBox, Qt::TextDontClip | Qt::AlignHCenter, mTitle);
  }
}

/*! \internal

  If an axis background is provided via \ref setAxisBackground, this function first buffers the
  scaled version depending on \ref setAxisBackgroundScaled and \ref setAxisBackgroundScaledMode and
  then draws it inside the current axisRect with the provided \a painter. The scaled version is
  buffered in mScaledAxisBackground to prevent the need for rescaling at every redraw. It is only
  updated, when the axisRect has changed in a way that requires a rescale of the background pixmap
  (this is dependant on the \ref setAxisBackgroundScaledMode), or when a differend axis backgroud
  was set.
  
  \see draw, setAxisBackground, setAxisBackgroundScaled, setAxisBackgroundScaledMode
*/
void QCustomPlot::drawAxisBackground(QCPPainter *painter)
{
  if (!mAxisBackground.isNull())
  {
    if (mAxisBackgroundScaled)
    {
      // check whether mScaledAxisBackground needs to be updated:
      QSize scaledSize(mAxisBackground.size());
      scaledSize.scale(mAxisRect.size(), mAxisBackgroundScaledMode);
      if (mScaledAxisBackground.size() != scaledSize)
        mScaledAxisBackground = mAxisBackground.scaled(mAxisRect.size(), mAxisBackgroundScaledMode, Qt::SmoothTransformation);
      painter->drawPixmap(mAxisRect.topLeft(), mScaledAxisBackground, QRect(0, 0, mAxisRect.width(), mAxisRect.height()) & mScaledAxisBackground.rect());
    } else
    {
      painter->drawPixmap(mAxisRect.topLeft(), mAxisBackground, QRect(0, 0, mAxisRect.width(), mAxisRect.height()));
    }
  }
}

/*! \internal
  
  calculates mAxisRect by applying the margins inward to mViewport. The axisRect is then passed on
  to all axes via QCPAxis::setAxisRect
  
  \see setMargin, setAxisRect
*/
void QCustomPlot::updateAxisRect()
{
  mAxisRect = mViewport.adjusted(mMarginLeft, mMarginTop, -mMarginRight, -mMarginBottom);
  xAxis->setAxisRect(mAxisRect);
  yAxis->setAxisRect(mAxisRect);
  xAxis2->setAxisRect(mAxisRect);
  yAxis2->setAxisRect(mAxisRect);
}

/*! \internal
  
  Returns whether the point \a pos in pixels hits the plot title.
*/
bool QCustomPlot::selectTestTitle(const QPointF &pos) const
{
  return mTitleBoundingBox.contains(pos.toPoint());
}

/*!
  Saves the plot to a rastered image file \a fileName in the image format \a
  format. The plot is sized to \a width and \a height in pixels and scaled with
  \a scale. (width 100 and scale 2.0 lead to a full resolution file with width
  200) If the \a format supports compression, \a quality may be between 0 and
  100 to control it.
  
  Returns true on success. If this function fails, most likely the given \a format isn't supported
  by the system, see Qt docs about QImageWriter::supportedImageFormats().
  
  \see saveBmp, saveJpg, savePng
*/
bool QCustomPlot::saveRastered(const QString &fileName, int width, int height, double scale, const char *format, int quality)
{
  int newWidth, newHeight;
  if (width == 0 || height == 0)
  {
    newWidth = this->width();
    newHeight = this->height();
  } else
  {
    newWidth = width;
    newHeight = height;
  }
  int scaledWidth = qRound(scale*newWidth);
  int scaledHeight = qRound(scale*newHeight);

  QPixmap pngBuffer(scaledWidth, scaledHeight); // use QPixmap instead of QImage (like live painting buffer), because it supports background transparency (of mColor).
  pngBuffer.fill(mColor);
  QCPPainter painter(&pngBuffer);
  QRect oldViewport = mViewport;
  mViewport = QRect(0, 0, newWidth, newHeight);
  updateAxisRect();
  if (!qFuzzyCompare(scale, 1.0))
  {
    if (scale > 1.0) // for scale < 1 we always want cosmetic pens where possible, because else lines would disappear
    {
      painter.setScaledExportMode(true);
      painter.setRenderHint(QPainter::NonCosmeticDefaultPen);
    }
    painter.scale(scale, scale);
  }
  draw(&painter);
  mViewport = oldViewport;
  updateAxisRect();
  return pngBuffer.save(fileName, format, quality);
}


// ================================================================================
// =================== QCPAbstractPlottable
// ================================================================================

/*! \class QCPAbstractPlottable
  \brief The abstract base class for all data representing objects in a plot.

  It defines a very basic interface like name, pen, brush, visibility etc. Since this class is
  abstract, it can't be instantiated. Use one of the subclasses or create a subclass yourself (see
  below), to create new ways of displaying data.
  
  All further specifics are in the subclasses, for example:
  \li A normal graph with possibly a line, scatter points and error bars is displayed by \ref QCPGraph
  (typically created with \ref QCustomPlot::addGraph).
  \li A parametric curve can be displayed with \ref QCPCurve.
  \li A stackable bar chart can be achieved with \ref QCPBars.
  \li A box of a statistical box plot is created with \ref QCPStatisticalBox.
  
  \section plottables-subclassing Creating own plottables
  
  To create an own plottable, you implement a subclass of QCPAbstractPlottable. These are the pure
  virtual functions, you must implement:
  \li \ref clearData
  \li \ref selectTest
  \li \ref draw
  \li \ref drawLegendIcon
  \li \ref getKeyRange
  \li \ref getValueRange
  
  See the documentation of those functions for what they need to do.
  
  For drawing your plot, you can use the \ref coordsToPixels functions to translate a point in plot
  coordinates to pixel coordinates. This function is quite convenient, because it takes the
  orientation of the key and value axes into account for you (x and y are swapped when the key axis
  is vertical and the value axis horizontal). If you are worried about performance (i.e. you need
  to translate many points in a loop like QCPGraph), you can directly use \ref
  QCPAxis::coordToPixel. However, you must then take care about the orientation of the axis
  yourself.
  
  From QCPAbstractPlottable you inherit the following members you may use:
  <table>
  <tr>
    <td>QCustomPlot *\b mParentPlot</td>
    <td>A pointer to the parent QCustomPlot instance. This is adopted from the axes that are passed in the constructor.</td>
  </tr><tr>
    <td>QString \b mName</td>
    <td>The name of the plottable.</td>
  </tr><tr>
    <td>bool \b mVisible</td>
    <td>Whether the plot is visible or not. When this is false, you shouldn't draw the data in the \ref draw function (\ref draw is always called, no matter what mVisible is).</td>
  </tr><tr>
    <td>QPen \b mPen</td>
    <td>The generic pen of the plottable. You should use this pen for the most prominent data representing lines in the plottable (e.g QCPGraph uses this pen for its graph lines and scatters)</td>
  </tr><tr>
    <td>QPen \b mSelectedPen</td>
    <td>The generic pen that should be used when the plottable is selected (hint: \ref mainPen gives you the right pen, depending on selection state).</td>
  </tr><tr>
    <td>QBrush \b mBrush</td>
    <td>The generic brush of the plottable. You should use this brush for the most prominent fillable structures in the plottable (e.g. QCPGraph uses this brush to control filling under the graph)</td>
  </tr><tr>
    <td>QBrush \b mSelectedBrush</td>
    <td>The generic brush that should be used when the plottable is selected (hint: \ref mainBrush gives you the right brush, depending on selection state).</td>
  </tr><tr>
    <td>QCPAxis *\b mKeyAxis, *\b mValueAxis</td>
    <td>The key and value axes this plottable is attached to. Call their QCPAxis::coordToPixel functions to translate coordinates to pixels in either the key or value dimension.</td>
  </tr><tr>
    <td>bool \b mSelected</td>
    <td>indicates whether the plottable is selected or not.</td>
  </tr>
  </table>
*/

/* start of documentation of pure virtual functions */

/*! \fn void QCPAbstractPlottable::clearData() = 0
  Clears all data in the plottable.
*/

/*! \fn double QCPAbstractPlottable::selectTest(const QPointF &pos) const = 0
  
  This function is used to decide whether a click hits a plottable or not.

  \a pos is a point in pixel coordinates on the QCustomPlot surface. This function returns the
  shortest pixel distance of this point to the plottable (e.g. to the scatters/lines of a graph).
  If the plottable is either invisible, contains no data or the distance couldn't be determined,
  -1.0 is returned. \ref setSelectable has no influence on the return value of this function.

  If the plottable is represented not by single lines but by an area like QCPBars or
  QCPStatisticalBox, a click inside the area returns a constant value greater zero (typically 99%
  of the selectionTolerance of the parent QCustomPlot). If the click lies outside the area, this
  function returns -1.0.
  
  Providing a constant value for area objects allows selecting line objects even when they are
  obscured by such area objects, by clicking close to the lines (i.e. closer than
  0.99*selectionTolerance).
  
  The actual setting of the selection state is not done by this function. This is handled by the
  parent QCustomPlot when the mouseReleaseEvent occurs.
  
  \see setSelected, QCustomPlot::setInteractions
*/

/*! \fn void QCPAbstractPlottable::draw(QCPPainter *painter) = 0
  \internal
  
  Draws this plottable with the provided \a painter. Called by \ref QCustomPlot::draw on all its
  visible plottables.
  
  The cliprect of the provided painter is set to the axis rect of the key/value axis of this
  plottable (what \ref clipRect returns), before this function is called.
*/

/*! \fn void QCPAbstractPlottable::drawLegendIcon(QCPPainter *painter, const QRect &rect) const = 0
  \internal
  
  called by QCPLegend::draw (via QCPPlottableLegendItem::draw) to create a graphical representation
  of this plottable inside \a rect, next to the plottable name.
*/

/*! \fn QCPRange QCPAbstractPlottable::getKeyRange(bool &validRange, SignDomain inSignDomain) const = 0
  \internal
  
  called by rescaleAxes functions to get the full data key bounds. For logarithmic plots, one can
  set \a inSignDomain to either \ref sdNegative or \ref sdPositive in order to restrict the
  returned range to that sign domain. E.g. when only negative range is wanted, set \a inSignDomain
  to \ref sdNegative and all positive points will be ignored for range calculation. For no
  restriction, just set \a inSignDomain to \ref sdBoth (default). \a validRange is an output
  parameter that indicates whether a proper range could be found or not. If this is false, you
  shouldn't use the returned range (e.g. no points in data).
  
  \see rescaleAxes, getValueRange
*/

/*! \fn QCPRange QCPAbstractPlottable::getValueRange(bool &validRange, SignDomain inSignDomain) const = 0
  \internal
  
  called by rescaleAxes functions to get the full data value bounds. For logarithmic plots, one can
  set \a inSignDomain to either \ref sdNegative or \ref sdPositive in order to restrict the
  returned range to that sign domain. E.g. when only negative range is wanted, set \a inSignDomain
  to \ref sdNegative and all positive points will be ignored for range calculation. For no
  restriction, just set \a inSignDomain to \ref sdBoth (default). \a validRange is an output
  parameter that indicates whether a proper range could be found or not. If this is false, you
  shouldn't use the returned range (e.g. no points in data).
  
  \see rescaleAxes, getKeyRange
*/

/* end of documentation of pure virtual functions */
/* start of documentation of signals */

/*! \fn void QCPAbstractPlottable::selectionChanged(bool selected)
  This signal is emitted when the selection state of this plottable has changed, either by user interaction
  or by a direct call to \ref setSelected.
*/

/* end of documentation of signals */

/*!
  Constructs an abstract plottable which uses \a keyAxis as its key axis ("x") and \a valueAxis as
  its value axis ("y"). \a keyAxis and \a valueAxis must reside in the same QCustomPlot instance
  and not have the same orientation. If either of these restrictions is violated, a corresponding
  message is printed to the debug output (qDebug), the construction is not aborted, though.
  
  Since QCPAbstractPlottable is an abstract class that defines the basic interface to plottables
  (i.e. any form of data representation inside a plot, like graphs, curves etc.), it can't be
  directly instantiated.
  
  You probably want one of the subclasses like \ref QCPGraph and \ref QCPCurve instead.
  \see setKeyAxis, setValueAxis
*/
QCPAbstractPlottable::QCPAbstractPlottable(QCPAxis *keyAxis, QCPAxis *valueAxis) :
  QCPLayerable(keyAxis->parentPlot()),
  mName(""),
  mAntialiasedFill(true),
  mAntialiasedScatters(true),
  mAntialiasedErrorBars(false),
  mPen(Qt::black),
  mSelectedPen(Qt::black),
  mBrush(Qt::NoBrush),
  mSelectedBrush(Qt::NoBrush),
  mKeyAxis(keyAxis),
  mValueAxis(valueAxis),
  mSelected(false),
  mSelectable(true)
{
  if (keyAxis->parentPlot() != valueAxis->parentPlot())
    qDebug() << Q_FUNC_INFO << "Parent plot of keyAxis is not the same as that of valueAxis.";
  if (keyAxis->orientation() == valueAxis->orientation())
    qDebug() << Q_FUNC_INFO << "keyAxis and valueAxis must be orthogonal to each other.";
}

/*!
   The name is the textual representation of this plottable as it is displayed in the QCPLegend of
   the parent QCustomPlot. It may contain any utf-8 characters, including newlines.
*/
void QCPAbstractPlottable::setName(const QString &name)
{
  mName = name;
}

/*!
  Sets whether fills of this plottable is drawn antialiased or not.
  
  Note that this setting may be overridden by \ref QCustomPlot::setAntialiasedElements and \ref
  QCustomPlot::setNotAntialiasedElements.
*/
void QCPAbstractPlottable::setAntialiasedFill(bool enabled)
{
  mAntialiasedFill = enabled;
}

/*!
  Sets whether the scatter symbols of this plottable are drawn antialiased or not.
  
  Note that this setting may be overridden by \ref QCustomPlot::setAntialiasedElements and \ref
  QCustomPlot::setNotAntialiasedElements.
*/
void QCPAbstractPlottable::setAntialiasedScatters(bool enabled)
{
  mAntialiasedScatters = enabled;
}

/*!
  Sets whether the error bars of this plottable are drawn antialiased or not.
  
  Note that this setting may be overridden by \ref QCustomPlot::setAntialiasedElements and \ref
  QCustomPlot::setNotAntialiasedElements.
*/
void QCPAbstractPlottable::setAntialiasedErrorBars(bool enabled)
{
  mAntialiasedErrorBars = enabled;
}


/*!
  The pen is used to draw basic lines that make up the plottable representation in the
  plot.
  
  For example, the \ref QCPGraph subclass draws its graph lines and scatter points
  with this pen.

  \see setBrush
*/
void QCPAbstractPlottable::setPen(const QPen &pen)
{
  mPen = pen;
}

/*!
  When the plottable is selected, this pen is used to draw basic lines instead of the normal
  pen set via \ref setPen.

  \see setSelected, setSelectable, setSelectedBrush, selectTest
*/
void QCPAbstractPlottable::setSelectedPen(const QPen &pen)
{
  mSelectedPen = pen;
}

/*!
  The brush is used to draw basic fills of the plottable representation in the
  plot. The Fill can be a color, gradient or texture, see the usage of QBrush.
  
  For example, the \ref QCPGraph subclass draws the fill under the graph with this brush, when
  it's not set to Qt::NoBrush.

  \see setPen
*/
void QCPAbstractPlottable::setBrush(const QBrush &brush)
{
  mBrush = brush;
}

/*!
  When the plottable is selected, this brush is used to draw fills instead of the normal
  brush set via \ref setBrush.

  \see setSelected, setSelectable, setSelectedPen, selectTest
*/
void QCPAbstractPlottable::setSelectedBrush(const QBrush &brush)
{
  mSelectedBrush = brush;
}

/*!
  The key axis of a plottable can be set to any axis of a QCustomPlot, as long as it is orthogonal
  to the plottable's value axis. This function performs no checks to make sure this is the case.
  The typical mathematical choice is to use the x-axis (QCustomPlot::xAxis) as key axis and the
  y-axis (QCustomPlot::yAxis) as value axis.
  
  Normally, the key and value axes are set in the constructor of the plottable (or \ref
  QCustomPlot::addGraph when working with QCPGraphs through the dedicated graph interface).

  \see setValueAxis
*/
void QCPAbstractPlottable::setKeyAxis(QCPAxis *axis)
{
  mKeyAxis = axis;
}

/*!
  The value axis of a plottable can be set to any axis of a QCustomPlot, as long as it is
  orthogonal to the plottable's key axis. This function performs no checks to make sure this is the
  case. The typical mathematical choice is to use the x-axis (QCustomPlot::xAxis) as key axis and
  the y-axis (QCustomPlot::yAxis) as value axis.

  Normally, the key and value axes are set in the constructor of the plottable (or \ref
  QCustomPlot::addGraph when working with QCPGraphs through the dedicated graph interface).
  
  \see setKeyAxis
*/
void QCPAbstractPlottable::setValueAxis(QCPAxis *axis)
{
  mValueAxis = axis;
}

/*!
  Sets whether the user can (de-)select this plottable by clicking on the QCustomPlot surface.
  (When \ref QCustomPlot::setInteractions contains iSelectPlottables.)
  
  However, even when \a selectable was set to false, it is possible to set the selection manually,
  by calling \ref setSelected directly.
  
  \see setSelected
*/
void QCPAbstractPlottable::setSelectable(bool selectable)
{
  mSelectable = selectable;
}

/*!
  Sets whether this plottable is selected or not. When selected, it uses a different pen and brush
  to draw its lines and fills, see \ref setSelectedPen and \ref setSelectedBrush.

  The entire selection mechanism for plottables is handled automatically when \ref
  QCustomPlot::setInteractions contains iSelectPlottables. You only need to call this function when
  you wish to change the selection state manually.
  
  This function can change the selection state even when \ref setSelectable was set to false.
  
  emits the \ref selectionChanged signal when \a selected is different from the previous selection state.
  
  \see selectTest
*/
void QCPAbstractPlottable::setSelected(bool selected)
{
  if (mSelected != selected)
  {
    mSelected = selected;
    emit selectionChanged(mSelected);
  }
}

/*!
  Rescales the key and value axes associated with this plottable to contain all displayed data, so
  the whole plottable is visible. If the scaling of an axis is logarithmic, rescaleAxes will make
  sure not to rescale to an illegal range i.e. a range containing different signs and/or zero.
  Instead it will stay in the current sign domain and ignore all parts of the plottable that lie
  outside of that domain.
  
  \a onlyEnlarge makes sure the ranges are only expanded, never reduced. So it's possible to show
  multiple plottables in their entirety by multiple calls to rescaleAxes where the first call has
  \a onlyEnlarge set to false (the default), and all subsequent set to true.
*/
void QCPAbstractPlottable::rescaleAxes(bool onlyEnlarge) const
{
  rescaleKeyAxis(onlyEnlarge);
  rescaleValueAxis(onlyEnlarge);
}

/*!
  Rescales the key axis of the plottable so the whole plottable is visible.
  
  See \ref rescaleAxes for detailed behaviour.
*/
void QCPAbstractPlottable::rescaleKeyAxis(bool onlyEnlarge) const
{
  SignDomain signDomain = sdBoth;
  if (mKeyAxis->scaleType() == QCPAxis::stLogarithmic)
    signDomain = (mKeyAxis->range().upper < 0 ? sdNegative : sdPositive);
  
  bool validRange;
  QCPRange newRange = getKeyRange(validRange, signDomain);
  if (validRange)
  {
    if (onlyEnlarge)
    {
      if (mKeyAxis->range().lower < newRange.lower)
        newRange.lower = mKeyAxis->range().lower;
      if (mKeyAxis->range().upper > newRange.upper)
        newRange.upper = mKeyAxis->range().upper;
    }
    mKeyAxis->setRange(newRange);
  }
}

/*!
  Rescales the value axis of the plottable so the whole plottable is visible.
  
  See \ref rescaleAxes for detailed behaviour.
*/
void QCPAbstractPlottable::rescaleValueAxis(bool onlyEnlarge) const
{
  SignDomain signDomain = sdBoth;
  if (mValueAxis->scaleType() == QCPAxis::stLogarithmic)
    signDomain = (mValueAxis->range().upper < 0 ? sdNegative : sdPositive);
  
  bool validRange;
  QCPRange newRange = getValueRange(validRange, signDomain);
  
  if (validRange)
  {
    if (onlyEnlarge)
    {
      if (mValueAxis->range().lower < newRange.lower)
        newRange.lower = mValueAxis->range().lower;
      if (mValueAxis->range().upper > newRange.upper)
        newRange.upper = mValueAxis->range().upper;
    }
    mValueAxis->setRange(newRange);
  }
}

/*!
  Adds this plottable to the legend of the parent QCustomPlot.
    
  Normally, a QCPPlottableLegendItem is created and inserted into the legend. If the plottable
  needs a more specialized representation in the plot, this function will take this into account
  and instead create the specialized subclass of QCPAbstractLegendItem.
    
  Returns true on success, i.e. when a legend item associated with this plottable isn't already in
  the legend.
    
  \see removeFromLegend, QCPLegend::addItem
*/
bool QCPAbstractPlottable::addToLegend()
{
  if (!mParentPlot->legend->hasItemWithPlottable(this))
  {
    mParentPlot->legend->addItem(new QCPPlottableLegendItem(mParentPlot->legend, this));
    return true;
  } else
    return false;
}

/*! 
  Removes the plottable from the legend of the parent QCustomPlot. This means the
  QCPAbstractLegendItem (usually a QCPPlottableLegendItem) that is associated with this plottable
  is removed.
    
  Returns true on success, i.e. if a legend item associated with this plottable was found and
  removed from the legend.
    
  \see addToLegend, QCPLegend::removeItem
*/
bool QCPAbstractPlottable::removeFromLegend() const
{
  if (QCPPlottableLegendItem *lip = mParentPlot->legend->itemWithPlottable(this))
    return mParentPlot->legend->removeItem(lip);
  else
    return false;
}

/* inherits documentation from base class */
QRect QCPAbstractPlottable::clipRect() const
{
  return mKeyAxis->axisRect() | mValueAxis->axisRect();
}

/*! \internal
  
  Convenience function for transforming a key/value pair to pixels on the QCustomPlot surface,
  taking the orientations of the axes associated with this plottable into account (e.g. whether key
  represents x or y).
  
  \a key and \a value are transformed to the coodinates in pixels and are written to \a x and \a y.
    
  \see pixelsToCoords, QCPAxis::coordToPixel
*/
void QCPAbstractPlottable::coordsToPixels(double key, double value, double &x, double &y) const
{
  if (mKeyAxis->orientation() == Qt::Horizontal)
  {
    x = mKeyAxis->coordToPixel(key);
    y = mValueAxis->coordToPixel(value);
  } else
  {
    y = mKeyAxis->coordToPixel(key);
    x = mValueAxis->coordToPixel(value);
  }
}

/*! \internal 
  \overload
  
  Returns the input as pixel coordinates in a QPointF.
*/
const QPointF QCPAbstractPlottable::coordsToPixels(double key, double value) const
{
  if (mKeyAxis->orientation() == Qt::Horizontal)
    return QPointF(mKeyAxis->coordToPixel(key), mValueAxis->coordToPixel(value));
  else
    return QPointF(mValueAxis->coordToPixel(value), mKeyAxis->coordToPixel(key));
}

/*! \internal
  
  Convenience function for transforming a x/y pixel pair on the QCustomPlot surface to plot coordinates,
  taking the orientations of the axes associated with this plottable into account (e.g. whether key
  represents x or y).
  
  \a x and \a y are transformed to the plot coodinates and are written to \a key and \a value.
    
  \see coordsToPixels, QCPAxis::coordToPixel
*/
void QCPAbstractPlottable::pixelsToCoords(double x, double y, double &key, double &value) const
{
  if (mKeyAxis->orientation() == Qt::Horizontal)
  {
    key = mKeyAxis->pixelToCoord(x);
    value = mValueAxis->pixelToCoord(y);
  } else
  {
    key = mKeyAxis->pixelToCoord(y);
    value = mValueAxis->pixelToCoord(x);
  }
}

/*! \internal
  \overload

  Returns the pixel input \a pixelPos as plot coordinates \a key and \a value.
*/
void QCPAbstractPlottable::pixelsToCoords(const QPointF &pixelPos, double &key, double &value) const
{
  pixelsToCoords(pixelPos.x(), pixelPos.y(), key, value);
}

/*! \internal

  Returns the pen that should be used for drawing lines of the plottable. Returns mPen when the
  graph is not selected and mSelectedPen when it is.
*/
QPen QCPAbstractPlottable::mainPen() const
{
  return mSelected ? mSelectedPen : mPen;
}

/*! \internal

  Returns the brush that should be used for drawing fills of the plottable. Returns mBrush when the
  graph is not selected and mSelectedBrush when it is.
*/
QBrush QCPAbstractPlottable::mainBrush() const
{
  return mSelected ? mSelectedBrush : mBrush;
}

/*! \internal

  A convenience function to easily set the QPainter::Antialiased hint on the provided \a painter
  before drawing plottable lines.

  This is the antialiasing state the painter passed to the \ref draw method is in by default.
  
  This function takes into account the local setting of the antialiasing flag as well as
  the overrides set e.g. with \ref QCustomPlot::setNotAntialiasedElements.
  
  \see setAntialiased, applyFillAntialiasingHint, applyScattersAntialiasingHint, applyErrorBarsAntialiasingHint
*/
void QCPAbstractPlottable::applyDefaultAntialiasingHint(QCPPainter *painter) const
{
  applyAntialiasingHint(painter, mAntialiased, QCP::aePlottables);
}

/*! \internal

  A convenience function to easily set the QPainter::Antialiased hint on the provided \a painter
  before drawing plottable fills.
  
  This function takes into account the local setting of the fill antialiasing flag as well as
  the overrides set e.g. with \ref QCustomPlot::setNotAntialiasedElements.
  
  \see setAntialiased, applyDefaultAntialiasingHint, applyScattersAntialiasingHint, applyErrorBarsAntialiasingHint
*/
void QCPAbstractPlottable::applyFillAntialiasingHint(QCPPainter *painter) const
{
  applyAntialiasingHint(painter, mAntialiasedFill, QCP::aeFills);
}

/*! \internal

  A convenience function to easily set the QPainter::Antialiased hint on the provided \a painter
  before drawing plottable scatter points.
  
  This function takes into account the local setting of the scatters antialiasing flag as well as
  the overrides set e.g. with \ref QCustomPlot::setNotAntialiasedElements.
  
  \see setAntialiased, applyFillAntialiasingHint, applyDefaultAntialiasingHint, applyErrorBarsAntialiasingHint
*/
void QCPAbstractPlottable::applyScattersAntialiasingHint(QCPPainter *painter) const
{
  applyAntialiasingHint(painter, mAntialiasedScatters, QCP::aeScatters);
}

/*! \internal

  A convenience function to easily set the QPainter::Antialiased hint on the provided \a painter
  before drawing plottable error bars.
  
  This function takes into account the local setting of the error bars antialiasing flag as well as
  the overrides set e.g. with \ref QCustomPlot::setNotAntialiasedElements.
  
  \see setAntialiased, applyFillAntialiasingHint, applyScattersAntialiasingHint, applyDefaultAntialiasingHint
*/
void QCPAbstractPlottable::applyErrorBarsAntialiasingHint(QCPPainter *painter) const
{
  applyAntialiasingHint(painter, mAntialiasedErrorBars, QCP::aeErrorBars);
}

/*! \internal

  Finds the shortest squared distance of \a point to the line segment defined by \a start and \a
  end.
  
  This function may be used to help with the implementation of the \ref selectTest function for
  specific plottables.
  
  \note This function is identical to QCPAbstractItem::distSqrToLine
*/
double QCPAbstractPlottable::distSqrToLine(const QPointF &start, const QPointF &end, const QPointF &point) const
{
  QVector2D a(start);
  QVector2D b(end);
  QVector2D p(point);
  QVector2D v(b-a);
  
  double vLengthSqr = v.lengthSquared();
  if (!qFuzzyIsNull(vLengthSqr))
  {
    double mu = QVector2D::dotProduct(p-a, v)/vLengthSqr;
    if (mu < 0)
      return (a-p).lengthSquared();
    else if (mu > 1)
      return (b-p).lengthSquared();
    else
      return ((a + mu*v)-p).lengthSquared();
  } else
    return (a-p).lengthSquared();
}


// ================================================================================
// =================== QCPAbstractLegendItem
// ================================================================================

/*! \class QCPAbstractLegendItem
  \brief The abstract base class for all items in a QCPLegend.
  
  It defines a very basic interface to items in a QCPLegend. For representing plottables in the
  legend, the subclass QCPPlottableLegendItem is more suitable.
  
  Only derive directly from this class when you need absolute freedom (i.e. a legend item that's
  not associated with a plottable).

  You must implement the following pure virtual functions:
  \li \ref draw
  \li \ref size
  
  You inherit the following members you may use:
  <table>
    <tr>
      <td>QCPLegend *\b mParentLegend</td>
      <td>A pointer to the parent QCPLegend.</td>
    </tr><tr>
      <td>QFont \b mFont</td>
      <td>The generic font of the item. You should use this font for all or at least the most prominent text of the item.</td>
    </tr>
  </table>
*/

/* start documentation of pure virtual functions */

/*! \fn void QCPAbstractLegendItem::draw(QCPPainter *painter, const QRect &rect) const = 0;
  
  Draws this legend item with \a painter inside the specified \a rect. The \a rect typically has
  the size which was returned from a preceding \ref size call.
*/

/*! \fn QSize QCPAbstractLegendItem::size(const QSize &targetSize) const = 0;

  Returns the size this item occupies in the legend. The legend will adapt its layout with the help
  of this function. If this legend item can have a variable width (e.g. auto-wrapping text), this
  function tries to find a size with a width close to the width of \a targetSize. The height of \a
  targetSize only may have meaning in specific sublasses. Typically, it's ignored.
*/

/* end documentation of pure virtual functions */
/* start of documentation of signals */

/*! \fn void QCPAbstractLegendItem::selectionChanged(bool selected)
  
  This signal is emitted when the selection state of this legend item has changed, either by user interaction
  or by a direct call to \ref setSelected.
*/

/* end of documentation of signals */

/*!
  Constructs a QCPAbstractLegendItem and associates it with the QCPLegend \a parent. This does not
  cause the item to be added to \a parent, so \ref QCPLegend::addItem must be called separately.
*/
QCPAbstractLegendItem::QCPAbstractLegendItem(QCPLegend *parent) : 
  QObject(parent),
  mParentLegend(parent),
  mAntialiased(false),
  mFont(parent->font()),
  mTextColor(parent->textColor()),
  mSelectedFont(parent->selectedFont()),
  mSelectedTextColor(parent->selectedTextColor()),
  mSelectable(true),
  mSelected(false)
{
}

/*!
  Sets whether this legend item is drawn antialiased or not.
  
  Note that this setting may be overridden by \ref QCustomPlot::setAntialiasedElements and \ref
  QCustomPlot::setNotAntialiasedElements.
*/
void QCPAbstractLegendItem::setAntialiased(bool enabled)
{
  mAntialiased = enabled;
}

/*!
  Sets the default font of this specific legend item to \a font.
  
  \see setTextColor, QCPLegend::setFont
*/
void QCPAbstractLegendItem::setFont(const QFont &font)
{
  mFont = font;
}

/*!
  Sets the default text color of this specific legend item to \a color.
  
  \see setFont, QCPLegend::setTextColor
*/
void QCPAbstractLegendItem::setTextColor(const QColor &color)
{
  mTextColor = color;
}

/*!
  When this legend item is selected, \a font is used to draw generic text, instead of the normal
  font set with \ref setFont.
  
  \see setFont, QCPLegend::setSelectedFont
*/
void QCPAbstractLegendItem::setSelectedFont(const QFont &font)
{
  mSelectedFont = font;
}

/*!
  When this legend item is selected, \a color is used to draw generic text, instead of the normal
  color set with \ref setTextColor.
  
  \see setTextColor, QCPLegend::setSelectedTextColor
*/
void QCPAbstractLegendItem::setSelectedTextColor(const QColor &color)
{
  mSelectedTextColor = color;
}

/*!
  Sets whether this specific legend item is selectable.
  
  \see setSelected, QCustomPlot::setInteractions
*/
void QCPAbstractLegendItem::setSelectable(bool selectable)
{
  mSelectable = selectable;
}

/*!
  Sets whether this specific legend item is selected. The selection state of the parent QCPLegend
  is updated correspondingly.
  
  It is possible to set the selection state of this item by calling this function directly, even if
  setSelectable is set to false.
  
  \see setSelectable, QCustomPlot::setInteractions
*/
void QCPAbstractLegendItem::setSelected(bool selected)
{
  if (mSelected != selected)
  {
    mSelected = selected;
    emit selectionChanged(mSelected);
    mParentLegend->updateSelectionState();
  }
}

/*! \internal

  Sets the QPainter::Antialiasing render hint on the provided \a painter, depending on the \ref
  setAntialiased state of this legend item as well as the overrides \ref
  QCustomPlot::setAntialiasedElements and \ref QCustomPlot::setNotAntialiasedElements.
*/
void QCPAbstractLegendItem::applyAntialiasingHint(QCPPainter *painter) const
{
  if (mParentLegend->mParentPlot->notAntialiasedElements().testFlag(QCP::aeLegendItems))
    painter->setAntialiasing(false);
  else if (mParentLegend->mParentPlot->antialiasedElements().testFlag(QCP::aeLegendItems))
    painter->setAntialiasing(true);
  else
    painter->setAntialiasing(mAntialiased);
}


// ================================================================================
// =================== QCPPlottableLegendItem
// ================================================================================
/*! \class QCPPlottableLegendItem
  \brief A legend item representing a plottable with an icon and the plottable name.
  
  This is the standard legend item for plottables. It displays an icon of the plottable next to the
  plottable name. The icon is drawn by the respective plottable itself (\ref
  QCPAbstractPlottable::drawLegendIcon), and tries to give an intuitive symbol for the plottable.
  For example, the QCPGraph draws a centered horizontal line with a single scatter point in the
  middle and filling (if enabled) below.
  
  Legend items of this type are always associated with one plottable (retrievable via the
  plottable() function and settable with the constructor). You may change the font of the plottable
  name with \ref setFont. If \ref setTextWrap is set to true, the plottable name will wrap at the
  right legend boundary (see \ref QCPLegend::setMinimumSize). Icon padding and border pen is taken
  from the parent QCPLegend, see \ref QCPLegend::setIconBorderPen and \ref
  QCPLegend::setIconTextPadding.

  The function \ref QCPAbstractPlottable::addToLegend/\ref QCPAbstractPlottable::removeFromLegend
  creates/removes legend items of this type in the default implementation. However, these functions
  may be reimplemented such that a different kind of legend item (e.g a direct subclass of
  QCPAbstractLegendItem) is used for that plottable.
*/

/*!
  Creates a new legend item associated with \a plottable.
  
  Once it's created, it can be added to the legend via \ref QCPLegend::addItem.
  
  A more convenient way of adding/removing a plottable to/from the legend is via the functions \ref
  QCPAbstractPlottable::addToLegend and \ref QCPAbstractPlottable::removeFromLegend.
*/
QCPPlottableLegendItem::QCPPlottableLegendItem(QCPLegend *parent, QCPAbstractPlottable *plottable) :
  QCPAbstractLegendItem(parent),
  mPlottable(plottable),
  mTextWrap(false)
{
}

/*!
  Sets whether the text of the legend item is wrapped at word boundaries to fit the with of the
  legend.
  
  To prevent the legend autoSize feature (QCPLegend::setAutoSize) from compressing the text too
  strong by wrapping it very often, set an appropriate minimum width with
  QCPLegend::setMinimumSize.
*/
void QCPPlottableLegendItem::setTextWrap(bool wrap)
{
  mTextWrap = wrap;
}

/*! \internal
  
  Returns the pen that shall be used to draw the icon border, taking into account the selection
  state of this item.
*/
QPen QCPPlottableLegendItem::getIconBorderPen() const
{
  return mSelected ? mParentLegend->selectedIconBorderPen() : mParentLegend->iconBorderPen();
}

/*! \internal
  
  Returns the text color that shall be used to draw text, taking into account the selection state
  of this item.
*/
QColor QCPPlottableLegendItem::getTextColor() const
{
  return mSelected ? mSelectedTextColor : mTextColor;
}

/*! \internal
  
  Returns the font that shall be used to draw text, taking into account the selection state of this
  item.
*/
QFont QCPPlottableLegendItem::getFont() const
{
  return mSelected ? mSelectedFont : mFont;
}

/*! \internal
  
  Draws the item with \a painter into \a rect.

  The width of the passed rect is used as text wrapping width, when \ref setTextWrap is enabled.
  The height is ignored. The rect is not used as a clipping rect (overpainting is not prevented
  inside this function), so you should set an appropriate clipping rect on the painter before
  calling this function. Ideally, the width of the rect should be the result of a preceding call to
  \ref size.
*/
void QCPPlottableLegendItem::draw(QCPPainter *painter, const QRect &rect) const
{
  if (!mPlottable) return;
  painter->setFont(getFont());
  painter->setPen(QPen(getTextColor()));
  int iconTextPadding = mParentLegend->iconTextPadding();
  QSize iconSize = mParentLegend->iconSize();
  QRect textRect;
  QRect iconRect(rect.topLeft(), iconSize);
  if (mTextWrap)
  {
    // take width from rect since our text should wrap there (only icon must fit at least):
    textRect = painter->fontMetrics().boundingRect(0, 0, rect.width()-iconTextPadding-iconSize.width(), rect.height(), Qt::TextDontClip | Qt::TextWordWrap, mPlottable->name());
    if (textRect.height() < iconSize.height()) // text smaller than icon, center text vertically in icon height
    {
      painter->drawText(rect.x()+iconSize.width()+iconTextPadding, rect.y(), rect.width()-iconTextPadding-iconSize.width(), iconSize.height(), Qt::TextDontClip | Qt::TextWordWrap, mPlottable->name());
    } else // text bigger than icon, position top of text with top of icon
    {
      painter->drawText(rect.x()+iconSize.width()+iconTextPadding, rect.y(), rect.width()-iconTextPadding-iconSize.width(), textRect.height(), Qt::TextDontClip | Qt::TextWordWrap, mPlottable->name());
    }
  } else
  {
    // text can't wrap (except with explicit newlines), center at current item size (icon size)
    textRect = painter->fontMetrics().boundingRect(0, 0, 0, rect.height(), Qt::TextDontClip, mPlottable->name());
    if (textRect.height() < iconSize.height()) // text smaller than icon, center text vertically in icon height
    {
      painter->drawText(rect.x()+iconSize.width()+iconTextPadding, rect.y(), rect.width(), iconSize.height(), Qt::TextDontClip, mPlottable->name());
    } else // text bigger than icon, position top of text with top of icon
    {
      painter->drawText(rect.x()+iconSize.width()+iconTextPadding, rect.y(), rect.width(), textRect.height(), Qt::TextDontClip, mPlottable->name());
    }
  }
  // draw icon:
  painter->save();
  painter->setClipRect(iconRect, Qt::IntersectClip);
  mPlottable->drawLegendIcon(painter, iconRect);
  painter->restore();
  // draw icon border:
  if (getIconBorderPen().style() != Qt::NoPen)
  {
    painter->setPen(getIconBorderPen());
    painter->setBrush(Qt::NoBrush);
    painter->drawRect(iconRect);
  }
}

/*! \internal
  
  Calculates and returns the size of this item. If \ref setTextWrap is enabled, the width of \a
  targetSize will be used as the text wrapping width. This does not guarantee, that the width of
  the returned QSize is the same as the width of \a targetSize, since wrapping occurs only at word
  boundaries. So a single word that extends beyond the width of \a targetSize, will stretch the
  returned QSize accordingly.
  
  The height of \a targetSize is ignored. The height of the returned QSize is either the height
  of the icon or the height of the text bounding box, whichever is larger.
*/
QSize QCPPlottableLegendItem::size(const QSize &targetSize) const
{
  if (!mPlottable) return QSize();
  QSize result(0, 0);
  QRect textRect;
  QFontMetrics fontMetrics(getFont());
  int iconTextPadding = mParentLegend->iconTextPadding();
  QSize iconSize = mParentLegend->iconSize();
  if (mTextWrap)
  {
    // take width from targetSize since our text can wrap (Only icon must fit at least):
    textRect = fontMetrics.boundingRect(0, 0, targetSize.width()-iconTextPadding-iconSize.width(), iconSize.height(), Qt::TextDontClip | Qt::TextWordWrap, mPlottable->name());
  } else
  {
    // text can't wrap (except with explicit newlines), center at current item size (icon size)
    textRect = fontMetrics.boundingRect(0, 0, 0, iconSize.height(), Qt::TextDontClip, mPlottable->name());
  }
  result.setWidth(iconSize.width() + mParentLegend->iconTextPadding() + textRect.width());
  result.setHeight(qMax(textRect.height(), iconSize.height()));
  return result;
}

// ================================================================================
// =================== QCPCurve
// ================================================================================
/*! \class QCPCurve
  \brief A plottable representing a parametric curve in a plot.

  To plot data, assign it with the \ref setData or \ref addData functions.
  
  \section appearance Changing the appearance
  
  The appearance of the curve is determined by the pen and the brush (\ref setPen, \ref setBrush).
  \section usage Usage
  
  Like all data representing objects in QCustomPlot, the QCPCurve is a plottable (QCPAbstractPlottable). So
  the plottable-interface of QCustomPlot applies (QCustomPlot::plottable, QCustomPlot::addPlottable, QCustomPlot::removePlottable, etc.) 
  
  Usually, you first create an instance:
  \code
  QCPCurve *newCurve = new QCPCurve(customPlot->xAxis, customPlot->yAxis);\endcode
  add it to the customPlot with QCustomPlot::addPlottable:
  \code
  customPlot->addPlottable(newCurve);\endcode
  and then modify the properties of the newly created plottable, e.g.:
  \code
  newCurve->setName("Fermat's Spiral");
  newCurve->setData(tData, xData, yData);\endcode
*/

/*!
  Constructs a curve which uses \a keyAxis as its key axis ("x") and \a valueAxis as its value
  axis ("y"). \a keyAxis and \a valueAxis must reside in the same QCustomPlot instance and not have
  the same orientation. If either of these restrictions is violated, a corresponding message is
  printed to the debug output (qDebug), the construction is not aborted, though.
  
  The constructed QCPCurve can be added to the plot with QCustomPlot::addPlottable, QCustomPlot
  then takes ownership of the graph.
*/
QCPCurve::QCPCurve(QCPAxis *keyAxis, QCPAxis *valueAxis) :
  QCPAbstractPlottable(keyAxis, valueAxis)
{
  mData = new QCPCurveDataMap;
  mPen.setColor(Qt::blue);
  mPen.setStyle(Qt::SolidLine);
  mBrush.setColor(Qt::blue);
  mBrush.setStyle(Qt::NoBrush);
  mSelectedPen = mPen;
  mSelectedPen.setWidthF(2.5);
  mSelectedPen.setColor(QColor(80, 80, 255)); // lighter than Qt::blue of mPen
  mSelectedBrush = mBrush;
  
  setScatterSize(6);
  setScatterStyle(QCP::ssNone);
  setLineStyle(lsLine);
}

QCPCurve::~QCPCurve()
{
  delete mData;
}

/*!
  Replaces the current data with the provided \a data.
  
  If \a copy is set to true, data points in \a data will only be copied. if false, the plottable
  takes ownership of the passed data and replaces the internal data pointer with it. This is
  significantly faster than copying for large datasets.
*/
void QCPCurve::setData(QCPCurveDataMap *data, bool copy)
{
  if (copy)
  {
    *mData = *data;
  } else
  {
    delete mData;
    mData = data;
  }
}

/*! \overload
  
  Replaces the current data with the provided points in \a t, \a key and \a value tuples. The
  provided vectors should have equal length. Else, the number of added points will be the size of
  the smallest vector.
*/
void QCPCurve::setData(const QVector<double> &t, const QVector<double> &key, const QVector<double> &value)
{
  mData->clear();
  int n = t.size();
  n = qMin(n, key.size());
  n = qMin(n, value.size());
  QCPCurveData newData;
  for (int i=0; i<n; ++i)
  {
    newData.t = t[i];
    newData.key = key[i];
    newData.value = value[i];
    mData->insertMulti(newData.t, newData);
  }
}

/*! \overload
  
  Replaces the current data with the provided \a key and \a value pairs. The t parameter
  of each data point will be set to the integer index of the respective key/value pair.
*/
void QCPCurve::setData(const QVector<double> &key, const QVector<double> &value)
{
  mData->clear();
  int n = key.size();
  n = qMin(n, value.size());
  QCPCurveData newData;
  for (int i=0; i<n; ++i)
  {
    newData.t = i; // no t vector given, so we assign t the index of the key/value pair
    newData.key = key[i];
    newData.value = value[i];
    mData->insertMulti(newData.t, newData);
  }
}

/*! 
  Sets the visual appearance of single data points in the plot. If set to \ref QCP::ssNone, no scatter points
  are drawn (e.g. for line-only-plots with appropriate line style).
  \see ScatterStyle, setLineStyle
*/
void QCPCurve::setScatterStyle(QCP::ScatterStyle style)
{
  mScatterStyle = style;
}

/*! 
  This defines how big (in pixels) single scatters are drawn, if scatter style (\ref
  setScatterStyle) isn't \ref QCP::ssNone, \ref QCP::ssDot or \ref QCP::ssPixmap. Floating point values are
  allowed for fine grained control over optical appearance with antialiased painting.
  
  \see ScatterStyle
*/
void QCPCurve::setScatterSize(double size)
{
  mScatterSize = size;
}

/*! 
  If the scatter style (\ref setScatterStyle) is set to ssPixmap, this function defines the QPixmap
  that will be drawn centered on the data point coordinate.
  
  \see ScatterStyle
*/
void QCPCurve::setScatterPixmap(const QPixmap &pixmap)
{
  mScatterPixmap = pixmap;
}

/*!
  Sets how the single data points are connected in the plot or how they are represented visually
  apart from the scatter symbol. For scatter-only plots, set \a style to \ref lsNone and \ref
  setScatterStyle to the desired scatter style.
  
  \see setScatterStyle
*/
void QCPCurve::setLineStyle(QCPCurve::LineStyle style)
{
  mLineStyle = style;
}

/*!
  Adds the provided data points in \a dataMap to the current data.
  \see removeData
*/
void QCPCurve::addData(const QCPCurveDataMap &dataMap)
{
  mData->unite(dataMap);
}

/*! \overload
  Adds the provided single data point in \a data to the current data.
  \see removeData
*/
void QCPCurve::addData(const QCPCurveData &data)
{
  mData->insertMulti(data.t, data);
}

/*! \overload
  Adds the provided single data point as \a t, \a key and \a value tuple to the current data
  \see removeData
*/
void QCPCurve::addData(double t, double key, double value)
{
  QCPCurveData newData;
  newData.t = t;
  newData.key = key;
  newData.value = value;
  mData->insertMulti(newData.t, newData);
}

/*! \overload
  
  Adds the provided single data point as \a key and \a value pair to the current data The t
  parameter of the data point is set to the t of the last data point plus 1. If there is no last
  data point, t will be set to 0.
  
  \see removeData
*/
void QCPCurve::addData(double key, double value)
{
  QCPCurveData newData;
  if (!mData->isEmpty())
    newData.t = (mData->constEnd()-1).key()+1;
  else
    newData.t = 0;
  newData.key = key;
  newData.value = value;
  mData->insertMulti(newData.t, newData);
}

/*! \overload
  Adds the provided data points as \a t, \a key and \a value tuples to the current data.
  \see removeData
*/
void QCPCurve::addData(const QVector<double> &ts, const QVector<double> &keys, const QVector<double> &values)
{
  int n = ts.size();
  n = qMin(n, keys.size());
  n = qMin(n, values.size());
  QCPCurveData newData;
  for (int i=0; i<n; ++i)
  {
    newData.t = ts[i];
    newData.key = keys[i];
    newData.value = values[i];
    mData->insertMulti(newData.t, newData);
  }
}

/*!
  Removes all data points with curve parameter t smaller than \a t.
  \see addData, clearData
*/
void QCPCurve::removeDataBefore(double t)
{
  QCPCurveDataMap::iterator it = mData->begin();
  while (it != mData->end() && it.key() < t)
    it = mData->erase(it);
}

/*!
  Removes all data points with curve parameter t greater than \a t.
  \see addData, clearData
*/
void QCPCurve::removeDataAfter(double t)
{
  if (mData->isEmpty()) return;
  QCPCurveDataMap::iterator it = mData->upperBound(t);
  while (it != mData->end())
    it = mData->erase(it);
}

/*!
  Removes all data points with curve parameter t between \a fromt and \a tot. if \a fromt is
  greater or equal to \a tot, the function does nothing. To remove a single data point with known
  t, use \ref removeData(double t).
  
  \see addData, clearData
*/
void QCPCurve::removeData(double fromt, double tot)
{
  if (fromt >= tot || mData->isEmpty()) return;
  QCPCurveDataMap::iterator it = mData->upperBound(fromt);
  QCPCurveDataMap::iterator itEnd = mData->upperBound(tot);
  while (it != itEnd)
    it = mData->erase(it);
}

/*! \overload
  
  Removes a single data point at curve parameter \a t. If the position is not known with absolute
  precision, consider using \ref removeData(double fromt, double tot) with a small fuzziness
  interval around the suspected position, depeding on the precision with which the curve parameter
  is known.
  
  \see addData, clearData
*/
void QCPCurve::removeData(double t)
{
  mData->remove(t);
}

/*!
  Removes all data points.
  \see removeData, removeDataAfter, removeDataBefore
*/
void QCPCurve::clearData()
{
  mData->clear();
}

/* inherits documentation from base class */
double QCPCurve::selectTest(const QPointF &pos) const
{
  if (mData->isEmpty() || !mVisible)
    return -1;
  
  return pointDistance(pos);
}

/* inherits documentation from base class */
void QCPCurve::draw(QCPPainter *painter)
{
  if (mData->isEmpty()) return;
  
  // allocate line vector:
  QVector<QPointF> *lineData = new QVector<QPointF>;
  // fill with curve data:
  getCurveData(lineData);
  // draw curve fill:
  if (mainBrush().style() != Qt::NoBrush && mainBrush().color().alpha() != 0)
  {
    applyFillAntialiasingHint(painter);
    painter->setPen(Qt::NoPen);
    painter->setBrush(mainBrush());
    painter->drawPolygon(QPolygonF(*lineData));
  }
  // draw curve line:
  if (mLineStyle != lsNone && mainPen().style() != Qt::NoPen && mainPen().color().alpha() != 0)
  {
    applyDefaultAntialiasingHint(painter);
    painter->setPen(mainPen());
    painter->setBrush(Qt::NoBrush);
    // if drawing solid line and not in PDF, use much faster line drawing instead of polyline:
    if (mParentPlot->plottingHints().testFlag(QCP::phFastPolylines) &&
        painter->pen().style() == Qt::SolidLine &&
        !painter->pdfExportMode())
    {
      for (int i=1; i<lineData->size(); ++i)
        painter->drawLine(lineData->at(i-1), lineData->at(i));
    } else
    {  
      painter->drawPolyline(QPolygonF(*lineData));
    }
  }
  // draw scatters:
  if (mScatterStyle != QCP::ssNone)
    drawScatterPlot(painter, lineData);
  // free allocated line data:
  delete lineData;
}

/* inherits documentation from base class */
void QCPCurve::drawLegendIcon(QCPPainter *painter, const QRect &rect) const
{
  // draw fill:
  if (mBrush.style() != Qt::NoBrush)
  {
    applyFillAntialiasingHint(painter);
    painter->fillRect(QRectF(rect.left(), rect.top()+rect.height()/2.0, rect.width(), rect.height()/3.0), mBrush);
  }
  // draw line vertically centered:
  if (mLineStyle != lsNone)
  {
    applyDefaultAntialiasingHint(painter);
    painter->setPen(mPen);
    painter->drawLine(QLineF(rect.left(), rect.top()+rect.height()/2.0, rect.right()+5, rect.top()+rect.height()/2.0)); // +5 on x2 else last segment is missing from dashed/dotted pens
  }
  // draw scatter symbol:
  if (mScatterStyle != QCP::ssNone)
  {
    if (mScatterStyle == QCP::ssPixmap && (mScatterPixmap.size().width() > rect.width() || mScatterPixmap.size().height() > rect.height()))
    {
      // handle pixmap scatters that are larger than legend icon rect separately.
      // We resize them and draw them manually, instead of calling drawScatter:
      QSize newSize = mScatterPixmap.size();
      newSize.scale(rect.size(), Qt::KeepAspectRatio);
      QRect targetRect;
      targetRect.setSize(newSize);
      targetRect.moveCenter(rect.center());
      bool smoothBackup = painter->testRenderHint(QPainter::SmoothPixmapTransform);
      painter->setRenderHint(QPainter::SmoothPixmapTransform, true);
      painter->drawPixmap(targetRect, mScatterPixmap);
      painter->setRenderHint(QPainter::SmoothPixmapTransform, smoothBackup);
    } else
    {
      applyScattersAntialiasingHint(painter);
      painter->setPen(mPen);
      painter->drawScatter(QRectF(rect).center().x(), QRectF(rect).center().y(), mScatterSize, mScatterStyle);
    }
  }
}

/*! \internal
  
  Draws scatter symbols at every data point passed in \a pointData. scatter symbols are independent of
  the line style and are always drawn if scatter style is not \ref QCP::ssNone.
*/
void QCPCurve::drawScatterPlot(QCPPainter *painter, const QVector<QPointF> *pointData) const
{
  // draw scatter point symbols:
  applyScattersAntialiasingHint(painter);
  painter->setPen(mainPen());
  painter->setBrush(mainBrush());
  painter->setScatterPixmap(mScatterPixmap);
  for (int i=0; i<pointData->size(); ++i)
    painter->drawScatter(pointData->at(i).x(), pointData->at(i).y(), mScatterSize, mScatterStyle);
}

/*! \internal
  
  called by QCPCurve::draw to generate a point vector (pixels) which represents the line of the
  curve. Line segments that aren't visible in the current axis rect are handled in an optimized
  way.
*/
void QCPCurve::getCurveData(QVector<QPointF> *lineData) const
{
  /* Extended sides of axis rect R divide space into 9 regions:
     1__|_4_|__7  
     2__|_R_|__8
     3  | 6 |  9 
     General idea: If the two points of a line segment are in the same region (that is not R), the line segment corner is removed.
     Curves outside R become straight lines closely outside of R which greatly reduces drawing time, yet keeps the look of lines and
     fills inside R consistent.
     The region R has index 5.
  */
  lineData->reserve(mData->size());
  QCPCurveDataMap::const_iterator it;
  int lastRegion = 5;
  int currentRegion = 5;
  double RLeft = mKeyAxis->range().lower;
  double RRight = mKeyAxis->range().upper;
  double RBottom = mValueAxis->range().lower;
  double RTop = mValueAxis->range().upper;
  double x, y; // current key/value
  bool addedLastAlready = true;
  bool firstPoint = true; // first point must always be drawn, to make sure fill works correctly
  for (it = mData->constBegin(); it != mData->constEnd(); ++it)
  {
    x = it.value().key;
    y = it.value().value;
    // determine current region:
    if (x < RLeft) // region 123
    {
      if (y > RTop)
        currentRegion = 1;
      else if (y < RBottom)
        currentRegion = 3;
      else
        currentRegion = 2;
    } else if (x > RRight) // region 789
    {
      if (y > RTop)
        currentRegion = 7;
      else if (y < RBottom)
        currentRegion = 9;
      else
        currentRegion = 8;
    } else // region 456
    {
      if (y > RTop)
        currentRegion = 4;
      else if (y < RBottom)
        currentRegion = 6;
      else
        currentRegion = 5;
    }
    
    /*
      Watch out, the next part is very tricky. It modifies the curve such that it seems like the
      whole thing is still drawn, but actually the points outside the axisRect are simplified
      ("optimized") greatly. There are some subtle special cases when line segments are large and
      thereby each subsequent point may be in a different region or even skip some.
    */
    // determine whether to keep current point:
    if (currentRegion == 5 || (firstPoint && mBrush.style() != Qt::NoBrush)) // current is in R, add current and last if it wasn't added already
    {
      if (!addedLastAlready) // in case curve just entered R, make sure the last point outside R is also drawn correctly
        lineData->append(coordsToPixels((it-1).value().key, (it-1).value().value)); // add last point to vector
      else if (lastRegion != 5) // added last already. If that's the case, we probably added it at optimized position. So go back and make sure it's at original position (else the angle changes under which this segment enters R)
      {
        if (!firstPoint) // because on firstPoint, currentRegion is 5 and addedLastAlready is true, although there is no last point
          lineData->replace(lineData->size()-1, coordsToPixels((it-1).value().key, (it-1).value().value));
      }
      lineData->append(coordsToPixels(it.value().key, it.value().value)); // add current point to vector
      addedLastAlready = true; // so in next iteration, we don't add this point twice
    } else if (currentRegion != lastRegion) // changed region, add current and last if not added already
    {
      // using outsideCoordsToPixels instead of coorsToPixels for optimized point placement (places points just outside axisRect instead of potentially far away)
      
      // if we're coming from R or we skip diagonally over the corner regions (so line might still be visible in R), we can't place points optimized
      if (lastRegion == 5 || // coming from R
          ((lastRegion==2 && currentRegion==4) || (lastRegion==4 && currentRegion==2)) || // skip top left diagonal
          ((lastRegion==4 && currentRegion==8) || (lastRegion==8 && currentRegion==4)) || // skip top right diagonal
          ((lastRegion==8 && currentRegion==6) || (lastRegion==6 && currentRegion==8)) || // skip bottom right diagonal
          ((lastRegion==6 && currentRegion==2) || (lastRegion==2 && currentRegion==6))    // skip bottom left diagonal
          )
      {
        // always add last point if not added already, original:
        if (!addedLastAlready)
          lineData->append(coordsToPixels((it-1).value().key, (it-1).value().value));
        // add current point, original:
        lineData->append(coordsToPixels(it.value().key, it.value().value));
      } else // no special case that forbids optimized point placement, so do it:
      {
        // always add last point if not added already, optimized:
        if (!addedLastAlready)
          lineData->append(outsideCoordsToPixels((it-1).value().key, (it-1).value().value, currentRegion));
        // add current point, optimized:
        lineData->append(outsideCoordsToPixels(it.value().key, it.value().value, currentRegion));
      }
      addedLastAlready = true; // so that if next point enters 5, or crosses another region boundary, we don't add this point twice
    } else // neither in R, nor crossed a region boundary, skip current point
    {
      addedLastAlready = false;
    }
    lastRegion = currentRegion;
    firstPoint = false;
  }
  // If curve ends outside R, we want to add very last point so the fill looks like it should when the curve started inside R:
  if (lastRegion != 5 && mBrush.style() != Qt::NoBrush && !mData->isEmpty())
    lineData->append(coordsToPixels((mData->constEnd()-1).value().key, (mData->constEnd()-1).value().value));
}

/*! \internal 
  
  Calculates the (minimum) distance (in pixels) the curve's representation has from the given \a
  pixelPoint in pixels. This is used to determine whether the curve was clicked or not, e.g. in
  \ref selectTest.
*/
double QCPCurve::pointDistance(const QPointF &pixelPoint) const
{
  if (mData->isEmpty())
  {
    qDebug() << Q_FUNC_INFO << "requested point distance on curve" << mName << "without data";
    return 500;
  }
  if (mData->size() == 1)
  {
    QPointF dataPoint = coordsToPixels(mData->constBegin().key(), mData->constBegin().value().value);
    return QVector2D(dataPoint-pixelPoint).length();
  }
  
  // calculate minimum distance to line segments:
  QVector<QPointF> *lineData = new QVector<QPointF>;
  getCurveData(lineData);
  double minDistSqr = std::numeric_limits<double>::max();
  for (int i=0; i<lineData->size()-1; ++i)
  {
    double currentDistSqr = distSqrToLine(lineData->at(i), lineData->at(i+1), pixelPoint);
    if (currentDistSqr < minDistSqr)
      minDistSqr = currentDistSqr;
  }
  delete lineData;
  return sqrt(minDistSqr);
}

/*! \internal
  
  This is a specialized \ref coordsToPixels function for points that are outside the visible
  axisRect and just crossing a boundary (since \ref getCurveData reduces non-visible curve segments
  to those line segments that cross region boundaries, see documentation there). It only uses the
  coordinate parallel to the region boundary of the axisRect. The other coordinate is picked 10
  pixels outside the axisRect. Together with the optimization in \ref getCurveData this improves
  performance for large curves (or zoomed in ones) significantly while keeping the illusion the
  whole curve and its filling is still being drawn for the viewer.
*/
QPointF QCPCurve::outsideCoordsToPixels(double key, double value, int region) const
{
  int margin = 10;
  QRect axisRect = mKeyAxis->axisRect() | mValueAxis->axisRect();
  QPointF result = coordsToPixels(key, value);
  switch (region)
  {
    case 2: result.setX(axisRect.left()-margin); break; // left
    case 8: result.setX(axisRect.right()+margin); break; // right
    case 4: result.setY(axisRect.top()-margin); break; // top
    case 6: result.setY(axisRect.bottom()+margin); break; // bottom
    case 1: result.setX(axisRect.left()-margin);
            result.setY(axisRect.top()-margin); break; // top left
    case 7: result.setX(axisRect.right()+margin);
            result.setY(axisRect.top()-margin); break; // top right
    case 9: result.setX(axisRect.right()+margin);
            result.setY(axisRect.bottom()+margin); break; // bottom right
    case 3: result.setX(axisRect.left()-margin);
            result.setY(axisRect.bottom()+margin); break; // bottom left
  }
  return result;
}

/* inherits documentation from base class */
QCPRange QCPCurve::getKeyRange(bool &validRange, SignDomain inSignDomain) const
{
  QCPRange range;
  bool haveLower = false;
  bool haveUpper = false;
  
  double current;
  
  QCPCurveDataMap::const_iterator it = mData->constBegin();
  while (it != mData->constEnd())
  {
    current = it.value().key;
    if (inSignDomain == sdBoth || (inSignDomain == sdNegative && current < 0) || (inSignDomain == sdPositive && current > 0))
    {
      if (current < range.lower || !haveLower)
      {
        range.lower = current;
        haveLower = true;
      }
      if (current > range.upper || !haveUpper)
      {
        range.upper = current;
        haveUpper = true;
      }
    }
    ++it;
  }
  
  validRange = haveLower && haveUpper;
  return range;
}

/* inherits documentation from base class */
QCPRange QCPCurve::getValueRange(bool &validRange, SignDomain inSignDomain) const
{
  QCPRange range;
  bool haveLower = false;
  bool haveUpper = false;
  
  double current;
  
  QCPCurveDataMap::const_iterator it = mData->constBegin();
  while (it != mData->constEnd())
  {
    current = it.value().value;
    if (inSignDomain == sdBoth || (inSignDomain == sdNegative && current < 0) || (inSignDomain == sdPositive && current > 0))
    {
      if (current < range.lower || !haveLower)
      {
        range.lower = current;
        haveLower = true;
      }
      if (current > range.upper || !haveUpper)
      {
        range.upper = current;
        haveUpper = true;
      }
    }
    ++it;
  }
  
  validRange = haveLower && haveUpper;
  return range;
}

// ================================================================================
// =================== QCPBars
// ================================================================================
/*! \class QCPBars
  \brief A plottable representing a bar chart in a plot.

  To plot data, assign it with the \ref setData or \ref addData functions.
  
  \section appearance Changing the appearance
  
  The appearance of the bars is determined by the pen and the brush (\ref setPen, \ref setBrush).
  
  Bar charts are stackable. This means, Two QCPBars plottables can be placed on top of each other
  (see \ref QCPBars::moveAbove). Then, when two bars are at the same key position, they will appear
  stacked.
  
  \section usage Usage
  
  Like all data representing objects in QCustomPlot, the QCPBars is a plottable
  (QCPAbstractPlottable). So the plottable-interface of QCustomPlot applies
  (QCustomPlot::plottable, QCustomPlot::addPlottable, QCustomPlot::removePlottable, etc.)
  
  Usually, you first create an instance:
  \code
  QCPBars *newBars = new QCPBars(customPlot->xAxis, customPlot->yAxis);\endcode
  add it to the customPlot with QCustomPlot::addPlottable:
  \code
  customPlot->addPlottable(newBars);\endcode
  and then modify the properties of the newly created plottable, e.g.:
  \code
  newBars->setName("Country population");
  newBars->setData(xData, yData);\endcode
*/

/*! \fn QCPBars *QCPBars::barBelow() const
  Returns the bars plottable that is directly below this bars plottable.
  If there is no such plottable, returns 0.
  
  \see barAbove, moveBelow, moveAbove
*/

/*! \fn QCPBars *QCPBars::barAbove() const
  Returns the bars plottable that is directly above this bars plottable.
  If there is no such plottable, returns 0.
  
  \see barBelow, moveBelow, moveAbove
*/

/*!
  Constructs a bar chart which uses \a keyAxis as its key axis ("x") and \a valueAxis as its value
  axis ("y"). \a keyAxis and \a valueAxis must reside in the same QCustomPlot instance and not have
  the same orientation. If either of these restrictions is violated, a corresponding message is
  printed to the debug output (qDebug), the construction is not aborted, though.
  
  The constructed QCPBars can be added to the plot with QCustomPlot::addPlottable, QCustomPlot
  then takes ownership of the bar chart.
*/
QCPBars::QCPBars(QCPAxis *keyAxis, QCPAxis *valueAxis) :
  QCPAbstractPlottable(keyAxis, valueAxis),
  mBarBelow(0),
  mBarAbove(0)
{
  mData = new QCPBarDataMap;
  mPen.setColor(Qt::blue);
  mPen.setStyle(Qt::SolidLine);
  mBrush.setColor(QColor(40, 50, 255, 30));
  mBrush.setStyle(Qt::SolidPattern);
  mSelectedPen = mPen;
  mSelectedPen.setWidthF(2.5);
  mSelectedPen.setColor(QColor(80, 80, 255)); // lighter than Qt::blue of mPen
  mSelectedBrush = mBrush;
  
  mWidth = 0.75;
}

QCPBars::~QCPBars()
{
  if (mBarBelow || mBarAbove)
    connectBars(mBarBelow, mBarAbove); // take this bar out of any stacking
  delete mData;
}

/*!
  Sets the width of the bars in plot (key) coordinates.
*/
void QCPBars::setWidth(double width)
{
  mWidth = width;
}

/*!
  Replaces the current data with the provided \a data.
  
  If \a copy is set to true, data points in \a data will only be copied. if false, the plottable
  takes ownership of the passed data and replaces the internal data pointer with it. This is
  significantly faster than copying for large datasets.
*/
void QCPBars::setData(QCPBarDataMap *data, bool copy)
{
  if (copy)
  {
    *mData = *data;
  } else
  {
    delete mData;
    mData = data;
  }
}

/*! \overload
  
  Replaces the current data with the provided points in \a key and \a value tuples. The
  provided vectors should have equal length. Else, the number of added points will be the size of
  the smallest vector.
*/
void QCPBars::setData(const QVector<double> &key, const QVector<double> &value)
{
  mData->clear();
  int n = key.size();
  n = qMin(n, value.size());
  QCPBarData newData;
  for (int i=0; i<n; ++i)
  {
    newData.key = key[i];
    newData.value = value[i];
    mData->insertMulti(newData.key, newData);
  }
}

/*!
  Moves this bars plottable below \a bars. In other words, the bars of this plottable will appear
  below the bars of \a bars. The move target \a bars must use the same key and value axis as this
  plottable.
  
  Inserting into and removing from existing bar stacking is handled gracefully. If \a bars already
  has a bars object below itself, this bars object is inserted between the two. If this bars object
  is already between two other bars, the two other bars will be stacked on top of each other after
  the operation.
  
  To remove this bars plottable from any stacking, set \a bars to 0.
  
  \see moveBelow, barAbove, barBelow
*/
void QCPBars::moveBelow(QCPBars *bars)
{
  if (bars == this) return;
  if (bars->keyAxis() != mKeyAxis || bars->valueAxis() != mValueAxis)
  {
    qDebug() << Q_FUNC_INFO << "passed QCPBars* doesn't have same key and value axis as this QCPBars";
    return;
  }
  // remove from stacking:
  connectBars(mBarBelow, mBarAbove); // Note: also works if one (or both) of them is 0
  // if new bar given, insert this bar below it:
  if (bars)
  {
    if (bars->mBarBelow)
      connectBars(bars->mBarBelow, this);
    connectBars(this, bars);
  }
}

/*!
  Moves this bars plottable above \a bars. In other words, the bars of this plottable will appear
  above the bars of \a bars. The move target \a bars must use the same key and value axis as this
  plottable.
  
  Inserting into and removing from existing bar stacking is handled gracefully. If \a bars already
  has a bars object below itself, this bars object is inserted between the two. If this bars object
  is already between two other bars, the two other bars will be stacked on top of each other after
  the operation.
  
  To remove this bars plottable from any stacking, set \a bars to 0.
  
  \see moveBelow, barBelow, barAbove
*/
void QCPBars::moveAbove(QCPBars *bars)
{
  if (bars == this) return;
  if (bars && (bars->keyAxis() != mKeyAxis || bars->valueAxis() != mValueAxis))
  {
    qDebug() << Q_FUNC_INFO << "passed QCPBars* doesn't have same key and value axis as this QCPBars";
    return;
  }
  // remove from stacking:
  connectBars(mBarBelow, mBarAbove); // Note: also works if one (or both) of them is 0
  // if new bar given, insert this bar above it:
  if (bars)
  {
    if (bars->mBarAbove)
      connectBars(this, bars->mBarAbove);
    connectBars(bars, this);
  }
}

/*!
  Adds the provided data points in \a dataMap to the current data.
  \see removeData
*/
void QCPBars::addData(const QCPBarDataMap &dataMap)
{
  mData->unite(dataMap);
}

/*! \overload
  Adds the provided single data point in \a data to the current data.
  \see removeData
*/
void QCPBars::addData(const QCPBarData &data)
{
  mData->insertMulti(data.key, data);
}

/*! \overload
  Adds the provided single data point as \a key and \a value tuple to the current data
  \see removeData
*/
void QCPBars::addData(double key, double value)
{
  QCPBarData newData;
  newData.key = key;
  newData.value = value;
  mData->insertMulti(newData.key, newData);
}

/*! \overload
  Adds the provided data points as \a key and \a value tuples to the current data.
  \see removeData
*/
void QCPBars::addData(const QVector<double> &keys, const QVector<double> &values)
{
  int n = keys.size();
  n = qMin(n, values.size());
  QCPBarData newData;
  for (int i=0; i<n; ++i)
  {
    newData.key = keys[i];
    newData.value = values[i];
    mData->insertMulti(newData.key, newData);
  }
}

/*!
  Removes all data points with key smaller than \a key.
  \see addData, clearData
*/
void QCPBars::removeDataBefore(double key)
{
  QCPBarDataMap::iterator it = mData->begin();
  while (it != mData->end() && it.key() < key)
    it = mData->erase(it);
}

/*!
  Removes all data points with key greater than \a key.
  \see addData, clearData
*/
void QCPBars::removeDataAfter(double key)
{
  if (mData->isEmpty()) return;
  QCPBarDataMap::iterator it = mData->upperBound(key);
  while (it != mData->end())
    it = mData->erase(it);
}

/*!
  Removes all data points with key between \a fromKey and \a toKey. if \a fromKey is
  greater or equal to \a toKey, the function does nothing. To remove a single data point with known
  key, use \ref removeData(double key).
  
  \see addData, clearData
*/
void QCPBars::removeData(double fromKey, double toKey)
{
  if (fromKey >= toKey || mData->isEmpty()) return;
  QCPBarDataMap::iterator it = mData->upperBound(fromKey);
  QCPBarDataMap::iterator itEnd = mData->upperBound(toKey);
  while (it != itEnd)
    it = mData->erase(it);
}

/*! \overload
  
  Removes a single data point at \a key. If the position is not known with absolute precision,
  consider using \ref removeData(double fromKey, double toKey) with a small fuzziness interval
  around the suspected position, depeding on the precision with which the key is known.
  
  \see addData, clearData
*/
void QCPBars::removeData(double key)
{
  mData->remove(key);
}

/*!
  Removes all data points.
  \see removeData, removeDataAfter, removeDataBefore
*/
void QCPBars::clearData()
{
  mData->clear();
}

/* inherits documentation from base class */
double QCPBars::selectTest(const QPointF &pos) const
{
  QCPBarDataMap::ConstIterator it;
  double posKey, posValue;
  pixelsToCoords(pos, posKey, posValue);
  for (it = mData->constBegin(); it != mData->constEnd(); ++it)
  {
    double baseValue = getBaseValue(it.key(), it.value().value >=0);
    QCPRange keyRange(it.key()-mWidth*0.5, it.key()+mWidth*0.5);
    QCPRange valueRange(baseValue, baseValue+it.value().value);
    if (keyRange.contains(posKey) && valueRange.contains(posValue))
      return mParentPlot->selectionTolerance()*0.99;
  }
  return -1;
}

/* inherits documentation from base class */
void QCPBars::draw(QCPPainter *painter)
{
  if (mData->isEmpty()) return;
  
  QCPBarDataMap::const_iterator it;
  for (it = mData->constBegin(); it != mData->constEnd(); ++it)
  {
    if (it.key()+mWidth*0.5 < mKeyAxis->range().lower || it.key()-mWidth*0.5 > mKeyAxis->range().upper)
      continue;
    QPolygonF barPolygon = getBarPolygon(it.key(), it.value().value);
    // draw bar fill:
    if (mainBrush().style() != Qt::NoBrush && mainBrush().color().alpha() != 0)
    {
      applyFillAntialiasingHint(painter);
      painter->setPen(Qt::NoPen);
      painter->setBrush(mainBrush());
      painter->drawPolygon(barPolygon);
    }
    // draw bar line:
    if (mainPen().style() != Qt::NoPen && mainPen().color().alpha() != 0)
    {
      applyDefaultAntialiasingHint(painter);
      painter->setPen(mainPen());
      painter->setBrush(Qt::NoBrush);
      painter->drawPolyline(barPolygon);
    }
  }
}

/* inherits documentation from base class */
void QCPBars::drawLegendIcon(QCPPainter *painter, const QRect &rect) const
{
  // draw filled rect:
  applyDefaultAntialiasingHint(painter);
  painter->setBrush(mBrush);
  painter->setPen(mPen);
  QRectF r = QRectF(0, 0, rect.width()*0.67, rect.height()*0.67);
  r.moveCenter(rect.center());
  painter->drawRect(r);
}

/*! \internal
  
  Returns the polygon of a single bar with \a key and \a value. The Polygon is open at the bottom
  and shifted according to the bar stacking (see \ref moveAbove).
*/
QPolygonF QCPBars::getBarPolygon(double key, double value) const
{
  QPolygonF result;
  double baseValue = getBaseValue(key, value >= 0);
  result << coordsToPixels(key-mWidth*0.5, baseValue);
  result << coordsToPixels(key-mWidth*0.5, baseValue+value);
  result << coordsToPixels(key+mWidth*0.5, baseValue+value);
  result << coordsToPixels(key+mWidth*0.5, baseValue);
  return result;
}

/*! \internal
  
  This function is called to find at which value to start drawing the base of a bar at \a key, when
  it is stacked on top of another QCPBars (e.g. with \ref moveAbove).
  
  positive and negative bars are separated per stack (positive are stacked above 0-value upwards,
  negative are stacked below 0-value downwards). This can be indicated with \a positive. So if the
  bar for which we need the base value is negative, set \a positive to false.
*/
double QCPBars::getBaseValue(double key, bool positive) const
{
  if (mBarBelow)
  {
    double max = 0;
    // find bars of mBarBelow that are approximately at key and find largest one:
    QCPBarDataMap::const_iterator it = mBarBelow->mData->lowerBound(key-mWidth*0.1);
    QCPBarDataMap::const_iterator itEnd = mBarBelow->mData->upperBound(key+mWidth*0.1);
    while (it != itEnd)
    {
      if ((positive && it.value().value > max) ||
          (!positive && it.value().value < max))
        max = it.value().value;
      ++it;
    }
    // recurse down the bar-stack to find the total height:
    return max + mBarBelow->getBaseValue(key, positive);
  } else
    return 0;
}

/*! \internal

  Connects \a below and \a above to each other via their mBarAbove/mBarBelow properties.
  The bar(s) currently below lower and upper will become disconnected to lower/upper.
  
  If lower is zero, upper will be disconnected at the bottom.
  If upper is zero, lower will be disconnected at the top.
*/
void QCPBars::connectBars(QCPBars *lower, QCPBars *upper)
{
  if (!lower && !upper) return;
  
  if (!lower) // disconnect upper at bottom
  {
    // disconnect old bar below upper:
    if (upper->mBarBelow && upper->mBarBelow->mBarAbove == upper)
      upper->mBarBelow->mBarAbove = 0;
    upper->mBarBelow = 0;
  } else if (!upper) // disconnect lower at top
  {
    // disconnect old bar above lower:
    if (lower->mBarAbove && lower->mBarAbove->mBarBelow == lower)
      lower->mBarAbove->mBarBelow = 0;
    lower->mBarAbove = 0;
  } else // connect lower and upper
  {
    // disconnect old bar above lower:
    if (lower->mBarAbove && lower->mBarAbove->mBarBelow == lower)
      lower->mBarAbove->mBarBelow = 0;
    // disconnect old bar below upper:
    if (upper->mBarBelow && upper->mBarBelow->mBarAbove == upper)
      upper->mBarBelow->mBarAbove = 0;
    lower->mBarAbove = upper;
    upper->mBarBelow = lower;
  }
}

/* inherits documentation from base class */
QCPRange QCPBars::getKeyRange(bool &validRange, SignDomain inSignDomain) const
{
  QCPRange range;
  bool haveLower = false;
  bool haveUpper = false;
  
  double current;
  double barWidthHalf = mWidth*0.5;
  QCPBarDataMap::const_iterator it = mData->constBegin();
  while (it != mData->constEnd())
  {
    current = it.value().key;
    if (inSignDomain == sdBoth || (inSignDomain == sdNegative && current+barWidthHalf < 0) || (inSignDomain == sdPositive && current-barWidthHalf > 0))
    {
      if (current-barWidthHalf < range.lower || !haveLower)
      {
        range.lower = current-barWidthHalf;
        haveLower = true;
      }
      if (current+barWidthHalf > range.upper || !haveUpper)
      {
        range.upper = current+barWidthHalf;
        haveUpper = true;
      }
    }
    ++it;
  }
  
  validRange = haveLower && haveUpper;
  return range;
}

/* inherits documentation from base class */
QCPRange QCPBars::getValueRange(bool &validRange, SignDomain inSignDomain) const
{
  QCPRange range;
  bool haveLower = true; // set to true, because 0 should always be visible in bar charts
  bool haveUpper = true; // set to true, because 0 should always be visible in bar charts
  
  double current;
  
  QCPBarDataMap::const_iterator it = mData->constBegin();
  while (it != mData->constEnd())
  {
    current = it.value().value + getBaseValue(it.value().key, it.value().value >= 0);
    if (inSignDomain == sdBoth || (inSignDomain == sdNegative && current < 0) || (inSignDomain == sdPositive && current > 0))
    {
      if (current < range.lower || !haveLower)
      {
        range.lower = current;
        haveLower = true;
      }
      if (current > range.upper || !haveUpper)
      {
        range.upper = current;
        haveUpper = true;
      }
    }
    ++it;
  }
  
  validRange = range.lower < range.upper;
  return range;
}


// ================================================================================
// =================== QCPStatisticalBox
// ================================================================================

/*! \class QCPStatisticalBox
  \brief A plottable representing a single statistical box in a plot.

  To plot data, assign it with the individual parameter functions or use \ref setData to set all
  parameters at once. The individual funcions are:
  \li \ref setMinimum
  \li \ref setLowerQuartile
  \li \ref setMedian
  \li \ref setUpperQuartile
  \li \ref setMaximum
  
  Additionally you can define a list of outliers, drawn as circle datapoints:
  \li \ref setOutliers
  
  \section appearance Changing the appearance
  
  The appearance of the box itself is controlled via \ref setPen and \ref setBrush. You
  may change the width of the box with \ref setWidth in plot coordinates (not pixels).

  Analog functions exist for the minimum/maximum-whiskers: \ref setWhiskerPen, \ref
  setWhiskerBarPen, \ref setWhiskerWidth. The whisker width is the width of the bar at the top
  (maximum) or bottom (minimum).
  
  The median indicator line has its own pen, \ref setMedianPen.
  
  If the pens are changed, especially the whisker pen, make sure to set the capStyle to
  Qt::FlatCap. Else, e.g. the whisker line might exceed the bar line by a few pixels due to the pen
  cap being not perfectly flat.
  
  The Outlier data points are drawn normal scatter points. Their look can be controlled with \ref
  setOutlierStyle and \ref setOutlierPen. The size (diameter) can be set with \ref setOutlierSize
  in pixels.
  
  \section usage Usage
  
  Like all data representing objects in QCustomPlot, the QCPStatisticalBox is a plottable
  (QCPAbstractPlottable). So the plottable-interface of QCustomPlot applies
  (QCustomPlot::plottable, QCustomPlot::addPlottable, QCustomPlot::removePlottable, etc.)
  
  Usually, you first create an instance:
  \code
  QCPStatisticalBox *newBox = new QCPStatisticalBox(customPlot->xAxis, customPlot->yAxis);\endcode
  add it to the customPlot with QCustomPlot::addPlottable:
  \code
  customPlot->addPlottable(newBox);\endcode
  and then modify the properties of the newly created plottable, e.g.:
  \code
  newBox->setName("Measurement Series 1");
  newBox->setData(1, 3, 4, 5, 7);
  newBox->setOutliers(QVector<double>() << 0.5 << 0.64 << 7.2 << 7.42);\endcode
*/

/*!
  Constructs a statistical box which uses \a keyAxis as its key axis ("x") and \a valueAxis as its
  value axis ("y"). \a keyAxis and \a valueAxis must reside in the same QCustomPlot instance and
  not have the same orientation. If either of these restrictions is violated, a corresponding
  message is printed to the debug output (qDebug), the construction is not aborted, though.
  
  The constructed statistical box can be added to the plot with QCustomPlot::addPlottable,
  QCustomPlot then takes ownership of the statistical box.
*/
QCPStatisticalBox::QCPStatisticalBox(QCPAxis *keyAxis, QCPAxis *valueAxis) :
  QCPAbstractPlottable(keyAxis, valueAxis),
  mKey(0),
  mMinimum(0),
  mLowerQuartile(0),
  mMedian(0),
  mUpperQuartile(0),
  mMaximum(0)
{
  setOutlierStyle(QCP::ssCircle);
  setOutlierSize(5);
  setWhiskerWidth(0.2);
  setWidth(0.5);
  
  setPen(QPen(Qt::black));
  setSelectedPen(QPen(Qt::blue, 2.5));
  setMedianPen(QPen(Qt::black, 3, Qt::SolidLine, Qt::FlatCap));
  setWhiskerPen(QPen(Qt::black, 0, Qt::DashLine, Qt::FlatCap));
  setWhiskerBarPen(QPen(Qt::black));
  setOutlierPen(QPen(Qt::blue));
  setBrush(Qt::NoBrush);
  setSelectedBrush(Qt::NoBrush);
}

QCPStatisticalBox::~QCPStatisticalBox()
{
}

/*!
  Sets the key coordinate of the statistical box.
*/
void QCPStatisticalBox::setKey(double key)
{
  mKey = key;
}

/*!
  Sets the parameter "minimum" of the statistical box plot. This is the position of the lower
  whisker, typically the minimum measurement of the sample that's not considered an outlier.
  
  \see setMaximum, setWhiskerPen, setWhiskerBarPen, setWhiskerWidth
*/
void QCPStatisticalBox::setMinimum(double value)
{
  mMinimum = value;
}

/*!
  Sets the parameter "lower Quartile" of the statistical box plot. This is the lower end of the
  box. The lower and the upper quartiles are the two statistical quartiles around the median of the
  sample, they contain 50% of the sample data.
  
  \see setUpperQuartile, setPen, setBrush, setWidth
*/
void QCPStatisticalBox::setLowerQuartile(double value)
{
  mLowerQuartile = value;
}

/*!
  Sets the parameter "median" of the statistical box plot. This is the value of the median mark
  inside the quartile box. The median separates the sample data in half (50% of the sample data is
  below/above the median).
  
  \see setMedianPen
*/
void QCPStatisticalBox::setMedian(double value)
{
  mMedian = value;
}

/*!
  Sets the parameter "upper Quartile" of the statistical box plot. This is the upper end of the
  box. The lower and the upper quartiles are the two statistical quartiles around the median of the
  sample, they contain 50% of the sample data.
  
  \see setLowerQuartile, setPen, setBrush, setWidth
*/
void QCPStatisticalBox::setUpperQuartile(double value)
{
  mUpperQuartile = value;
}

/*!
  Sets the parameter "maximum" of the statistical box plot. This is the position of the upper
  whisker, typically the maximum measurement of the sample that's not considered an outlier.
  
  \see setMinimum, setWhiskerPen, setWhiskerBarPen, setWhiskerWidth
*/
void QCPStatisticalBox::setMaximum(double value)
{
  mMaximum = value;
}

/*!
  Sets a vector of outlier values that will be drawn as circles. Any data points in the sample that
  are not within the whiskers (\ref setMinimum, \ref setMaximum) should be considered outliers and
  displayed as such.
  
  \see setOutlierPen, setOutlierBrush, setOutlierSize
*/
void QCPStatisticalBox::setOutliers(const QVector<double> &values)
{
  mOutliers = values;
}

/*!
  Sets all parameters of the statistical box plot at once.
  
  \see setKey, setMinimum, setLowerQuartile, setMedian, setUpperQuartile, setMaximum
*/
void QCPStatisticalBox::setData(double key, double minimum, double lowerQuartile, double median, double upperQuartile, double maximum)
{
  setKey(key);
  setMinimum(minimum);
  setLowerQuartile(lowerQuartile);
  setMedian(median);
  setUpperQuartile(upperQuartile);
  setMaximum(maximum);
}

/*!
  Sets the width of the box in key coordinates.
  
  \see setWhiskerWidth
*/
void QCPStatisticalBox::setWidth(double width)
{
  mWidth = width;
}

/*!
  Sets the width of the whiskers (\ref setMinimum, \ref setMaximum) in key coordinates.
  
  \see setWidth
*/
void QCPStatisticalBox::setWhiskerWidth(double width)
{
  mWhiskerWidth = width;
}

/*!
  Sets the pen used for drawing the whisker backbone (That's the line parallel to the value axis).
  
  Make sure to set the \a pen capStyle to Qt::FlatCap to prevent the backbone from reaching a few
  pixels past the bars, when using a non-zero pen width.
  
  \see setWhiskerBarPen
*/
void QCPStatisticalBox::setWhiskerPen(const QPen &pen)
{
  mWhiskerPen = pen;
}

/*!
  Sets the pen used for drawing the whisker bars (Those are the lines parallel to the key axis at
  each end of the backbone).
  
  \see setWhiskerPen
*/
void QCPStatisticalBox::setWhiskerBarPen(const QPen &pen)
{
  mWhiskerBarPen = pen;
}

/*!
  Sets the pen used for drawing the median indicator line inside the statistical box.
  
  Make sure to set the \a pen capStyle to Qt::FlatCap to prevent the median line from reaching a
  few pixels outside the box, when using a non-zero pen width.
*/
void QCPStatisticalBox::setMedianPen(const QPen &pen)
{
  mMedianPen = pen;
}

/*!
  Sets the pixel size of the scatter symbols that represent the outlier data points.
  
  \see setOutlierPen, setOutliers
*/
void QCPStatisticalBox::setOutlierSize(double pixels)
{
  mOutlierSize = pixels;
}

/*!
  Sets the pen used to draw the outlier data points.
  
  \see setOutlierSize, setOutliers
*/
void QCPStatisticalBox::setOutlierPen(const QPen &pen)
{
  mOutlierPen = pen;
}

/*!
  Sets the scatter style of the outlier data points.
  
  \see setOutlierSize, setOutlierPen, setOutliers
*/
void QCPStatisticalBox::setOutlierStyle(QCP::ScatterStyle style)
{
  mOutlierStyle = style;
}

/* inherits documentation from base class */
void QCPStatisticalBox::clearData()
{
  setOutliers(QVector<double>());
  setKey(0);
  setMinimum(0);
  setLowerQuartile(0);
  setMedian(0);
  setUpperQuartile(0);
  setMaximum(0);
}

/* inherits documentation from base class */
double QCPStatisticalBox::selectTest(const QPointF &pos) const
{
  double posKey, posValue;
  pixelsToCoords(pos, posKey, posValue);
  // quartile box:
  QCPRange keyRange(mKey-mWidth*0.5, mKey+mWidth*0.5);
  QCPRange valueRange(mLowerQuartile, mUpperQuartile);
  if (keyRange.contains(posKey) && valueRange.contains(posValue))
    return mParentPlot->selectionTolerance()*0.99;
  
  // min/max whiskers:
  if (QCPRange(mMinimum, mMaximum).contains(posValue))
    return qAbs(mKeyAxis->coordToPixel(mKey)-mKeyAxis->coordToPixel(posKey));
  
  return -1;
}

/* inherits documentation from base class */
void QCPStatisticalBox::draw(QCPPainter *painter)
{
  QRectF quartileBox;
  drawQuartileBox(painter, &quartileBox);
  
  painter->save();
  painter->setClipRect(quartileBox, Qt::IntersectClip);
  drawMedian(painter);
  painter->restore();
  
  drawWhiskers(painter);
  drawOutliers(painter);
}

/* inherits documentation from base class */
void QCPStatisticalBox::drawLegendIcon(QCPPainter *painter, const QRect &rect) const
{
  // draw filled rect:
  applyDefaultAntialiasingHint(painter);
  painter->setPen(mPen);
  painter->setBrush(mBrush);
  QRectF r = QRectF(0, 0, rect.width()*0.67, rect.height()*0.67);
  r.moveCenter(rect.center());
  painter->drawRect(r);
}

/*! \internal
  
  Draws the quartile box. \a box is an output parameter that returns the quartile box (in pixel
  coordinates) which is used to set the clip rect of the painter before calling \ref drawMedian (so
  the median doesn't draw outside the quartile box).
*/
void QCPStatisticalBox::drawQuartileBox(QCPPainter *painter, QRectF *quartileBox) const
{
  QRectF box;
  box.setTopLeft(coordsToPixels(mKey-mWidth*0.5, mUpperQuartile));
  box.setBottomRight(coordsToPixels(mKey+mWidth*0.5, mLowerQuartile));
  applyDefaultAntialiasingHint(painter);
  painter->setPen(mainPen());
  painter->setBrush(mainBrush());
  painter->drawRect(box);
  if (quartileBox)
    *quartileBox = box;
}

/*! \internal
  
  Draws the median line inside the quartile box.
*/
void QCPStatisticalBox::drawMedian(QCPPainter *painter) const
{
  QLineF medianLine;
  medianLine.setP1(coordsToPixels(mKey-mWidth*0.5, mMedian));
  medianLine.setP2(coordsToPixels(mKey+mWidth*0.5, mMedian));
  applyDefaultAntialiasingHint(painter);
  painter->setPen(mMedianPen);
  painter->drawLine(medianLine);
}

/*! \internal
  
  Draws both whisker backbones and bars.
*/
void QCPStatisticalBox::drawWhiskers(QCPPainter *painter) const
{
  QLineF backboneMin, backboneMax, barMin, barMax;
  backboneMax.setPoints(coordsToPixels(mKey, mUpperQuartile), coordsToPixels(mKey, mMaximum));
  backboneMin.setPoints(coordsToPixels(mKey, mLowerQuartile), coordsToPixels(mKey, mMinimum));
  barMax.setPoints(coordsToPixels(mKey-mWhiskerWidth*0.5, mMaximum), coordsToPixels(mKey+mWhiskerWidth*0.5, mMaximum));
  barMin.setPoints(coordsToPixels(mKey-mWhiskerWidth*0.5, mMinimum), coordsToPixels(mKey+mWhiskerWidth*0.5, mMinimum));
  applyErrorBarsAntialiasingHint(painter);
  painter->setPen(mWhiskerPen);
  painter->drawLine(backboneMin);
  painter->drawLine(backboneMax);
  painter->setPen(mWhiskerBarPen);
  painter->drawLine(barMin);
  painter->drawLine(barMax);
}

/*! \internal
  
  Draws the outlier circles.
*/
void QCPStatisticalBox::drawOutliers(QCPPainter *painter) const
{
  applyScattersAntialiasingHint(painter);
  painter->setPen(mOutlierPen);
  painter->setBrush(Qt::NoBrush);
  for (int i=0; i<mOutliers.size(); ++i)
  {
    QPointF dataPoint = coordsToPixels(mKey, mOutliers.at(i));
    painter->drawScatter(dataPoint.x(), dataPoint.y(), mOutlierSize, mOutlierStyle);
  }
}

/* inherits documentation from base class */
QCPRange QCPStatisticalBox::getKeyRange(bool &validRange, SignDomain inSignDomain) const
{
  validRange = mWidth > 0;
  if (inSignDomain == sdBoth)
  {
    return QCPRange(mKey-mWidth*0.5, mKey+mWidth*0.5);
  } else if (inSignDomain == sdNegative)
  {
    if (mKey+mWidth*0.5 < 0)
      return QCPRange(mKey-mWidth*0.5, mKey+mWidth*0.5);
    else if (mKey < 0)
      return QCPRange(mKey-mWidth*0.5, mKey);
    else
    {
      validRange = false;
      return QCPRange();
    }
  } else if (inSignDomain == sdPositive)
  {
    if (mKey-mWidth*0.5 > 0)
      return QCPRange(mKey-mWidth*0.5, mKey+mWidth*0.5);
    else if (mKey > 0)
      return QCPRange(mKey, mKey+mWidth*0.5);
    else
    {
      validRange = false;
      return QCPRange();
    }
  }
  validRange = false;
  return QCPRange();
}

/* inherits documentation from base class */
QCPRange QCPStatisticalBox::getValueRange(bool &validRange, SignDomain inSignDomain) const
{
  if (inSignDomain == sdBoth)
  {
    double lower = qMin(mMinimum, qMin(mMedian, mLowerQuartile));
    double upper = qMax(mMaximum, qMax(mMedian, mUpperQuartile));
    for (int i=0; i<mOutliers.size(); ++i)
    {
      if (mOutliers.at(i) < lower)
        lower = mOutliers.at(i);
      if (mOutliers.at(i) > upper)
        upper = mOutliers.at(i);
    }
    validRange = upper > lower;
    return QCPRange(lower, upper);
  } else
  {
    QVector<double> values; // values that must be considered (i.e. all outliers and the five box-parameters)
    values.reserve(mOutliers.size() + 5);
    values << mMaximum << mUpperQuartile << mMedian << mLowerQuartile << mMinimum;
    values << mOutliers;
    // go through values and find the ones in legal range:
    bool haveUpper = false;
    bool haveLower = false;
    double upper = 0;
    double lower = 0;
    for (int i=0; i<values.size(); ++i)
    {
      if ((inSignDomain == sdNegative && values.at(i) < 0) ||
          (inSignDomain == sdPositive && values.at(i) > 0))
      {
        if (values.at(i) > upper || !haveUpper)
        {
          upper = values.at(i);
          haveUpper = true;
        }
        if (values.at(i) < lower || !haveLower)
        {
          lower = values.at(i);
          haveLower = true;
        }
      }
    }
    // return the bounds if we found some sensible values:
    if (haveLower && haveUpper && lower < upper)
    {
      validRange = true;
      return QCPRange(lower, upper);
    } else
    {
      validRange = false;
      return QCPRange();
    }
  }
}


// ================================================================================
// =================== QCPAbstractItem
// ================================================================================

/*! \class QCPAbstractItem
  \brief The abstract base class for all items in a plot.
  
  In QCustomPlot, items are supplemental graphical elements that are neither plottables
  (QCPAbstractPlottable) nor axes (QCPAxis). While plottables are always tied to two axes and thus
  plot coordinates, items can also be placed in absolute coordinates independent of any axes. Each
  specific item has at least one QCPItemPosition member which controls the positioning. Some items
  are defined by more than one coordinate and thus have two or more QCPItemPosition members (For
  example, QCPItemRect has \a topLeft and \a bottomRight).
  
  This abstract base class defines a very basic interface like visibility and clipping. Since this
  class is abstract, it can't be instantiated. Use one of the subclasses or create a subclass
  yourself to create new items.
  
  The built-in items are:
  <table>
  <tr><td>QCPItemLine</td><td>A line defined by a start and an end point. May have different ending styles on each side (e.g. arrows).</td></tr>
  <tr><td>QCPItemStraightLine</td><td>A straight line defined by a start and a direction point. Unlike QCPItemLine, the straight line is infinitely long and has no endings.</td></tr>
  <tr><td>QCPItemCurve</td><td>A curve defined by start, end and two intermediate control points. May have different ending styles on each side (e.g. arrows).</td></tr>
  <tr><td>QCPItemRect</td><td>A rectangle</td></tr>
  <tr><td>QCPItemEllipse</td><td>An ellipse</td></tr>
  <tr><td>QCPItemPixmap</td><td>An arbitrary pixmap</td></tr>
  <tr><td>QCPItemText</td><td>A text label</td></tr>
  <tr><td>QCPItemBracket</td><td>A bracket which may be used to reference/highlight certain parts in the plot.</td></tr>
  <tr><td>QCPItemTracer</td><td>An item that can be attached to a QCPGraph and sticks to its data points, given a key coordinate.</td></tr>
  </table>
  
  \section items-using Using items
  
  First you instantiate the item you want to use and add it to the plot:
  \code
  QCPItemLine *line = new QCPItemLine(customPlot);
  customPlot->addItem(line);
  \endcode
  by default, the positions of the item are bound to the x- and y-Axis of the plot. So we can just
  set the plot coordinates where the line should start/end:
  \code
  line->start->setCoords(-0.1, 0.8);
  line->end->setCoords(1.1, 0.2);
  \endcode
  If we wanted the line to be positioned not in plot coordinates but a different coordinate system,
  e.g. absolute pixel positions on the QCustomPlot surface, we would have changed the position type
  like this:
  \code
  line->start->setType(QCPItemPosition::ptAbsolute);
  line->end->setType(QCPItemPosition::ptAbsolute);
  \endcode
  Then we can set the coordinates, this time in pixels:
  \code
  line->start->setCoords(100, 200);
  line->end->setCoords(450, 320);
  \endcode
  
  \section items-subclassing Creating own items
  
  To create an own item, you implement a subclass of QCPAbstractItem. These are the pure
  virtual functions, you must implement:
  \li \ref selectTest
  \li \ref draw
  
  See the documentation of those functions for what they need to do.
  
  \subsection items-positioning Allowing the item to be positioned
  
  As mentioned, item positions are represented by QCPItemPosition members. Let's assume the new item shall
  have only one coordinate as its position (as opposed to two like a rect or multiple like a polygon). You then add
  a public member of type QCPItemPosition like so:
  
  \code QCPItemPosition * const myPosition;\endcode
  
  the const makes sure the pointer itself can't be modified from the user of your new item (the QCPItemPosition
  instance it points to, can be modified, of course).
  The initialization of this pointer is made easy with the \ref createPosition function. Just assign
  the return value of this function to each QCPItemPosition in the constructor of your item. \ref createPosition
  takes a string which is the name of the position, typically this is identical to the variable name.
  For example, the constructor of QCPItemExample could look like this:
  
  \code
  QCPItemExample::QCPItemExample(QCustomPlot *parentPlot) :
    QCPAbstractItem(parentPlot),
    myPosition(createPosition("myPosition"))
  {
    // other constructor code
  }
  \endcode
  
  \subsection items-drawing The draw function
  
  Your implementation of the draw function should check whether the item is visible (\a mVisible)
  and then draw the item. You can retrieve its position in pixel coordinates from the position
  member(s) via \ref QCPItemPosition::pixelPoint.

  To optimize performance you should calculate a bounding rect first (don't forget to take the pen
  width into account), check whether it intersects the \ref clipRect, and only draw the item at all
  if this is the case.
  
  \subsection items-selection The selectTest function
  
  Your implementation of the \ref selectTest function may use the helpers \ref distSqrToLine and
  \ref rectSelectTest. With these, the implementation of the selection test becomes significantly
  simpler for most items.
  
  \subsection anchors Providing anchors
  
  Providing anchors (QCPItemAnchor) starts off like adding a position. First you create a public
  member, e.g.
  
  \code QCPItemAnchor * const bottom;\endcode

  and create it in the constructor with the \ref createAnchor function, assigning it a name and an
  anchor id (an integer enumerating all anchors on the item, you may create an own enum for this).
  Since anchors can be placed anywhere, relative to the item's position(s), your item needs to
  provide the position of every anchor with the reimplementation of the \ref anchorPixelPoint(int
  anchorId) function.
  
  In essence the QCPItemAnchor is merely an intermediary that itself asks your item for the pixel
  position when anything attached to the anchor needs to know the coordinates.
*/

/* start of documentation of inline functions */

/*! \fn QList<QCPItemPosition*> QCPAbstractItem::positions() const
  
  Returns all positions of the item in a list.
  
  \see anchors, position
*/

/*! \fn QList<QCPItemAnchor*> QCPAbstractItem::anchors() const
  
  Returns all anchors of the item in a list. Note that since a position (QCPItemPosition) is always
  also an anchor, the list will also contain the positions of this item.
  
  \see positions, anchor
*/

/* end of documentation of inline functions */
/* start documentation of pure virtual functions */

/*! \fn double QCPAbstractItem::selectTest(const QPointF &pos) const = 0
  
  This function is used to decide whether a click hits an item or not.

  \a pos is a point in pixel coordinates on the QCustomPlot surface. This function returns the
  shortest pixel distance of this point to the item. If the item is either invisible or the
  distance couldn't be determined, -1.0 is returned. \ref setSelectable has no influence on the
  return value of this function.

  If the item is represented not by single lines but by an area like QCPItemRect or QCPItemText, a
  click inside the area returns a constant value greater zero (typically 99% of the
  selectionTolerance of the parent QCustomPlot). If the click lies outside the area, this function
  returns -1.0.
  
  Providing a constant value for area objects allows selecting line objects even when they are
  obscured by such area objects, by clicking close to the lines (i.e. closer than
  0.99*selectionTolerance).
  
  The actual setting of the selection state is not done by this function. This is handled by the
  parent QCustomPlot when the mouseReleaseEvent occurs.
  
  \see setSelected, QCustomPlot::setInteractions
*/

/*! \fn void QCPAbstractItem::draw(QCPPainter *painter) = 0
  \internal
  
  Draws this item with the provided \a painter. Called by \ref QCustomPlot::draw on all its
  visible items.
  
  The cliprect of the provided painter is set to the rect returned by \ref clipRect before this
  function is called. For items this depends on the clipping settings defined by \ref
  setClipToAxisRect, \ref setClipKeyAxis and \ref setClipValueAxis.
*/

/* end documentation of pure virtual functions */
/* start documentation of signals */

/*! \fn void QCPAbstractItem::selectionChanged(bool selected)
  This signal is emitted when the selection state of this item has changed, either by user interaction
  or by a direct call to \ref setSelected.
*/

/* end documentation of signals */

/*!
  Base class constructor which initializes base class members.
*/
QCPAbstractItem::QCPAbstractItem(QCustomPlot *parentPlot) :
  QCPLayerable(parentPlot),
  mClipToAxisRect(true),
  mClipKeyAxis(parentPlot->xAxis),
  mClipValueAxis(parentPlot->yAxis),
  mSelectable(true),
  mSelected(false)
{
}

QCPAbstractItem::~QCPAbstractItem()
{
  // don't delete mPositions because every position is also an anchor and thus in mAnchors
  qDeleteAll(mAnchors);
}

/*!
  Sets whether the item shall be clipped to the axis rect or whether it shall be visible on the
  entire QCustomPlot. The axis rect is defined by the clip axes which can be set via \ref
  setClipAxes or individually with \ref setClipKeyAxis and \ref setClipValueAxis.
*/
void QCPAbstractItem::setClipToAxisRect(bool clip)
{
  mClipToAxisRect = clip;
}

/*!
  Sets both clip axes. Together they define the axis rect that will be used to clip the item
  when \ref setClipToAxisRect is set to true.
  
  \see setClipToAxisRect, setClipKeyAxis, setClipValueAxis
*/
void QCPAbstractItem::setClipAxes(QCPAxis *keyAxis, QCPAxis *valueAxis)
{
  mClipKeyAxis = keyAxis;
  mClipValueAxis = valueAxis;
}

/*!
  Sets the clip key axis. Together with the clip value axis it defines the axis rect that will be
  used to clip the item when \ref setClipToAxisRect is set to true.
  
  \see setClipToAxisRect, setClipAxes, setClipValueAxis
*/
void QCPAbstractItem::setClipKeyAxis(QCPAxis *axis)
{
  mClipKeyAxis = axis;
}

/*!
  Sets the clip value axis. Together with the clip key axis it defines the axis rect that will be
  used to clip the item when \ref setClipToAxisRect is set to true.
  
  \see setClipToAxisRect, setClipAxes, setClipKeyAxis
*/
void QCPAbstractItem::setClipValueAxis(QCPAxis *axis)
{
  mClipValueAxis = axis;
}

/*!
  Sets whether the user can (de-)select this item by clicking on the QCustomPlot surface.
  (When \ref QCustomPlot::setInteractions contains QCustomPlot::iSelectItems.)
  
  However, even when \a selectable was set to false, it is possible to set the selection manually,
  by calling \ref setSelected directly.
  
  \see QCustomPlot::setInteractions, setSelected
*/
void QCPAbstractItem::setSelectable(bool selectable)
{
  mSelectable = selectable;
}

/*!
  Sets whether this item is selected or not. When selected, it might use a different visual
  appearance (e.g. pen and brush), this depends on the specific item, though.

  The entire selection mechanism for items is handled automatically when \ref
  QCustomPlot::setInteractions contains QCustomPlot::iSelectItems. You only need to call this function when you
  wish to change the selection state manually.
  
  This function can change the selection state even when \ref setSelectable was set to false.
  
  emits the \ref selectionChanged signal when \a selected is different from the previous selection state.
  
  \see selectTest
*/
void QCPAbstractItem::setSelected(bool selected)
{
  if (mSelected != selected)
  {
    mSelected = selected;
    emit selectionChanged(mSelected);
  }
}

/*!
  Returns the QCPItemPosition with the specified \a name. If this item doesn't have a position by
  that name, returns 0.
  
  This function provides an alternative way to access item positions. Normally, you access
  positions direcly by their member pointers (which typically have the same variable name as \a
  name).
  
  \see positions, anchor 
*/
QCPItemPosition *QCPAbstractItem::position(const QString &name) const
{
  for (int i=0; i<mPositions.size(); ++i)
  {
    if (mPositions.at(i)->name() == name)
      return mPositions.at(i);
  }
  qDebug() << Q_FUNC_INFO << "position with name not found:" << name;
  return 0;
}

/*!
  Returns the QCPItemAnchor with the specified \a name. If this item doesn't have an anchor by
  that name, returns 0.
  
  This function provides an alternative way to access item anchors. Normally, you access
  anchors direcly by their member pointers (which typically have the same variable name as \a
  name).
  
  \see anchors, position 
*/
QCPItemAnchor *QCPAbstractItem::anchor(const QString &name) const
{
  for (int i=0; i<mAnchors.size(); ++i)
  {
    if (mAnchors.at(i)->name() == name)
      return mAnchors.at(i);
  }
  qDebug() << Q_FUNC_INFO << "anchor with name not found:" << name;
  return 0;
}

/*!
  Returns whether this item has an anchor with the specified \a name.
  
  Note that you can check for positions with this function, too, because every position is also an
  anchor (QCPItemPosition inherits from QCPItemAnchor).
  
  \see anchor, position 
*/
bool QCPAbstractItem::hasAnchor(const QString &name) const
{
  for (int i=0; i<mAnchors.size(); ++i)
  {
    if (mAnchors.at(i)->name() == name)
      return true;
  }
  return false;
}

/*! \internal
  
  Returns the rect the visual representation of this item is clipped to. This depends on the
  current setting of \ref setClipToAxisRect aswell as the clip axes set with \ref setClipAxes.
  
  If the item is not clipped to an axis rect, the \ref QCustomPlot::viewport rect is returned.
  
  \see draw
*/
QRect QCPAbstractItem::clipRect() const
{
  if (mClipToAxisRect)
  {
    if (mClipKeyAxis && mClipValueAxis)
      return mClipKeyAxis->axisRect() | mClipValueAxis->axisRect();
    else if (mClipKeyAxis)
      return mClipKeyAxis->axisRect();
    else if (mClipValueAxis)
      return mClipValueAxis->axisRect();
  }
  
  return mParentPlot->viewport();
}

/*! \internal

  A convenience function to easily set the QPainter::Antialiased hint on the provided \a painter
  before drawing item lines.

  This is the antialiasing state the painter passed to the \ref draw method is in by default.
  
  This function takes into account the local setting of the antialiasing flag as well as
  the overrides set e.g. with \ref QCustomPlot::setNotAntialiasedElements.
  
  \see setAntialiased
*/
void QCPAbstractItem::applyDefaultAntialiasingHint(QCPPainter *painter) const
{
  applyAntialiasingHint(painter, mAntialiased, QCP::aeItems);
}

/*! \internal

  Finds the shortest squared distance of \a point to the line segment defined by \a start and \a
  end.
  
  This function may be used to help with the implementation of the \ref selectTest function for
  specific items.
  
  \note This function is identical to QCPAbstractPlottable::distSqrToLine
  
  \see rectSelectTest
*/
double QCPAbstractItem::distSqrToLine(const QPointF &start, const QPointF &end, const QPointF &point) const
{
  QVector2D a(start);
  QVector2D b(end);
  QVector2D p(point);
  QVector2D v(b-a);
  
  double vLengthSqr = v.lengthSquared();
  if (!qFuzzyIsNull(vLengthSqr))
  {
    double mu = QVector2D::dotProduct(p-a, v)/vLengthSqr;
    if (mu < 0)
      return (a-p).lengthSquared();
    else if (mu > 1)
      return (b-p).lengthSquared();
    else
      return ((a + mu*v)-p).lengthSquared();
  } else
    return (a-p).lengthSquared();
}

/*! \internal

  A convenience function which returns the selectTest value for a specified \a rect and a specified
  click position \a pos. \a filledRect defines whether a click inside the rect should also be
  considered a hit or whether only the rect border is sensitive to hits.
  
  This function may be used to help with the implementation of the \ref selectTest function for
  specific items.
  
  For example, if your item consists of four rects, call this function four times, once for each
  rect, in your \ref selectTest reimplementation. Finally, return the minimum of all four returned
  values which were greater or equal to zero. (Because this function may return -1.0 when \a pos
  doesn't hit \a rect at all). If all calls returned -1.0, return -1.0, too, because your item
  wasn't hit.
  
  \see distSqrToLine
*/
double QCPAbstractItem::rectSelectTest(const QRectF &rect, const QPointF &pos, bool filledRect) const
{
  double result = -1;

  // distance to border:
  QList<QLineF> lines;
  lines << QLineF(rect.topLeft(), rect.topRight()) << QLineF(rect.bottomLeft(), rect.bottomRight())
        << QLineF(rect.topLeft(), rect.bottomLeft()) << QLineF(rect.topRight(), rect.bottomRight());
  double minDistSqr = std::numeric_limits<double>::max();
  for (int i=0; i<lines.size(); ++i)
  {
    double distSqr = distSqrToLine(lines.at(i).p1(), lines.at(i).p2(), pos);
    if (distSqr < minDistSqr)
      minDistSqr = distSqr;
  }
  result = qSqrt(minDistSqr);
  
  // filled rect, allow click inside to count as hit:
  if (filledRect && result > mParentPlot->selectionTolerance()*0.99)
  {
    if (rect.contains(pos))
      result = mParentPlot->selectionTolerance()*0.99;
  }
  return result;
}

/*! \internal

  Returns the pixel position of the anchor with Id \a anchorId. This function must be reimplemented in
  item subclasses if they want to provide anchors (QCPItemAnchor).
  
  For example, if the item has two anchors with id 0 and 1, this function takes one of these anchor
  ids and returns the respective pixel points of the specified anchor.
  
  \see createAnchor
*/
QPointF QCPAbstractItem::anchorPixelPoint(int anchorId) const
{
  qDebug() << Q_FUNC_INFO << "called on item which shouldn't have any anchors (anchorPixelPos not reimplemented). anchorId" << anchorId;
  return QPointF();
}

/*! \internal

  Creates a QCPItemPosition, registers it with this item and returns a pointer to it. The specified
  \a name must be a unique string that is usually identical to the variable name of the position
  member (This is needed to provide the name based \ref position access to positions).
  
  Don't delete positions created by this function manually, as the item will take care of it.
  
  Use this function in the constructor (initialization list) of the specific item subclass to
  create each position member. Don't create QCPItemPositions with \b new yourself, because they
  won't be registered with the item properly.
  
  \see createAnchor
*/
QCPItemPosition *QCPAbstractItem::createPosition(const QString &name)
{
  if (hasAnchor(name))
    qDebug() << Q_FUNC_INFO << "anchor/position with name exists already:" << name;
  QCPItemPosition *newPosition = new QCPItemPosition(mParentPlot, this, name);
  mPositions.append(newPosition);
  mAnchors.append(newPosition); // every position is also an anchor
  newPosition->setType(QCPItemPosition::ptPlotCoords);
  newPosition->setAxes(mParentPlot->xAxis, mParentPlot->yAxis);
  newPosition->setCoords(0, 0);
  return newPosition;
}

/*! \internal

  Creates a QCPItemAnchor, registers it with this item and returns a pointer to it. The specified
  \a name must be a unique string that is usually identical to the variable name of the anchor
  member (This is needed to provide the name based \ref anchor access to anchors).
  
  The \a anchorId must be a number identifying the created anchor. It is recommended to create an
  enum (e.g. "AnchorIndex") for this on each item that uses anchors. This id is used by the anchor
  to identify itself when it calls QCPAbstractItem::anchorPixelPoint. That function then returns
  the correct pixel coordinates for the passed anchor id.
  
  Don't delete anchors created by this function manually, as the item will take care of it.
  
  Use this function in the constructor (initialization list) of the specific item subclass to
  create each anchor member. Don't create QCPItemAnchors with \b new yourself, because then they
  won't be registered with the item properly.
  
  \see createPosition
*/
QCPItemAnchor *QCPAbstractItem::createAnchor(const QString &name, int anchorId)
{
  if (hasAnchor(name))
    qDebug() << Q_FUNC_INFO << "anchor/position with name exists already:" << name;
  QCPItemAnchor *newAnchor = new QCPItemAnchor(mParentPlot, this, name, anchorId);
  mAnchors.append(newAnchor);
  return newAnchor;
}


// ================================================================================
// =================== QCPItemPosition
// ================================================================================

/*! \class QCPItemPosition
  \brief Manages the position of an item.
  
  Every item has at least one public QCPItemPosition member pointer which provides ways to position the
  item on the QCustomPlot surface. Some items have multiple positions, for example QCPItemRect has two:
  \a topLeft and \a bottomRight.

  QCPItemPosition has a type (\ref PositionType) that can be set with \ref setType. This type defines
  how coordinates passed to \ref setCoords are to be interpreted, e.g. as absolute pixel coordinates, as
  plot coordinates of certain axes, etc.

  Further, QCPItemPosition may have a parent QCPItemAnchor, see \ref setParentAnchor. (Note that every
  QCPItemPosition inherits from QCPItemAnchor and thus can itself be used as parent anchor for other
  positions.) This way you can tie multiple items together. If the QCPItemPosition has a parent, the
  coordinates set with \ref setCoords are considered to be absolute values in the reference frame of the
  parent anchor, where (0, 0) means directly ontop of the parent anchor. For example, You could attach
  the \a start position of a QCPItemLine to the \a bottom anchor of a QCPItemText to make the starting
  point of the line always be centered under the text label, no matter where the text is moved to, or is
  itself tied to.

  To set the apparent pixel position on the QCustomPlot surface directly, use \ref setPixelPoint. This
  works no matter what type this QCPItemPosition is or what parent-child situation it is in, as \ref
  setPixelPoint transforms the coordinates appropriately, to make the position appear at the specified
  pixel values.
*/

/*!
  Creates a new QCPItemPosition. You shouldn't create QCPItemPosition instances directly, even if
  you want to make a new item subclass. Use \ref QCPAbstractItem::createPosition instead, as
  explained in the subclassing section of the QCPAbstractItem documentation.
*/
QCPItemPosition::QCPItemPosition(QCustomPlot *parentPlot, QCPAbstractItem *parentItem, const QString name) :
  QCPItemAnchor(parentPlot, parentItem, name),
  mPositionType(ptAbsolute),
  mKeyAxis(0),
  mValueAxis(0),
  mKey(0),
  mValue(0),
  mParentAnchor(0)
{
}

QCPItemPosition::~QCPItemPosition()
{
  // unregister as parent at children:
  // Note: this is done in ~QCPItemAnchor again, but it's important QCPItemPosition does it itself, because only then
  //       the setParentAnchor(0) call the correct QCPItemPosition::pixelPos function instead of QCPItemAnchor::pixelPos
  QList<QCPItemPosition*> currentChildren(mChildren.toList());
  for (int i=0; i<currentChildren.size(); ++i)
    currentChildren.at(i)->setParentAnchor(0); // this acts back on this anchor and child removes itself from mChildren
  // unregister as child in parent:
  if (mParentAnchor)
    mParentAnchor->removeChild(this);
}

/*!
  Sets the type of the position. The type defines how the coordinates passed to \ref setCoords
  should be handled and how the QCPItemPosition should behave in the plot. Note that the position
  type \ref ptPlotCoords is only available (and sensible) when the position has no parent anchor
  (\ref setParentAnchor).
  
  The possible values for \a type can be separated in two main categories:

  \li The position is regarded as a point in plot coordinates. This corresponds to \ref ptPlotCoords
  and requires two axes that define the plot coordinate system. They can be specified with \ref setAxes.
  By default, the QCustomPlot's x- and yAxis are used.
  
  \li The position is fixed on the QCustomPlot surface, i.e. independant of axis ranges. This
  corresponds to all other types, i.e. \ref ptAbsolute, \ref ptViewportRatio and \ref ptAxisRectRatio. They
  differ only in the way the absolute position is described, see the documentation of PositionType
  for details.
  
  \note If the type is changed, the apparent pixel position on the plot is preserved. This means
  the coordinates as retrieved with coords() and set with \ref setCoords may change in the process.
*/
void QCPItemPosition::setType(QCPItemPosition::PositionType type)
{
  if (mPositionType != type)
  {
    QPointF pixelP = pixelPoint();
    mPositionType = type;
    setPixelPoint(pixelP);
  }
}

/*!
  Sets the parent of this QCPItemPosition to \a parentAnchor. This means the position will now
  follow any position changes of the anchor. The local coordinate system of positions with a parent
  anchor always is absolute with (0, 0) being exactly on top of the parent anchor. (Hence the type
  shouldn't be \ref ptPlotCoords for positions with parent anchors.)
  
  if \a keepPixelPosition is true, the current pixel position of the QCPItemPosition is preserved
  during reparenting. If it's set to false, the coordinates are set to (0, 0), i.e. the position
  will be exactly on top of the parent anchor.
  
  To remove this QCPItemPosition from any parent anchor, set \a parentAnchor to 0.
  
  \note If the QCPItemPosition previously had no parent and the type is \ref ptPlotCoords, the type
  is set to \ref ptAbsolute, to keep the position in a valid state.
*/
bool QCPItemPosition::setParentAnchor(QCPItemAnchor *parentAnchor, bool keepPixelPosition)
{
  // make sure self is not assigned as parent:
  if (parentAnchor == this)
  {
    qDebug() << Q_FUNC_INFO << "can't set self as parent anchor" << reinterpret_cast<quintptr>(parentAnchor);
    return false;
  }
  // make sure no recursive parent-child-relationships are created:
  QCPItemAnchor *currentParent = parentAnchor;
  while (currentParent)
  {
    if (QCPItemPosition *currentParentPos = dynamic_cast<QCPItemPosition*>(currentParent))
    {
      // is a QCPItemPosition, might have further parent, so keep iterating
      if (currentParentPos == this)
      {
        qDebug() << Q_FUNC_INFO << "can't create recursive parent-child-relationship" << reinterpret_cast<quintptr>(parentAnchor);
        return false;
      }
      currentParent = currentParentPos->mParentAnchor;
    } else
    {
      // is a QCPItemAnchor, can't have further parent, so just compare parent items
      if (currentParent->mParentItem == mParentItem)
      {
        qDebug() << Q_FUNC_INFO << "can't create recursive parent-child-relationship" << reinterpret_cast<quintptr>(parentAnchor);
        return false;
      }
      break;
    }
  }
  
  // if previously no parent set and PosType is still ptPlotCoords, set to ptAbsolute:
  if (!mParentAnchor && mPositionType == ptPlotCoords)
    setType(ptAbsolute);
  
  // save pixel position:
  QPointF pixelP;
  if (keepPixelPosition)
    pixelP = pixelPoint();
  // unregister at current parent anchor:
  if (mParentAnchor)
    mParentAnchor->removeChild(this);
  // register at new parent anchor:
  if (parentAnchor)
    parentAnchor->addChild(this);
  mParentAnchor = parentAnchor;
  // restore pixel position under new parent:
  if (keepPixelPosition)
    setPixelPoint(pixelP);
  else
    setCoords(0, 0);
  return true;
}

/*!
  Sets the coordinates of this QCPItemPosition. What the coordinates mean, is defined by the type
  (\ref setType).
  
  For example, if the type is \ref ptAbsolute, \a key and \a value mean the x and y pixel position
  on the QCustomPlot surface where the origin (0, 0) is in the top left corner of the QCustomPlot
  viewport. If the type is \ref ptPlotCoords, \a key and \a value mean a point in the plot
  coordinate system defined by the axes set by \ref setAxes. (By default the QCustomPlot's x- and
  yAxis.)

  \see setPixelPoint
*/
void QCPItemPosition::setCoords(double key, double value)
{
  mKey = key;
  mValue = value;
}

/*! \overload

  Sets the coordinates as a QPointF \a pos where pos.x has the meaning of \a key and pos.y the
  meaning of \a value of the \ref setCoords(double key, double value) function.
*/
void QCPItemPosition::setCoords(const QPointF &pos)
{
  setCoords(pos.x(), pos.y());
}

/*!
  Returns the final absolute pixel position of the QCPItemPosition on the QCustomPlot surface. It
  includes all effects of type (\ref setType) and possible parent anchors (\ref setParentAnchor).

  \see setPixelPoint
*/
QPointF QCPItemPosition::pixelPoint() const
{
  switch (mPositionType)
  {
    case ptAbsolute:
    {
      if (mParentAnchor)
        return QPointF(mKey, mValue) + mParentAnchor->pixelPoint();
      else
        return QPointF(mKey, mValue);
    }
    
    case ptViewportRatio:
    {
      if (mParentAnchor)
      {
        return QPointF(mKey*mParentPlot->viewport().width(),
                       mValue*mParentPlot->viewport().height()) + mParentAnchor->pixelPoint();
      } else
      {
        return QPointF(mKey*mParentPlot->viewport().width(),
                       mValue*mParentPlot->viewport().height()) + mParentPlot->viewport().topLeft();
      }
    }
      
    case ptAxisRectRatio:
    {
      if (mParentAnchor)
      {
        return QPointF(mKey*mParentPlot->axisRect().width(),
                       mValue*mParentPlot->axisRect().height()) + mParentAnchor->pixelPoint();
      } else
      {
        return QPointF(mKey*mParentPlot->axisRect().width(),
                       mValue*mParentPlot->axisRect().height()) + mParentPlot->axisRect().topLeft();
      }
    }
    
    case ptPlotCoords:
    {
      double x, y;
      if (mKeyAxis && mValueAxis)
      {
        // both key and value axis are given, translate key/value to x/y coordinates:
        if (mKeyAxis->orientation() == Qt::Horizontal)
        {
          x = mKeyAxis->coordToPixel(mKey);
          y = mValueAxis->coordToPixel(mValue);
        } else
        {
          y = mKeyAxis->coordToPixel(mKey);
          x = mValueAxis->coordToPixel(mValue);
        }
      } else if (mKeyAxis)
      {
        // only key axis is given, depending on orientation only transform x or y to key coordinate, other stays pixel:
        if (mKeyAxis->orientation() == Qt::Horizontal)
        {
          x = mKeyAxis->coordToPixel(mKey);
          y = mValue;
        } else
        {
          y = mKeyAxis->coordToPixel(mKey);
          x = mValue;
        }
      } else if (mValueAxis)
      {
        // only value axis is given, depending on orientation only transform x or y to value coordinate, other stays pixel:
        if (mValueAxis->orientation() == Qt::Horizontal)
        {
          x = mValueAxis->coordToPixel(mValue);
          y = mKey;
        } else
        {
          y = mValueAxis->coordToPixel(mValue);
          x = mKey;
        }
      } else
      {
        // no axis given, basically the same as if mAnchorType were atNone
        x = mKey;
        y = mValue;
      }
      return QPointF(x, y);
    }
  }
  return QPointF();
}

/*!
  When \ref setType is ptPlotCoords, this function may be used to specify the axes the coordinates set
  with \ref setCoords relate to.
*/
void QCPItemPosition::setAxes(QCPAxis *keyAxis, QCPAxis *valueAxis)
{
  mKeyAxis = keyAxis;
  mValueAxis = valueAxis;
}

/*!
  Sets the apparent pixel position. This works no matter what type this QCPItemPosition is or what
  parent-child situation it is in, as \ref setPixelPoint transforms the coordinates appropriately, to
  make the position appear at the specified pixel values.

  Only if the type is \ref ptAbsolute and no parent anchor is set, this function is identical to \ref
  setCoords.

  \see setCoords
*/
void QCPItemPosition::setPixelPoint(const QPointF &pixelPoint)
{
  switch (mPositionType)
  {
    case ptAbsolute:
    {
      if (mParentAnchor)
        setCoords(pixelPoint-mParentAnchor->pixelPoint());
      else
        setCoords(pixelPoint);
      break;
    }
      
    case ptViewportRatio:
    {
      if (mParentAnchor)
      {
        QPointF p(pixelPoint-mParentAnchor->pixelPoint());
        p.rx() /= (double)mParentPlot->viewport().width();
        p.ry() /= (double)mParentPlot->viewport().height();
        setCoords(p);
      } else
      {
        QPointF p(pixelPoint-mParentPlot->viewport().topLeft());
        p.rx() /= (double)mParentPlot->viewport().width();
        p.ry() /= (double)mParentPlot->viewport().height();
        setCoords(p);
      }
      break;
    }
      
    case ptAxisRectRatio:
    {
      if (mParentAnchor)
      {
        QPointF p(pixelPoint-mParentAnchor->pixelPoint());
        p.rx() /= (double)mParentPlot->axisRect().width();
        p.ry() /= (double)mParentPlot->axisRect().height();
        setCoords(p);
      } else
      {
        QPointF p(pixelPoint-mParentPlot->axisRect().topLeft());
        p.rx() /= (double)mParentPlot->axisRect().width();
        p.ry() /= (double)mParentPlot->axisRect().height();
        setCoords(p);
      }
      break;
    }
      
    case ptPlotCoords:
    {
      double newKey, newValue;
      if (mKeyAxis && mValueAxis)
      {
        // both key and value axis are given, translate point to key/value coordinates:
        if (mKeyAxis->orientation() == Qt::Horizontal)
        {
          newKey = mKeyAxis->pixelToCoord(pixelPoint.x());
          newValue = mValueAxis->pixelToCoord(pixelPoint.y());
        } else
        {
          newKey = mKeyAxis->pixelToCoord(pixelPoint.y());
          newValue = mValueAxis->pixelToCoord(pixelPoint.x());
        }
      } else if (mKeyAxis)
      {
        // only key axis is given, depending on orientation only transform x or y to key coordinate, other stays pixel:
        if (mKeyAxis->orientation() == Qt::Horizontal)
        {
          newKey = mKeyAxis->pixelToCoord(pixelPoint.x());
          newValue = pixelPoint.y();
        } else
        {
          newKey = mKeyAxis->pixelToCoord(pixelPoint.y());
          newValue = pixelPoint.x();
        }
      } else if (mValueAxis)
      {
        // only value axis is given, depending on orientation only transform x or y to value coordinate, other stays pixel:
        if (mValueAxis->orientation() == Qt::Horizontal)
        {
          newKey = pixelPoint.y();
          newValue = mValueAxis->pixelToCoord(pixelPoint.x());
        } else
        {
          newKey = pixelPoint.x();
          newValue = mValueAxis->pixelToCoord(pixelPoint.y());
        }
      } else
      {
        // no axis given, basically the same as if mAnchorType were atNone
        newKey = pixelPoint.x();
        newValue = pixelPoint.y();
      }
      setCoords(newKey, newValue);
      break;
    }
  }
}


// ================================================================================
// =================== QCPItemStraightLine
// ================================================================================

/*! \class QCPItemStraightLine
  \brief A straight line that spans infinitely in both directions

  \image html QCPItemStraightLine.png "Straight line example. Blue dotted circles are anchors, solid blue discs are positions."

  It has two positions, \a point1 and \a point2, which define the straight line.
*/

/*!
  Creates a straight line item and sets default values.
  
  The constructed item can be added to the plot with QCustomPlot::addItem.
*/
QCPItemStraightLine::QCPItemStraightLine(QCustomPlot *parentPlot) :
  QCPAbstractItem(parentPlot),
  point1(createPosition("point1")),
  point2(createPosition("point2"))
{
  point1->setCoords(0, 0);
  point2->setCoords(1, 1);
  
  setPen(QPen(Qt::black));
  setSelectedPen(QPen(Qt::blue,2));
}

QCPItemStraightLine::~QCPItemStraightLine()
{
}

/*!
  Sets the pen that will be used to draw the line
  
  \see setSelectedPen
*/
void QCPItemStraightLine::setPen(const QPen &pen)
{
  mPen = pen;
}

/*!
  Sets the pen that will be used to draw the line when selected
  
  \see setPen, setSelected
*/
void QCPItemStraightLine::setSelectedPen(const QPen &pen)
{
  mSelectedPen = pen;
}

/* inherits documentation from base class */
double QCPItemStraightLine::selectTest(const QPointF &pos) const
{
  if (!mVisible)
    return -1;
  
  return distToStraightLine(QVector2D(point1->pixelPoint()), QVector2D(point2->pixelPoint()-point1->pixelPoint()), QVector2D(pos));
}

/* inherits documentation from base class */
void QCPItemStraightLine::draw(QCPPainter *painter)
{
  QVector2D start(point1->pixelPoint());
  QVector2D end(point2->pixelPoint());
  // get visible segment of straight line inside clipRect:
  double clipPad = mainPen().widthF();
  QLineF line = getRectClippedStraightLine(start, end-start, clipRect().adjusted(-clipPad, -clipPad, clipPad, clipPad));
  // paint visible segment, if existent:
  if (!line.isNull())
  {
    painter->setPen(mainPen());
    painter->drawLine(line);
  }
}

/*! \internal

  finds the shortest distance of \a point to the straight line defined by the base point \a
  base and the direction vector \a vec.
  
  This is a helper function for \ref selectTest.
*/
double QCPItemStraightLine::distToStraightLine(const QVector2D &base, const QVector2D &vec, const QVector2D &point) const
{
  return qAbs((base.y()-point.y())*vec.x()-(base.x()-point.x())*vec.y())/vec.length();
}

/*! \internal

  Returns the section of the straight line defined by \a base and direction vector \a
  vec, that is visible in the specified \a rect.
  
  This is a helper function for \ref draw.
*/
QLineF QCPItemStraightLine::getRectClippedStraightLine(const QVector2D &base, const QVector2D &vec, const QRect &rect) const
{
  double bx, by;
  double gamma;
  QLineF result;
  if (vec.x() == 0 && vec.y() == 0)
    return result;
  if (qFuzzyIsNull(vec.x())) // line is vertical
  {
    // check top of rect:
    bx = rect.left();
    by = rect.top();
    gamma = base.x()-bx + (by-base.y())*vec.x()/vec.y();
    if (gamma >= 0 && gamma <= rect.width())
      result.setLine(bx+gamma, rect.top(), bx+gamma, rect.bottom()); // no need to check bottom because we know line is vertical
  } else if (qFuzzyIsNull(vec.y())) // line is horizontal
  {
    // check left of rect:
    bx = rect.left();
    by = rect.top();
    gamma = base.y()-by + (bx-base.x())*vec.y()/vec.x();
    if (gamma >= 0 && gamma <= rect.height())
      result.setLine(rect.left(), by+gamma, rect.right(), by+gamma); // no need to check right because we know line is horizontal
  } else // line is skewed
  {
    QList<QVector2D> pointVectors;
    // check top of rect:
    bx = rect.left();
    by = rect.top();
    gamma = base.x()-bx + (by-base.y())*vec.x()/vec.y();
    if (gamma >= 0 && gamma <= rect.width())
      pointVectors.append(QVector2D(bx+gamma, by));
    // check bottom of rect:
    bx = rect.left();
    by = rect.bottom();
    gamma = base.x()-bx + (by-base.y())*vec.x()/vec.y();
    if (gamma >= 0 && gamma <= rect.width())
      pointVectors.append(QVector2D(bx+gamma, by));
    // check left of rect:
    bx = rect.left();
    by = rect.top();
    gamma = base.y()-by + (bx-base.x())*vec.y()/vec.x();
    if (gamma >= 0 && gamma <= rect.height())
      pointVectors.append(QVector2D(bx, by+gamma));
    // check right of rect:
    bx = rect.right();
    by = rect.top();
    gamma = base.y()-by + (bx-base.x())*vec.y()/vec.x();
    if (gamma >= 0 && gamma <= rect.height())
      pointVectors.append(QVector2D(bx, by+gamma));
    
    // evaluate points:
    if (pointVectors.size() == 2)
    {
      result.setPoints(pointVectors.at(0).toPointF(), pointVectors.at(1).toPointF());
    } else if (pointVectors.size() > 2)
    {
      // line probably goes through corner of rect, and we got two points there. single out the point pair with greatest distance:
      double distSqrMax = 0;
      QVector2D pv1, pv2;
      for (int i=0; i<pointVectors.size()-1; ++i)
      {
        for (int k=i+1; k<pointVectors.size(); ++k)
        {
          double distSqr = (pointVectors.at(i)-pointVectors.at(k)).lengthSquared();
          if (distSqr > distSqrMax)
          {
            pv1 = pointVectors.at(i);
            pv2 = pointVectors.at(k);
            distSqrMax = distSqr;
          }
        }
      }
      result.setPoints(pv1.toPointF(), pv2.toPointF());
    }
  }
  return result;
}

/*! \internal

  Returns the pen that should be used for drawing lines. Returns mPen when the
  item is not selected and mSelectedPen when it is.
*/
QPen QCPItemStraightLine::mainPen() const
{
  return mSelected ? mSelectedPen : mPen;
}


// ================================================================================
// =================== QCPItemLine
// ================================================================================

/*! \class QCPItemLine
  \brief A line from one point to another

  \image html QCPItemLine.png "Line example. Blue dotted circles are anchors, solid blue discs are positions."

  It has two positions, \a start and \a end, which define the end points of the line.
  
  With \ref setHead and \ref setTail you may set different line ending styles, e.g. to create an arrow.
*/

/*!
  Creates a line item and sets default values.
  
  The constructed item can be added to the plot with QCustomPlot::addItem.
*/
QCPItemLine::QCPItemLine(QCustomPlot *parentPlot) :
  QCPAbstractItem(parentPlot),
  start(createPosition("start")),
  end(createPosition("end"))
{
  start->setCoords(0, 0);
  end->setCoords(1, 1);
  
  setPen(QPen(Qt::black));
  setSelectedPen(QPen(Qt::blue,2));
}

QCPItemLine::~QCPItemLine()
{
}

/*!
  Sets the pen that will be used to draw the line
  
  \see setSelectedPen
*/
void QCPItemLine::setPen(const QPen &pen)
{
  mPen = pen;
}

/*!
  Sets the pen that will be used to draw the line when selected
  
  \see setPen, setSelected
*/
void QCPItemLine::setSelectedPen(const QPen &pen)
{
  mSelectedPen = pen;
}

/*!
  Sets the line ending style of the head. The head corresponds to the \a end position.
  
  Note that due to the overloaded QCPLineEnding constructor, you may directly specify
  a QCPLineEnding::EndingStyle here, e.g. \code setHead(QCPLineEnding::esSpikeArrow) \endcode
  
  \see setTail
*/
void QCPItemLine::setHead(const QCPLineEnding &head)
{
  mHead = head;
}

/*!
  Sets the line ending style of the tail. The tail corresponds to the \a start position.
  
  Note that due to the overloaded QCPLineEnding constructor, you may directly specify
  a QCPLineEnding::EndingStyle here, e.g. \code setTail(QCPLineEnding::esSpikeArrow) \endcode
  
  \see setHead
*/
void QCPItemLine::setTail(const QCPLineEnding &tail)
{
  mTail = tail;
}

/* inherits documentation from base class */
double QCPItemLine::selectTest(const QPointF &pos) const
{
  if (!mVisible)
    return -1;
  
  return qSqrt(distSqrToLine(start->pixelPoint(), end->pixelPoint(), pos));
}

/* inherits documentation from base class */
void QCPItemLine::draw(QCPPainter *painter)
{
  QVector2D startVec(start->pixelPoint());
  QVector2D endVec(end->pixelPoint());
  if (startVec.toPoint() == endVec.toPoint())
    return;
  // get visible segment of straight line inside clipRect:
  double clipPad = qMax(mHead.boundingDistance(), mTail.boundingDistance());
  clipPad = qMax<double>(clipPad, mainPen().widthF());
  QLineF line = getRectClippedLine(startVec, endVec, clipRect().adjusted(-clipPad, -clipPad, clipPad, clipPad));
  // paint visible segment, if existent:
  if (!line.isNull())
  {
    painter->setPen(mainPen());
    painter->drawLine(line);
    painter->setBrush(Qt::SolidPattern);
    if (mTail.style() != QCPLineEnding::esNone)
      mTail.draw(painter, startVec, startVec-endVec);
    if (mHead.style() != QCPLineEnding::esNone)
      mHead.draw(painter, endVec, endVec-startVec);
  }
}

/*! \internal

  Returns the section of the line defined by \a start and \a end, that is visible in the specified
  \a rect.
  
  This is a helper function for \ref draw.
*/
QLineF QCPItemLine::getRectClippedLine(const QVector2D &start, const QVector2D &end, const QRect &rect) const
{
  bool containsStart = rect.contains(start.x(), start.y());
  bool containsEnd = rect.contains(end.x(), end.y());
  if (containsStart && containsEnd)
    return QLineF(start.toPointF(), end.toPointF());
  
  QVector2D base = start;
  QVector2D vec = end-start;
  double bx, by;
  double gamma, mu;
  QLineF result;
  QList<QVector2D> pointVectors;

  if (!qFuzzyIsNull(vec.y())) // line is not horizontal
  {
    // check top of rect:
    bx = rect.left();
    by = rect.top();
    mu = (by-base.y())/vec.y();
    if (mu >= 0 && mu <= 1)
    {
      gamma = base.x()-bx + mu*vec.x();
      if (gamma >= 0 && gamma <= rect.width())
        pointVectors.append(QVector2D(bx+gamma, by));
    }
    // check bottom of rect:
    bx = rect.left();
    by = rect.bottom();
    mu = (by-base.y())/vec.y();
    if (mu >= 0 && mu <= 1)
    {
      gamma = base.x()-bx + mu*vec.x();
      if (gamma >= 0 && gamma <= rect.width())
        pointVectors.append(QVector2D(bx+gamma, by));
    }
  }
  if (!qFuzzyIsNull(vec.x())) // line is not vertical
  {
    // check left of rect:
    bx = rect.left();
    by = rect.top();
    mu = (bx-base.x())/vec.x();
    if (mu >= 0 && mu <= 1)
    {
      gamma = base.y()-by + mu*vec.y();
      if (gamma >= 0 && gamma <= rect.height())
        pointVectors.append(QVector2D(bx, by+gamma));
    }
    // check right of rect:
    bx = rect.right();
    by = rect.top();
    mu = (bx-base.x())/vec.x();
    if (mu >= 0 && mu <= 1)
    {
      gamma = base.y()-by + mu*vec.y();
      if (gamma >= 0 && gamma <= rect.height())
        pointVectors.append(QVector2D(bx, by+gamma));
    }
  }
  
  if (containsStart)
    pointVectors.append(start);
  if (containsEnd)
    pointVectors.append(end);
  
  // evaluate points:
  if (pointVectors.size() == 2)
  {
    result.setPoints(pointVectors.at(0).toPointF(), pointVectors.at(1).toPointF());
  } else if (pointVectors.size() > 2)
  {
    // line probably goes through corner of rect, and we got two points there. single out the point pair with greatest distance:
    double distSqrMax = 0;
    QVector2D pv1, pv2;
    for (int i=0; i<pointVectors.size()-1; ++i)
    {
      for (int k=i+1; k<pointVectors.size(); ++k)
      {
        double distSqr = (pointVectors.at(i)-pointVectors.at(k)).lengthSquared();
        if (distSqr > distSqrMax)
        {
          pv1 = pointVectors.at(i);
          pv2 = pointVectors.at(k);
          distSqrMax = distSqr;
        }
      }
    }
    result.setPoints(pv1.toPointF(), pv2.toPointF());
  }
  return result;
}

/*! \internal

  Returns the pen that should be used for drawing lines. Returns mPen when the
  item is not selected and mSelectedPen when it is.
*/
QPen QCPItemLine::mainPen() const
{
  return mSelected ? mSelectedPen : mPen;
}


// ================================================================================
// =================== QCPItemEllipse
// ================================================================================

/*! \class QCPItemEllipse
  \brief An ellipse

  \image html QCPItemEllipse.png "Ellipse example. Blue dotted circles are anchors, solid blue discs are positions."

  It has two positions, \a topLeft and \a bottomRight, which define the rect the ellipse will be drawn in.
*/

/*!
  Creates an ellipse item and sets default values.
  
  The constructed item can be added to the plot with QCustomPlot::addItem.
*/
QCPItemEllipse::QCPItemEllipse(QCustomPlot *parentPlot) :
  QCPAbstractItem(parentPlot),
  topLeft(createPosition("topLeft")),
  bottomRight(createPosition("bottomRight")),
  topLeftRim(createAnchor("topLeftRim", aiTopLeftRim)),
  top(createAnchor("top", aiTop)),
  topRightRim(createAnchor("topRightRim", aiTopRightRim)),
  right(createAnchor("right", aiRight)),
  bottomRightRim(createAnchor("bottomRightRim", aiBottomRightRim)),
  bottom(createAnchor("bottom", aiBottom)),
  bottomLeftRim(createAnchor("bottomLeftRim", aiBottomLeftRim)),
  left(createAnchor("left", aiLeft))
{
  topLeft->setCoords(0, 1);
  bottomRight->setCoords(1, 0);
  
  setPen(QPen(Qt::black));
  setSelectedPen(QPen(Qt::blue, 2));
  setBrush(Qt::NoBrush);
  setSelectedBrush(Qt::NoBrush);
}

QCPItemEllipse::~QCPItemEllipse()
{
}

/*!
  Sets the pen that will be used to draw the line of the ellipse
  
  \see setSelectedPen, setBrush
*/
void QCPItemEllipse::setPen(const QPen &pen)
{
  mPen = pen;
}

/*!
  Sets the pen that will be used to draw the line of the ellipse when selected
  
  \see setPen, setSelected
*/
void QCPItemEllipse::setSelectedPen(const QPen &pen)
{
  mSelectedPen = pen;
}

/*!
  Sets the brush that will be used to fill the ellipse. To disable filling, set \a brush to
  Qt::NoBrush.
  
  \see setSelectedBrush, setPen
*/
void QCPItemEllipse::setBrush(const QBrush &brush)
{
  mBrush = brush;
}

/*!
  Sets the brush that will be used to fill the ellipse when selected. To disable filling, set \a
  brush to Qt::NoBrush.
  
  \see setBrush
*/
void QCPItemEllipse::setSelectedBrush(const QBrush &brush)
{
  mSelectedBrush = brush;
}

/* inherits documentation from base class */
double QCPItemEllipse::selectTest(const QPointF &pos) const
{
  double result = -1;
  QPointF p1 = topLeft->pixelPoint();
  QPointF p2 = bottomRight->pixelPoint();
  QPointF center((p1+p2)/2.0);
  double a = qAbs(p1.x()-p2.x())/2.0;
  double b = qAbs(p1.y()-p2.y())/2.0;
  double x = pos.x()-center.x();
  double y = pos.y()-center.y();
  
  // distance to border:
  double c = 1.0/qSqrt(x*x/(a*a)+y*y/(b*b));
  result = qAbs(c-1)*qSqrt(x*x+y*y);
  // filled ellipse, allow click inside to count as hit:
  if (result > mParentPlot->selectionTolerance()*0.99 && mBrush.style() != Qt::NoBrush && mBrush.color().alpha() != 0)
  {
    if (x*x/(a*a) + y*y/(b*b) <= 1)
      result = mParentPlot->selectionTolerance()*0.99;
  }
  return result;
}

/* inherits documentation from base class */
void QCPItemEllipse::draw(QCPPainter *painter)
{
  QPointF p1 = topLeft->pixelPoint();
  QPointF p2 = bottomRight->pixelPoint();
  if (p1.toPoint() == p2.toPoint())
    return;
  QRectF ellipseRect = QRectF(p1, p2).normalized();
  QRect clip = clipRect().adjusted(-mainPen().widthF(), -mainPen().widthF(), mainPen().widthF(), mainPen().widthF());
  if (ellipseRect.intersects(clip)) // only draw if bounding rect of ellipse is visible in cliprect
  {
    painter->setPen(mainPen());
    painter->setBrush(mainBrush());
    try
    {
      painter->drawEllipse(ellipseRect);
    } catch (...)
    {
      qDebug() << Q_FUNC_INFO << "Item too large for memory, setting invisible";
      setVisible(false);
    }
  }
}

/* inherits documentation from base class */
QPointF QCPItemEllipse::anchorPixelPoint(int anchorId) const
{
  QRectF rect = QRectF(topLeft->pixelPoint(), bottomRight->pixelPoint());
  switch (anchorId)
  {
    case aiTopLeftRim:     return rect.center()+(rect.topLeft()-rect.center())*1/qSqrt(2);
    case aiTop:            return (rect.topLeft()+rect.topRight())*0.5;
    case aiTopRightRim:    return rect.center()+(rect.topRight()-rect.center())*1/qSqrt(2);
    case aiRight:          return (rect.topRight()+rect.bottomRight())*0.5;
    case aiBottomRightRim: return rect.center()+(rect.bottomRight()-rect.center())*1/qSqrt(2);
    case aiBottom:         return (rect.bottomLeft()+rect.bottomRight())*0.5;
    case aiBottomLeftRim:  return rect.center()+(rect.bottomLeft()-rect.center())*1/qSqrt(2);
    case aiLeft:           return (rect.topLeft()+rect.bottomLeft())*0.5;;
  }
  
  qDebug() << Q_FUNC_INFO << "invalid anchorId" << anchorId;
  return QPointF();
}

/*! \internal

  Returns the pen that should be used for drawing lines. Returns mPen when the item is not selected
  and mSelectedPen when it is.
*/
QPen QCPItemEllipse::mainPen() const
{
  return mSelected ? mSelectedPen : mPen;
}

/*! \internal

  Returns the brush that should be used for drawing fills of the item. Returns mBrush when the item
  is not selected and mSelectedBrush when it is.
*/
QBrush QCPItemEllipse::mainBrush() const
{
  return mSelected ? mSelectedBrush : mBrush;
}


// ================================================================================
// =================== QCPItemRect
// ================================================================================

/*! \class QCPItemRect
  \brief A rectangle

  \image html QCPItemRect.png "Rectangle example. Blue dotted circles are anchors, solid blue discs are positions."

  It has two positions, \a topLeft and \a bottomRight, which define the rectangle.
*/

/*!
  Creates a rectangle item and sets default values.
  
  The constructed item can be added to the plot with QCustomPlot::addItem.
*/
QCPItemRect::QCPItemRect(QCustomPlot *parentPlot) :
  QCPAbstractItem(parentPlot),
  topLeft(createPosition("topLeft")),
  bottomRight(createPosition("bottomRight")),
  top(createAnchor("top", aiTop)),
  topRight(createAnchor("topRight", aiTopRight)),
  right(createAnchor("right", aiRight)),
  bottom(createAnchor("bottom", aiBottom)),
  bottomLeft(createAnchor("bottomLeft", aiBottomLeft)),
  left(createAnchor("left", aiLeft))
{
  topLeft->setCoords(0, 1);
  bottomRight->setCoords(1, 0);
  
  setPen(QPen(Qt::black));
  setSelectedPen(QPen(Qt::blue,2));
  setBrush(Qt::NoBrush);
  setSelectedBrush(Qt::NoBrush);
}

QCPItemRect::~QCPItemRect()
{
}

/*!
  Sets the pen that will be used to draw the line of the rectangle
  
  \see setSelectedPen, setBrush
*/
void QCPItemRect::setPen(const QPen &pen)
{
  mPen = pen;
}

/*!
  Sets the pen that will be used to draw the line of the rectangle when selected
  
  \see setPen, setSelected
*/
void QCPItemRect::setSelectedPen(const QPen &pen)
{
  mSelectedPen = pen;
}

/*!
  Sets the brush that will be used to fill the rectangle. To disable filling, set \a brush to
  Qt::NoBrush.
  
  \see setSelectedBrush, setPen
*/
void QCPItemRect::setBrush(const QBrush &brush)
{
  mBrush = brush;
}

/*!
  Sets the brush that will be used to fill the rectangle when selected. To disable filling, set \a
  brush to Qt::NoBrush.
  
  \see setBrush
*/
void QCPItemRect::setSelectedBrush(const QBrush &brush)
{
  mSelectedBrush = brush;
}

/* inherits documentation from base class */
double QCPItemRect::selectTest(const QPointF &pos) const
{
  if (!mVisible)
    return -1;
  
  QRectF rect = QRectF(topLeft->pixelPoint(), bottomRight->pixelPoint()).normalized();
  bool filledRect = mBrush.style() != Qt::NoBrush && mBrush.color().alpha() != 0;
  return rectSelectTest(rect, pos, filledRect);
}

/* inherits documentation from base class */
void QCPItemRect::draw(QCPPainter *painter)
{
  QPointF p1 = topLeft->pixelPoint();
  QPointF p2 = bottomRight->pixelPoint();
  if (p1.toPoint() == p2.toPoint())
    return;
  QRectF rect = QRectF(p1, p2).normalized();
  double clipPad = mainPen().widthF();
  QRectF boundingRect = rect.adjusted(-clipPad, -clipPad, clipPad, clipPad);
  if (boundingRect.intersects(clipRect())) // only draw if bounding rect of rect item is visible in cliprect
  {
    painter->setPen(mainPen());
    painter->setBrush(mainBrush());
    painter->drawRect(rect);
  }
}

/* inherits documentation from base class */
QPointF QCPItemRect::anchorPixelPoint(int anchorId) const
{
  QRectF rect = QRectF(topLeft->pixelPoint(), bottomRight->pixelPoint());
  switch (anchorId)
  {
    case aiTop:         return (rect.topLeft()+rect.topRight())*0.5;
    case aiTopRight:    return rect.topRight();
    case aiRight:       return (rect.topRight()+rect.bottomRight())*0.5;
    case aiBottom:      return (rect.bottomLeft()+rect.bottomRight())*0.5;
    case aiBottomLeft:  return rect.bottomLeft();
    case aiLeft:        return (rect.topLeft()+rect.bottomLeft())*0.5;;
  }
  
  qDebug() << Q_FUNC_INFO << "invalid anchorId" << anchorId;
  return QPointF();
}

/*! \internal

  Returns the pen that should be used for drawing lines. Returns mPen when the item is not selected
  and mSelectedPen when it is.
*/
QPen QCPItemRect::mainPen() const
{
  return mSelected ? mSelectedPen : mPen;
}

/*! \internal

  Returns the brush that should be used for drawing fills of the item. Returns mBrush when the item
  is not selected and mSelectedBrush when it is.
*/
QBrush QCPItemRect::mainBrush() const
{
  return mSelected ? mSelectedBrush : mBrush;
}


// ================================================================================
// =================== QCPItemPixmap
// ================================================================================

/*! \class QCPItemPixmap
  \brief An arbitrary pixmap

  \image html QCPItemPixmap.png "Pixmap example. Blue dotted circles are anchors, solid blue discs are positions."

  It has two positions, \a topLeft and \a bottomRight, which define the rectangle the pixmap will
  be drawn in. Depending on the scale setting (\ref setScaled), the pixmap will be either scaled to
  fit the rectangle or be drawn aligned to the topLeft position.
  
  If scaling is enabled and \a topLeft is further to the bottom/right than \a bottomRight (as shown
  on the right side of the example image), the pixmap will be flipped in the respective
  orientations.
*/

/*!
  Creates a rectangle item and sets default values.
  
  The constructed item can be added to the plot with QCustomPlot::addItem.
*/
QCPItemPixmap::QCPItemPixmap(QCustomPlot *parentPlot) :
  QCPAbstractItem(parentPlot),
  topLeft(createPosition("topLeft")),
  bottomRight(createPosition("bottomRight")),
  top(createAnchor("top", aiTop)),
  topRight(createAnchor("topRight", aiTopRight)),
  right(createAnchor("right", aiRight)),
  bottom(createAnchor("bottom", aiBottom)),
  bottomLeft(createAnchor("bottomLeft", aiBottomLeft)),
  left(createAnchor("left", aiLeft))
{
  topLeft->setCoords(0, 1);
  bottomRight->setCoords(1, 0);
  
  setPen(Qt::NoPen);
  setSelectedPen(QPen(Qt::blue));
  setScaled(false, Qt::KeepAspectRatio);
}

QCPItemPixmap::~QCPItemPixmap()
{
}

/*!
  Sets the pixmap that will be displayed.
*/
void QCPItemPixmap::setPixmap(const QPixmap &pixmap)
{
  mPixmap = pixmap;
}

/*!
  Sets whether the pixmap will be scaled to fit the rectangle defined by the \a topLeft and \a
  bottomRight positions.
*/
void QCPItemPixmap::setScaled(bool scaled, Qt::AspectRatioMode aspectRatioMode)
{
  mScaled = scaled;
  mAspectRatioMode = aspectRatioMode;
  updateScaledPixmap();
}

/*!
  Sets the pen that will be used to draw a border around the pixmap.
  
  \see setSelectedPen, setBrush
*/
void QCPItemPixmap::setPen(const QPen &pen)
{
  mPen = pen;
}

/*!
  Sets the pen that will be used to draw a border around the pixmap when selected
  
  \see setPen, setSelected
*/
void QCPItemPixmap::setSelectedPen(const QPen &pen)
{
  mSelectedPen = pen;
}

/* inherits documentation from base class */
double QCPItemPixmap::selectTest(const QPointF &pos) const
{
  if (!mVisible)
    return -1;
  
  return rectSelectTest(getFinalRect(), pos, true);
}

/* inherits documentation from base class */
void QCPItemPixmap::draw(QCPPainter *painter)
{
  bool flipHorz = false;
  bool flipVert = false;
  QRect rect = getFinalRect(&flipHorz, &flipVert);
  double clipPad = mainPen().style() == Qt::NoPen ? 0 : mainPen().widthF();
  QRect boundingRect = rect.adjusted(-clipPad, -clipPad, clipPad, clipPad);
  if (boundingRect.intersects(clipRect()))
  {
    updateScaledPixmap(rect, flipHorz, flipVert);
    painter->drawPixmap(rect.topLeft(), mScaled ? mScaledPixmap : mPixmap);
    QPen pen = mainPen();
    if (pen.style() != Qt::NoPen)
    {
      painter->setPen(pen);
      painter->setBrush(Qt::NoBrush);
      painter->drawRect(rect);
    }
  }
}

/* inherits documentation from base class */
QPointF QCPItemPixmap::anchorPixelPoint(int anchorId) const
{
  bool flipHorz;
  bool flipVert;
  QRect rect = getFinalRect(&flipHorz, &flipVert);
  // we actually want denormal rects (negative width/height) here, so restore
  // the flipped state:
  if (flipHorz)
    rect.adjust(rect.width(), 0, -rect.width(), 0);
  if (flipVert)
    rect.adjust(0, rect.height(), 0, -rect.height());
  
  switch (anchorId)
  {
    case aiTop:         return (rect.topLeft()+rect.topRight())*0.5;
    case aiTopRight:    return rect.topRight();
    case aiRight:       return (rect.topRight()+rect.bottomRight())*0.5;
    case aiBottom:      return (rect.bottomLeft()+rect.bottomRight())*0.5;
    case aiBottomLeft:  return rect.bottomLeft();
    case aiLeft:        return (rect.topLeft()+rect.bottomLeft())*0.5;;
  }
  
  qDebug() << Q_FUNC_INFO << "invalid anchorId" << anchorId;
  return QPointF();
}

/*! \internal
  
  Creates the buffered scaled image (\a mScaledPixmap) to fit the specified \a finalRect. The
  parameters \a flipHorz and \a flipVert control whether the resulting image shall be flipped
  horizontally or vertically. (This is used when \a topLeft is further to the bottom/right than \a
  bottomRight.)
  
  This function only creates the scaled pixmap when the buffered pixmap has a different size than
  the expected result, so calling this function repeatedly, e.g. in the \ref draw function, does
  not cause expensive rescaling every time.
  
  If scaling is disabled, sets mScaledPixmap to a null QPixmap.
*/
void QCPItemPixmap::updateScaledPixmap(QRect finalRect, bool flipHorz, bool flipVert)
{
  if (mScaled)
  {
    if (finalRect.isNull())
      finalRect = getFinalRect(&flipHorz, &flipVert);
    if (finalRect.size() != mScaledPixmap.size())
    {
      mScaledPixmap = mPixmap.scaled(finalRect.size(), mAspectRatioMode, Qt::SmoothTransformation);
      if (flipHorz || flipVert)
        mScaledPixmap = QPixmap::fromImage(mScaledPixmap.toImage().mirrored(flipHorz, flipVert));
    }
  } else if (!mScaledPixmap.isNull())
    mScaledPixmap = QPixmap();
}

/*! \internal
  
  Returns the final (tight) rect the pixmap is drawn in, depending on the current item positions
  and scaling settings.
  
  The output parameters \a flippedHorz and \a flippedVert return whether the pixmap should be drawn
  flipped horizontally or vertically in the returned rect. (The returned rect itself is always
  normalized, i.e. the top left corner of the rect is actually further to the top/left than the
  bottom right corner). This is the case when the item position \a topLeft is further to the
  bottom/right than \a bottomRight.
  
  If scaling is disabled, returns a rect with size of the original pixmap and the top left corner
  aligned with the item position \a topLeft. The position \a bottomRight is ignored.
*/
QRect QCPItemPixmap::getFinalRect(bool *flippedHorz, bool *flippedVert) const
{
  QRect result;
  bool flipHorz = false;
  bool flipVert = false;
  QPoint p1 = topLeft->pixelPoint().toPoint();
  QPoint p2 = bottomRight->pixelPoint().toPoint();
  if (p1 == p2)
    return QRect(p1, QSize(0, 0));
  if (mScaled)
  {
    QSize newSize = QSize(p2.x()-p1.x(), p2.y()-p1.y());
    QPoint topLeft = p1;
    if (newSize.width() < 0)
    {
      flipHorz = true;
      newSize.rwidth() *= -1;
      topLeft.setX(p2.x());
    }
    if (newSize.height() < 0)
    {
      flipVert = true;
      newSize.rheight() *= -1;
      topLeft.setY(p2.y());
    }
    QSize scaledSize = mPixmap.size();
    scaledSize.scale(newSize, mAspectRatioMode);
    result = QRect(topLeft, scaledSize);
  } else
  {
    result = QRect(p1, mPixmap.size());
  }
  if (flippedHorz)
    *flippedHorz = flipHorz;
  if (flippedVert)
    *flippedVert = flipVert;
  return result;
}

/*! \internal

  Returns the pen that should be used for drawing lines. Returns mPen when the item is not selected
  and mSelectedPen when it is.
*/
QPen QCPItemPixmap::mainPen() const
{
  return mSelected ? mSelectedPen : mPen;
}


// ================================================================================
// =================== QCPItemText
// ================================================================================

/*! \class QCPItemText
  \brief A text label

  \image html QCPItemText.png "Text example. Blue dotted circles are anchors, solid blue discs are positions."

  Its position is defined by the member \a position and the setting of \ref setPositionAlignment.
  The latter controls which part of the text rect shall be aligned with \a position.
  
  The text alignment itself (i.e. left, center, right) can be controlled with \ref
  setTextAlignment.
  
  The text may be rotated around the \a position point with \ref setRotation.
*/

/*!
  Creates a text item and sets default values.
  
  The constructed item can be added to the plot with QCustomPlot::addItem.
*/
QCPItemText::QCPItemText(QCustomPlot *parentPlot) :
  QCPAbstractItem(parentPlot),
  position(createPosition("position")),
  topLeft(createAnchor("topLeft", aiTopLeft)),
  top(createAnchor("top", aiTop)),
  topRight(createAnchor("topRight", aiTopRight)),
  right(createAnchor("right", aiRight)),
  bottomRight(createAnchor("bottomRight", aiBottomRight)),
  bottom(createAnchor("bottom", aiBottom)),
  bottomLeft(createAnchor("bottomLeft", aiBottomLeft)),
  left(createAnchor("left", aiLeft))
{
  position->setCoords(0, 0);
  
  setRotation(0);
  setTextAlignment(Qt::AlignTop|Qt::AlignHCenter);
  setPositionAlignment(Qt::AlignCenter);
  setText("text");
  
  setPen(Qt::NoPen);
  setSelectedPen(Qt::NoPen);
  setBrush(Qt::NoBrush);
  setSelectedBrush(Qt::NoBrush);
  setColor(Qt::black);
  setSelectedColor(Qt::blue);
}

QCPItemText::~QCPItemText()
{
}

/*!
  Sets the color of the text.
*/
void QCPItemText::setColor(const QColor &color)
{
  mColor = color;
}

/*!
  Sets the color of the text that will be used when the item is selected.
*/
void QCPItemText::setSelectedColor(const QColor &color)
{
  mSelectedColor = color;
}

/*!
  Sets the pen that will be used do draw a rectangular border around the text. To disable the
  border, set \a pen to Qt::NoPen.
  
  \see setSelectedPen, setBrush, setPadding
*/
void QCPItemText::setPen(const QPen &pen)
{
  mPen = pen;
}

/*!
  Sets the pen that will be used do draw a rectangular border around the text, when the item is
  selected. To disable the border, set \a pen to Qt::NoPen.
  
  \see setPen
*/
void QCPItemText::setSelectedPen(const QPen &pen)
{
  mSelectedPen = pen;
}

/*!
  Sets the brush that will be used do fill the background of the text. To disable the
  background, set \a brush to Qt::NoBrush.
  
  \see setSelectedBrush, setPen, setPadding
*/
void QCPItemText::setBrush(const QBrush &brush)
{
  mBrush = brush;
}

/*!
  Sets the brush that will be used do fill the background of the text, when the item is selected. To disable the
  background, set \a brush to Qt::NoBrush.
  
  \see setBrush
*/
void QCPItemText::setSelectedBrush(const QBrush &brush)
{
  mSelectedBrush = brush;
}

/*!
  Sets the font of the text.
  
  \see setSelectedFont, setColor
*/
void QCPItemText::setFont(const QFont &font)
{
  mFont = font;
}

/*!
  Sets the font of the text that will be used when the item is selected.
  
  \see setFont
*/
void QCPItemText::setSelectedFont(const QFont &font)
{
  mSelectedFont = font;
}

/*!
  Sets the text that will be displayed. Multi-line texts are supported by inserting a line break
  character, e.g. '\n'.
  
  \see setFont, setColor, setTextAlignment
*/
void QCPItemText::setText(const QString &text)
{
  mText = text;
}

/*!
  Sets which point of the text rect shall be aligned with \a position.
  
  Examples:
  \li If \a alignment is <tt>Qt::AlignHCenter | Qt::AlignTop</tt>, the text will be positioned such
  that the top of the text rect will be horizontally centered on \a position.
  \li If \a alignment is <tt>Qt::AlignLeft | Qt::AlignBottom</tt>, \a position will indicate the
  bottom left corner of the text rect.
  
  If you want to control the alignment of (multi-lined) text within the text rect, use \ref
  setTextAlignment.
*/
void QCPItemText::setPositionAlignment(Qt::Alignment alignment)
{
  mPositionAlignment = alignment;
}

/*!
  Controls how (multi-lined) text is aligned inside the text rect (typically Qt::AlignLeft, Qt::AlignCenter or Qt::AlignRight).
*/
void QCPItemText::setTextAlignment(Qt::Alignment alignment)
{
  mTextAlignment = alignment;
}

/*!
  Sets the angle in degrees by which the text (and the text rectangle, if visible) will be rotated
  around \a position.
*/
void QCPItemText::setRotation(double degrees)
{
  mRotation = degrees;
}

/*!
  Sets the distance between the border of the text rectangle and the text. The appearance (and
  visibility) of the text rectangle can be controlled with \ref setPen and \ref setBrush.
*/
void QCPItemText::setPadding(const QMargins &padding)
{
  mPadding = padding;
}

/* inherits documentation from base class */
double QCPItemText::selectTest(const QPointF &pos) const
{
  if (!mVisible)
    return -1;
  
  // The rect may be rotated, so we transform the actual clicked pos to the rotated
  // coordinate system, wo we can use the normal rectSelectTest function for non-rotated rects:
  QPointF positionPixels(position->pixelPoint());
  QTransform inputTransform;
  inputTransform.translate(positionPixels.x(), positionPixels.y());
  inputTransform.rotate(-mRotation);
  inputTransform.translate(-positionPixels.x(), -positionPixels.y());
  QPointF rotatedPos = inputTransform.map(pos);
  QFontMetrics fontMetrics(mFont);
  QRect textRect = fontMetrics.boundingRect(0, 0, 0, 0, Qt::TextDontClip|mTextAlignment, mText);
  QRect textBoxRect = textRect.adjusted(-mPadding.left(), -mPadding.top(), mPadding.right(), mPadding.bottom());
  QPointF textPos = getTextDrawPoint(positionPixels, textBoxRect, mPositionAlignment);
  textBoxRect.moveTopLeft(textPos.toPoint());

  return rectSelectTest(textBoxRect, rotatedPos, true);
}

/* inherits documentation from base class */
void QCPItemText::draw(QCPPainter *painter)
{
  QPointF pos(position->pixelPoint());
  QTransform transform;
  transform.translate(pos.x(), pos.y());
  if (!qFuzzyIsNull(mRotation))
    transform.rotate(mRotation);
  painter->setFont(mainFont());
  QRect textRect = painter->fontMetrics().boundingRect(0, 0, 0, 0, Qt::TextDontClip|mTextAlignment, mText);
  QRect textBoxRect = textRect.adjusted(-mPadding.left(), -mPadding.top(), mPadding.right(), mPadding.bottom());
  QPointF textPos = getTextDrawPoint(QPointF(0, 0), textBoxRect, mPositionAlignment); // 0, 0 because the transform does the translation
  textRect.moveTopLeft(textPos.toPoint()+QPoint(mPadding.left(), mPadding.top()));
  textBoxRect.moveTopLeft(textPos.toPoint());
  double clipPad = mainPen().widthF();
  QRect boundingRect = textBoxRect.adjusted(-clipPad, -clipPad, clipPad, clipPad);
  if (transform.mapRect(boundingRect).intersects(clipRect()))
  {
    painter->setTransform(transform);
    if ((mainBrush().style() != Qt::NoBrush && mainBrush().color().alpha() != 0) ||
        (mainPen().style() != Qt::NoPen && mainPen().color().alpha() != 0))
    {
      painter->setPen(mainPen());
      painter->setBrush(mainBrush());
      painter->drawRect(textBoxRect);
    }
    painter->setBrush(Qt::NoBrush);
    painter->setPen(QPen(mainColor()));
    painter->drawText(textRect, Qt::TextDontClip|mTextAlignment, mText);
  }
}

/* inherits documentation from base class */
QPointF QCPItemText::anchorPixelPoint(int anchorId) const
{
  // get actual rect points (pretty much copied from draw function):
  QPointF pos(position->pixelPoint());
  QTransform transform;
  transform.translate(pos.x(), pos.y());
  if (!qFuzzyIsNull(mRotation))
    transform.rotate(mRotation);
  QFontMetrics fontMetrics(mainFont());
  QRect textRect = fontMetrics.boundingRect(0, 0, 0, 0, Qt::TextDontClip|mTextAlignment, mText);
  QRectF textBoxRect = textRect.adjusted(-mPadding.left(), -mPadding.top(), mPadding.right(), mPadding.bottom());
  QPointF textPos = getTextDrawPoint(QPointF(0, 0), textBoxRect, mPositionAlignment); // 0, 0 because the transform does the translation
  textBoxRect.moveTopLeft(textPos.toPoint());
  QPolygonF rectPoly = transform.map(QPolygonF(textBoxRect));
  
  switch (anchorId)
  {
    case aiTopLeft:     return rectPoly.at(0);
    case aiTop:         return (rectPoly.at(0)+rectPoly.at(1))*0.5;
    case aiTopRight:    return rectPoly.at(1);
    case aiRight:       return (rectPoly.at(1)+rectPoly.at(2))*0.5;
    case aiBottomRight: return rectPoly.at(2);
    case aiBottom:      return (rectPoly.at(2)+rectPoly.at(3))*0.5;
    case aiBottomLeft:  return rectPoly.at(3);
    case aiLeft:        return (rectPoly.at(3)+rectPoly.at(0))*0.5;
  }
  
  qDebug() << Q_FUNC_INFO << "invalid anchorId" << anchorId;
  return QPointF();
}

/*! \internal
  
  Returns the point that must be given to the QPainter::drawText function (which expects the top
  left point of the text rect), according to the position \a pos, the text bounding box \a rect and
  the requested \a positionAlignment.
  
  For example, if \a positionAlignment is <tt>Qt::AlignLeft | Qt::AlignBottom</tt> the returned point
  will be shifted upward by the height of \a rect, starting from \a pos. So if the text is finally
  drawn at that point, the lower left corner of the resulting text rect is at \a pos.
*/
QPointF QCPItemText::getTextDrawPoint(const QPointF &pos, const QRectF &rect, Qt::Alignment positionAlignment) const
{
  if (positionAlignment == 0 || positionAlignment == (Qt::AlignLeft|Qt::AlignTop))
    return pos;
  
  QPointF result = pos; // start at top left
  if (positionAlignment.testFlag(Qt::AlignHCenter))
    result.rx() -= rect.width()/2.0;
  else if (positionAlignment.testFlag(Qt::AlignRight))
    result.rx() -= rect.width();
  if (positionAlignment.testFlag(Qt::AlignVCenter))
    result.ry() -= rect.height()/2.0;
  else if (positionAlignment.testFlag(Qt::AlignBottom))
    result.ry() -= rect.height();
  return result;
}

/*! \internal

  Returns the font that should be used for drawing text. Returns mFont when the item is not selected
  and mSelectedFont when it is.
*/
QFont QCPItemText::mainFont() const
{
  return mSelected ? mSelectedFont : mFont;
}

/*! \internal

  Returns the color that should be used for drawing text. Returns mColor when the item is not
  selected and mSelectedColor when it is.
*/
QColor QCPItemText::mainColor() const
{
  return mSelected ? mSelectedColor : mColor;
}

/*! \internal

  Returns the pen that should be used for drawing lines. Returns mPen when the item is not selected
  and mSelectedPen when it is.
*/
QPen QCPItemText::mainPen() const
{
  return mSelected ? mSelectedPen : mPen;
}

/*! \internal

  Returns the brush that should be used for drawing fills of the item. Returns mBrush when the item
  is not selected and mSelectedBrush when it is.
*/
QBrush QCPItemText::mainBrush() const
{
  return mSelected ? mSelectedBrush : mBrush;
}


// ================================================================================
// =================== QCPPainter
// ================================================================================

/*! \class QCPPainter
  \brief QPainter subclass used internally
  
  This internal class is used to provide some extended functionality e.g. for tweaking position
  consistency between antialiased and non-antialiased painting and drawing common shapes (like
  scatter symbols). Further it provides workarounds for QPainter quirks.
  
  \warning This class intentionally hides non-virtual functions of QPainter, e.g. setPen, save and
  restore. So while it is possible to pass a QCPPainter instance to a function that expects a
  QPainter pointer, some of the workarounds and tweaks will be unavailable to the function (because
  it will call the base class implementations of the functions actually hidden by QCPPainter).
*/

/*!
  Creates a new QCPPainter instance and sets default values
*/
QCPPainter::QCPPainter() :
  QPainter(),
  mScaledExportMode(false),
  mPdfExportMode(false),
  mIsAntialiasing(false)
{
}

/*!
  Creates a new QCPPainter instance on the specified paint \a device and sets default values. Just
  like the analogous QPainter constructor, begins painting on \a device immediately.
*/
QCPPainter::QCPPainter(QPaintDevice *device) :
  QPainter(device),
  mScaledExportMode(false),
  mPdfExportMode(false),
  mIsAntialiasing(false)
{
}

QCPPainter::~QCPPainter()
{
}

/*!
  Sets the pixmap that will be used to draw scatters with \ref drawScatter, when the style is
  QCP::ssPixmap.
*/
void QCPPainter::setScatterPixmap(const QPixmap pm)
{
  mScatterPixmap = pm;
}

/*!
  Sets the pen of the painter and applies certain fixes to it, depending on the mode of this
  QCPPainter.
  
  \note this function hides the non-virtual base class implementation.
*/
void QCPPainter::setPen(const QPen &pen)
{
  QPainter::setPen(pen);
  if (mScaledExportMode)
    fixScaledPen();
}

/*! \overload
  
  Sets the pen (by color) of the painter and applies certain fixes to it, depending on the mode of
  this QCPPainter.
  
  \note this function hides the non-virtual base class implementation.
*/
void QCPPainter::setPen(const QColor &color)
{
  QPainter::setPen(color);
  if (mScaledExportMode)
    fixScaledPen();
}

/*! \overload
  
  Sets the pen (by style) of the painter and applies certain fixes to it, depending on the mode of
  this QCPPainter.
  
  \note this function hides the non-virtual base class implementation.
*/
void QCPPainter::setPen(Qt::PenStyle penStyle)
{
  QPainter::setPen(penStyle);
  if (mScaledExportMode)
    fixScaledPen();
}

/*! \overload
  
  Works around a Qt bug introduced with Qt 4.8 which makes drawing QLineF unpredictable when
  antialiasing is disabled.
  
  \note this function hides the non-virtual base class implementation.
*/
void QCPPainter::drawLine(const QLineF &line)
{
  if (mIsAntialiasing)
    QPainter::drawLine(line);
  else
    QPainter::drawLine(line.toLine());
}

/*! 
  Sets whether painting uses antialiasing or not. Use this method instead of using setRenderHint
  with QPainter::Antialiasing directly, as it allows QCPPainter to regain pixel exactness between
  antialiased and non-antialiased painting (Since Qt uses slightly different coordinate systems for
  AA/Non-AA painting).
*/
void QCPPainter::setAntialiasing(bool enabled)
{
  if (mPdfExportMode)
    return;
  
  setRenderHint(QPainter::Antialiasing, enabled);
  if (mIsAntialiasing != enabled)
  {
    if (mIsAntialiasing)
      translate(-0.5, -0.5);
    else
      translate(0.5, 0.5);
    mIsAntialiasing = enabled;
  }
}

/*!
  Saves the painter (see QPainter::save). Since QCPPainter adds some new internal state to
  QPainter, the save/restore functions are reimplemented to also save/restore those members.
  
  \note this function hides the non-virtual base class implementation.
  
  \see restore
*/
void QCPPainter::save()
{
  mAntialiasingStack.push(mIsAntialiasing);
  QPainter::save();
}

/*!
  Restores the painter (see QPainter::restore). Since QCPPainter adds some new internal state to
  QPainter, the save/restore functions are reimplemented to also save/restore those members.
  
  \note this function hides the non-virtual base class implementation.
  
  \see save
*/
void QCPPainter::restore()
{
  if (!mAntialiasingStack.isEmpty())
    mIsAntialiasing = mAntialiasingStack.pop();
  else
    qDebug() << Q_FUNC_INFO << "Unbalanced save/restore";
  QPainter::restore();
}

/*!
  Sets whether the painter shall adjust its fixes/workarounds optimized for vectorized pdf export.

  This means for example, that the antialiasing/non-antialiasing fix introduced with \ref
  setAntialiasing is not used, since PDF is not rastered and thus works with floating point data
  natively.
*/
void QCPPainter::setPdfExportMode(bool enabled)
{
  mPdfExportMode = enabled;
}

/*!
  Sets whether the painter shall adjust its fixes/workarounds optimized for scaled export to
  rastered image formats.

  For example this provides a workaround for a QPainter bug that prevents scaling of pen widths for
  pens with width 0, although the QPainter::NonCosmeticDefaultPen render hint is set.
*/
void QCPPainter::setScaledExportMode(bool enabled)
{
  mScaledExportMode = enabled;
}

/*!
  Provides a workaround for a QPainter bug that prevents scaling of pen widths for pens with width
  0, although the QPainter::NonCosmeticDefaultPen render hint is set.
  
  Changes the pen width from 0 to 1, if appropriate.
  
  Does nothing if the QCPPainter is not in scaled export mode (\ref setScaledExportMode).
*/
void QCPPainter::fixScaledPen()
{
  if (mScaledExportMode && pen().isCosmetic() && qFuzzyIsNull(pen().widthF()))
  {
    QPen p = pen();
    p.setWidth(1);
    QPainter::setPen(p);
  }
}

/*! 
  Draws a single scatter point with the specified \a style and \a size in pixels at the pixel position \a x and \a y.
  
  If the \a style is ssPixmap, make sure to pass the respective pixmap with \ref setScatterPixmap before calling
  this function.
*/
void QCPPainter::drawScatter(double x, double y, double size, QCP::ScatterStyle style)
{
  double w = size/2.0;
  switch (style)
  {
    case QCP::ssNone: break;
    case QCP::ssDot:
    {
      drawPoint(QPointF(x, y));
      break;
    }
    case QCP::ssCross:
    {
      drawLine(QLineF(x-w, y-w, x+w, y+w));
      drawLine(QLineF(x-w, y+w, x+w, y-w));
      break;
    }
    case QCP::ssPlus:
    {
      drawLine(QLineF(x-w, y, x+w, y));
      drawLine(QLineF(x, y+w, x, y-w));
      break;
    }
    case QCP::ssCircle:
    {
      setBrush(Qt::NoBrush);
      drawEllipse(QPointF(x,y), w, w);
      break;
    }
    case QCP::ssDisc:
    {
      setBrush(QBrush(pen().color()));
      drawEllipse(QPointF(x,y), w, w);
      break;
    }
    case QCP::ssSquare:
    {
      setBrush(Qt::NoBrush);
      drawRect(QRectF(x-w, y-w, size, size));
      break;
    }
    case QCP::ssDiamond:
    {
      setBrush(Qt::NoBrush);
      drawLine(QLineF(x-w, y, x, y-w));
      drawLine(QLineF(x, y-w, x+w, y));
      drawLine(QLineF(x+w, y, x, y+w));
      drawLine(QLineF(x, y+w, x-w, y));
      break;
    }
    case QCP::ssStar:
    {
      drawLine(QLineF(x-w, y, x+w, y));
      drawLine(QLineF(x, y+w, x, y-w));
      drawLine(QLineF(x-w*0.707, y-w*0.707, x+w*0.707, y+w*0.707));
      drawLine(QLineF(x-w*0.707, y+w*0.707, x+w*0.707, y-w*0.707));
      break;
    }
    case QCP::ssTriangle:
    {
      drawLine(QLineF(x-w, y+0.755*w, x+w, y+0.755*w));
      drawLine(QLineF(x+w, y+0.755*w, x, y-0.977*w));
      drawLine(QLineF(x, y-0.977*w, x-w, y+0.755*w));
      break;
    }
    case QCP::ssTriangleInverted:
    {
      drawLine(QLineF(x-w, y-0.755*w, x+w, y-0.755*w));
      drawLine(QLineF(x+w, y-0.755*w, x, y+0.977*w));
      drawLine(QLineF(x, y+0.977*w, x-w, y-0.755*w));
      break;
    }
    case QCP::ssCrossSquare:
    {
      setBrush(Qt::NoBrush);
      drawLine(QLineF(x-w, y-w, x+w*0.95, y+w*0.95));
      drawLine(QLineF(x-w, y+w*0.95, x+w*0.95, y-w));
      drawRect(QRectF(x-w,y-w,size,size));
      break;
    }
    case QCP::ssPlusSquare:
    {
      setBrush(Qt::NoBrush);
      drawLine(QLineF(x-w, y, x+w*0.95, y));
      drawLine(QLineF(x, y+w, x, y-w));
      drawRect(QRectF(x-w, y-w, size, size));
      break;
    }
    case QCP::ssCrossCircle:
    {
      setBrush(Qt::NoBrush);
      drawLine(QLineF(x-w*0.707, y-w*0.707, x+w*0.67, y+w*0.67));
      drawLine(QLineF(x-w*0.707, y+w*0.67, x+w*0.67, y-w*0.707));
      drawEllipse(QPointF(x,y), w, w);
      break;
    }
    case QCP::ssPlusCircle:
    {
      setBrush(Qt::NoBrush);
      drawLine(QLineF(x-w, y, x+w, y));
      drawLine(QLineF(x, y+w, x, y-w));
      drawEllipse(QPointF(x,y), w, w);
      break;
    }
    case QCP::ssPeace:
    {
      setBrush(Qt::NoBrush);
      drawLine(QLineF(x, y-w, x, y+w));
      drawLine(QLineF(x, y, x-w*0.707, y+w*0.707));
      drawLine(QLineF(x, y, x+w*0.707, y+w*0.707));
      drawEllipse(QPointF(x,y), w, w);
      break;
    }
    case QCP::ssPixmap:
    {
      drawPixmap(x-mScatterPixmap.width()*0.5, y-mScatterPixmap.height()*0.5, mScatterPixmap);
      // if something in here is changed, adapt QCP::ssPixmap special case in drawLegendIcon(), too
      break;
    }
  }
}


// ================================================================================
// =================== QCPLineEnding
// ================================================================================

/*! \class QCPLineEnding
  \brief Handles the different ending decorations for line-like items
  
  \image html QCPLineEnding.png "The various ending styles currently supported"
  
  For every ending a line-like item has, an instance of this class exists. For example, QCPItemLine
  has two endings which can be set with QCPItemLine::setHead and QCPItemLine::setTail.
 
  The styles themselves are defined via the enum QCPLineEnding::EndingStyle. Most decorations can
  be modified regarding width and length, see \ref setWidth and \ref setLength. The direction of
  the ending decoration (e.g. direction an arrow is pointing) is controlled by the line-like item.
  For example, when both endings of a QCPItemLine are set to be arrows, they will point to opposite
  directions, e.g. "outward". This can be changed by \ref setInverted, which would make the
  respective arrow point inward.
  
  Note that due to the overloaded QCPLineEnding constructor, you may directly specify a
  QCPLineEnding::EndingStyle where actually a QCPLineEnding is expected, e.g. \code
  myItemLine->setHead(QCPLineEnding::esSpikeArrow) \endcode
*/

/*!
  Creates a QCPLineEnding instance with default values (style \ref esNone).
*/
QCPLineEnding::QCPLineEnding() :
  mStyle(esNone),
  mWidth(8),
  mLength(10),
  mInverted(false)
{
}

/*!
  Creates a QCPLineEnding instance with the specified values.
*/
QCPLineEnding::QCPLineEnding(QCPLineEnding::EndingStyle style, double width, double length, bool inverted) :
  mStyle(style),
  mWidth(width),
  mLength(length),
  mInverted(inverted)
{
}

/*!
  Sets the style of the ending decoration.
*/
void QCPLineEnding::setStyle(QCPLineEnding::EndingStyle style)
{
  mStyle = style;
}

/*!
  Sets the width of the ending decoration, if the style supports it. On arrows, for example, the
  width defines the size perpendicular to the arrow's pointing direction.
  
  \see setLength
*/
void QCPLineEnding::setWidth(double width)
{
  mWidth = width;
}

/*!
  Sets the length of the ending decoration, if the style supports it. On arrows, for example, the
  length defines the size in pointing direction.
  
  \see setWidth
*/
void QCPLineEnding::setLength(double length)
{
  mLength = length;
}

/*!
  Sets whether the direction of the ending decoration shall be inverted with respect to the natural
  direction given by the parent item. For example, an arrow decoration will point inward when
  \a inverted is set to true.
*/
void QCPLineEnding::setInverted(bool inverted)
{
  mInverted = inverted;
}

/*! \internal
  
  Returns the maximum pixel radius the ending decoration might cover, starting from the position
  the decoration is drawn at (typically a line ending/\ref QCPItemPosition of an item).
  
  This is relevant for clipping. Only omit painting of the decoration when the position where the
  decoration is supposed to be drawn is farther away from the clipping rect than the returned
  distance.
*/
double QCPLineEnding::boundingDistance() const
{
  switch (mStyle)
  {
    case esNone:
      return 0;
      
    case esFlatArrow:
    case esSpikeArrow:
    case esLineArrow:
      return qSqrt(mWidth*mWidth+mLength*mLength); // items that have width and length
      
    case esDisc:
    case esSquare:
    case esDiamond:
    case esBar:
      return mWidth*1.42; // items that only have a width -> with*sqrt(2)
  }
  return 0;
}

/*! \internal
  
  Draws the line ending with the specified \a painter at the position \a pos. The direction of the
  line ending is controlled with \a dir.
*/
void QCPLineEnding::draw(QCPPainter *painter, const QVector2D &pos, const QVector2D &dir) const
{
  if (mStyle == esNone)
    return;
  
  QVector2D lengthVec(dir.normalized()*(mInverted ? -1 : 1));
  if (lengthVec.isNull())
    lengthVec = QVector2D(1, 0);
  QVector2D widthVec(-lengthVec.y(), lengthVec.x());
  lengthVec *= mLength;
  widthVec *= mWidth*0.5;
  
  QPen penBackup = painter->pen();
  QPen miterPen = penBackup;
  miterPen.setJoinStyle(Qt::MiterJoin);
  switch (mStyle)
  {
    case esNone: break;
    case esFlatArrow:
    {
      QPointF points[3] = {pos.toPointF(),
                           (pos-lengthVec+widthVec).toPointF(),
                           (pos-lengthVec-widthVec).toPointF()
                          };
      painter->setPen(miterPen);
      painter->drawConvexPolygon(points, 3);
      painter->setPen(penBackup);
      break;
    }
    case esSpikeArrow:
    {
      QPointF points[4] = {pos.toPointF(),
                           (pos-lengthVec+widthVec).toPointF(),
                           (pos-lengthVec*0.8).toPointF(),
                           (pos-lengthVec-widthVec).toPointF()
                          };
      painter->setPen(miterPen);
      painter->drawConvexPolygon(points, 4);
      painter->setPen(penBackup);
      break;
    }
    case esLineArrow:
    {
      QPointF points[3] = {(pos-lengthVec+widthVec).toPointF(),
                           pos.toPointF(),
                           (pos-lengthVec-widthVec).toPointF()
                          };
      painter->setPen(miterPen);
      painter->drawPolyline(points, 3);
      painter->setPen(penBackup);
      break;
    }
    case esDisc:
    {
      painter->drawEllipse(pos.toPointF(),  mWidth*0.5, mWidth*0.5);
      break;
    }
    case esSquare:
    {
      QVector2D widthVecPerp(-widthVec.y(), widthVec.x());
      QPointF points[4] = {(pos-widthVecPerp+widthVec).toPointF(),
                           (pos-widthVecPerp-widthVec).toPointF(),
                           (pos+widthVecPerp-widthVec).toPointF(),
                           (pos+widthVecPerp+widthVec).toPointF()
                          };
      painter->setPen(miterPen);
      painter->drawConvexPolygon(points, 4);
      painter->setPen(penBackup);
      break;
    }
    case esDiamond:
    {
      QVector2D widthVecPerp(-widthVec.y(), widthVec.x());
      QPointF points[4] = {(pos-widthVecPerp).toPointF(),
                           (pos-widthVec).toPointF(),
                           (pos+widthVecPerp).toPointF(),
                           (pos+widthVec).toPointF()
                          };
      painter->setPen(miterPen);
      painter->drawConvexPolygon(points, 4);
      painter->setPen(penBackup);
      break;
    }
    case esBar:
    {
      painter->drawLine((pos+widthVec).toPointF(), (pos-widthVec).toPointF());
      break;
    }
  }
}

/*! \internal
  \overload
  
  Draws the line ending. The direction is controlled with the \a angle parameter in radians.
*/
void QCPLineEnding::draw(QCPPainter *painter, const QVector2D &pos, double angle) const
{
  draw(painter, pos, QVector2D(qCos(angle), qSin(angle)));
}


// ================================================================================
// =================== QCPItemCurve
// ================================================================================

/*! \class QCPItemCurve
  \brief A curved line from one point to another

  \image html QCPItemCurve.png "Curve example. Blue dotted circles are anchors, solid blue discs are positions."

  It has four positions, \a start and \a end, which define the end points of the line, and two
  control points which define the direction the line exits from the start and the direction from
  which it approaches the end: \a startDir and \a endDir.
  
  With \ref setHead and \ref setTail you may set different line ending styles, e.g. to create an
  arrow.
  
  Often it is desirable for the control points to stay at fixed relative positions to the start/end
  point. This can be achieved by setting the parent anchor e.g. of \a startDir simply to \a start,
  and then specify the desired pixel offset with QCPItemPosition::setCoords on \a startDir.
*/

/*!
  Creates a curve item and sets default values.
  
  The constructed item can be added to the plot with QCustomPlot::addItem.
*/
QCPItemCurve::QCPItemCurve(QCustomPlot *parentPlot) :
  QCPAbstractItem(parentPlot),
  start(createPosition("start")),
  startDir(createPosition("startDir")),
  endDir(createPosition("endDir")),
  end(createPosition("end"))
{
  start->setCoords(0, 0);
  startDir->setCoords(0.5, 0);
  endDir->setCoords(0, 0.5);
  end->setCoords(1, 1);
  
  setPen(QPen(Qt::black));
  setSelectedPen(QPen(Qt::blue,2));
}

QCPItemCurve::~QCPItemCurve()
{
}

/*!
  Sets the pen that will be used to draw the line
  
  \see setSelectedPen
*/
void QCPItemCurve::setPen(const QPen &pen)
{
  mPen = pen;
}

/*!
  Sets the pen that will be used to draw the line when selected
  
  \see setPen, setSelected
*/
void QCPItemCurve::setSelectedPen(const QPen &pen)
{
  mSelectedPen = pen;
}

/*!
  Sets the line ending style of the head. The head corresponds to the \a end position.
  
  Note that due to the overloaded QCPLineEnding constructor, you may directly specify
  a QCPLineEnding::EndingStyle here, e.g. \code setHead(QCPLineEnding::esSpikeArrow) \endcode
  
  \see setTail
*/
void QCPItemCurve::setHead(const QCPLineEnding &head)
{
  mHead = head;
}

/*!
  Sets the line ending style of the tail. The tail corresponds to the \a start position.
  
  Note that due to the overloaded QCPLineEnding constructor, you may directly specify
  a QCPLineEnding::EndingStyle here, e.g. \code setTail(QCPLineEnding::esSpikeArrow) \endcode
  
  \see setHead
*/
void QCPItemCurve::setTail(const QCPLineEnding &tail)
{
  mTail = tail;
}

/* inherits documentation from base class */
double QCPItemCurve::selectTest(const QPointF &pos) const
{
  if (!mVisible)
    return -1;
  
  QPointF startVec(start->pixelPoint());
  QPointF startDirVec(startDir->pixelPoint());
  QPointF endDirVec(endDir->pixelPoint());
  QPointF endVec(end->pixelPoint());

  QPainterPath cubicPath(startVec);
  cubicPath.cubicTo(startDirVec, endDirVec, endVec);
  
  QPolygonF polygon = cubicPath.toSubpathPolygons().first();
  double minDistSqr = std::numeric_limits<double>::max();
  for (int i=1; i<polygon.size(); ++i)
  {
    double distSqr = distSqrToLine(polygon.at(i-1), polygon.at(i), pos);
    if (distSqr < minDistSqr)
      minDistSqr = distSqr;
  }
  return qSqrt(minDistSqr);
}

/* inherits documentation from base class */
void QCPItemCurve::draw(QCPPainter *painter)
{
  QPointF startVec(start->pixelPoint());
  QPointF startDirVec(startDir->pixelPoint());
  QPointF endDirVec(endDir->pixelPoint());
  QPointF endVec(end->pixelPoint());
  if (QVector2D(endVec-startVec).length() > 1e10) // too large curves cause crash
    return;

  QPainterPath cubicPath(startVec);
  cubicPath.cubicTo(startDirVec, endDirVec, endVec);

  // paint visible segment, if existent:
  QRect clip = clipRect().adjusted(-mainPen().widthF(), -mainPen().widthF(), mainPen().widthF(), mainPen().widthF());
  QRect cubicRect = cubicPath.controlPointRect().toRect();
  if (cubicRect.isEmpty()) // may happen when start and end exactly on same x or y position
    cubicRect.adjust(0, 0, 1, 1);
  if (clip.intersects(cubicRect))
  {
    painter->setPen(mainPen());
    painter->drawPath(cubicPath);
    painter->setBrush(Qt::SolidPattern);
    if (mTail.style() != QCPLineEnding::esNone)
      mTail.draw(painter, QVector2D(startVec), M_PI-cubicPath.angleAtPercent(0)/180.0*M_PI);
    if (mHead.style() != QCPLineEnding::esNone)
      mHead.draw(painter, QVector2D(endVec), -cubicPath.angleAtPercent(1)/180.0*M_PI);
  }
}

/*! \internal

  Returns the pen that should be used for drawing lines. Returns mPen when the
  item is not selected and mSelectedPen when it is.
*/
QPen QCPItemCurve::mainPen() const
{
  return mSelected ? mSelectedPen : mPen;
}


// ================================================================================
// =================== QCPLayer
// ================================================================================

/*! \class QCPLayer
  \brief A layer that may contain objects, to control the rendering order
  
  The Layering system of QCustomPlot is the mechanism to control the rendering order of the
  elements inside the plot, e.g. that the grid is drawn behind plottables etc.
  
  It is based on the two classes QCPLayer and QCPLayerable. A QCustomPlot contains an ordered list
  of one or more instances of QCPLayer (see QCustomPlot::addLayer, QCustomPlot::layer,
  QCustomPlot::moveLayer, etc.). The layers are drawn in the order they are in the list.

  A QCPLayer itself contains an ordered list of QCPLayerable instances. QCPLayerable is an abstract
  base class from which almost all visible objects derive, like axes, grids, graphs, items, etc.
  
  By default, QCustomPlot has three layers: "grid", "main" and "axes" (in that order). Initially
  the QCPGrid instances are on the "grid" layer, so the grid will be drawn beneath the objects on
  the other two layers. The top layer is "axes" and contains all four axes, so they will be drawn
  on top. Between these two layers, there is the "main" layer. It is initially empty and set as the
  current layer (see QCustomPlot::setCurrentLayer). This means, all new plottables, items etc.
  are created on this layer by default, and are thus drawn above the grid but below the axes.
  
  Controlling the ordering of objects is easy: Create a new layer in the position you want it to
  be, e.g. above "main", with QCustomPlot::addLayer. Then set the current layer with
  QCustomPlot::setCurrentLayer to that new layer and finally create the objects normally. They will
  be placed on the new layer automatically, due to the current layer setting. Alternatively you
  could have also ignored the current layer setting and just moved the objects with
  QCPLayerable::setLayer to the desired layer after creating them.
  
  It is also possible to move whole layers. For example, If you want the grid to be shown in front
  of all plottables/items on the "main" layer, just move it above "main" with
  QCustomPlot::moveLayer. This way the ordering might now be "main", "grid", "axes", so while the
  grid will still be beneath the axes, it will now be drawn above plottables/items on "main", as
  intended.
  
  The rendering order within one layer is simply by order of creation. The item created last (or
  added last to the layer), is drawn on top of all other objects on that layer.  
  
  When a layer is deleted, the objects on it are not deleted with it, but fall on the layer below
  the deleted layer, see QCustomPlot::removeLayer.
*/

/* start documentation of inline functions */

/*! \fn QList<QCPLayerable*> QCPLayer::children() const
  
  Returns a list of all layerables on this layer. The order corresponds to the rendering order,
  i.e. layerables with higher indices are drawn above layerables with lower indices.
*/

/* end documentation of inline functions */

/*!
  Creates a new QCPLayer instance.
  
  Normally you shouldn't directly create layers like this, use QCustomPlot::addLayer instead.
  
  \warning It is not checked that \a layerName is actually an unique layer name in \a parentPlot.
  This check is only performed by QCustomPlot::addLayer.
*/
QCPLayer::QCPLayer(QCustomPlot *parentPlot, const QString &layerName) :
  mParentPlot(parentPlot),
  mName(layerName)
{
  // Note: no need to make sure layerName doesn't already, because layer
  // management is done with QCustomPlot functions.
}

QCPLayer::~QCPLayer()
{
}

/*!
  Returns the index this layer has in the QCustomPlot. The index is the integer number by which this layer can be
  accessed via QCustomPlot::layer.
  
  Layers with greater indices will be drawn above layers with smaller indices.
*/
int QCPLayer::index() const
{
  return mParentPlot->mLayers.indexOf(const_cast<QCPLayer*>(this));
}

/*! \internal
  
  Adds the \a layerable to the list of this layer. If \a prepend is set to true, the layerable will
  be prepended to the list, i.e. be drawn beneath the other layerables already in the list.
  
  This function does not change the \a mLayer member of \a layerable to this layer. (Use
  QCPLayerable::setLayer to change the layer of an object, not this function.)
  
  \see removeChild
*/
void QCPLayer::addChild(QCPLayerable *layerable, bool prepend)
{
  if (!mChildren.contains(layerable))
  {
    if (prepend)
      mChildren.prepend(layerable);
    else
      mChildren.append(layerable);
  } else
    qDebug() << Q_FUNC_INFO << "layerable is already child of this layer" << reinterpret_cast<quintptr>(layerable);
}

/*! \internal
  
  Removes the \a layerable from the list of this layer.
  
  This function does not change the \a mLayer member of \a layerable. (Use QCPLayerable::setLayer
  to change the layer of an object, not this function.)
  
  \see addChild
*/
void QCPLayer::removeChild(QCPLayerable *layerable)
{
  if (!mChildren.removeOne(layerable))
    qDebug() << Q_FUNC_INFO << "layerable is not child of this layer" << reinterpret_cast<quintptr>(layerable);
}


// ================================================================================
// =================== QCPLayerable
// ================================================================================

/*! \class QCPLayerable
  \brief Base class for all objects that can be placed on layers
  
  This is the abstract base class most visible objects derive from, e.g. plottables, axes, grid
  etc.

  Every layerable is on a layer (QCPLayer) which allows controlling the rendering order by stacking
  the layers accordingly.
  
  For details about the layering mechanism, see the QCPLayer documentation.
*/

/* start documentation of pure virtual functions */

/*! \fn virtual void QCPLayerable::applyDefaultAntialiasingHint(QCPPainter *painter) const = 0
  \internal
  
  This function applies the default antialiasing setting to the specified \a painter, using the
  function \ref applyAntialiasingHint. This is the antialiasing state the painter is in, when \ref
  draw is called on the layerable. If the layerable has multiple entities whose antialiasing
  setting may be specified individually, this function should set the antialiasing state of the
  most prominent entity. In this case however, the \ref draw function usually calls the specialized
  versions of this function before drawing each entity, effectively overriding the setting of the
  default antialiasing hint.
  
  <b>First example:</b> QCPGraph has multiple entities that have an antialiasing setting: The graph
  line, fills, scatters and error bars. Those can be configured via QCPGraph::setAntialiased,
  QCPGraph::setAntialiasedFill, QCPGraph::setAntialiasedScatters etc. Consequently, there isn't
  only the QCPGraph::applyDefaultAntialiasingHint function (which corresponds to the graph line's
  antialiasing), but specialized ones like QCPGraph::applyFillAntialiasingHint and
  QCPGraph::applyScattersAntialiasingHint. So before drawing one of those entities, QCPGraph::draw
  calls the respective specialized applyAntialiasingHint function.
  
  <b>Second example:</b> QCPItemLine consists only of a line so there is only one antialiasing
  setting which can be controlled with QCPItemLine::setAntialiased. (This function is inherited by
  all layerables. The specialized functions, as seen on QCPGraph, must be added explicitly to the
  respective layerable subclass.) Consequently it only has the normal
  QCPItemLine::applyDefaultAntialiasingHint. The \ref QCPItemLine::draw function doesn't need to
  care about setting any antialiasing states, because the default antialiasing hint is already set
  on the painter when the \ref draw function is called, and that's the state it wants to draw the
  line with.
*/

/*! \fn virtual void QCPLayerable::draw(QCPPainter *painter) const = 0
  \internal
  
  This function draws the layerable to the specified \a painter.
  
  Before this function is called, the painter's antialiasing state is set via \ref
  applyDefaultAntialiasingHint, see the documentation there. Further, its clipping rectangle was
  set to \ref clipRect.
*/

/* end documentation of pure virtual functions */

/*!
  Creates a new QCPLayerable instance.
  
  Since QCPLayerable is an abstract base class, it can't be instantiated directly. Use one of the
  derived classes.
*/
QCPLayerable::QCPLayerable(QCustomPlot *parentPlot) :
  QObject(0), // rather not bind to parentPlot, incase we want to allow moving of objects between customplots some day
  mVisible(true),
  mParentPlot(parentPlot),
  mLayer(0),
  mAntialiased(true)
{
  if (mParentPlot)
    setLayer(mParentPlot->currentLayer());
}

QCPLayerable::~QCPLayerable()
{
  if (mLayer)
  {
    mLayer->removeChild(this);
    mLayer = 0;
  }
}

/*!
  Sets the visibility of this layerable object. If an object is not visible, it will not be drawn
  on the QCustomPlot surface, and user interaction with it (e.g. click/selection) is not possible.
*/
void QCPLayerable::setVisible(bool on)
{
  mVisible = on;
}

/*!
  Sets the \a layer of this layerable object. The object will be placed on top of the other objects
  already on \a layer.
  
  Returns true on success, i.e. if \a layer is a valid layer.
*/
bool QCPLayerable::setLayer(QCPLayer *layer)
{
  return moveToLayer(layer, false);
}

/*! \overload
  Sets the layer of this layerable object by name
  
  Returns true on success, i.e. if \a layerName is a valid layer name.
*/
bool QCPLayerable::setLayer(const QString &layerName)
{
  if (!mParentPlot)
  {
    qDebug() << Q_FUNC_INFO << "no parent QCustomPlot set";
    return false;
  }
  if (QCPLayer *layer = mParentPlot->layer(layerName))
  {
    return setLayer(layer);
  } else
  {
    qDebug() << Q_FUNC_INFO << "there is no layer with name" << layerName;
    return false;
  }
}

/*!
  Sets whether this object will be drawn antialiased or not.
  
  Note that antialiasing settings may be overridden by QCustomPlot::setAntialiasedElements and
  QCustomPlot::setNotAntialiasedElements.
*/
void QCPLayerable::setAntialiased(bool enabled)
{
  mAntialiased = enabled;
}

/*! \internal
  
  Moves this layerable object to \a layer. If \a prepend is true, this object will be prepended to
  the new layer's list, i.e. it will be drawn below the objects already on the layer. If it is
  false, the object will be appended.
  
  Returns true on success, i.e. if \a layer is a valid layer.
*/
bool QCPLayerable::moveToLayer(QCPLayer *layer, bool prepend)
{
  if (!mParentPlot)
  {
    qDebug() << Q_FUNC_INFO << "no parent QCustomPlot set";
    return false;
  }
  if (layer && layer->parentPlot() != mParentPlot)
  {
    qDebug() << Q_FUNC_INFO << "layer" << layer->name() << "is not in same QCustomPlot as this layerable";
    return false;
  }
  
  if (mLayer)
    mLayer->removeChild(this);
  mLayer = layer;
  if (mLayer)
    mLayer->addChild(this, prepend);
  return true;
}

/*! \internal

  Sets the QPainter::Antialiasing render hint on the provided \a painter, depending on the
  \a localAntialiased value as well as the overrides \ref QCustomPlot::setAntialiasedElements and
  \ref QCustomPlot::setNotAntialiasedElements. Which override enum this function takes into account is
  controlled via \a overrideElement.
*/
void QCPLayerable::applyAntialiasingHint(QCPPainter *painter, bool localAntialiased, QCP::AntialiasedElement overrideElement) const
{
  if (mParentPlot && mParentPlot->notAntialiasedElements().testFlag(overrideElement))
    painter->setAntialiasing(false);
  else if (mParentPlot && mParentPlot->antialiasedElements().testFlag(overrideElement))
    painter->setAntialiasing(true);
  else
    painter->setAntialiasing(localAntialiased);
}

/*! \internal
  
  Returns the clipping rectangle of this layerable object. By default, this is the viewport of the parent QCustomPlot.
  Specific subclasses may reimplement this function to provide different clipping rects.
  
  The returned clipping rect is set on the painter before the draw function of the respective
  object is called.
*/
QRect QCPLayerable::clipRect() const
{
  if (mParentPlot)
    return mParentPlot->viewport();
  else
    return QRect();
}


// ================================================================================
// =================== QCPGrid
// ================================================================================

/*! \class QCPGrid
  \brief Responsible for drawing the grid of a QCPAxis.
  
  This class is tightly bound to QCPAxis. Every axis owns a grid instance internally and uses it to
  draw the grid. Normally, you don't need to interact with the QCPGrid instance, because QCPAxis
  reproduces the grid interface in its own interface.
  
  The axis and grid drawing was split into two classes to allow them to be placed on different
  layers (both QCPAxis and QCPGrid inherit from QCPLayerable). So it is possible to have the grid
  at the background and the axes in the foreground, and any plottables/items in between. This
  described situation is the default setup, see QCPLayer documentation.  
*/

/*!
  Creates a QCPGrid instance and sets default values.
  
  You shouldn't instantiate grids on their own, since every QCPAxis brings its own QCPGrid
  internally
*/
QCPGrid::QCPGrid(QCPAxis *parentAxis) :
  QCPLayerable(parentAxis->parentPlot()),
  mParentAxis(parentAxis)
{
  setPen(QPen(QColor(200,200,200), 0, Qt::DotLine));
  setSubGridPen(QPen(QColor(220,220,220), 0, Qt::DotLine));
  setZeroLinePen(QPen(QColor(200,200,200), 0, Qt::SolidLine));
  setSubGridVisible(false);
  setAntialiased(false);
  setAntialiasedSubGrid(false);
  setAntialiasedZeroLine(false);
}

QCPGrid::~QCPGrid()
{
}

/*!
  Sets whether grid lines at sub tick marks are drawn.
  
  \see setSubGridPen
*/
void QCPGrid::setSubGridVisible(bool visible)
{
  mSubGridVisible = visible;
}

/*!
  Sets whether sub grid lines are drawn antialiased.
*/
void QCPGrid::setAntialiasedSubGrid(bool enabled)
{
  mAntialiasedSubGrid = enabled;
}

/*!
  Sets whether zero lines are drawn antialiased.
*/
void QCPGrid::setAntialiasedZeroLine(bool enabled)
{
  mAntialiasedZeroLine = enabled;
}

/*!
  Sets the pen with which (major) grid lines are drawn.
*/
void QCPGrid::setPen(const QPen &pen)
{
  mPen = pen;
}

/*!
  Sets the pen with which sub grid lines are drawn.
*/
void QCPGrid::setSubGridPen(const QPen &pen)
{
  mSubGridPen = pen;
}

/*!
  Sets the pen with which zero lines are drawn.
  
  Zero lines are lines at coordinate 0 which may be drawn with a different pen than other grid
  lines. To disable zero lines and just draw normal grid lines at zero, set \a pen to Qt::NoPen.
*/
void QCPGrid::setZeroLinePen(const QPen &pen)
{
  mZeroLinePen = pen;
}

/*! \internal

  A convenience function to easily set the QPainter::Antialiased hint on the provided \a painter
  before drawing the major grid lines.

  This is the antialiasing state the painter passed to the \ref draw method is in by default.
  
  This function takes into account the local setting of the antialiasing flag as well as
  the overrides set e.g. with \ref QCustomPlot::setNotAntialiasedElements.
  
  \see setAntialiased
*/
void QCPGrid::applyDefaultAntialiasingHint(QCPPainter *painter) const
{
  applyAntialiasingHint(painter, mAntialiased, QCP::aeGrid);
}

/*! \internal
  
  Draws grid lines and sub grid lines at the positions of (sub) ticks of the parent axis, spanning
  over the complete axis rect. Also draws the zero line, if appropriate (\ref setZeroLinePen).
  
  Called by QCustomPlot::draw to draw the grid of an axis.
*/
void QCPGrid::draw(QCPPainter *painter)
{
  if (!mParentAxis->visible()) return; // also don't draw grid when parent axis isn't visible
  
  if (mSubGridVisible)
    drawSubGridLines(painter);
  drawGridLines(painter);
}

/*! \internal
  
  Draws the main grid lines and possibly a zero line with the specified painter.
  
  This is a helper function called by \ref draw.
*/
void QCPGrid::drawGridLines(QCPPainter *painter) const
{
  int lowTick = mParentAxis->mLowestVisibleTick;
  int highTick = mParentAxis->mHighestVisibleTick;
  double t; // helper variable, result of coordinate-to-pixel transforms
  if (mParentAxis->orientation() == Qt::Horizontal)
  {
    // draw zeroline:
    int zeroLineIndex = -1;
    if (mZeroLinePen.style() != Qt::NoPen && mParentAxis->mRange.lower < 0 && mParentAxis->mRange.upper > 0)
    {
      applyAntialiasingHint(painter, mAntialiasedZeroLine, QCP::aeZeroLine);
      painter->setPen(mZeroLinePen);
      double epsilon = mParentAxis->range().size()*1E-6; // for comparing double to zero
      for (int i=lowTick; i <= highTick; ++i)
      {
        if (qAbs(mParentAxis->mTickVector.at(i)) < epsilon)
        {
          zeroLineIndex = i;
          t = mParentAxis->coordToPixel(mParentAxis->mTickVector.at(i)); // x
          painter->drawLine(QLineF(t, mParentAxis->mAxisRect.bottom(), t, mParentAxis->mAxisRect.top()));
          break;
        }
      }
    }
    applyDefaultAntialiasingHint(painter);
    painter->setPen(mPen);
    for (int i=lowTick; i <= highTick; ++i)
    {
      if (i == zeroLineIndex) continue; // don't draw a gridline on top of the zeroline
      t = mParentAxis->coordToPixel(mParentAxis->mTickVector.at(i)); // x
      painter->drawLine(QLineF(t, mParentAxis->mAxisRect.bottom(), t, mParentAxis->mAxisRect.top()));
    }
  } else
  {
    // draw zeroline:
    int zeroLineIndex = -1;
    if (mZeroLinePen.style() != Qt::NoPen && mParentAxis->mRange.lower < 0 && mParentAxis->mRange.upper > 0)
    {
      applyAntialiasingHint(painter, mAntialiasedZeroLine, QCP::aeZeroLine);
      painter->setPen(mZeroLinePen);
      double epsilon = mParentAxis->mRange.size()*1E-6; // for comparing double to zero
      for (int i=lowTick; i <= highTick; ++i)
      {
        if (qAbs(mParentAxis->mTickVector.at(i)) < epsilon)
        {
          zeroLineIndex = i;
          t = mParentAxis->coordToPixel(mParentAxis->mTickVector.at(i)); // y
          painter->drawLine(QLineF(mParentAxis->mAxisRect.left(), t, mParentAxis->mAxisRect.right(), t));
          break;
        }
      }
    }
    // draw grid lines:
    applyDefaultAntialiasingHint(painter);
    painter->setPen(mPen);
    for (int i=lowTick; i <= highTick; ++i)
    {
      if (i == zeroLineIndex) continue; // don't draw a gridline on top of the zeroline
      t = mParentAxis->coordToPixel(mParentAxis->mTickVector.at(i)); // y
      painter->drawLine(QLineF(mParentAxis->mAxisRect.left(), t, mParentAxis->mAxisRect.right(), t));
    }
  }
}

/*! \internal
  
  Draws the sub grid lines with the specified painter.
  
  This is a helper function called by \ref draw.
*/
void QCPGrid::drawSubGridLines(QCPPainter *painter) const
{
  applyAntialiasingHint(painter, mAntialiasedSubGrid, QCP::aeSubGrid);
  double t; // helper variable, result of coordinate-to-pixel transforms
  painter->setPen(mSubGridPen);
  if (mParentAxis->orientation() == Qt::Horizontal)
  {
    for (int i=0; i<mParentAxis->mSubTickVector.size(); ++i)
    {
      t = mParentAxis->coordToPixel(mParentAxis->mSubTickVector.at(i)); // x
      painter->drawLine(QLineF(t, mParentAxis->mAxisRect.bottom(), t, mParentAxis->mAxisRect.top()));
    }
  } else
  {
    for (int i=0; i<mParentAxis->mSubTickVector.size(); ++i)
    {
      t = mParentAxis->coordToPixel(mParentAxis->mSubTickVector.at(i)); // y
      painter->drawLine(QLineF(mParentAxis->mAxisRect.left(), t, mParentAxis->mAxisRect.right(), t));
    }
  }
}


// ================================================================================
// =================== QCPItemAnchor
// ================================================================================

/*! \class QCPItemAnchor
  \brief An anchor of an item to which positions can be attached to.
  
  An item (QCPAbstractItem) may have one or more anchors. Unlike QCPItemPosition, an anchor doesn't
  control anything on its item, but provides a way to tie other items via their positions to the
  anchor.

  For example, a QCPItemRect is defined by its positions \a topLeft and \a bottomRight.
  Additionally it has various anchors like \a top, \a topRight or \a bottomLeft etc. So you can
  attach the \a start (which is a QCPItemPosition) of a QCPItemLine to one of the anchors by
  calling QCPItemPosition::setParentAnchor on \a start, passing the wanted anchor of the
  QCPItemRect. This way the start of the line will now always follow the respective anchor location
  on the rect item.
  
  Note that QCPItemPosition derives from QCPItemAnchor, so every position can also serve as an
  anchor to other positions.
  
  To learn how to provide anchors in your own item subclasses, see the subclassing section of the
  QCPAbstractItem documentation.
*/

/*!
  Creates a new QCPItemAnchor. You shouldn't create QCPItemAnchor instances directly, even if
  you want to make a new item subclass. Use \ref QCPAbstractItem::createAnchor instead, as
  explained in the subclassing section of the QCPAbstractItem documentation.
*/
QCPItemAnchor::QCPItemAnchor(QCustomPlot *parentPlot, QCPAbstractItem *parentItem, const QString name, int anchorId) :
  mParentPlot(parentPlot),
  mParentItem(parentItem),
  mAnchorId(anchorId),
  mName(name)
{
}

QCPItemAnchor::~QCPItemAnchor()
{
  // unregister as parent at children:
  QList<QCPItemPosition*> currentChildren(mChildren.toList());
  for (int i=0; i<currentChildren.size(); ++i)
    currentChildren.at(i)->setParentAnchor(0); // this acts back on this anchor and child removes itself from mChildren
}

/*!
  Returns the final absolute pixel position of the QCPItemAnchor on the QCustomPlot surface.
  
  The pixel information is internally retrieved via QCPAbstractItem::anchorPixelPosition of the
  parent item, QCPItemAnchor is just an intermediary.
*/
QPointF QCPItemAnchor::pixelPoint() const
{
  if (mParentItem)
  {
    if (mAnchorId > -1)
    {
      return mParentItem->anchorPixelPoint(mAnchorId);
    } else
    {
      qDebug() << Q_FUNC_INFO << "no valid anchor id set:" << mAnchorId;
      return QPointF();
    }
  } else
  {
    qDebug() << Q_FUNC_INFO << "no parent item set";
    return QPointF();
  }
}

/*! \internal

  Adds \a pos to the child list of this anchor. This is necessary to notify the children prior to
  destruction of the anchor.
  
  Note that this function does not change the parent setting in \a pos.
*/
void QCPItemAnchor::addChild(QCPItemPosition *pos)
{
  if (!mChildren.contains(pos))
    mChildren.insert(pos);
  else
    qDebug() << Q_FUNC_INFO << "provided pos is child already" << reinterpret_cast<quintptr>(pos);
}

/*! \internal

  Removes \a pos from the child list of this anchor.
  
  Note that this function does not change the parent setting in \a pos.
*/
void QCPItemAnchor::removeChild(QCPItemPosition *pos)
{
  if (!mChildren.remove(pos))
    qDebug() << Q_FUNC_INFO << "provided pos isn't child" << reinterpret_cast<quintptr>(pos);
}


// ================================================================================
// =================== QCPItemBracket
// ================================================================================

/*! \class QCPItemBracket
  \brief A bracket for referencing/highlighting certain parts in the plot.

  \image html QCPItemBracket.png "Bracket example. Blue dotted circles are anchors, solid blue discs are positions."

  It has two positions, \a left and \a right, which define the span of the bracket. If \a left is
  actually farther to the left than \a right, the bracket is opened to the bottom, as shown in the
  example image.
  
  The bracket supports multiple styles via \ref setStyle. The length, i.e. how far the bracket
  stretches away from the embraced span, can be controlled with \ref setLength.
  
  \image html QCPItemBracket-length.png
  <center>Demonstrating the effect of different values for \ref setLength, for styles \ref
  bsCalligraphic and \ref bsSquare. Anchors and positions are displayed for reference.</center>
  
  It provides an anchor \a center, to allow connection of other items, e.g. an arrow (QCPItemLine
  or QCPItemCurve) or a text label (QCPItemText), to the bracket.
*/

/*!
  Creates a bracket item and sets default values.
  
  The constructed item can be added to the plot with QCustomPlot::addItem.
*/
QCPItemBracket::QCPItemBracket(QCustomPlot *parentPlot) :
  QCPAbstractItem(parentPlot),
  left(createPosition("left")),
  right(createPosition("right")),
  center(createAnchor("center", aiCenter))
{
  left->setCoords(0, 0);
  right->setCoords(1, 1);
  
  setPen(QPen(Qt::black));
  setSelectedPen(QPen(Qt::blue, 2));
  setLength(8);
  setStyle(bsCalligraphic);
}

QCPItemBracket::~QCPItemBracket()
{
}

/*!
  Sets the pen that will be used to draw the bracket.
  
  Note that when the style is \ref bsCalligraphic, only the color will be taken from the pen, the
  stroke and width are ignored. To change the apparent stroke width of a calligraphic bracket, use
  \ref setLength, which has a similar effect.
  
  \see setSelectedPen
*/
void QCPItemBracket::setPen(const QPen &pen)
{
  mPen = pen;
}

/*!
  Sets the pen that will be used to draw the bracket when selected
  
  \see setPen, setSelected
*/
void QCPItemBracket::setSelectedPen(const QPen &pen)
{
  mSelectedPen = pen;
}

/*!
  Sets the \a length in pixels how far the bracket extends in the direction towards the embraced
  span of the bracket (i.e. perpendicular to the <i>left</i>-<i>right</i>-direction)
  
  \image html QCPItemBracket-length.png
  <center>Demonstrating the effect of different values for \ref setLength, for styles \ref
  bsCalligraphic and \ref bsSquare. Anchors and positions are displayed for reference.</center>
*/
void QCPItemBracket::setLength(double length)
{
  mLength = length;
}

/*!
  Sets the style of the bracket, i.e. the shape/visual appearance.
  
  \see setPen
*/
void QCPItemBracket::setStyle(QCPItemBracket::BracketStyle style)
{
  mStyle = style;
}

/* inherits documentation from base class */
double QCPItemBracket::selectTest(const QPointF &pos) const
{
  if (!mVisible)
    return -1;
  
  QVector2D leftVec(left->pixelPoint());
  QVector2D rightVec(right->pixelPoint());
  if (leftVec.toPoint() == rightVec.toPoint())
    return -1;
  
  QVector2D widthVec = (rightVec-leftVec)*0.5;
  QVector2D lengthVec(-widthVec.y(), widthVec.x());
  lengthVec = lengthVec.normalized()*mLength;
  QVector2D centerVec = (rightVec+leftVec)*0.5-lengthVec;
  
  return qSqrt(distSqrToLine((centerVec-widthVec).toPointF(), (centerVec+widthVec).toPointF(), pos));
}

/* inherits documentation from base class */
void QCPItemBracket::draw(QCPPainter *painter)
{
  QVector2D leftVec(left->pixelPoint());
  QVector2D rightVec(right->pixelPoint());
  if (leftVec.toPoint() == rightVec.toPoint())
    return;
  
  QVector2D widthVec = (rightVec-leftVec)*0.5;
  QVector2D lengthVec(-widthVec.y(), widthVec.x());
  lengthVec = lengthVec.normalized()*mLength;
  QVector2D centerVec = (rightVec+leftVec)*0.5-lengthVec;

  QPolygon boundingPoly;
  boundingPoly << leftVec.toPoint() << rightVec.toPoint()
               << (rightVec-lengthVec).toPoint() << (leftVec-lengthVec).toPoint();
  QRect clip = clipRect().adjusted(-mainPen().widthF(), -mainPen().widthF(), mainPen().widthF(), mainPen().widthF());
  if (clip.intersects(boundingPoly.boundingRect()))
  {
    painter->setPen(mainPen());
    switch (mStyle)
    {
      case bsSquare:
      {
        painter->drawLine((centerVec+widthVec).toPointF(), (centerVec-widthVec).toPointF());
        painter->drawLine((centerVec+widthVec).toPointF(), (centerVec+widthVec+lengthVec).toPointF());
        painter->drawLine((centerVec-widthVec).toPointF(), (centerVec-widthVec+lengthVec).toPointF());
        break;
      }
      case bsRound:
      {
        painter->setBrush(Qt::NoBrush);
        QPainterPath path;
        path.moveTo((centerVec+widthVec+lengthVec).toPointF());
        path.cubicTo((centerVec+widthVec).toPointF(), (centerVec+widthVec).toPointF(), centerVec.toPointF());
        path.cubicTo((centerVec-widthVec).toPointF(), (centerVec-widthVec).toPointF(), (centerVec-widthVec+lengthVec).toPointF());
        painter->drawPath(path);
        break;
      }
      case bsCurly:
      {
        painter->setBrush(Qt::NoBrush);
        QPainterPath path;
        path.moveTo((centerVec+widthVec+lengthVec).toPointF());
        path.cubicTo((centerVec+widthVec*1-lengthVec*0.8).toPointF(), (centerVec+0.4*widthVec+1*lengthVec).toPointF(), centerVec.toPointF());
        path.cubicTo((centerVec-0.4*widthVec+1*lengthVec).toPointF(), (centerVec-widthVec*1-lengthVec*0.8).toPointF(), (centerVec-widthVec+lengthVec).toPointF());
        painter->drawPath(path);
        break;
      }
      case bsCalligraphic:
      {
        painter->setPen(Qt::NoPen);
        painter->setBrush(QBrush(mainPen().color()));
        QPainterPath path;
        path.moveTo((centerVec+widthVec+lengthVec).toPointF());
        
        path.cubicTo((centerVec+widthVec*1-lengthVec*0.8).toPointF(), (centerVec+0.4*widthVec+0.8*lengthVec).toPointF(), centerVec.toPointF());
        path.cubicTo((centerVec-0.4*widthVec+0.8*lengthVec).toPointF(), (centerVec-widthVec*1-lengthVec*0.8).toPointF(), (centerVec-widthVec+lengthVec).toPointF());
        
        path.cubicTo((centerVec-widthVec*1-lengthVec*0.5).toPointF(), (centerVec-0.2*widthVec+1.2*lengthVec).toPointF(), (centerVec+lengthVec*0.2).toPointF());
        path.cubicTo((centerVec+0.2*widthVec+1.2*lengthVec).toPointF(), (centerVec+widthVec*1-lengthVec*0.5).toPointF(), (centerVec+widthVec+lengthVec).toPointF());
        
        painter->drawPath(path);
        break;
      }
    }
  }
}

/* inherits documentation from base class */
QPointF QCPItemBracket::anchorPixelPoint(int anchorId) const
{
  QVector2D leftVec(left->pixelPoint());
  QVector2D rightVec(right->pixelPoint());
  if (leftVec.toPoint() == rightVec.toPoint())
    return leftVec.toPointF();
  
  QVector2D widthVec = (rightVec-leftVec)*0.5;
  QVector2D lengthVec(-widthVec.y(), widthVec.x());
  lengthVec = lengthVec.normalized()*mLength;
  QVector2D centerVec = (rightVec+leftVec)*0.5-lengthVec;
  
  switch (anchorId)
  {
    case aiCenter:
      return centerVec.toPointF();
  }
  qDebug() << Q_FUNC_INFO << "invalid anchorId" << anchorId;
  return QPointF();
}

/*! \internal

  Returns the pen that should be used for drawing lines. Returns mPen when the
  item is not selected and mSelectedPen when it is.
*/
QPen QCPItemBracket::mainPen() const
{
    return mSelected ? mSelectedPen : mPen;
}


// ================================================================================
// =================== QCPItemTracer
// ================================================================================

/*! \class QCPItemTracer
  \brief Item that sticks to QCPGraph data points

  \image html QCPItemTracer.png "Tracer example. Blue dotted circles are anchors, solid blue discs are positions."

  The tracer can be connected with a QCPGraph via \ref setGraph. Then it will automatically adopt
  the coordinate axes of the graph and update its \a position to be on the graph's data. This means
  the key stays controllable via \ref setGraphKey, but the value will follow the graph data. If a
  QCPGraph is connected, note that setting the coordinates directly via \a position will have no
  effect, i.e. be overriden in the next redraw (this is when the coodinate update happens).
  
  If the specified key in \ref setGraphKey is outside the key bounds of the graph, the tracer will
  stay at the respective end of the graph.
  
  With \ref setInterpolating you may specify whether the tracer may only stay exactly on data
  points or whether it interpolates data points linearly, if given a key that lies between two data
  points of the graph.
  
  The tracer has different visual styles, see \ref setStyle. It is also possible to make the tracer
  have no own visual appearance (set the style to \ref tsNone), and just connect other item
  positions to the tracer \a position (used as an anchor) via \ref
  QCPItemPosition::setParentAnchor.
  
  \note The tracer position is only automatically updated upon redraws. This means when, for
  example, the data of the graph changes and you immediately afterwards (without a redraw) read the
  \a position coordinates of the tracer, they will not reflect the updated data of the graph. In
  this case you should call \ref updatePosition manually, prior to reading the tracer coordinates.
*/

/*!
  Creates a tracer item and sets default values.
  
  The constructed item can be added to the plot with QCustomPlot::addItem.
*/
QCPItemTracer::QCPItemTracer(QCustomPlot *parentPlot) :
  QCPAbstractItem(parentPlot),
  position(createPosition("position")),
  mGraph(0)
{
  position->setCoords(0, 0);

  setBrush(Qt::NoBrush);
  setSelectedBrush(Qt::NoBrush);
  setPen(QPen(Qt::black));
  setSelectedPen(QPen(Qt::blue, 2));
  setStyle(tsCrosshair);
  setSize(6);
  setInterpolating(false);
  setGraphKey(0);
}

QCPItemTracer::~QCPItemTracer()
{
}

/*!
  Sets the pen that will be used to draw the line of the tracer
  
  \see setSelectedPen, setBrush
*/
void QCPItemTracer::setPen(const QPen &pen)
{
  mPen = pen;
}

/*!
  Sets the pen that will be used to draw the line of the tracer when selected
  
  \see setPen, setSelected
*/
void QCPItemTracer::setSelectedPen(const QPen &pen)
{
  mSelectedPen = pen;
}

/*!
  Sets the brush that will be used to draw any fills of the tracer
  
  \see setSelectedBrush, setPen
*/
void QCPItemTracer::setBrush(const QBrush &brush)
{
  mBrush = brush;
}

/*!
  Sets the brush that will be used to draw any fills of the tracer, when selected.
  
  \see setBrush, setSelected
*/
void QCPItemTracer::setSelectedBrush(const QBrush &brush)
{
  mSelectedBrush = brush;
}

/*!
  Sets the size of the tracer in pixels, if the style supports setting a size (e.g. \ref tsSquare
  does, \ref tsCrosshair does not).
*/
void QCPItemTracer::setSize(double size)
{
  mSize = size;
}

/*!
  Sets the style/visual appearance of the tracer.
  
  If you only want to use the tracer \a position as an anchor for other items, set \a style to
  \ref tsNone.
*/
void QCPItemTracer::setStyle(QCPItemTracer::TracerStyle style)
{
  mStyle = style;
}

/*!
  Sets the QCPGraph this tracer sticks to. The tracer \a position will be set to type
  QCPItemPosition::ptPlotCoords and the axes will be set to the axes of \a graph.
  
  To free the tracer from any graph, set \a graph to 0. The tracer \a position can then be placed
  freely like any other item position. This is the state the tracer will assume when its graph gets
  deleted while still attached to it.
  
  \see setGraphKey
*/
void QCPItemTracer::setGraph(QCPGraph *graph)
{
  if (graph)
  {
    if (graph->parentPlot() == mParentPlot)
    {
      position->setType(QCPItemPosition::ptPlotCoords);
      position->setAxes(graph->keyAxis(), graph->valueAxis());
      mGraph = graph;
      updatePosition();
    } else
      qDebug() << Q_FUNC_INFO << "graph isn't in same QCustomPlot instance as this item";
  } else
  {
    mGraph = 0;
  }
}

/*!
  Sets the key of the graph's data point the tracer will be positioned at. This is the only free
  cordinate of a tracer when attached to a graph.
  
  Depending on \ref setInterpolating, the tracer will be either positioned on the data point
  closest to \a key, or will stay exactly at \a key and interpolate the value linearly.
  
  \see setGraph, setInterpolating
*/
void QCPItemTracer::setGraphKey(double key)
{
  mGraphKey = key;
}

/*!
  Sets whether the value of the graph's data points shall be interpolated, when positioning the
  tracer.
  
  If \a enabled is set to false and a key is given with \ref setGraphKey, the tracer is placed on
  the data point of the graph which is closest to the key, but which is not necessarily exactly
  there. If \a enabled is true, the tracer will be positioned exactly at the specified key, and
  the appropriate value will be interpolated from the graph's data points linearly.
  
  \see setGraph, setGraphKey
*/
void QCPItemTracer::setInterpolating(bool enabled)
{
  mInterpolating = enabled;
}

/* inherits documentation from base class */
double QCPItemTracer::selectTest(const QPointF &pos) const
{
  if (!mVisible || mStyle == tsNone)
    return -1;

  QPointF center(position->pixelPoint());
  double w = mSize/2.0;
  QRect clip = clipRect();
  switch (mStyle)
  {
    case tsNone: return -1;
    case tsPlus:
    {
      if (clipRect().intersects(QRectF(center-QPointF(w, w), center+QPointF(w, w)).toRect()))
        return qSqrt(qMin(distSqrToLine(center+QPointF(-w, 0), center+QPointF(w, 0), pos),
                          distSqrToLine(center+QPointF(0, -w), center+QPointF(0, w), pos)));
      break;
    }
    case tsCrosshair:
    {
      return qSqrt(qMin(distSqrToLine(QPointF(clip.left(), center.y()), QPointF(clip.right(), center.y()), pos),
                        distSqrToLine(QPointF(center.x(), clip.top()), QPointF(center.x(), clip.bottom()), pos)));
      break;
    }
    case tsCircle:
    {
      if (clip.intersects(QRectF(center-QPointF(w, w), center+QPointF(w, w)).toRect()))
      {
        // distance to border:
        double centerDist = QVector2D(center-pos).length();
        double circleLine = w;
        double result = qAbs(centerDist-circleLine);
        // filled ellipse, allow click inside to count as hit:
        if (result > mParentPlot->selectionTolerance()*0.99 && mBrush.style() != Qt::NoBrush && mBrush.color().alpha() != 0)
        {
          if (centerDist <= circleLine)
            result = mParentPlot->selectionTolerance()*0.99;
        }
        return result;
      }
      break;
    }
    case tsSquare:
    {
      if (clip.intersects(QRectF(center-QPointF(w, w), center+QPointF(w, w)).toRect()))
      {
        QRectF rect = QRectF(center-QPointF(w, w), center+QPointF(w, w));
        bool filledRect = mBrush.style() != Qt::NoBrush && mBrush.color().alpha() != 0;
        return rectSelectTest(rect, pos, filledRect);
      }
      break;
    }
  }
  return -1;
}

/* inherits documentation from base class */
void QCPItemTracer::draw(QCPPainter *painter)
{
  updatePosition();
  if (mStyle == tsNone)
    return;

  painter->setPen(mainPen());
  painter->setBrush(mainBrush());
  QPointF center(position->pixelPoint());
  double w = mSize/2.0;
  QRect clip = clipRect();
  switch (mStyle)
  {
    case tsNone: return;
    case tsPlus:
    {
      if (clip.intersects(QRectF(center-QPointF(w, w), center+QPointF(w, w)).toRect()))
      {
        painter->drawLine(QLineF(center+QPointF(-w, 0), center+QPointF(w, 0)));
        painter->drawLine(QLineF(center+QPointF(0, -w), center+QPointF(0, w)));
      }
      break;
    }
    case tsCrosshair:
    {
      if (center.y() > clip.top() && center.y() < clip.bottom())
        painter->drawLine(QLineF(clip.left(), center.y(), clip.right(), center.y()));
      if (center.x() > clip.left() && center.x() < clip.right())
        painter->drawLine(QLineF(center.x(), clip.top(), center.x(), clip.bottom()));
      break;
    }
    case tsCircle:
    {
      if (clip.intersects(QRectF(center-QPointF(w, w), center+QPointF(w, w)).toRect()))
        painter->drawEllipse(center, w, w);
      break;
    }
    case tsSquare:
    {
      if (clip.intersects(QRectF(center-QPointF(w, w), center+QPointF(w, w)).toRect()))
        painter->drawRect(QRectF(center-QPointF(w, w), center+QPointF(w, w)));
      break;
    }
  }
}

/*!
  If the tracer is connected with a graph (\ref setGraph), this function updates the tracer's \a
  position to reside on the graph data, depending on the configured key (\ref setGraphKey).
  
  It is called automatically on every redraw and normally doesn't need to be called manually. One
  exception is when you want to read the tracer coordinates via \a position and are not sure that
  the graph's data (or the tracer key with \ref setGraphKey) hasn't changed since the last redraw.
  In that situation, call this function before accessing \a position, to make sure you don't get
  out-of-date coordinates.
  
  If there is no graph set on this tracer, this function does nothing.
*/
void QCPItemTracer::updatePosition()
{
  if (mGraph)
  {
    if (mParentPlot->hasPlottable(mGraph))
    {
      if (mGraph->data()->size() > 1)
      {
        QCPDataMap::const_iterator first = mGraph->data()->constBegin();
        QCPDataMap::const_iterator last = mGraph->data()->constEnd()-1;
        if (mGraphKey < first.key())
          position->setCoords(first.key(), first.value().value);
        else if (mGraphKey > last.key())
          position->setCoords(last.key(), last.value().value);
        else
        {
          QCPDataMap::const_iterator it = first;
          it = mGraph->data()->lowerBound(mGraphKey);
          if (it != first) // mGraphKey is somewhere between iterators
          {
            QCPDataMap::const_iterator prevIt = it-1;
            if (mInterpolating)
            {
              // interpolate between iterators around mGraphKey:
              double slope = (it.value().value-prevIt.value().value)/(it.key()-prevIt.key());
              position->setCoords(mGraphKey, (mGraphKey-prevIt.key())*slope+prevIt.value().value);
            } else
            {
              // find iterator with key closest to mGraphKey:
              if (mGraphKey < (prevIt.key()+it.key())*0.5)
                it = prevIt;
              position->setCoords(it.key(), it.value().value);
            }
          } else // mGraphKey is exactly on first iterator
            position->setCoords(it.key(), it.value().value);
        }
      } else if (mGraph->data()->size() == 1)
      {
        QCPDataMap::const_iterator it = mGraph->data()->constBegin();
        position->setCoords(it.key(), it.value().value);
      } else
        qDebug() << Q_FUNC_INFO << "graph has no data";
    } else
      qDebug() << Q_FUNC_INFO << "graph not contained in QCustomPlot instance (anymore)";
  }
}

/*! \internal

  Returns the pen that should be used for drawing lines. Returns mPen when the item is not selected
  and mSelectedPen when it is.
*/
QPen QCPItemTracer::mainPen() const
{
  return mSelected ? mSelectedPen : mPen;
}

/*! \internal

  Returns the brush that should be used for drawing fills of the item. Returns mBrush when the item
  is not selected and mSelectedBrush when it is.
*/
QBrush QCPItemTracer::mainBrush() const
{
  return mSelected ? mSelectedBrush : mBrush;
}







