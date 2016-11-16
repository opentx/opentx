/****************************************************************************
 **
 ** Copyright (C) Qxt Foundation. Some rights reserved.
 **
 ** This file is part of the QxtCore module of the Qxt library.
 **
 ** This library is free software; you can redistribute it and/or modify it
 ** under the terms of the Common Public License, version 1.0, as published
 ** by IBM, and/or under the terms of the GNU Lesser General Public License,
 ** version 2.1, as published by the Free Software Foundation.
 **
 ** This file is provided "AS IS", without WARRANTIES OR CONDITIONS OF ANY
 ** KIND, EITHER EXPRESS OR IMPLIED INCLUDING, WITHOUT LIMITATION, ANY
 ** WARRANTIES OR CONDITIONS OF TITLE, NON-INFRINGEMENT, MERCHANTABILITY OR
 ** FITNESS FOR A PARTICULAR PURPOSE.
 **
 ** You should have received a copy of the CPL and the LGPL along with this
 ** file. See the LICENSE file and the cpl1.0.txt/lgpl-2.1.txt files
 ** included with the source distribution for more information.
 ** If you did not receive a copy of the licenses, contact the Qxt Foundation.
 **
 ** <http://libqxt.org>  <foundation@libqxt.org>
 **
 ****************************************************************************/

#include "qxtcommandoptions.h"

#include <QTextStream>
#include <QIODevice>
#include <QtDebug>
#include <QDir>

/*!
\class QxtCommandOptions

\inmodule QxtCore

\brief The QxtCommandOptions class is a Parser for command-line options

This class is used by applications that need to accept command-line arguments.  It can
also automatically generate help text, which keeps it from accidentally falling out of
sync with the supported options, and it can produce warnings for common errors.

It accepts Windows-style ("/option"), UNIX-style ("-option"), and GNU-style
("--option") options. By default, QxtCommandOptions uses Windows-style options
(QxtCommandOptions::Slash) on Windows and GNU-style options (QxtCommandOptions::DoubleDash)
on UNIX and Mac. When using GNU-style options, single-character option names only
require a single leading dash and can be grouped together, for example, "-abc".

Any parameter that does not start with the option prefix is considered a positional
parameter. Most applications treat positional parameters as filenames. When using
GNU- or UNIX-style options, use a double dash ("--") alone to force the remaining
parameters to be interpreted as positional parameters.

To use QxtCommandOptions, first add the supported options using the add() and alias()
methods, then process the command line using the parse() method, and finally retrieve
the processed options using the positional(), count(), value() and/or parameters()
methods.

Mutually-exclusive options can be specified by using the \a group parameter to add().
Only one option in a group will be accepted on the command line; if multiple options
are provided, only the last one takes effect.

Some options may accept an optional or required parameter. Depending on the value
of the paramStyle() property, the parameter may be separated from the option by
an equals sign ("/option=value") or by a space ("-option value"). By default,
Windows uses an equals sign and UNIX and Mac accept both an equals sign and a
space. Optional parameters always require an equals sign. Note that, when using
GNU-style options, single-character options cannot have optional parameters.

A simple archiving application might use code similar to this:
\code
QxtCommandOptions options;
options.add("compress", "create a new archive");
options.alias("compress", "c");
options.add("extract", "extract files from an archive");
options.alias("extract", "x");
options.add("level", "set the compression level (0-9)", QxtCommandOptions::Required);
options.alias("level", "l");
options.add("verbose", "show more information about the process; specify twice for more detail", QxtCommandOptions::AllowMultiple);
options.alias("verbose", "v");
options.add("help", "show this help text");
options.alias("help", "h");
options.parse(QCoreApplication::arguments());
if(options.count("help") || options.showUnrecognizedWarning()) {
    options.showUsage();
    return -1;
}
bool verbose = options.count("verbose");
int level = 5;
if(options.count("level")) {
    level = options.value("level").toInt();
}
\endcode

\sa QCoreApplication::arguments()
*/

static const char* qxt_qt_options[] =
{
    "=style",       QT_TRANSLATE_NOOP("QxtCommandOptions", "sets the application GUI style"),
    "=stylesheet",  QT_TRANSLATE_NOOP("QxtCommandOptions", "sets the application stylesheet"),
    "=session",     QT_TRANSLATE_NOOP("QxtCommandOptions", "restores the application from an earlier session"),
    "widgetcount",  QT_TRANSLATE_NOOP("QxtCommandOptions", "displays debugging information about widgets"),
    "reverse",      QT_TRANSLATE_NOOP("QxtCommandOptions", "use right-to-left layout"),
#ifdef QT_DEBUG
    "nograb",       QT_TRANSLATE_NOOP("QxtCommandOptions", "never grab the mouse or keyboard"),
#endif
#if defined(QT_DEBUG) && defined(Q_WS_X11)
    "dograb",       QT_TRANSLATE_NOOP("QxtCommandOptions", "grab the mouse/keyboard even in a debugger"),
    "sync",         QT_TRANSLATE_NOOP("QxtCommandOptions", "run in synchronous mode for debugging"),
#endif
#ifdef Q_WS_WIN
    "direct3d",     QT_TRANSLATE_NOOP("QxtCommandOptions", "use Direct3D by default"),
#endif
#ifdef Q_WS_X11
    "=display",     QT_TRANSLATE_NOOP("QxtCommandOptions", "sets the X11 display"),
    "=geometry",    QT_TRANSLATE_NOOP("QxtCommandOptions", "sets the geometry of the first window"),
    "=font",        "",
    "=fn",          QT_TRANSLATE_NOOP("QxtCommandOptions", "sets the default font"),
    "=background",  "",
    "=bg",          QT_TRANSLATE_NOOP("QxtCommandOptions", "sets the default background color"),
    "=foreground",  "",
    "=fg",          QT_TRANSLATE_NOOP("QxtCommandOptions", "sets the default foreground color"),
    "=button",      "",
    "=btn",         QT_TRANSLATE_NOOP("QxtCommandOptions", "sets the default button color"),
    "=name",        QT_TRANSLATE_NOOP("QxtCommandOptions", "sets the application name"),
    "=title",       QT_TRANSLATE_NOOP("QxtCommandOptions", "sets the application title"),
    "=visual",      QT_TRANSLATE_NOOP("QxtCommandOptions", "sets the X11 visual type"),
    "=ncols",       QT_TRANSLATE_NOOP("QxtCommandOptions", "limit the number of colors on an 8-bit display"),
    "cmap",         QT_TRANSLATE_NOOP("QxtCommandOptions", "use a private color map"),
    "=im",          QT_TRANSLATE_NOOP("QxtCommandOptions", "sets the input method server"),
    "noxim",        QT_TRANSLATE_NOOP("QxtCommandOptions", "disable the X Input Method"),
    "=inputstyle",  QT_TRANSLATE_NOOP("QxtCommandOptions", "sets the style used by the input method"),
#endif
    0,              0
};

/*
 * This function is used to check to see if a parameter
 * is used by Qt.
 */
static int isQtOption(const QString& param)
{
    // Qt options all start with a single dash regardless of platform
    if (param.length() < 2) return 0;
    if (param[0] != '-') return 0;
    if (param[1] == '-') return 0;
#ifdef Q_OS_MAC
    if (param.left(5) == "-psn_") return 1;
#endif
    QString name = param.mid(1), value;
    // bool hasEquals;

    // Separate the option and the value, if present
    if (name.indexOf('=') != -1)
    {
        value = param.section('=', 1);
        name = param.section('=', 0, 0);
        // hasEquals = true;
    }
    else
    {
        value = "";
        // hasEquals = false;
    }

    const char* option;
    bool optionHasValue;
    for (int i = 0; qxt_qt_options[i]; i += 2)
    {
        option = qxt_qt_options[i];
        // In the table above, options that require parameters start with =
        if (option[0] == '=')
        {
            optionHasValue = true;
            option = option + 1; // pointer math to skip =
        }
        else
        {
            optionHasValue = false;
        }

        // The return value indicates how many parameters to skip
        if (name == option)
        {
            if (optionHasValue) return 2;
            return 1;
        }
    }

    return 0;
}

// Storage structure for option data
struct QxtCommandOption
{
    QStringList names;      // aliases accepted at the command line
    QString canonicalName;  // name used for alias()/count()/value()
    QString desc;           // documentation string
    QStringList values;     // values passed on command line
    QxtCommandOptions::ParamTypes paramType;    // flags
    quint16 group;          // mutual exclusion group
};

class QxtCommandOptionsPrivate : public QxtPrivate<QxtCommandOptions>
{
    Q_DECLARE_TR_FUNCTIONS(QxtCommandOptions)
public:
    QXT_DECLARE_PUBLIC(QxtCommandOptions)

    QList<QxtCommandOption> options;
    QHash<QString, QxtCommandOption*> lookup;       // cache structure to simplify processing
    QHash<int, QList<QxtCommandOption*> > groups;   // cache structure to simplify processing
    QxtCommandOptions::FlagStyle flagStyle;
    QxtCommandOptions::ParamStyle paramStyle;
    QStringList positional;                         // prefixless parameters
    QStringList unrecognized;                       // prefixed parameters not in recognized options
    QStringList missingParams;                      // parameters with required values missing
    int screenWidth;
    bool parsed;

    QxtCommandOption* findOption(const QString& name);
    const QxtCommandOption* findOption(const QString& name) const;
    void setOption(QxtCommandOption* option, const QString& value = QString());
    void parse(const QStringList& params);
};

/* Looks up an option in qxt_d().options by canonical name */
QxtCommandOption* QxtCommandOptionsPrivate::findOption(const QString& name)
{
    // The backwards loop will find what we're looking for more quickly in the
    // typical use case, where you add aliases immediately after adding the option.
    for (int i = options.count() - 1; i >= 0; --i)
    {
        if (options[i].canonicalName == name) return &options[i];
    }
    qWarning() << qPrintable(QString("QxtCommandOptions: ") + tr("option \"%1\" not found").arg(name));
    return 0;
}

/* Looks up an option in qxt_d().options by canonical name
 * This is a const overload for const functions */
const QxtCommandOption* QxtCommandOptionsPrivate::findOption(const QString& name) const
{
    // The backwards loop will find what we're looking for more quickly in the
    // typical use case, where you add aliases immediately after adding the option.
    for (int i = options.count() - 1; i >= 0; --i)
    {
        if (options[i].canonicalName == name) return &(options.at(i));
    }
    qWarning() << qPrintable(QString("QxtCommandOptions: ") + tr("option \"%1\" not found").arg(name));
    return 0;
}

/*!
 * Constructs a QxtCommandOptions object.
 */
QxtCommandOptions::QxtCommandOptions()
{
    QXT_INIT_PRIVATE(QxtCommandOptions);
    qxt_d().screenWidth = 80;
    qxt_d().parsed = false;
#ifdef Q_OS_WIN
    setFlagStyle(Slash);
    setParamStyle(Equals);
#else
    setFlagStyle(DoubleDash);
    setParamStyle(SpaceAndEquals);
#endif
}

/*!
 * Sets which prefix is used to identify options. The default value is Slash on Windows
 * and DoubleDash on all other platforms.
 *
 * Note that Qt's built-in options (see QApplication) always use a single dash,
 * regardless of this setting.
 */
void QxtCommandOptions::setFlagStyle(FlagStyle style)
{
    qxt_d().flagStyle = style;
}

/*!
 * Gets which prefix is used to identify options.
 */
QxtCommandOptions::FlagStyle QxtCommandOptions::flagStyle() const
{
    return qxt_d().flagStyle;
}

/*!
 * Sets which value separator is used for options that accept parameters.
 * The default value is Equals on Windows and SpaceAndEquals on all other
 * platforms.
 *
 * Single-letter options with optional parameters in DoubleDash mode
 * always use an equals sign, regardless of this setting.
 *
 * Qt's built-in options always behave as SpaceAndEquals, regardless of
 * this setting.
 */
void QxtCommandOptions::setParamStyle(ParamStyle style)
{
    qxt_d().paramStyle = style;
}

/*!
 * Sets which value separator is used for options that accept parameters.
 */
QxtCommandOptions::ParamStyle QxtCommandOptions::paramStyle() const
{
    return qxt_d().paramStyle;
}

/*!
 * Sets the width of the screen, in characters. This is used for word-wrapping
 * the automatically-generated help text to the size of the screen. The default
 * value is 80 characters. Pass 0 to disable word-wrapping.
 */
void QxtCommandOptions::setScreenWidth(quint16 width)
{
    qxt_d().screenWidth = width;
}

/*!
 * Gets the width of the screen, in characters.
 */
quint16 QxtCommandOptions::screenWidth() const
{
    return qxt_d().screenWidth;
}

/*!
 * Adds a section separator. Section separators are only used in generating
 * help text, and can be used to visually separate related groups of
 * options.
 */
void QxtCommandOptions::addSection(const QString& name)
{
    QxtCommandOption option;
    option.canonicalName.clear();
    option.desc = name;
    qxt_d().options.append(option);
}

/*!
 * Adds an option to the parser.
 *
 * The name parameter defines the name that will be used by the alias(),
 * count(), value(), and parameters() methods. Additional names for the
 * same option can be defined using the alias() method.
 *
 * The group parameter, if used, defines a set of mutually-exclusive options.
 * If more than one option in the same group is passed on the command line,
 * only the last one takes effect.
 */
void QxtCommandOptions::add(const QString& name, const QString& desc, ParamTypes paramType, int group)
{
    QxtCommandOption option;
    option.canonicalName = name;
    option.desc = desc;
    option.paramType = paramType;
    option.group = group;
    qxt_d().options.append(option);
    if (group != -1)
        qxt_d().groups[group].append(&(qxt_d().options.last()));
    // Connect the canonical name to a usable name
    alias(name, name);
}

/*!
 * Provides an alias for an option. An alias is another name for the option that can be
 * given on the command line. Aliases cannot be used as parameters to alias(), count()
 * or value() nor can single-letter aliases be created for options with an optional value.
 *
 * The from parameter must be a name has previously been added with the add() method.
 */
void QxtCommandOptions::alias(const QString& from, const QString& to)
{
    QxtCommandOption* option = qxt_d().findOption(from);
    if (!option) return; // findOption outputs the warning
    option->names.append(to);
    qxt_d().lookup[to] = option;
    if (option->paramType & ValueOptional && qxt_d().flagStyle == DoubleDash && to.length() == 1)
        qWarning() << qPrintable(QString("QxtCommandOptions: ") + tr("Short options cannot have optional parameters"));
}

/*!
 * Returns the positional parameters from the command line, that is, the arguments that
 * do not begin with the option prefix.
 *
 * \sa flagStyle()
 */
QStringList QxtCommandOptions::positional() const
{
    if (!qxt_d().parsed)
        qWarning() << qPrintable(QString("QxtCommandOptions: ") + tr("positional() called before parse()"));
    return qxt_d().positional;
}

/*!
 * Returns the options that could not be parsed.
 *
 * An argument is unrecognized if it begins with the option prefix but was never
 * defined using the add() or alias() methods, or if it requires a value but the
 * user did not provide one.
 */
QStringList QxtCommandOptions::unrecognized() const
{
    if (!qxt_d().parsed)
        qWarning() << qPrintable(QString("QxtCommandOptions: ") + tr("unrecognized() called before parse()"));
    return qxt_d().unrecognized + qxt_d().missingParams;
}

/*!
 * Returns the number of times an option was passed on the command line.
 *
 * This function will only return 0 or 1 for options that were not created with the
 * QxtCommandOptions::AllowMultiple flag set.
 */
int QxtCommandOptions::count(const QString& name) const
{
    if (!qxt_d().parsed)
        qWarning() << qPrintable(QString("QxtCommandOptions: ") + tr("count() called before parse()"));
    const QxtCommandOption* option = qxt_d().findOption(name);
    if (!option) return 0; // findOption outputs the warning
    return option->values.count();
}

/*!
 * Returns the value or values for an option passed on the command line.
 *
 * If the option was not passed on the command line, this function returns a null QVariant.
 * If the option was created with the QxtCommandOptions::AllowMultiple flag, and the option
 * was passed more than once, this function returns a QStringList containing the values.
 * Otherwise, this function returns the last (or only) value given to the option on the
 * command line.  When an option allowing an optional value is provided on the command
 * line and for which no value is provided, an empty but non-null QString will be returned
 * in the QVariant.
 */
QVariant QxtCommandOptions::value(const QString& name) const
{
    if (!qxt_d().parsed)
        qWarning() << qPrintable(QString("QxtCommandOptions: ") + tr("value() called before parse()"));
    const QxtCommandOption* option = qxt_d().findOption(name);
    if (!option) return QVariant(); // findOption outputs the warning
    int ct = option->values.count();
    if (ct == 0) return QVariant();
    if (ct == 1) return option->values.first();
    return option->values;
}

/*!
 * Returns all of the recognized options passed on the command line.
 */
QMultiHash<QString, QVariant> QxtCommandOptions::parameters() const
{
    if (!qxt_d().parsed)
        qWarning() << qPrintable(QString("QxtCommandOptions: ") + tr("parameters() called before parse()"));
    QMultiHash<QString, QVariant> params;
    int ct;
    foreach(const QxtCommandOption& option, qxt_d().options)
    {
        ct = option.values.count();
        if (!ct)
        {
            continue;
        }
        else if (!(option.paramType & (ValueOptional | ValueRequired)))
        {
            // Valueless options are really a true/false flag
            params.insert(option.canonicalName, true);
        }
        else
        {
            foreach(const QVariant& value, option.values)
                params.insert(option.canonicalName, value);
        }
    }
    return params;
}

/*!
 * This is an overloaded member function, provided for convenience.
 *
 * Process a set of command-line options. This overload accepts a number of
 * arguments and a pointer to the list of arguments.
 *
 * Note that parse() may be invoked multiple times to handle arguments from
 * more than one source.
 */
void QxtCommandOptions::parse(int argc, char** argv)
{
    QStringList args;
    for (int i = 0; i < argc; i++)
        args << argv[i];
    parse(args);
}

/*!
 * Process a set of command-line options. This overload accepts a QStringList
 * containing the command-line options, such as the one returned by
 * QCoreApplication::arguments().
 *
 * Note that parse() may be invoked multiple times to handle arguments from
 * more than one source.
 *
 * \sa QCoreApplication::arguments()
 */
void QxtCommandOptions::parse(QStringList params)
{
    qxt_d().parse(params);
    qxt_d().parsed = true;
}

/* Update the internal data structures with an option from the command line. */
void QxtCommandOptionsPrivate::setOption(QxtCommandOption* option, const QString& value)
{
    if (groups.contains(option->group))
    {
        // Clear mutually-exclusive options
        QList<QxtCommandOption*>& others = groups[option->group];
        foreach(QxtCommandOption* other, others)
        {
            if (other != option) other->values.clear();
        }
    }
    // Clear all previous values if multiples are not accepted
    if (!(option->paramType & QxtCommandOptions::AllowMultiple))
        option->values.clear();
    option->values.append(value);
}

/* Do the work of parsing the command line */
void QxtCommandOptionsPrivate::parse(const QStringList& params)
{
    int pos = 1;    // 0 is the application name
    int ct = params.count();
    int skip = 0;
    bool endFlags = false;
    bool /*notUnrecognized, */hasEquals;
    QString name, param, value;

    while (pos < ct)
    {
        // Ignore Qt built-in options
        while ((skip = isQtOption(params[pos])))
        {
            pos += skip;
            if (pos >= ct)
                return;
        }

        param = params[pos];
        pos++;

        if (!endFlags && ((flagStyle == QxtCommandOptions::Slash && param[0] == '/') ||
                          (flagStyle != QxtCommandOptions::Slash && param[0] == '-')))
        {
            // tagged argument
            if (param.length() == 1)
            {
                // "-" or "/" alone can't possibly match a flag, so use positional.
                positional.append(param);
                continue;
            }
            // notUnrecognized = false;

            if (flagStyle != QxtCommandOptions::Slash && param == "--")
            {
                // End of parameters flag
                endFlags = true;
            }
            else if (flagStyle == QxtCommandOptions::DoubleDash && param[1] != '-')
            {
                // Handle short-form options
                int len = param.length();
                QxtCommandOption* option;
                for (int i = 1; i < len; i++)
                {
                    QString ch(param[i]);
                    if (ch == "-")
                    {
                        endFlags = true;
                    }
                    else
                    {
                        option = lookup.value(ch, 0);
                        if (!option)
                        {
                            // single-letter flag has no known equivalent
                            unrecognized.append(QString("-") + param[i]);
                        }
                        else
                        {
                            if (option->paramType & QxtCommandOptions::ValueRequired)
                            {
                                // Check for required parameters
                                // Short options can't have optional parameters
                                if (pos >= params.count())
                                {
                                    missingParams.append(param);
                                    break;
                                }
                                value = params[pos];
                            }
                            else
                            {
                                value = "";
                            }
                            setOption(option, value);
                        }
                    }
                }
            }
            else
            {
                // Break apart a value
                if (param.indexOf('=') != -1)
                {
                    value = param.section('=', 1);
                    param = param.section('=', 0, 0);
                    hasEquals = true;
                }
                else
                {
                    value = "";
                    hasEquals = false;
                }

                if (flagStyle == QxtCommandOptions::DoubleDash)
                    name = param.mid(2);
                else
                    name = param.mid(1);

                QxtCommandOption* option = lookup.value(name, 0);
                if (!option)
                {
                    unrecognized.append(param);
                }
                else
                {
                    if (option->paramType & QxtCommandOptions::ValueRequired && !hasEquals)
                    {
                        // Check for parameters
                        if (pos >= params.count())
                        {
                            missingParams.append(param);
                            break;
                        }
                        value = params[pos];
                        pos++;
                    }
                    else if ((paramStyle & QxtCommandOptions::Space)
                            && (option->paramType & QxtCommandOptions::ValueOptional)
                            && !hasEquals) 
                    {
                        if (pos < params.count())
                        {
                            if (!((flagStyle == QxtCommandOptions::Slash && params.at(pos)[0] == '/') ||
                                (flagStyle != QxtCommandOptions::Slash && params.at(pos)[0] == '-')))
                            {
                                value = params[pos];
                                pos++;
                            }
                        }
                    }

                    setOption(option, value);
                }
            }
        }
        else
        {
            // positional argument
            positional.append(param);
        }
    }
}

/*!
 * This is an overloaded member function, provided for convenience.
 *
 * Outputs a warning about any unrecognized options to the provided device, or
 * standard error by default. The device must already be opened for writing.
 *
 * This function returns true if any warnings were output, or false otherwise.
 *
 * If a QCoreApplication or a subclass of QCoreApplication has been instantiated,
 * this function uses QCoreApplication::applicationFilePath() to get the name
 * of the executable to include in the message.
 *
 * \sa QCoreApplication::applicationFilePath()
 */
bool QxtCommandOptions::showUnrecognizedWarning(QIODevice* device) const
{
    if (!device)
    {
        QTextStream stream(stderr);
        return showUnrecognizedWarning(*&stream);
    }
    else
    {
        QTextStream stream(device);
        return showUnrecognizedWarning(*&stream);
    }
}

/*!
 * Returns the automatically-generated warning text about any unrecognized options.
 *
 * If a QCoreApplication or a subclass of QCoreApplication has been instantiated,
 * this function uses QCoreApplication::applicationFilePath() to get the name
 * of the executable to include in the message.
 *
 * \sa QCoreApplication::applicationFilePath()
 */
QString QxtCommandOptions::getUnrecognizedWarning() const
{
    QString usage;
    QTextStream stream(&usage);
    showUnrecognizedWarning(*&stream);
    return usage;
}

/*!
 * This is an overloaded member function, provided for convenience.
 *
 * Outputs a warning about any unrecognized options to the provided stream.
 *
 * This function returns true if any warnings were output, or false otherwise.
 *
 * If a QCoreApplication or a subclass of QCoreApplication has been instantiated,
 * this function uses QCoreApplication::applicationFilePath() to get the name
 * of the executable to include in the message.
 *
 * \sa QCoreApplication::applicationFilePath()
 */
bool QxtCommandOptions::showUnrecognizedWarning(QTextStream& stream) const
{
    if (!qxt_d().unrecognized.count() && !qxt_d().missingParams.count()) return false;

    QString name;
    if (QCoreApplication::instance())
        name = QDir(QCoreApplication::applicationFilePath()).dirName();
    if (name.isEmpty())
        name = "QxtCommandOptions";

    if (qxt_d().unrecognized.count())
        stream << name << ": " << tr("unrecognized parameters: ") << qxt_d().unrecognized.join(" ") << endl;

    foreach(const QString& param, qxt_d().missingParams)
        stream << name << ": " << tr("%1 requires a parameter").arg(param) << endl;

    return true;
}

/*!
 * This is an overloaded member function, provided for convenience.
 *
 * Outputs automatically-generated usage text for the accepted options to the provided
 * device, or standard error by default. The device must already be opened for writing.
 *
 * Pass true to showQtOptions to output usage text for the options recognized by
 * QApplication.
 *
 * \sa QApplication
 */
void QxtCommandOptions::showUsage(bool showQtOptions, QIODevice* device) const
{
    if (!device)
    {
        QTextStream stream(stdout);
        showUsage(showQtOptions, *&stream);
    }
    else
    {
        QTextStream stream(device);
        showUsage(showQtOptions, *&stream);
    }
}

/*!
 * Returns the automatically-generated usage text for the accepted options.
 */
QString QxtCommandOptions::getUsage(bool showQtOptions) const
{
    QString usage;
    QTextStream stream(&usage);
    showUsage(showQtOptions, *&stream);
    return usage;
}

/*!
 * This is an overloaded member function, provided for convenience.
 *
 * Outputs automatically-generated usage text for the accepted options to the provided
 * stream.
 *
 * Pass true to showQtOptions to output usage text for the options recognized by
 * QApplication.
 *
 * \sa QApplication
 */
void QxtCommandOptions::showUsage(bool showQtOptions, QTextStream& stream) const
{
    QStringList names;
    QStringList descs;
    int maxNameLength = 0;
    QString name;

    foreach(const QxtCommandOption& option, qxt_d().options)
    {
        // Don't generate usage for undocumented parameters
        if (option.paramType & Undocumented) continue;

        foreach(const QString& n, option.names)
        {
            if (name.length()) name += ", ";
            if (qxt_d().flagStyle == Slash)
                name += '/';
            else if (qxt_d().flagStyle == DoubleDash && n.length() > 1)
                name += "--";
            else
                name += '-';
            name += n;
            if (option.paramType & (ValueOptional | ValueRequired))
            {
                if (option.paramType & ValueOptional)
                    name += "[=x]";
                else if (qxt_d().paramStyle == SpaceAndEquals)
                    name += "[=]x";
                else if (qxt_d().paramStyle == Equals)
                    name += "=x";
                else
                    name += " x";
            }
        }

        // The maximum name length is used for formatting the output columns
        if (name.length() > maxNameLength)
            maxNameLength = name.length();
        names.append(name);
        descs.append(option.desc);
        name = "";
    }

    if (showQtOptions)
    {
        // Add a section header
        names.append(QString());
        descs.append("Common Qt Options");

        // Parse through qxt_qt_options
        const char* option;
        bool optionHasValue;
        for (int i = 0; qxt_qt_options[i]; i += 2)
        {
            option = qxt_qt_options[i];

            if (option[0] == '=')
            {
                // The option takes a parameter
                optionHasValue = true;
                option = option + 1; // pointer math to skip the =
            }
            else
            {
                optionHasValue = false;
            }

            // Concatenate on the option alias
            if (!name.isEmpty()) name += ", ";
            name += '-';
            name += option;
            if (optionHasValue) name += "[=]x";

            if (qxt_qt_options[i+1][0] != 0)
            {
                // The last alias for the option has the description
                if (name.length() > maxNameLength)
                    maxNameLength = name.length();
                names.append(name);
                descs.append(qxt_qt_options[i+1]);
                name = "";
            }
        }
    }

    int ct = names.count();
    QString line, wrap(maxNameLength + 3, ' ');
    for (int i = 0; i < ct; i++)
    {
        if (names[i].isEmpty())
        {
            // Section headers have no name entry
            stream << endl << descs[i] << ":" << endl;
            continue;
        }
        line = ' ' + names[i] + QString(maxNameLength - names[i].length() + 2, ' ');
        foreach(const QString& word, descs[i].split(' ', QString::SkipEmptyParts))
        {
            if (qxt_d().screenWidth > 0 && line.length() + word.length() >= qxt_d().screenWidth)
            {
                stream << line << endl;
                line = wrap;
            }
            line += word + ' ';
        }
        stream << line << endl;
    }
}
