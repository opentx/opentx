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

/*! \file */

#ifndef QCUSTOMPLOT_H
#define QCUSTOMPLOT_H

#include <QObject>
#include <QWidget>
#include <QPainter>
#include <QPaintEvent>
#include <QPixmap>
#include <QVector>
#include <QString>
#include <QPrinter>
#include <QDateTime>
#include <QMultiMap>
#include <QFlags>
#include <QDebug>
#include <QVector2D>
#include <QStack>
#include <qmath.h>
#include <limits>

// decl definitions for shared library compilation/usage:
#if defined(QCUSTOMPLOT_COMPILE_LIBRARY)
#  define QCP_LIB_DECL Q_DECL_EXPORT
#elif defined(QCUSTOMPLOT_USE_LIBRARY)
#  define QCP_LIB_DECL Q_DECL_IMPORT
#else
#  define QCP_LIB_DECL
#endif

class QCustomPlot;
class QCPLegend;
class QCPRange;
class QCPLayerable;
class QCPAbstractItem;
class QCPItemPosition;
class QCPAxis;
class QCPData;

/*!
  The QCP Namespace contains general enums and QFlags 
*/
namespace QCP
{
/*!
  Defines the symbol used for scatter points.
  
  On plottables/items that draw scatters, the sizes of these visualizations (with exception of \ref
  QCP::ssDot and \ref QCP::ssPixmap) can be controlled with a \a setScatterSize function. Scatters
  are in general drawn with the main pen set on the plottable/item.
  
  \see QCPGraph::setScatterStyle, QCPStatisticalBox::setOutlierStyle
*/
enum ScatterStyle { ssNone       ///< no scatter symbols are drawn (e.g. in QCPGraph, data only represented with lines)
                    ,ssDot       ///< a single pixel
                    ,ssCross     ///< a cross (x)
                    ,ssPlus      ///< a plus (+)
                    ,ssCircle    ///< a circle which is not filled
                    ,ssDisc      ///< a circle which is filled with the color of the pen (not the brush!)
                    ,ssSquare    ///< a square which is not filled
                    ,ssDiamond   ///< a diamond which is not filled
                    ,ssStar      ///< a star with eight arms, i.e. a combination of cross and plus
                    ,ssTriangle  ///< an equilateral triangle which is not filled, standing on baseline
                    ,ssTriangleInverted ///< an equilateral triangle which is not filled, standing on corner
                    ,ssCrossSquare      ///< a square which is not filled, with a cross inside
                    ,ssPlusSquare       ///< a square which is not filled, with a plus inside
                    ,ssCrossCircle      ///< a circle which is not filled, with a cross inside
                    ,ssPlusCircle       ///< a circle which is not filled, with a plus inside
                    ,ssPeace     ///< a circle which is not filled, with one vertical and two downward diagonal lines
                    ,ssPixmap    ///< a custom pixmap specified by setScatterPixmap, centered on the data point coordinates
                  };

/*!
  Defines what elements of a plot can be forcibly drawn antialiased/not antialiased. If an
  element is neither forcibly drawn antialiased nor forcibly drawn not antialiased, it is up to
  the respective element how it is drawn. Typically it provides a \a setAntialiased function for
  this.
  
  \c AntialiasedElements is a flag of or-combined elements of this enum type.
  
  \see QCustomPlot::setAntialiasedElements, QCustomPlot::setNotAntialiasedElements
*/
enum AntialiasedElement { aeAxes           = 0x0001 ///< <tt>0x0001</tt> Axis base line and tick marks
                          ,aeGrid          = 0x0002 ///< <tt>0x0002</tt> Grid lines
                          ,aeSubGrid       = 0x0004 ///< <tt>0x0004</tt> Sub grid lines
                          ,aeLegend        = 0x0008 ///< <tt>0x0008</tt> Legend box
                          ,aeLegendItems   = 0x0010 ///< <tt>0x0010</tt> Legend items
                          ,aePlottables    = 0x0020 ///< <tt>0x0020</tt> Main lines of plottables (excluding error bars, see element \ref aeErrorBars)
                          ,aeItems         = 0x0040 ///< <tt>0x0040</tt> Main lines of items
                          ,aeScatters      = 0x0080 ///< <tt>0x0080</tt> Scatter symbols of plottables (excluding scatter symbols of type ssPixmap)
                          ,aeErrorBars     = 0x0100 ///< <tt>0x0100</tt> Error bars
                          ,aeFills         = 0x0200 ///< <tt>0x0200</tt> Borders of fills (e.g. under or between graphs)
                          ,aeZeroLine      = 0x0400 ///< <tt>0x0400</tt> Zero-lines, see \ref QCPAxis::setZeroLinePen
                          ,aeAll           = 0xFFFF ///< <tt>0xFFFF</tt> All elements
                          ,aeNone          = 0x0000 ///< <tt>0x0000</tt> No elements
                        }; 
Q_DECLARE_FLAGS(AntialiasedElements, AntialiasedElement)

/*!
  Defines plotting hints that control various aspects of the quality and speed of plotting.
  \see QCustomPlot::setPlottingHints
*/
enum PlottingHint { phNone            = 0x000 ///< <tt>0x000</tt> No hints are set
                    ,phFastPolylines  = 0x001 ///< <tt>0x001</tt> Graph/Curve lines are drawn with a faster method. This reduces the quality
                                              ///<                especially of the line segment joins. (Only used for solid line pens.)
                    ,phForceRepaint   = 0x002 ///< <tt>0x002</tt> causes an immediate repaint() instead of a soft update() when QCustomPlot::replot() is called. This is set by default
                                              ///<                on Windows-Systems to prevent the plot from freezing on fast consecutive replots (e.g. user drags ranges with mouse).
                  };
Q_DECLARE_FLAGS(PlottingHints, PlottingHint)
}

Q_DECLARE_OPERATORS_FOR_FLAGS(QCP::AntialiasedElements)
Q_DECLARE_OPERATORS_FOR_FLAGS(QCP::PlottingHints)

class QCP_LIB_DECL QCPData
{
public:
  QCPData();
  QCPData(double key, double value);
  double key, value;
  double keyErrorPlus, keyErrorMinus;
  double valueErrorPlus, valueErrorMinus;
};
Q_DECLARE_TYPEINFO(QCPData, Q_MOVABLE_TYPE);

/*! \typedef QCPDataMap
  Container for storing QCPData items in a sorted fashion. The key of the map
  is the key member of the QCPData instance.
  
  This is the container in which QCPGraph holds its data.
  \see QCPData, QCPGraph::setData
*/
typedef QMap<double, QCPData> QCPDataMap;
typedef QMapIterator<double, QCPData> QCPDataMapIterator;
typedef QMutableMapIterator<double, QCPData> QCPDataMutableMapIterator;

class QCP_LIB_DECL QCPCurveData
{
public:
  QCPCurveData();
  QCPCurveData(double t, double key, double value);
  double t, key, value;
};
Q_DECLARE_TYPEINFO(QCPCurveData, Q_MOVABLE_TYPE);

/*! \typedef QCPCurveDataMap
  Container for storing QCPCurveData items in a sorted fashion. The key of the map
  is the t member of the QCPCurveData instance.
  
  This is the container in which QCPCurve holds its data.
  \see QCPCurveData, QCPCurve::setData
*/

typedef QMap<double, QCPCurveData> QCPCurveDataMap;
typedef QMapIterator<double, QCPCurveData> QCPCurveDataMapIterator;
typedef QMutableMapIterator<double, QCPCurveData> QCPCurveDataMutableMapIterator;

class QCP_LIB_DECL QCPBarData
{
public:
  QCPBarData();
  QCPBarData(double key, double value);
  double key, value;
};
Q_DECLARE_TYPEINFO(QCPBarData, Q_MOVABLE_TYPE);

/*! \typedef QCPBarDataMap
  Container for storing QCPBarData items in a sorted fashion. The key of the map
  is the key member of the QCPBarData instance.
  
  This is the container in which QCPBars holds its data.
  \see QCPBarData, QCPBars::setData
*/
typedef QMap<double, QCPBarData> QCPBarDataMap;
typedef QMapIterator<double, QCPBarData> QCPBarDataMapIterator;
typedef QMutableMapIterator<double, QCPBarData> QCPBarDataMutableMapIterator;

class QCP_LIB_DECL QCPPainter : public QPainter
{
public:
  QCPPainter();
  QCPPainter(QPaintDevice *device);
  ~QCPPainter();
  
  // getters:
  QPixmap scatterPixmap() const { return mScatterPixmap; }
  bool antialiasing() const { return testRenderHint(QPainter::Antialiasing); }
  bool pdfExportMode() const { return mPdfExportMode; }
  bool scaledExportMode() const { return mScaledExportMode; }
  
  // setters:
  void setScatterPixmap(const QPixmap pm);
  void setAntialiasing(bool enabled);
  void setPdfExportMode(bool enabled);
  void setScaledExportMode(bool enabled);
 
  // methods hiding non-virtual base class functions (QPainter bug workarounds):
  void setPen(const QPen &pen);
  void setPen(const QColor &color);
  void setPen(Qt::PenStyle penStyle);
  void drawLine(const QLineF &line);
  void drawLine(const QPointF &p1, const QPointF &p2) {drawLine(QLineF(p1, p2));}
  void save();
  void restore();

  // helpers:
  void fixScaledPen();
  void drawScatter(double x, double y, double size, QCP::ScatterStyle style);
  
protected:
  QPixmap mScatterPixmap;
  bool mScaledExportMode;
  bool mPdfExportMode;
  bool mIsAntialiasing;
  QStack<bool> mAntialiasingStack;
};

class QCP_LIB_DECL QCPLineEnding
{
public:
  /*!
    Defines the type of ending decoration for line-like items, e.g. an arrow.
    
    \image html QCPLineEnding.png
    
    The width and length of these decorations can be controlled with the functions \ref setWidth
    and \ref setLength. Some decorations like \ref esDisc, \ref esSquare, \ref esDiamond and \ref esBar only
    support a width, the length property is ignored.
    
    \see QCPItemLine::setHead, QCPItemLine::setTail, QCPItemCurve::setHead, QCPItemCurve::setTail
  */
  enum EndingStyle { esNone          ///< No ending decoration
                     ,esFlatArrow    ///< A filled arrow head with a straight/flat back (a triangle)
                     ,esSpikeArrow   ///< A filled arrow head with an indented back
                     ,esLineArrow    ///< A non-filled arrow head with open back
                     ,esDisc         ///< A filled circle
                     ,esSquare       ///< A filled square
                     ,esDiamond      ///< A filled diamond (45° rotated square)
                     ,esBar          ///< A bar perpendicular to the line
                   };
  
  QCPLineEnding();
  QCPLineEnding(EndingStyle style, double width=8, double length=10, bool inverted=false);
  
  // getters:
  EndingStyle style() const { return mStyle; }
  double width() const { return mWidth; }
  double length() const { return mLength; }
  bool inverted() const { return mInverted; }
  
  // setters:
  void setStyle(EndingStyle style);
  void setWidth(double width);
  void setLength(double length);
  void setInverted(bool inverted);
  
  // non-property methods:
  double boundingDistance() const;
  void draw(QCPPainter *painter, const QVector2D &pos, const QVector2D &dir) const;
  void draw(QCPPainter *painter, const QVector2D &pos, double angle) const;
  
protected:
  EndingStyle mStyle;
  double mWidth, mLength;
  bool mInverted;
};
Q_DECLARE_TYPEINFO(QCPLineEnding, Q_MOVABLE_TYPE);

class QCP_LIB_DECL QCPLayer
{
public:
  QCPLayer(QCustomPlot* parentPlot, const QString &layerName);
  ~QCPLayer();
  
  // getters:
  QCustomPlot *parentPlot() const { return mParentPlot; }
  QString name() const { return mName; }
  int index() const;
  QList<QCPLayerable*> children() const { return mChildren; }
  
protected:
  QCustomPlot *mParentPlot;
  QString mName;
  QList<QCPLayerable*> mChildren;
  
  void addChild(QCPLayerable *layerable, bool prepend);
  void removeChild(QCPLayerable *layerable);
  
private:
  Q_DISABLE_COPY(QCPLayer)
  
  friend class QCPLayerable;
};

class QCP_LIB_DECL QCPLayerable : public QObject
{
  Q_OBJECT
public:
  QCPLayerable(QCustomPlot *parentPlot);
  ~QCPLayerable();
  
  // getters:
  bool visible() const { return mVisible; }
  QCustomPlot *parentPlot() const { return mParentPlot; }
  QCPLayer *layer() const { return mLayer; }
  bool antialiased() const { return mAntialiased; }
  
  // setters:
  void setVisible(bool on);
  bool setLayer(QCPLayer *layer);
  bool setLayer(const QString &layerName);
  void setAntialiased(bool enabled);
  
protected:
  bool mVisible;
  QCustomPlot *mParentPlot;
  QCPLayer *mLayer;
  bool mAntialiased;
  
  // non-property methods:
  bool moveToLayer(QCPLayer *layer, bool prepend);
  
  void applyAntialiasingHint(QCPPainter *painter, bool localAntialiased, QCP::AntialiasedElement overrideElement) const;
  virtual void applyDefaultAntialiasingHint(QCPPainter *painter) const = 0;
  virtual QRect clipRect() const;
  virtual void draw(QCPPainter *painter) = 0;
  
private:
  Q_DISABLE_COPY(QCPLayerable)
  
  friend class QCustomPlot;
};

class QCP_LIB_DECL QCPAbstractPlottable : public QCPLayerable
{
  Q_OBJECT
public:
  QCPAbstractPlottable(QCPAxis *keyAxis, QCPAxis *valueAxis);
  virtual ~QCPAbstractPlottable() {}
  
  // getters:
  QString name() const { return mName; }
  bool antialiasedFill() const { return mAntialiasedFill; }
  bool antialiasedScatters() const { return mAntialiasedScatters; }
  bool antialiasedErrorBars() const { return mAntialiasedErrorBars; }
  QPen pen() const { return mPen; }
  QPen selectedPen() const { return mSelectedPen; }
  QBrush brush() const { return mBrush; }
  QBrush selectedBrush() const { return mSelectedBrush; }
  QCPAxis *keyAxis() const { return mKeyAxis; }
  QCPAxis *valueAxis() const { return mValueAxis; }
  bool selectable() const { return mSelectable; }
  bool selected() const { return mSelected; }
  
  // setters:
  void setName(const QString &name);
  void setAntialiasedFill(bool enabled);
  void setAntialiasedScatters(bool enabled);
  void setAntialiasedErrorBars(bool enabled);
  void setPen(const QPen &pen);
  void setSelectedPen(const QPen &pen);
  void setBrush(const QBrush &brush);
  void setSelectedBrush(const QBrush &brush);
  void setKeyAxis(QCPAxis *axis);
  void setValueAxis(QCPAxis *axis);
  void setSelectable(bool selectable);
  void setSelected(bool selected);

  // non-property methods:
  void rescaleAxes(bool onlyEnlarge=false) const;
  void rescaleKeyAxis(bool onlyEnlarge=false) const;
  void rescaleValueAxis(bool onlyEnlarge=false) const;
  virtual void clearData() = 0;
  virtual double selectTest(const QPointF &pos) const = 0;
  virtual bool addToLegend();
  virtual bool removeFromLegend() const;
  
signals:
  void selectionChanged(bool selected);
  
protected:
  /*!
    Represents negative and positive sign domain for passing to \ref getKeyRange and \ref getValueRange.
  */
  enum SignDomain { sdNegative  ///< The negative sign domain, i.e. numbers smaller than zero
                    ,sdBoth     ///< Both sign domains, including zero, i.e. all (rational) numbers
                    ,sdPositive ///< The positive sign domain, i.e. numbers greater than zero
                  };
  QString mName;
  bool mAntialiasedFill, mAntialiasedScatters, mAntialiasedErrorBars;
  QPen mPen, mSelectedPen;
  QBrush mBrush, mSelectedBrush;
  QCPAxis *mKeyAxis, *mValueAxis;
  bool mSelected, mSelectable;
  
  virtual QRect clipRect() const;
  virtual void draw(QCPPainter *painter) = 0;
  virtual void drawLegendIcon(QCPPainter *painter, const QRect &rect) const = 0;
  virtual QCPRange getKeyRange(bool &validRange, SignDomain inSignDomain=sdBoth) const = 0;
  virtual QCPRange getValueRange(bool &validRange, SignDomain inSignDomain=sdBoth) const = 0;
  
  // painting and coordinate transformation helpers:
  void coordsToPixels(double key, double value, double &x, double &y) const;
  const QPointF coordsToPixels(double key, double value) const;
  void pixelsToCoords(double x, double y, double &key, double &value) const;
  void pixelsToCoords(const QPointF &pixelPos, double &key, double &value) const;
  QPen mainPen() const;
  QBrush mainBrush() const;
  void applyDefaultAntialiasingHint(QCPPainter *painter) const;
  void applyFillAntialiasingHint(QCPPainter *painter) const;
  void applyScattersAntialiasingHint(QCPPainter *painter) const;
  void applyErrorBarsAntialiasingHint(QCPPainter *painter) const;
  
  // selection test helpers:
  double distSqrToLine(const QPointF &start, const QPointF &end, const QPointF &point) const;

private:
  Q_DISABLE_COPY(QCPAbstractPlottable)
  
  friend class QCustomPlot;
  friend class QCPPlottableLegendItem;
};

class QCP_LIB_DECL QCPGraph : public QCPAbstractPlottable
{
  Q_OBJECT
public:
  /*!
    Defines how the graph's line is represented visually in the plot. The line is drawn with the
    current pen of the graph (\ref setPen).
    \see setLineStyle
  */
  enum LineStyle { lsNone        ///< data points are not connected with any lines (e.g. data only represented
                                 ///< with symbols according to the scatter style, see \ref setScatterStyle)
                  ,lsLine        ///< data points are connected by a straight line
                  ,lsStepLeft    ///< line is drawn as steps where the step height is the value of the left data point
                  ,lsStepRight   ///< line is drawn as steps where the step height is the value of the right data point
                  ,lsStepCenter  ///< line is drawn as steps where the step is in between two data points
                  ,lsImpulse     ///< each data point is represented by a line parallel to the value axis, which reaches from the data point to the zero-value-line
                 };
  Q_ENUMS(LineStyle)
  /*!
    Defines what kind of error bars are drawn for each data point
  */
  enum ErrorType { etNone   ///< No error bars are shown
                  ,etKey    ///< Error bars for the key dimension of the data point are shown
                  ,etValue  ///< Error bars for the value dimension of the data point are shown
                  ,etBoth   ///< Error bars for both key and value dimensions of the data point are shown
                 };
  Q_ENUMS(ErrorType)
  
  explicit QCPGraph(QCPAxis *keyAxis, QCPAxis *valueAxis);
  virtual ~QCPGraph();
  
  // getters:
  const QCPDataMap *data() const { return mData; }
  LineStyle lineStyle() const { return mLineStyle; }
  QCP::ScatterStyle scatterStyle() const { return mScatterStyle; }
  double scatterSize() const { return mScatterSize; }
  const QPixmap scatterPixmap() const { return mScatterPixmap; }
  ErrorType errorType() const { return mErrorType; }
  QPen errorPen() const { return mErrorPen; }
  double errorBarSize() const { return mErrorBarSize; }
  bool errorBarSkipSymbol() const { return mErrorBarSkipSymbol; }
  QCPGraph *channelFillGraph() const { return mChannelFillGraph; }
  
  // setters:
  void setData(QCPDataMap *data, bool copy=false);
  void setData(const QVector<double> &key, const QVector<double> &value);
  void setDataKeyError(const QVector<double> &key, const QVector<double> &value, const QVector<double> &keyError);
  void setDataKeyError(const QVector<double> &key, const QVector<double> &value, const QVector<double> &keyErrorMinus, const QVector<double> &keyErrorPlus);
  void setDataValueError(const QVector<double> &key, const QVector<double> &value, const QVector<double> &valueError);
  void setDataValueError(const QVector<double> &key, const QVector<double> &value, const QVector<double> &valueErrorMinus, const QVector<double> &valueErrorPlus);
  void setDataBothError(const QVector<double> &key, const QVector<double> &value, const QVector<double> &keyError, const QVector<double> &valueError);
  void setDataBothError(const QVector<double> &key, const QVector<double> &value, const QVector<double> &keyErrorMinus, const QVector<double> &keyErrorPlus, const QVector<double> &valueErrorMinus, const QVector<double> &valueErrorPlus);
  void setLineStyle(LineStyle ls);
  void setScatterStyle(QCP::ScatterStyle ss);
  void setScatterSize(double size);
  void setScatterPixmap(const QPixmap &pixmap);
  void setErrorType(ErrorType errorType);
  void setErrorPen(const QPen &pen);
  void setErrorBarSize(double size);
  void setErrorBarSkipSymbol(bool enabled);
  void setChannelFillGraph(QCPGraph *targetGraph);
  
  // non-property methods:
  void addData(const QCPDataMap &dataMap);
  void addData(const QCPData &data);
  void addData(double key, double value);
  void addData(const QVector<double> &keys, const QVector<double> &values);
  void removeDataBefore(double key);
  void removeDataAfter(double key);
  void removeData(double fromKey, double toKey);
  void removeData(double key);
  virtual void clearData();
  virtual double selectTest(const QPointF &pos) const;
  using QCPAbstractPlottable::rescaleAxes;
  using QCPAbstractPlottable::rescaleKeyAxis;
  using QCPAbstractPlottable::rescaleValueAxis;
  virtual void rescaleAxes(bool onlyEnlarge, bool includeErrorBars) const; // overloads base class interface
  virtual void rescaleKeyAxis(bool onlyEnlarge, bool includeErrorBars) const; // overloads base class interface
  virtual void rescaleValueAxis(bool onlyEnlarge, bool includeErrorBars) const; // overloads base class interface
  
protected:
  QCPDataMap *mData;
  QPen mErrorPen;
  LineStyle mLineStyle;
  QCP::ScatterStyle mScatterStyle;
  double mScatterSize;
  QPixmap mScatterPixmap;
  ErrorType mErrorType;
  double mErrorBarSize;
  bool mErrorBarSkipSymbol;
  QCPGraph *mChannelFillGraph;

  virtual void draw(QCPPainter *painter);
  virtual void drawLegendIcon(QCPPainter *painter, const QRect &rect) const;

  // functions to generate plot data points in pixel coordinates:
  void getPlotData(QVector<QPointF> *lineData, QVector<QCPData> *pointData) const;
  // plot style specific functions to generate plot data, used by getPlotData:
  void getScatterPlotData(QVector<QCPData> *pointData) const;
  void getLinePlotData(QVector<QPointF> *lineData, QVector<QCPData> *pointData) const;
  void getStepLeftPlotData(QVector<QPointF> *lineData, QVector<QCPData> *pointData) const;
  void getStepRightPlotData(QVector<QPointF> *lineData, QVector<QCPData> *pointData) const;
  void getStepCenterPlotData(QVector<QPointF> *lineData, QVector<QCPData> *pointData) const;
  void getImpulsePlotData(QVector<QPointF> *lineData, QVector<QCPData> *pointData) const;
  
  // helper functions for drawing:
  void drawFill(QCPPainter *painter, QVector<QPointF> *lineData) const;
  void drawScatterPlot(QCPPainter *painter, QVector<QCPData> *pointData) const;
  void drawLinePlot(QCPPainter *painter, QVector<QPointF> *lineData) const;
  void drawImpulsePlot(QCPPainter *painter, QVector<QPointF> *lineData) const;
  void drawError(QCPPainter *painter, double x, double y, const QCPData &data) const;
  
  // helper functions:
  void getVisibleDataBounds(QCPDataMap::const_iterator &lower, QCPDataMap::const_iterator &upper, int &count) const;
  void addFillBasePoints(QVector<QPointF> *lineData) const;
  void removeFillBasePoints(QVector<QPointF> *lineData) const;
  QPointF lowerFillBasePoint(double lowerKey) const;
  QPointF upperFillBasePoint(double upperKey) const;
  const QPolygonF getChannelFillPolygon(const QVector<QPointF> *lineData) const;
  int findIndexBelowX(const QVector<QPointF> *data, double x) const;
  int findIndexAboveX(const QVector<QPointF> *data, double x) const;
  int findIndexBelowY(const QVector<QPointF> *data, double y) const;
  int findIndexAboveY(const QVector<QPointF> *data, double y) const;
  double pointDistance(const QPointF &pixelPoint) const;
  virtual QCPRange getKeyRange(bool &validRange, SignDomain inSignDomain=sdBoth) const;
  virtual QCPRange getValueRange(bool &validRange, SignDomain inSignDomain=sdBoth) const;
  virtual QCPRange getKeyRange(bool &validRange, SignDomain inSignDomain, bool includeErrors) const; // overloads base class interface
  virtual QCPRange getValueRange(bool &validRange, SignDomain inSignDomain, bool includeErrors) const; // overloads base class interface
  
  friend class QCustomPlot;
  friend class QCPLegend;
};

class QCP_LIB_DECL QCPCurve : public QCPAbstractPlottable
{
  Q_OBJECT
public:
  /*!
    Defines how the curve's line is represented visually in the plot. The line is drawn with the
    current pen of the curve (\ref setPen).
    \see setLineStyle
  */
  enum LineStyle { lsNone, ///< No line is drawn between data points (e.g. only scatters)
                   lsLine  ///< Data points are connected with a straight line
                 };
  explicit QCPCurve(QCPAxis *keyAxis, QCPAxis *valueAxis);
  virtual ~QCPCurve();
  
  // getters:
  const QCPCurveDataMap *data() const { return mData; }
  QCP::ScatterStyle scatterStyle() const { return mScatterStyle; }
  double scatterSize() const { return mScatterSize; }
  QPixmap scatterPixmap() const { return mScatterPixmap; }
  LineStyle lineStyle() const { return mLineStyle; }
  
  // setters:
  void setData(QCPCurveDataMap *data, bool copy=false);
  void setData(const QVector<double> &t, const QVector<double> &key, const QVector<double> &value);
  void setData(const QVector<double> &key, const QVector<double> &value);
  void setScatterStyle(QCP::ScatterStyle style);
  void setScatterSize(double size);
  void setScatterPixmap(const QPixmap &pixmap);
  void setLineStyle(LineStyle style);
  
  // non-property methods:
  void addData(const QCPCurveDataMap &dataMap);
  void addData(const QCPCurveData &data);
  void addData(double t, double key, double value);
  void addData(double key, double value);
  void addData(const QVector<double> &ts, const QVector<double> &keys, const QVector<double> &values);
  void removeDataBefore(double t);
  void removeDataAfter(double t);
  void removeData(double fromt, double tot);
  void removeData(double t);
  virtual void clearData();
  virtual double selectTest(const QPointF &pos) const;
  
protected:
  QCPCurveDataMap *mData;
  QCP::ScatterStyle mScatterStyle;
  double mScatterSize;
  QPixmap mScatterPixmap;
  LineStyle mLineStyle;
  
  virtual void draw(QCPPainter *painter);
  virtual void drawLegendIcon(QCPPainter *painter, const QRect &rect) const;
  // drawing helpers:
  virtual void drawScatterPlot(QCPPainter *painter, const QVector<QPointF> *pointData) const;
  
  // helper functions:
  void getCurveData(QVector<QPointF> *lineData) const;
  double pointDistance(const QPointF &pixelPoint) const;

  QPointF outsideCoordsToPixels(double key, double value, int region) const;
  virtual QCPRange getKeyRange(bool &validRange, SignDomain inSignDomain=sdBoth) const;
  virtual QCPRange getValueRange(bool &validRange, SignDomain inSignDomain=sdBoth) const;
  
  friend class QCustomPlot;
  friend class QCPLegend;
};

class QCP_LIB_DECL QCPBars : public QCPAbstractPlottable
{
  Q_OBJECT
public:
  explicit QCPBars(QCPAxis *keyAxis, QCPAxis *valueAxis);
  virtual ~QCPBars();
  
  // getters:
  double width() const { return mWidth; }
  QCPBars *barBelow() const { return mBarBelow; }
  QCPBars *barAbove() const { return mBarAbove; }
  const QCPBarDataMap *data() const { return mData; }
  
  // setters:
  void setWidth(double width);
  void setData(QCPBarDataMap *data, bool copy=false);
  void setData(const QVector<double> &key, const QVector<double> &value);
  
  // non-property methods:
  void moveBelow(QCPBars *bars);
  void moveAbove(QCPBars *bars);
  void addData(const QCPBarDataMap &dataMap);
  void addData(const QCPBarData &data);
  void addData(double key, double value);
  void addData(const QVector<double> &keys, const QVector<double> &values);
  void removeDataBefore(double key);
  void removeDataAfter(double key);
  void removeData(double fromKey, double toKey);
  void removeData(double key);
  virtual void clearData();
  virtual double selectTest(const QPointF &pos) const;
  
protected:
  QCPBarDataMap *mData;
  double mWidth;
  QCPBars *mBarBelow, *mBarAbove;
  
  virtual void draw(QCPPainter *painter);
  virtual void drawLegendIcon(QCPPainter *painter, const QRect &rect) const;
  
  QPolygonF getBarPolygon(double key, double value) const;
  double getBaseValue(double key, bool positive) const;
  static void connectBars(QCPBars* lower, QCPBars* upper);
  virtual QCPRange getKeyRange(bool &validRange, SignDomain inSignDomain=sdBoth) const;
  virtual QCPRange getValueRange(bool &validRange, SignDomain inSignDomain=sdBoth) const;
  
  friend class QCustomPlot;
  friend class QCPLegend;
};

class QCP_LIB_DECL QCPStatisticalBox : public QCPAbstractPlottable
{
  Q_OBJECT
public:
  explicit QCPStatisticalBox(QCPAxis *keyAxis, QCPAxis *valueAxis);
  virtual ~QCPStatisticalBox();
  
  // getters:
  double key() const { return mKey; }
  double minimum() const { return mMinimum; }
  double lowerQuartile() const { return mLowerQuartile; }
  double median() const { return mMedian; }
  double upperQuartile() const { return mUpperQuartile; }
  double maximum() const { return mMaximum; }
  QVector<double> outliers() const { return mOutliers; }
  double width() const { return mWidth; }
  double whiskerWidth() const { return mWhiskerWidth; }
  QPen whiskerPen() const { return mWhiskerPen; }
  QPen whiskerBarPen() const { return mWhiskerBarPen; }
  QPen medianPen() const { return mMedianPen; }
  double outlierSize() const { return mOutlierSize; }
  QPen outlierPen() const { return mOutlierPen; }
  QCP::ScatterStyle outlierStyle() const { return mOutlierStyle; }

  // setters:
  void setKey(double key);
  void setMinimum(double value);
  void setLowerQuartile(double value);
  void setMedian(double value);
  void setUpperQuartile(double value);
  void setMaximum(double value);
  void setOutliers(const QVector<double> &values);
  void setData(double key, double minimum, double lowerQuartile, double median, double upperQuartile, double maximum);
  void setWidth(double width);
  void setWhiskerWidth(double width);
  void setWhiskerPen(const QPen &pen);
  void setWhiskerBarPen(const QPen &pen);
  void setMedianPen(const QPen &pen);
  void setOutlierSize(double pixels);
  void setOutlierPen(const QPen &pen);
  void setOutlierStyle(QCP::ScatterStyle style);
  
  // non-property methods:
  virtual void clearData();
  virtual double selectTest(const QPointF &pos) const;
  
protected:
  QVector<double> mOutliers;
  double mKey, mMinimum, mLowerQuartile, mMedian, mUpperQuartile, mMaximum;
  double mWidth;
  double mWhiskerWidth;
  double mOutlierSize;
  QPen mWhiskerPen, mWhiskerBarPen, mOutlierPen, mMedianPen;
  QCP::ScatterStyle mOutlierStyle;
  
  virtual void draw(QCPPainter *painter);
  virtual void drawLegendIcon(QCPPainter *painter, const QRect &rect) const;
  
  virtual void drawQuartileBox(QCPPainter *painter, QRectF *quartileBox=0) const;
  virtual void drawMedian(QCPPainter *painter) const;
  virtual void drawWhiskers(QCPPainter *painter) const;
  virtual void drawOutliers(QCPPainter *painter) const;
  virtual QCPRange getKeyRange(bool &validRange, SignDomain inSignDomain=sdBoth) const;
  virtual QCPRange getValueRange(bool &validRange, SignDomain inSignDomain=sdBoth) const;
  
  friend class QCustomPlot;
  friend class QCPLegend;
};

class QCP_LIB_DECL QCPItemAnchor
{
public:
  QCPItemAnchor(QCustomPlot *parentPlot, QCPAbstractItem *parentItem, const QString name, int anchorId=-1);
  virtual ~QCPItemAnchor();
  
  QString name() const { return mName; }
  virtual QPointF pixelPoint() const;
  
protected:
  QCustomPlot *mParentPlot;
  QCPAbstractItem *mParentItem;
  int mAnchorId;
  QString mName;
  // non-property members:
  QSet<QCPItemPosition*> mChildren;
  
  void addChild(QCPItemPosition* pos); // called from pos when this anchor is set as parent
  void removeChild(QCPItemPosition *pos); // called from pos when its parent anchor is reset or pos deleted
  
private:
  Q_DISABLE_COPY(QCPItemAnchor)
  
  friend class QCPItemPosition;
};

class QCP_LIB_DECL QCPItemPosition : public QCPItemAnchor
{
public:
  /*!
    Defines the ways an item position can be specified. Thus it defines what the numbers passed to
    \ref setCoords actually mean.
    
    \see setType
  */
  enum PositionType { ptAbsolute        ///< Static positioning in pixels, starting from the top left corner of the viewport/widget.
                      ,ptViewportRatio  ///< Static positioning given by a ratio of the current viewport (coordinates 0 to 1).
                      ,ptAxisRectRatio  ///< Static positioning given by a ratio of the current axis rect (coordinates 0 to 1).
                      ,ptPlotCoords     ///< Dynamic positioning at a plot coordinate defined by two axes (see \ref setAxes).
                    };
  
  QCPItemPosition(QCustomPlot *parentPlot, QCPAbstractItem *parentItem, const QString name);
  virtual ~QCPItemPosition();
  
  // getters:
  PositionType type() const { return mPositionType; }
  QCPItemAnchor *parentAnchor() const { return mParentAnchor; }
  double key() const { return mKey; }
  double value() const { return mValue; }
  QPointF coords() const { return QPointF(mKey, mValue); }
  QCPAxis *keyAxis() const { return mKeyAxis; }
  QCPAxis *valueAxis() const { return mValueAxis; }
  virtual QPointF pixelPoint() const;
  
  // setters:
  void setType(PositionType type);
  bool setParentAnchor(QCPItemAnchor *parentAnchor, bool keepPixelPosition=false);
  void setCoords(double key, double value);
  void setCoords(const QPointF &coords);
  void setAxes(QCPAxis* keyAxis, QCPAxis* valueAxis);
  void setPixelPoint(const QPointF &pixelPoint);
  
protected:
  PositionType mPositionType;
  QCPAxis *mKeyAxis, *mValueAxis;
  double mKey, mValue;
  QCPItemAnchor *mParentAnchor;
  
private:
  Q_DISABLE_COPY(QCPItemPosition)
  
};

class QCP_LIB_DECL QCPAbstractItem : public QCPLayerable
{
  Q_OBJECT
public:
  QCPAbstractItem(QCustomPlot *parentPlot);
  virtual ~QCPAbstractItem();
  
  // getters:
  bool clipToAxisRect() const { return mClipToAxisRect; }
  QCPAxis *clipKeyAxis() const { return mClipKeyAxis; }
  QCPAxis *clipValueAxis() const { return mClipValueAxis; }
  bool selectable() const { return mSelectable; }
  bool selected() const { return mSelected; }
  
  // setters:
  void setClipToAxisRect(bool clip);
  void setClipAxes(QCPAxis *keyAxis, QCPAxis *valueAxis);
  void setClipKeyAxis(QCPAxis *axis);
  void setClipValueAxis(QCPAxis *axis);
  void setSelectable(bool selectable);
  void setSelected(bool selected);
  
  // non-property methods:
  virtual double selectTest(const QPointF &pos) const = 0;
  QList<QCPItemPosition*> positions() const { return mPositions; }
  QList<QCPItemAnchor*> anchors() const { return mAnchors; }
  QCPItemPosition *position(const QString &name) const;
  QCPItemAnchor *anchor(const QString &name) const;
  bool hasAnchor(const QString &name) const;
  
protected:
  bool mClipToAxisRect;
  QCPAxis *mClipKeyAxis, *mClipValueAxis;
  bool mSelectable, mSelected;
  QList<QCPItemPosition*> mPositions;
  QList<QCPItemAnchor*> mAnchors;
  
  virtual QRect clipRect() const;
  virtual void applyDefaultAntialiasingHint(QCPPainter *painter) const;
  virtual void draw(QCPPainter *painter) = 0;
  
  // helper functions for subclasses:
  double distSqrToLine(const QPointF &start, const QPointF &end, const QPointF &point) const;
  double rectSelectTest(const QRectF &rect, const QPointF &pos, bool filledRect) const;
  
  // anchor/position interface:
  virtual QPointF anchorPixelPoint(int anchorId) const;
  QCPItemPosition *createPosition(const QString &name);
  QCPItemAnchor *createAnchor(const QString &name, int anchorId);
  
signals:
  void selectionChanged(bool selected);
  
private:
  Q_DISABLE_COPY(QCPAbstractItem)
  
  friend class QCustomPlot;
  friend class QCPItemAnchor;
};

class QCP_LIB_DECL QCPItemStraightLine : public QCPAbstractItem
{
  Q_OBJECT
public:
  QCPItemStraightLine(QCustomPlot *parentPlot);
  virtual ~QCPItemStraightLine();
  
  // getters:
  QPen pen() const { return mPen; }
  QPen selectedPen() const { return mSelectedPen; }
  
  // setters;
  void setPen(const QPen &pen);
  void setSelectedPen(const QPen &pen);
  
  // non-property methods:
  virtual double selectTest(const QPointF &pos) const;
  
  QCPItemPosition * const point1;
  QCPItemPosition * const point2;
  
protected:
  QPen mPen, mSelectedPen;
  
  virtual void draw(QCPPainter *painter);
  
  // helper functions:
  double distToStraightLine(const QVector2D &point1, const QVector2D &vec, const QVector2D &point) const;
  QLineF getRectClippedStraightLine(const QVector2D &point1, const QVector2D &vec, const QRect &rect) const;
  QPen mainPen() const;
};

class QCP_LIB_DECL QCPItemLine : public QCPAbstractItem
{
  Q_OBJECT
public:
  QCPItemLine(QCustomPlot *parentPlot);
  virtual ~QCPItemLine();
  
  // getters:
  QPen pen() const { return mPen; }
  QPen selectedPen() const { return mSelectedPen; }
  QCPLineEnding head() const { return mHead; }
  QCPLineEnding tail() const { return mTail; }
  
  // setters;
  void setPen(const QPen &pen);
  void setSelectedPen(const QPen &pen);
  void setHead(const QCPLineEnding &head);
  void setTail(const QCPLineEnding &tail);
  
  // non-property methods:
  virtual double selectTest(const QPointF &pos) const;
  
  QCPItemPosition * const start;
  QCPItemPosition * const end;
  
protected:
  QPen mPen, mSelectedPen;
  QCPLineEnding mHead, mTail;
  
  virtual void draw(QCPPainter *painter);
  
  // helper functions:
  QLineF getRectClippedLine(const QVector2D &start, const QVector2D &end, const QRect &rect) const;
  QPen mainPen() const;
};

class QCP_LIB_DECL QCPItemEllipse : public QCPAbstractItem
{
  Q_OBJECT
public:
  QCPItemEllipse(QCustomPlot *parentPlot);
  virtual ~QCPItemEllipse();
  
  // getters:
  QPen pen() const { return mPen; }
  QPen selectedPen() const { return mSelectedPen; }
  QBrush brush() const { return mBrush; }
  QBrush selectedBrush() const { return mSelectedBrush; }
  
  // setters;
  void setPen(const QPen &pen);
  void setSelectedPen(const QPen &pen);
  void setBrush(const QBrush &brush);
  void setSelectedBrush(const QBrush &brush);
  
  // non-property methods:
  virtual double selectTest(const QPointF &pos) const;
  
  QCPItemPosition * const topLeft;
  QCPItemPosition * const bottomRight;
  QCPItemAnchor * const topLeftRim;
  QCPItemAnchor * const top;
  QCPItemAnchor * const topRightRim;
  QCPItemAnchor * const right;
  QCPItemAnchor * const bottomRightRim;
  QCPItemAnchor * const bottom;
  QCPItemAnchor * const bottomLeftRim;
  QCPItemAnchor * const left;
  
protected:
  enum AnchorIndex {aiTopLeftRim, aiTop, aiTopRightRim, aiRight, aiBottomRightRim, aiBottom, aiBottomLeftRim, aiLeft};
  QPen mPen, mSelectedPen;
  QBrush mBrush, mSelectedBrush;
  
  virtual void draw(QCPPainter *painter);
  virtual QPointF anchorPixelPoint(int anchorId) const;
  
  // helper functions:
  QPen mainPen() const;
  QBrush mainBrush() const;
};

class QCP_LIB_DECL QCPItemRect : public QCPAbstractItem
{
  Q_OBJECT
public:
  QCPItemRect(QCustomPlot *parentPlot);
  virtual ~QCPItemRect();
  
  // getters:
  QPen pen() const { return mPen; }
  QPen selectedPen() const { return mSelectedPen; }
  QBrush brush() const { return mBrush; }
  QBrush selectedBrush() const { return mSelectedBrush; }
  
  // setters;
  void setPen(const QPen &pen);
  void setSelectedPen(const QPen &pen);
  void setBrush(const QBrush &brush);
  void setSelectedBrush(const QBrush &brush);
  
  // non-property methods:
  virtual double selectTest(const QPointF &pos) const;
  
  QCPItemPosition * const topLeft;
  QCPItemPosition * const bottomRight;
  QCPItemAnchor * const top;
  QCPItemAnchor * const topRight;
  QCPItemAnchor * const right;
  QCPItemAnchor * const bottom;
  QCPItemAnchor * const bottomLeft;
  QCPItemAnchor * const left;
  
protected:
  enum AnchorIndex {aiTop, aiTopRight, aiRight, aiBottom, aiBottomLeft, aiLeft};
  QPen mPen, mSelectedPen;
  QBrush mBrush, mSelectedBrush;
  
  virtual void draw(QCPPainter *painter);
  virtual QPointF anchorPixelPoint(int anchorId) const;
  
  // helper functions:
  QPen mainPen() const;
  QBrush mainBrush() const;
};

class QCP_LIB_DECL QCPItemPixmap : public QCPAbstractItem
{
  Q_OBJECT
public:
  QCPItemPixmap(QCustomPlot *parentPlot);
  virtual ~QCPItemPixmap();
  
  // getters:
  QPixmap pixmap() const { return mPixmap; }
  bool scaled() const { return mScaled; }
  Qt::AspectRatioMode aspectRatioMode() const { return mAspectRatioMode; }
  QPen pen() const { return mPen; }
  QPen selectedPen() const { return mSelectedPen; }
  
  // setters;
  void setPixmap(const QPixmap &pixmap);
  void setScaled(bool scaled, Qt::AspectRatioMode aspectRatioMode=Qt::KeepAspectRatio);
  void setPen(const QPen &pen);
  void setSelectedPen(const QPen &pen);
  
  // non-property methods:
  virtual double selectTest(const QPointF &pos) const;
  
  QCPItemPosition * const topLeft;
  QCPItemPosition * const bottomRight;
  QCPItemAnchor * const top;
  QCPItemAnchor * const topRight;
  QCPItemAnchor * const right;
  QCPItemAnchor * const bottom;
  QCPItemAnchor * const bottomLeft;
  QCPItemAnchor * const left;
  
protected:
  enum AnchorIndex {aiTop, aiTopRight, aiRight, aiBottom, aiBottomLeft, aiLeft};
  QPixmap mPixmap;
  QPixmap mScaledPixmap;
  bool mScaled;
  Qt::AspectRatioMode mAspectRatioMode;
  QPen mPen, mSelectedPen;
  
  virtual void draw(QCPPainter *painter);
  virtual QPointF anchorPixelPoint(int anchorId) const;
  
  // helper functions:
  void updateScaledPixmap(QRect finalRect=QRect(), bool flipHorz=false, bool flipVert=false);
  QRect getFinalRect(bool *flippedHorz=0, bool *flippedVert=0) const;
  QPen mainPen() const;
};

class QCP_LIB_DECL QCPItemText : public QCPAbstractItem
{
  Q_OBJECT
public:
  QCPItemText(QCustomPlot *parentPlot);
  virtual ~QCPItemText();
  
  // getters:
  QColor color() const { return mColor; }
  QColor selectedColor() const { return mSelectedColor; }
  QPen pen() const { return mPen; }
  QPen selectedPen() const { return mSelectedPen; }
  QBrush brush() const { return mBrush; }
  QBrush selectedBrush() const { return mSelectedBrush; }
  QFont font() const { return mFont; }
  QFont selectedFont() const { return mSelectedFont; }
  QString text() const { return mText; }
  Qt::Alignment positionAlignment() const { return mPositionAlignment; }
  Qt::Alignment textAlignment() const { return mTextAlignment; }
  double rotation() const { return mRotation; }
  QMargins padding() const { return mPadding; }
  
  // setters;
  void setColor(const QColor &color);
  void setSelectedColor(const QColor &color);
  void setPen(const QPen &pen);
  void setSelectedPen(const QPen &pen);
  void setBrush(const QBrush &brush);
  void setSelectedBrush(const QBrush &brush);
  void setFont(const QFont &font);
  void setSelectedFont(const QFont &font);
  void setText(const QString &text);
  void setPositionAlignment(Qt::Alignment alignment);
  void setTextAlignment(Qt::Alignment alignment);
  void setRotation(double degrees);
  void setPadding(const QMargins &padding);
  
  // non-property methods:
  virtual double selectTest(const QPointF &pos) const;
  
  QCPItemPosition * const position;
  QCPItemAnchor * const topLeft;
  QCPItemAnchor * const top;
  QCPItemAnchor * const topRight;
  QCPItemAnchor * const right;
  QCPItemAnchor * const bottomRight;
  QCPItemAnchor * const bottom;
  QCPItemAnchor * const bottomLeft;
  QCPItemAnchor * const left;
  
protected:
  enum AnchorIndex {aiTopLeft, aiTop, aiTopRight, aiRight, aiBottomRight, aiBottom, aiBottomLeft, aiLeft};
  QColor mColor, mSelectedColor;
  QPen mPen, mSelectedPen;
  QBrush mBrush, mSelectedBrush;
  QFont mFont, mSelectedFont;
  QString mText;
  Qt::Alignment mPositionAlignment;
  Qt::Alignment mTextAlignment;
  double mRotation;
  QMargins mPadding;
  
  virtual void draw(QCPPainter *painter);
  virtual QPointF anchorPixelPoint(int anchorId) const;
  
  // helper functions:
  QPointF getTextDrawPoint(const QPointF &pos, const QRectF &rect, Qt::Alignment positionAlignment) const;
  QFont mainFont() const;
  QColor mainColor() const;
  QPen mainPen() const;
  QBrush mainBrush() const;
};

class QCP_LIB_DECL QCPItemCurve : public QCPAbstractItem
{
  Q_OBJECT
public:
  QCPItemCurve(QCustomPlot *parentPlot);
  virtual ~QCPItemCurve();
  
  // getters:
  QPen pen() const { return mPen; }
  QPen selectedPen() const { return mSelectedPen; }
  QCPLineEnding head() const { return mHead; }
  QCPLineEnding tail() const { return mTail; }
  
  // setters;
  void setPen(const QPen &pen);
  void setSelectedPen(const QPen &pen);
  void setHead(const QCPLineEnding &head);
  void setTail(const QCPLineEnding &tail);
  
  // non-property methods:
  virtual double selectTest(const QPointF &pos) const;
  
  QCPItemPosition * const start;
  QCPItemPosition * const startDir;
  QCPItemPosition * const endDir;
  QCPItemPosition * const end;
  
protected:
  QPen mPen, mSelectedPen;
  QCPLineEnding mHead, mTail;
  
  virtual void draw(QCPPainter *painter);
  
  // helper functions:
  QPen mainPen() const;
};

class QCP_LIB_DECL QCPItemBracket : public QCPAbstractItem
{
  Q_OBJECT
public:
  enum BracketStyle { bsSquare  ///< A brace with angled edges
                      ,bsRound  ///< A brace with round edges
                      ,bsCurly  ///< A curly brace
                      ,bsCalligraphic ///< A curly brace with varying stroke width giving a calligraphic impression
  };

  QCPItemBracket(QCustomPlot *parentPlot);
  virtual ~QCPItemBracket();
  
  // getters:
  QPen pen() const { return mPen; }
  QPen selectedPen() const { return mSelectedPen; }
  double length() const { return mLength; }
  BracketStyle style() const { return mStyle; }
  
  // setters;
  void setPen(const QPen &pen);
  void setSelectedPen(const QPen &pen);
  void setLength(double length);
  void setStyle(BracketStyle style);
  
  // non-property methods:
  virtual double selectTest(const QPointF &pos) const;
  
  QCPItemPosition * const left;
  QCPItemPosition * const right;
  QCPItemAnchor * const center;
  
protected:
  enum AnchorIndex {aiCenter};
  QPen mPen, mSelectedPen;
  double mLength;
  BracketStyle mStyle;
  
  virtual void draw(QCPPainter *painter);
  virtual QPointF anchorPixelPoint(int anchorId) const;
  
  // helper functions:
  QPen mainPen() const;
};

class QCP_LIB_DECL QCPItemTracer : public QCPAbstractItem
{
  Q_OBJECT
public:
  /*!
    The different visual appearances a tracer item can have. Some styles size may be controlled with \ref setSize.
    
    \see setStyle
  */
  enum TracerStyle { tsNone        ///< The tracer is not visible
                     ,tsPlus       ///< A plus shaped crosshair with limited size
                     ,tsCrosshair  ///< A plus shaped crosshair which spans the complete axis rect
                     ,tsCircle     ///< A circle
                     ,tsSquare     ///< A square
                   };
  Q_ENUMS(TracerStyle)

  QCPItemTracer(QCustomPlot *parentPlot);
  virtual ~QCPItemTracer();

  // getters:
  QPen pen() const { return mPen; }
  QPen selectedPen() const { return mSelectedPen; }
  QBrush brush() const { return mBrush; }
  QBrush selectedBrush() const { return mSelectedBrush; }
  double size() const { return mSize; }
  TracerStyle style() const { return mStyle; }
  QCPGraph *graph() const { return mGraph; }
  double graphKey() const { return mGraphKey; }
  bool interpolating() const { return mInterpolating; }

  // setters;
  void setPen(const QPen &pen);
  void setSelectedPen(const QPen &pen);
  void setBrush(const QBrush &brush);
  void setSelectedBrush(const QBrush &brush);
  void setSize(double size);
  void setStyle(TracerStyle style);
  void setGraph(QCPGraph *graph);
  void setGraphKey(double key);
  void setInterpolating(bool enabled);

  // non-property methods:
  virtual double selectTest(const QPointF &pos) const;
  void updatePosition();

  QCPItemPosition * const position;

protected:
  QPen mPen, mSelectedPen;
  QBrush mBrush, mSelectedBrush;
  double mSize;
  TracerStyle mStyle;
  QCPGraph *mGraph;
  double mGraphKey;
  bool mInterpolating;

  virtual void draw(QCPPainter *painter);

  // helper functions:
  QPen mainPen() const;
  QBrush mainBrush() const;
};

class QCP_LIB_DECL QCPRange
{
public:
  double lower, upper;
  QCPRange();
  QCPRange(double lower, double upper);
  double size() const;
  double center() const;
  void normalize();
  QCPRange sanitizedForLogScale() const;
  QCPRange sanitizedForLinScale() const;
  bool contains(double value) const;
  
  static bool validRange(double lower, double upper);
  static bool validRange(const QCPRange &range);
  static const double minRange; //1e-280;
  static const double maxRange; //1e280;
};
Q_DECLARE_TYPEINFO(QCPRange, Q_MOVABLE_TYPE);

class QCP_LIB_DECL QCPAbstractLegendItem : public QObject
{
  Q_OBJECT
public:
  QCPAbstractLegendItem(QCPLegend *parent);
  virtual ~QCPAbstractLegendItem() {}
  
  // getters:
  bool antialiased() const { return mAntialiased; }
  QFont font() const { return mFont; }
  QColor textColor() const { return mTextColor; }
  QFont selectedFont() const { return mSelectedFont; }
  QColor selectedTextColor() const { return mSelectedTextColor; }
  bool selectable() const { return mSelectable; }
  bool selected() const { return mSelected; }
  
  // setters:
  void setAntialiased(bool enabled);
  void setFont(const QFont &font);
  void setTextColor(const QColor &color);
  void setSelectedFont(const QFont &font);
  void setSelectedTextColor(const QColor &color);
  void setSelectable(bool selectable);
  void setSelected(bool selected);
  
signals:
  void selectionChanged(bool selected);
  
protected:
  QCPLegend *mParentLegend;
  bool mAntialiased;
  QFont mFont;
  QColor mTextColor;
  QFont mSelectedFont;
  QColor mSelectedTextColor;
  bool mSelectable, mSelected;
  
  virtual void draw(QCPPainter *painter, const QRect &rect) const = 0;
  virtual QSize size(const QSize &targetSize) const = 0;
  void applyAntialiasingHint(QCPPainter *painter) const;
  
private:
  Q_DISABLE_COPY(QCPAbstractLegendItem)
  
  friend class QCPLegend;
};

class QCP_LIB_DECL QCPPlottableLegendItem : public QCPAbstractLegendItem
{
  Q_OBJECT
public:
  QCPPlottableLegendItem(QCPLegend *parent, QCPAbstractPlottable *plottable);
  virtual ~QCPPlottableLegendItem() {}
  
  // getters:
  QCPAbstractPlottable *plottable() { return mPlottable; }
  bool textWrap() const { return mTextWrap; }
  
  // setters:
  void setTextWrap(bool wrap);
  
protected:
  QCPAbstractPlottable *mPlottable;
  bool mTextWrap;
  
  QPen getIconBorderPen() const;
  QColor getTextColor() const;
  QFont getFont() const;

  virtual void draw(QCPPainter *painter, const QRect &rect) const;
  virtual QSize size(const QSize &targetSize) const;
};

class QCP_LIB_DECL QCPLegend : public QCPLayerable
{
  Q_OBJECT
public:
  /*!
    Defines where the legend is positioned inside the QCustomPlot axis rect.
  */
  enum PositionStyle { psManual       ///< Position is not changed automatically. Set manually via \ref setPosition
                      ,psTopLeft      ///< Legend is positioned in the top left corner of the axis rect with distance to the border corresponding to the currently set top and left margins
                      ,psTop          ///< Legend is horizontally centered at the top of the axis rect with distance to the border corresponding to the currently set top margin
                      ,psTopRight     ///< Legend is positioned in the top right corner of the axis rect with distance to the border corresponding to the currently set top and right margins
                      ,psRight        ///< Legend is vertically centered at the right of the axis rect with distance to the border corresponding to the currently set right margin
                      ,psBottomRight  ///< Legend is positioned in the bottom right corner of the axis rect with distance to the border corresponding to the currently set bottom and right margins
                      ,psBottom       ///< Legend is horizontally centered at the bottom of the axis rect with distance to the border corresponding to the currently set bottom margin
                      ,psBottomLeft   ///< Legend is positioned in the bottom left corner of the axis rect with distance to the border corresponding to the currently set bottom and left margins
                      ,psLeft         ///< Legend is vertically centered at the left of the axis rect with distance to the border corresponding to the currently set left margin
                     };
  Q_ENUMS(PositionStyle)
  
  /*!
    Defines the selectable parts of a legend
  */
  enum SelectablePart { spNone       = 0      ///< None
                       ,spLegendBox  = 0x001  ///< The legend box (frame)
                       ,spItems      = 0x002  ///< Legend items individually (see \ref selectedItems)
                      };
  Q_ENUMS(SelectablePart)
  Q_DECLARE_FLAGS(SelectableParts, SelectablePart)
  
  explicit QCPLegend(QCustomPlot *parentPlot);
  virtual ~QCPLegend();
  
  // getters:
  QPen borderPen() const { return mBorderPen; }
  QBrush brush() const { return mBrush; }
  QFont font() const { return mFont; }
  QColor textColor() const { return mTextColor; }
  PositionStyle positionStyle() const { return mPositionStyle; }
  QPoint position() const { return mPosition; }
  bool autoSize() const { return mAutoSize; }
  QSize size() const { return mSize; }
  QSize minimumSize() const { return mMinimumSize; }
  int paddingLeft() const { return mPaddingLeft; }
  int paddingRight() const { return mPaddingRight; }
  int paddingTop() const { return mPaddingTop; }
  int paddingBottom() const { return mPaddingBottom; }
  int marginLeft() const { return mMarginLeft; }
  int marginRight() const { return mMarginRight; }
  int marginTop() const { return mMarginTop; }
  int marginBottom() const { return mMarginBottom; }
  int itemSpacing() const { return mItemSpacing; }
  QSize iconSize() const { return mIconSize; }
  int iconTextPadding() const { return mIconTextPadding; }
  QPen iconBorderPen() const { return mIconBorderPen; }
  SelectableParts selectable() const { return mSelectable; }
  SelectableParts selected() const { return mSelected; }
  QPen selectedBorderPen() const { return mSelectedBorderPen; }
  QPen selectedIconBorderPen() const { return mSelectedIconBorderPen; }
  QBrush selectedBrush() const { return mSelectedBrush; }
  QFont selectedFont() const { return mSelectedFont; }
  QColor selectedTextColor() const { return mSelectedTextColor; }
  
  // setters:
  void setBorderPen(const QPen &pen);
  void setBrush(const QBrush &brush);
  void setFont(const QFont &font);
  void setTextColor(const QColor &color);
  void setPositionStyle(PositionStyle legendPositionStyle);
  void setPosition(const QPoint &pixelPosition);
  void setAutoSize(bool on);
  void setSize(const QSize &size);
  void setSize(int width, int height);
  void setMinimumSize(const QSize &size);
  void setMinimumSize(int width, int height);
  void setPaddingLeft(int padding);
  void setPaddingRight(int padding);
  void setPaddingTop(int padding);
  void setPaddingBottom(int padding);
  void setPadding(int left, int right, int top, int bottom);
  void setMarginLeft(int margin);
  void setMarginRight(int margin);
  void setMarginTop(int margin);
  void setMarginBottom(int margin);
  void setMargin(int left, int right, int top, int bottom);
  void setItemSpacing(int spacing);
  void setIconSize(const QSize &size);
  void setIconSize(int width, int height);
  void setIconTextPadding(int padding);
  void setIconBorderPen(const QPen &pen);
  void setSelectable(const SelectableParts &selectable);
  void setSelected(const SelectableParts &selected);
  void setSelectedBorderPen(const QPen &pen);
  void setSelectedIconBorderPen(const QPen &pen);
  void setSelectedBrush(const QBrush &brush);
  void setSelectedFont(const QFont &font);
  void setSelectedTextColor(const QColor &color);

  // non-property methods:
  QCPAbstractLegendItem *item(int index) const;
  QCPPlottableLegendItem *itemWithPlottable(const QCPAbstractPlottable *plottable) const;
  int itemCount() const;
  bool hasItem(QCPAbstractLegendItem *item) const;
  bool hasItemWithPlottable(const QCPAbstractPlottable *plottable) const;
  bool addItem(QCPAbstractLegendItem *item);
  bool removeItem(int index);
  bool removeItem(QCPAbstractLegendItem *item);
  void clearItems();
  QList<QCPAbstractLegendItem*> selectedItems() const;
  void reArrange();
  
  bool selectTestLegend(const QPointF &pos) const;
  QCPAbstractLegendItem *selectTestItem(const QPoint pos) const;
  
signals:
  void selectionChanged(QCPLegend::SelectableParts selection);
  
protected:
  // simple properties with getters and setters:
  QPen mBorderPen, mIconBorderPen;
  QBrush mBrush;
  QFont mFont;
  QColor mTextColor;
  QPoint mPosition;
  QSize mSize, mMinimumSize, mIconSize;
  PositionStyle mPositionStyle;
  bool mAutoSize;
  int mPaddingLeft, mPaddingRight, mPaddingTop, mPaddingBottom;
  int mMarginLeft, mMarginRight, mMarginTop, mMarginBottom;
  int mItemSpacing, mIconTextPadding;
  SelectableParts mSelected, mSelectable;
  QPen mSelectedBorderPen, mSelectedIconBorderPen;
  QBrush mSelectedBrush;
  QFont mSelectedFont;
  QColor mSelectedTextColor;
  
  // internal or not explicitly exposed properties:
  QList<QCPAbstractLegendItem*> mItems;
  QMap<QCPAbstractLegendItem*, QRect> mItemBoundingBoxes;
  
  virtual void updateSelectionState();
  virtual bool handleLegendSelection(QMouseEvent *event, bool additiveSelection, bool &modified);
  // introduced methods:
  virtual void applyDefaultAntialiasingHint(QCPPainter *painter) const;
  virtual void draw(QCPPainter *painter);
  virtual void calculateAutoSize();
  virtual void calculateAutoPosition();
  
  // drawing helpers:
  QPen getBorderPen() const;
  QBrush getBrush() const;
  
private:
  Q_DISABLE_COPY(QCPLegend)
  
  friend class QCustomPlot;
  friend class QCPAbstractLegendItem;
};
Q_DECLARE_OPERATORS_FOR_FLAGS(QCPLegend::SelectableParts)

class QCP_LIB_DECL QCPGrid : public QCPLayerable
{
  Q_OBJECT
public:
  QCPGrid(QCPAxis *parentAxis);
  ~QCPGrid();
  
  // getters:
  bool subGridVisible() const { return mSubGridVisible; }
  bool antialiasedSubGrid() const { return mAntialiasedSubGrid; }
  bool antialiasedZeroLine() const { return mAntialiasedZeroLine; }
  QPen pen() const { return mPen; }
  QPen subGridPen() const { return mSubGridPen; }
  QPen zeroLinePen() const { return mZeroLinePen; }
  
  // setters:
  void setSubGridVisible(bool visible);
  void setAntialiasedSubGrid(bool enabled);
  void setAntialiasedZeroLine(bool enabled);
  void setPen(const QPen &pen);
  void setSubGridPen(const QPen &pen);
  void setZeroLinePen(const QPen &pen);
  
protected:
  QCPAxis *mParentAxis;
  bool mSubGridVisible;
  bool mAntialiasedSubGrid, mAntialiasedZeroLine;
  QPen mPen, mSubGridPen, mZeroLinePen;
  
  virtual void applyDefaultAntialiasingHint(QCPPainter *painter) const;
  virtual void draw(QCPPainter *painter);
  // drawing helpers:
  void drawGridLines(QCPPainter *painter) const;
  void drawSubGridLines(QCPPainter *painter) const;
  
  friend class QCPAxis;
};

class QCP_LIB_DECL QCPAxis : public QCPLayerable
{
  Q_OBJECT
  /// \cond INCLUDE_QPROPERTIES
  Q_PROPERTY(AxisType axisType READ axisType WRITE setAxisType)
  Q_PROPERTY(ScaleType scaleType READ scaleType WRITE setScaleType)
  Q_PROPERTY(double scaleLogBase READ scaleLogBase WRITE setScaleLogBase)
  Q_PROPERTY(QRect axisRect READ axisRect WRITE setAxisRect)
  Q_PROPERTY(QCPRange range READ range WRITE setRange)
  Q_PROPERTY(bool grid READ grid WRITE setGrid)
  Q_PROPERTY(bool subGrid READ subGrid WRITE setSubGrid)
  Q_PROPERTY(bool autoTicks READ autoTicks WRITE setAutoTicks)
  Q_PROPERTY(int autoTickCount READ autoTickCount WRITE setAutoTickCount)
  Q_PROPERTY(bool autoTickLabels READ autoTickLabels WRITE setAutoTickLabels)
  Q_PROPERTY(bool autoTickStep READ autoTickStep WRITE setAutoTickStep)
  Q_PROPERTY(bool autoSubTicks READ autoSubTicks WRITE setAutoSubTicks)
  Q_PROPERTY(bool ticks READ ticks WRITE setTicks)
  Q_PROPERTY(bool tickLabels READ tickLabels WRITE setTickLabels)
  Q_PROPERTY(int tickLabelPadding READ tickLabelPadding WRITE setTickLabelPadding)
  Q_PROPERTY(LabelType tickLabelType READ tickLabelType WRITE setTickLabelType)
  Q_PROPERTY(QFont tickLabelFont READ tickLabelFont WRITE setTickLabelFont)
  Q_PROPERTY(double tickLabelRotation READ tickLabelRotation WRITE setTickLabelRotation)
  Q_PROPERTY(QString dateTimeFormat READ dateTimeFormat WRITE setDateTimeFormat)
  Q_PROPERTY(QString numberFormat READ numberFormat WRITE setNumberFormat)
  Q_PROPERTY(double tickStep READ tickStep WRITE setTickStep)
  Q_PROPERTY(QVector<double> tickVector READ tickVector WRITE setTickVector)
  Q_PROPERTY(QVector<QString> tickVectorLabels READ tickVectorLabels WRITE setTickVectorLabels)
  Q_PROPERTY(int subTickCount READ subTickCount WRITE setSubTickCount)
  Q_PROPERTY(QPen basePen READ basePen WRITE setBasePen)
  Q_PROPERTY(QPen gridPen READ gridPen WRITE setGridPen)
  Q_PROPERTY(QPen subGridPen READ subGridPen WRITE setSubGridPen)
  Q_PROPERTY(QPen tickPen READ tickPen WRITE setTickPen)
  Q_PROPERTY(QPen subTickPen READ subTickPen WRITE setSubTickPen)
  Q_PROPERTY(QFont labelFont READ labelFont WRITE setLabelFont)
  Q_PROPERTY(QString label READ label WRITE setLabel)
  Q_PROPERTY(int labelPadding READ labelPadding WRITE setLabelPadding)
  /// \endcond
public:
  /*!
    Defines at which side of the axis rect the axis will appear. This also affects how the tick
    marks are drawn, on which side the labels are placed etc.
    \see setAxisType
  */
  enum AxisType { atLeft    ///< Axis is vertical and on the left side of the axis rect of the parent QCustomPlot
                  ,atRight  ///< Axis is vertical and on the right side of the axis rect of the parent QCustomPlot
                  ,atTop    ///< Axis is horizontal and on the top side of the axis rect of the parent QCustomPlot
                  ,atBottom ///< Axis is horizontal and on the bottom side of the axis rect of the parent QCustomPlot
                };
  Q_ENUMS(AxisType)
  /*!
    When automatic tick label generation is enabled (\ref setAutoTickLabels), defines how the
    numerical value (coordinate) of the tick position is translated into a string that will be
    drawn at the tick position.
    \see setTickLabelType
  */
  enum LabelType { ltNumber    ///< Tick coordinate is regarded as normal number and will be displayed as such. (see \ref setNumberFormat)
                   ,ltDateTime ///< Tick coordinate is regarded as a date/time (seconds since 1970-01-01T00:00:00 UTC, see QDateTime::toTime_t) and will be displayed and formatted as such. (see \ref setDateTimeFormat)
                 };
  Q_ENUMS(LabelType)
  /*!
    Defines the scale of an axis.
    \see setScaleType
  */
  enum ScaleType { stLinear       ///< Normal linear scaling
                   ,stLogarithmic ///< Logarithmic scaling with correspondingly transformed plots and (major) tick marks at every base power (see \ref setScaleLogBase).
                 };
  Q_ENUMS(ScaleType)
  /*!
    Defines the selectable parts of an axis.
    \see setSelectable, setSelected
  */
  enum SelectablePart { spNone        = 0      ///< None of the selectable parts
                        ,spAxis       = 0x001  ///< The axis backbone and tick marks
                        ,spTickLabels = 0x002  ///< Tick labels (numbers) of this axis (as a whole, not individually)
                        ,spAxisLabel  = 0x004  ///< The axis label
                      };
  Q_ENUMS(SelectablePart)
  Q_DECLARE_FLAGS(SelectableParts, SelectablePart)
  
  explicit QCPAxis(QCustomPlot *parentPlot, AxisType type);
  virtual ~QCPAxis();
      
  // getters:
  AxisType axisType() const { return mAxisType; }
  QRect axisRect() const { return mAxisRect; }
  ScaleType scaleType() const { return mScaleType; }
  double scaleLogBase() const { return mScaleLogBase; }
  const QCPRange range() const { return mRange; }
  bool rangeReversed() const { return mRangeReversed; }
  bool antialiasedGrid() const { return mGrid->antialiased(); }
  bool antialiasedSubGrid() const { return mGrid->antialiasedSubGrid(); }
  bool antialiasedZeroLine() const { return mGrid->antialiasedZeroLine(); }
  bool grid() const { return mGrid->visible(); }
  bool subGrid() const { return mGrid->subGridVisible(); }
  bool autoTicks() const { return mAutoTicks; }
  int autoTickCount() const { return mAutoTickCount; }
  bool autoTickLabels() const { return mAutoTickLabels; }
  bool autoTickStep() const { return mAutoTickStep; }
  bool autoSubTicks() const { return mAutoSubTicks; }
  bool ticks() const { return mTicks; }
  bool tickLabels() const { return mTickLabels; }
  int tickLabelPadding() const { return mTickLabelPadding; }
  LabelType tickLabelType() const { return mTickLabelType; }
  QFont tickLabelFont() const { return mTickLabelFont; }
  QColor tickLabelColor() const { return mTickLabelColor; }
  double tickLabelRotation() const { return mTickLabelRotation; }
  QString dateTimeFormat() const { return mDateTimeFormat; }
  QString numberFormat() const;
  int numberPrecision() const { return mNumberPrecision; }
  double tickStep() const { return mTickStep; }
  QVector<double> tickVector() const { return mTickVector; }
  QVector<QString> tickVectorLabels() const { return mTickVectorLabels; }
  int tickLengthIn() const { return mTickLengthIn; }
  int tickLengthOut() const { return mTickLengthOut; }
  int subTickCount() const { return mSubTickCount; }
  int subTickLengthIn() const { return mSubTickLengthIn; }
  int subTickLengthOut() const { return mSubTickLengthOut; }
  QPen basePen() const { return mBasePen; }
  QPen gridPen() const { return mGrid->pen(); }
  QPen subGridPen() const { return mGrid->subGridPen(); }
  QPen zeroLinePen() const { return mGrid->zeroLinePen(); }
  QPen tickPen() const { return mTickPen; }
  QPen subTickPen() const { return mSubTickPen; }
  QFont labelFont() const { return mLabelFont; }
  QColor labelColor() const { return mLabelColor; }
  QString label() const { return mLabel; }
  int labelPadding() const { return mLabelPadding; }
  int padding() const { return mPadding; }
  SelectableParts selected() const { return mSelected; }
  SelectableParts selectable() const { return mSelectable; }
  QFont selectedTickLabelFont() const { return mSelectedTickLabelFont; }
  QFont selectedLabelFont() const { return mSelectedLabelFont; }
  QColor selectedTickLabelColor() const { return mSelectedTickLabelColor; }
  QColor selectedLabelColor() const { return mSelectedLabelColor; }
  QPen selectedBasePen() const { return mSelectedBasePen; }
  QPen selectedTickPen() const { return mSelectedTickPen; }
  QPen selectedSubTickPen() const { return mSelectedSubTickPen; }
  
  // setters:
  void setScaleType(ScaleType type);
  void setScaleLogBase(double base);
  void setRange(double lower, double upper);
  void setRange(double position, double size, Qt::AlignmentFlag alignment);
  void setRangeLower(double lower);
  void setRangeUpper(double upper);
  void setRangeReversed(bool reversed);
  void setAntialiasedGrid(bool enabled);
  void setAntialiasedSubGrid(bool enabled);
  void setAntialiasedZeroLine(bool enabled);
  void setGrid(bool show);
  void setSubGrid(bool show);
  void setAutoTicks(bool on);
  void setAutoTickCount(int approximateCount);
  void setAutoTickLabels(bool on);
  void setAutoTickStep(bool on);
  void setAutoSubTicks(bool on);
  void setTicks(bool show);
  void setTickLabels(bool show);
  void setTickLabelPadding(int padding);
  void setTickLabelType(LabelType type);
  void setTickLabelFont(const QFont &font);
  void setTickLabelColor(const QColor &color);
  void setTickLabelRotation(double degrees);
  void setDateTimeFormat(const QString &format);
  void setNumberFormat(const QString &formatCode);
  void setNumberPrecision(int precision);
  void setTickStep(double step);
  void setTickVector(const QVector<double> &vec);
  void setTickVectorLabels(const QVector<QString> &vec);
  void setTickLength(int inside, int outside=0);
  void setSubTickCount(int count);
  void setSubTickLength(int inside, int outside=0);
  void setBasePen(const QPen &pen);
  void setGridPen(const QPen &pen);
  void setSubGridPen(const QPen &pen);
  void setZeroLinePen(const QPen &pen);
  void setTickPen(const QPen &pen);
  void setSubTickPen(const QPen &pen);
  void setLabelFont(const QFont &font);
  void setLabelColor(const QColor &color);
  void setLabel(const QString &str);
  void setLabelPadding(int padding);
  void setPadding(int padding);
  void setSelectedTickLabelFont(const QFont &font);
  void setSelectedLabelFont(const QFont &font);
  void setSelectedTickLabelColor(const QColor &color);
  void setSelectedLabelColor(const QColor &color);
  void setSelectedBasePen(const QPen &pen);
  void setSelectedTickPen(const QPen &pen);
  void setSelectedSubTickPen(const QPen &pen);
  
  // non-property methods:
  Qt::Orientation orientation() const { return mOrientation; }
  void moveRange(double diff);
  void scaleRange(double factor, double center);
  void setScaleRatio(const QCPAxis *otherAxis, double ratio=1.0);
  double pixelToCoord(double value) const;
  double coordToPixel(double value) const;
  SelectablePart selectTest(const QPointF &pos) const;
  
public slots:
  // slot setters:
  void setRange(const QCPRange &range);
  void setSelectable(const QCPAxis::SelectableParts &selectable);
  void setSelected(const QCPAxis::SelectableParts &selected);
  
signals:
  void ticksRequest();
  void rangeChanged(const QCPRange &newRange);
  void selectionChanged(QCPAxis::SelectableParts selection);

protected:
  // simple properties with getters and setters:
  QVector<double> mTickVector;
  QVector<QString> mTickVectorLabels;
  QCPRange mRange;
  QString mDateTimeFormat;
  QString mLabel;
  QRect mAxisRect;
  QPen mBasePen, mTickPen, mSubTickPen;
  QFont mTickLabelFont, mLabelFont;
  QColor mTickLabelColor, mLabelColor;
  LabelType mTickLabelType;
  ScaleType mScaleType;
  AxisType mAxisType;
  double mTickStep;
  double mScaleLogBase, mScaleLogBaseLogInv;
  int mSubTickCount, mTickLengthIn, mTickLengthOut, mSubTickLengthIn, mSubTickLengthOut;
  int mAutoTickCount;
  int mTickLabelPadding, mLabelPadding, mPadding;
  double mTickLabelRotation;
  bool mTicks, mTickLabels, mAutoTicks, mAutoTickLabels, mAutoTickStep, mAutoSubTicks;
  bool mRangeReversed;
  SelectableParts mSelectable, mSelected;
  QFont mSelectedTickLabelFont, mSelectedLabelFont;
  QColor mSelectedTickLabelColor, mSelectedLabelColor;
  QPen mSelectedBasePen, mSelectedTickPen, mSelectedSubTickPen;
  QRect mAxisSelectionBox, mTickLabelsSelectionBox, mLabelSelectionBox;
  
  // internal or not explicitly exposed properties:
  QCPGrid *mGrid;
  QVector<double> mSubTickVector;
  QChar mExponentialChar, mPositiveSignChar;
  int mNumberPrecision;
  char mNumberFormatChar;
  bool mNumberBeautifulPowers, mNumberMultiplyCross;
  Qt::Orientation mOrientation;
  int mLowestVisibleTick, mHighestVisibleTick;
  
  // internal setters:
  void setAxisType(AxisType type);
  void setAxisRect(const QRect &rect);
  
  // introduced methods:
  virtual void setupTickVectors();
  virtual void generateAutoTicks();
  virtual int calculateAutoSubTickCount(double tickStep) const;
  virtual int calculateMargin() const;
  virtual bool handleAxisSelection(QMouseEvent *event, bool additiveSelection, bool &modified);
  
  // drawing:
  virtual void applyDefaultAntialiasingHint(QCPPainter *painter) const;
  virtual void draw(QCPPainter *painter); 
  virtual void drawTickLabel(QCPPainter *painter, double position, int distanceToAxis, const QString &text, QSize *tickLabelsSize);
  virtual void getMaxTickLabelSize(const QFont &font, const QString &text, QSize *tickLabelsSize) const;
  
  // basic non virtual helpers:
  void visibleTickBounds(int &lowIndex, int &highIndex) const;
  double baseLog(double value) const;
  double basePow(double value) const;
  
  // helpers to get the right pen/font depending on selection state:
  QPen getBasePen() const;
  QPen getTickPen() const;
  QPen getSubTickPen() const;
  QFont getTickLabelFont() const;
  QFont getLabelFont() const;
  QColor getTickLabelColor() const;
  QColor getLabelColor() const;
  
private:
  Q_DISABLE_COPY(QCPAxis)
  
  friend class QCustomPlot;
  friend class QCPGrid;
};
Q_DECLARE_OPERATORS_FOR_FLAGS(QCPAxis::SelectableParts)

class QCP_LIB_DECL QCustomPlot : public QWidget
{
  Q_OBJECT
  /// \cond INCLUDE_QPROPERTIES
  Q_PROPERTY(QString title READ title WRITE setTitle)
  Q_PROPERTY(QRect axisRect READ axisRect WRITE setAxisRect)
  Q_PROPERTY(int marginLeft READ marginLeft WRITE setMarginLeft)
  Q_PROPERTY(int marginRight READ marginRight WRITE setMarginRight)
  Q_PROPERTY(int marginTop READ marginTop WRITE setMarginTop)
  Q_PROPERTY(int marginBottom READ marginBottom WRITE setMarginBottom)
  Q_PROPERTY(int autoMargin READ autoMargin WRITE setAutoMargin)
  Q_PROPERTY(QColor color READ color WRITE setColor)
  Q_PROPERTY(Qt::Orientations rangeDrag READ rangeDrag WRITE setRangeDrag)
  Q_PROPERTY(Qt::Orientations rangeZoom READ rangeZoom WRITE setRangeZoom)
  /// \endcond
public:
  /*!
    Defines the mouse interactions possible with QCustomPlot
    
    \c Interactions is a flag of or-combined elements of this enum type.
    \see setInteractions, setInteraction
  */
  enum Interaction { iRangeDrag         = 0x001 ///< <tt>0x001</tt> Axis ranges are draggable (see \ref setRangeDrag, \ref setRangeDragAxes)
                     ,iRangeZoom        = 0x002 ///< <tt>0x002</tt> Axis ranges are zoomable with the mouse wheel (see \ref setRangeZoom, \ref setRangeZoomAxes)
                     ,iMultiSelect      = 0x004 ///< <tt>0x004</tt> The user can select multiple objects by holding the modifier set by \ref setMultiSelectModifier while clicking
                     ,iSelectTitle      = 0x008 ///< <tt>0x008</tt> The plot title is selectable
                     ,iSelectPlottables = 0x010 ///< <tt>0x010</tt> Plottables are selectable
                     ,iSelectAxes       = 0x020 ///< <tt>0x020</tt> Axes are selectable (or parts of them, see QCPAxis::setSelectable)
                     ,iSelectLegend     = 0x040 ///< <tt>0x040</tt> Legends are selectable (or their child items, see QCPLegend::setSelectable)
                     ,iSelectItems      = 0x080 ///< <tt>0x080</tt> Items are selectable (Rectangles, Arrows, Textitems, etc. see \ref QCPAbstractItem)
                   };
  Q_ENUMS(Interaction)
  Q_DECLARE_FLAGS(Interactions, Interaction)
  /*!
    Defines how a layer should be inserted relative to a specified other layer.

    \see addLayer, moveLayer
  */
  enum LayerInsertMode { limBelow  ///< Layer is inserted below other layer
                         ,limAbove ///< Layer is inserted above other layer
                   };
  Q_ENUMS(LayerInsertMode)
  
  explicit QCustomPlot(QWidget *parent = 0);
  virtual ~QCustomPlot();
  
  // getters:
  QString title() const { return mTitle; }
  QFont titleFont() const { return mTitleFont; }
  QColor titleColor() const { return mTitleColor; }
  QRect axisRect() const { return mAxisRect; }
  QRect viewport() const { return mViewport; }
  int marginLeft() const { return mMarginLeft; }
  int marginRight() const { return mMarginRight; }
  int marginTop() const { return mMarginTop; }
  int marginBottom() const { return mMarginBottom; }
  bool autoMargin() const { return mAutoMargin; }
  QColor color() const { return mColor; }
  Qt::Orientations rangeDrag() const { return mRangeDrag; }
  Qt::Orientations rangeZoom() const { return mRangeZoom; }
  QCPAxis *rangeDragAxis(Qt::Orientation orientation);
  QCPAxis *rangeZoomAxis(Qt::Orientation orientation);
  double rangeZoomFactor(Qt::Orientation orientation);
  QCP::AntialiasedElements antialiasedElements() const { return mAntialiasedElements; }
  QCP::AntialiasedElements notAntialiasedElements() const { return mNotAntialiasedElements; }
  bool autoAddPlottableToLegend() const { return mAutoAddPlottableToLegend; }
  QPixmap axisBackground() const { return mAxisBackground; }
  bool axisBackgroundScaled() const { return mAxisBackgroundScaled; }
  Qt::AspectRatioMode axisBackgroundScaledMode() const { return mAxisBackgroundScaledMode; }
  const Interactions interactions() const { return mInteractions; }
  int selectionTolerance() const { return mSelectionTolerance; }
  QFont selectedTitleFont() const { return mSelectedTitleFont; }
  QColor selectedTitleColor() const { return mSelectedTitleColor; }
  bool titleSelected() const { return mTitleSelected; }
  bool noAntialiasingOnDrag() const { return mNoAntialiasingOnDrag; }
  QCP::PlottingHints plottingHints() const { return mPlottingHints; }
  Qt::KeyboardModifier multiSelectModifier() const { return mMultiSelectModifier; }

  // setters:
  void setTitle(const QString &title);
  void setTitleFont(const QFont &font);
  void setTitleColor(const QColor &color);
  void setAxisRect(const QRect &arect);
  void setMarginLeft(int margin);
  void setMarginRight(int margin);
  void setMarginTop(int margin);
  void setMarginBottom(int margin);
  void setMargin(int left, int right, int top, int bottom);
  void setAutoMargin(bool enabled);
  void setColor(const QColor &color);
  void setRangeDrag(Qt::Orientations orientations);
  void setRangeZoom(Qt::Orientations orientations);
  void setRangeDragAxes(QCPAxis *horizontal, QCPAxis *vertical);
  void setRangeZoomAxes(QCPAxis *horizontal, QCPAxis *vertical);
  void setRangeZoomFactor(double horizontalFactor, double verticalFactor);
  void setRangeZoomFactor(double factor);
  void setAntialiasedElements(const QCP::AntialiasedElements &antialiasedElements);
  void setAntialiasedElement(QCP::AntialiasedElement antialiasedElement, bool enabled=true);
  void setNotAntialiasedElements(const QCP::AntialiasedElements &notAntialiasedElements);
  void setNotAntialiasedElement(QCP::AntialiasedElement notAntialiasedElement, bool enabled=true);
  void setAutoAddPlottableToLegend(bool on);
  void setAxisBackground(const QPixmap &pm);
  void setAxisBackground(const QPixmap &pm, bool scaled, Qt::AspectRatioMode mode=Qt::KeepAspectRatioByExpanding);
  void setAxisBackgroundScaled(bool scaled);
  void setAxisBackgroundScaledMode(Qt::AspectRatioMode mode);
  void setInteractions(const Interactions &interactions);
  void setInteraction(const Interaction &interaction, bool enabled=true);
  void setSelectionTolerance(int pixels);
  void setSelectedTitleFont(const QFont &font);
  void setSelectedTitleColor(const QColor &color);
  void setTitleSelected(bool selected);
  void setNoAntialiasingOnDrag(bool enabled);
  void setPlottingHints(const QCP::PlottingHints &hints);
  void setPlottingHint(QCP::PlottingHint hint, bool enabled=true);
  void setMultiSelectModifier(Qt::KeyboardModifier modifier);
  
  // non-property methods:
  // plottable interface:
  QCPAbstractPlottable *plottable(int index);
  QCPAbstractPlottable *plottable();
  bool addPlottable(QCPAbstractPlottable *plottable);
  bool removePlottable(QCPAbstractPlottable *plottable);
  bool removePlottable(int index);
  int clearPlottables();
  int plottableCount() const;
  QList<QCPAbstractPlottable*> selectedPlottables() const;
  QCPAbstractPlottable *plottableAt(const QPointF &pos, bool onlySelectable=false) const;
  bool hasPlottable(QCPAbstractPlottable *plottable) const;

  // specialized interface for QCPGraph:
  QCPGraph *graph(int index) const;
  QCPGraph *graph() const;
  QCPGraph *addGraph(QCPAxis *keyAxis=0, QCPAxis *valueAxis=0);
  bool removeGraph(QCPGraph *graph);
  bool removeGraph(int index);
  int clearGraphs();
  int graphCount() const;
  QList<QCPGraph*> selectedGraphs() const;
  
  // item interface:
  QCPAbstractItem *item(int index) const;
  QCPAbstractItem *item() const;
  bool addItem(QCPAbstractItem* item);
  bool removeItem(QCPAbstractItem *item);
  bool removeItem(int index);
  int clearItems();
  int itemCount() const;
  QList<QCPAbstractItem*> selectedItems() const;
  QCPAbstractItem *itemAt(const QPointF &pos, bool onlySelectable=false) const;
  
  // layer interface:
  QCPLayer *layer(const QString &name) const;
  QCPLayer *layer(int index) const;
  QCPLayer *currentLayer() const;
  bool setCurrentLayer(const QString &name);
  bool setCurrentLayer(QCPLayer *layer);
  int layerCount() const;
  bool addLayer(const QString &name, QCPLayer *otherLayer=0, LayerInsertMode insertMode=limAbove);
  bool removeLayer(QCPLayer *layer);
  bool moveLayer(QCPLayer *layer, QCPLayer *otherLayer, LayerInsertMode insertMode=limAbove);
  
  QList<QCPAxis*> selectedAxes() const;
  QList<QCPLegend*> selectedLegends() const;
  void setupFullAxesBox();
  bool savePdf(const QString &fileName, bool noCosmeticPen=false, int width=0, int height=0);
  bool savePng(const QString &fileName, int width=0, int height=0, double scale=1.0, int quality=-1);
  bool saveJpg(const QString &fileName, int width=0, int height=0, double scale=1.0, int quality=-1);
  bool saveBmp(const QString &fileName, int width=0, int height=0, double scale=1.0);
  bool saveRastered(const QString &fileName, int width, int height, double scale, const char *format, int quality=-1);

  QCPAxis *xAxis, *yAxis, *xAxis2, *yAxis2;
  QCPLegend *legend;
  
public slots:
  void deselectAll();
  void replot();
  void rescaleAxes();
  
signals:
  void mouseDoubleClick(QMouseEvent *event);
  void mousePress(QMouseEvent *event);
  void mouseMove(QMouseEvent *event);
  void mouseRelease(QMouseEvent *event);
  void mouseWheel(QWheelEvent *event);
  
  void plottableClick(QCPAbstractPlottable *plottable, QMouseEvent *event);
  void plottableDoubleClick(QCPAbstractPlottable *plottable, QMouseEvent *event);
  void itemClick(QCPAbstractItem *item, QMouseEvent *event);
  void itemDoubleClick(QCPAbstractItem *item, QMouseEvent *event);
  void axisClick(QCPAxis *axis, QCPAxis::SelectablePart part, QMouseEvent *event);
  void axisDoubleClick(QCPAxis *axis, QCPAxis::SelectablePart part, QMouseEvent *event);
  void legendClick(QCPLegend *legend, QCPAbstractLegendItem *item, QMouseEvent *event);
  void legendDoubleClick(QCPLegend *legend,  QCPAbstractLegendItem *item, QMouseEvent *event);
  void titleClick(QMouseEvent *event);
  void titleDoubleClick(QMouseEvent *event);
  
  void selectionChangedByUser();
  void beforeReplot();
  void afterReplot();
  
protected:
  QString mTitle;
  QFont mTitleFont, mSelectedTitleFont;
  QColor mTitleColor, mSelectedTitleColor;
  QRect mViewport;
  QRect mAxisRect;
  int mMarginLeft, mMarginRight, mMarginTop, mMarginBottom;
  bool mAutoMargin, mAutoAddPlottableToLegend;
  QColor mColor;
  QList<QCPAbstractPlottable*> mPlottables;
  QList<QCPGraph*> mGraphs; // extra list of items also in mPlottables that are of type QCPGraph
  QList<QCPAbstractItem*> mItems;
  QList<QCPLayer*> mLayers;
  Qt::Orientations mRangeDrag, mRangeZoom;
  QCPAxis *mRangeDragHorzAxis, *mRangeDragVertAxis, *mRangeZoomHorzAxis, *mRangeZoomVertAxis;
  double mRangeZoomFactorHorz, mRangeZoomFactorVert;
  bool mDragging;
  QCP::AntialiasedElements mAntialiasedElements, mNotAntialiasedElements;
  QPixmap mAxisBackground;
  bool mAxisBackgroundScaled;
  Qt::AspectRatioMode mAxisBackgroundScaledMode;
  Interactions mInteractions;
  int mSelectionTolerance;
  bool mTitleSelected;
  QRect mTitleBoundingBox;
  bool mNoAntialiasingOnDrag;
  // not explicitly exposed properties:
  QPixmap mPaintBuffer;
  QPoint mDragStart;
  QCPRange mDragStartHorzRange, mDragStartVertRange;
  QPixmap mScaledAxisBackground;
  bool mReplotting;
  QCP::AntialiasedElements mAADragBackup, mNotAADragBackup;
  QCPLayer *mCurrentLayer;
  QCP::PlottingHints mPlottingHints;
  Qt::KeyboardModifier mMultiSelectModifier;
  
  // reimplemented methods:
  virtual QSize minimumSizeHint() const;
  virtual void paintEvent(QPaintEvent *event);
  virtual void resizeEvent(QResizeEvent *event);
  virtual void mouseDoubleClickEvent(QMouseEvent *event);
  virtual void mousePressEvent(QMouseEvent *event);
  virtual void mouseMoveEvent(QMouseEvent *event);
  virtual void mouseReleaseEvent(QMouseEvent *event);
  virtual void wheelEvent(QWheelEvent *event);
  // event helpers:
  virtual bool handlePlottableSelection(QMouseEvent *event, bool additiveSelection, bool &modified);  
  virtual bool handleItemSelection(QMouseEvent *event, bool additiveSelection, bool &modified);  
  virtual bool handleAxisSelection(QMouseEvent *event, bool additiveSelection, bool &modified);
  virtual bool handleTitleSelection(QMouseEvent *event, bool additiveSelection, bool &modified);
  
  // introduced methods:
  virtual void draw(QCPPainter *painter);
  virtual void drawAxisBackground(QCPPainter *painter);
  
  // helpers:
  void updateAxisRect();
  bool selectTestTitle(const QPointF &pos) const;
  friend class QCPLegend;
  friend class QCPAxis;
  friend class QCPLayer;
};
Q_DECLARE_OPERATORS_FOR_FLAGS(QCustomPlot::Interactions)

#endif // QCUSTOMPLOT_H
