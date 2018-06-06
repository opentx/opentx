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

#ifndef _MACROS_H_
#define _MACROS_H_

#include <QDataStream>
#include <iterator>

// #define DIM(arr) (sizeof((arr))/sizeof((arr)[0]))
// new way for c++11
#define DIM(arr__)   ((size_t)(std::end((arr__)) - std::begin((arr__))))

#ifndef CONCATENATE
  #define CONCATENATE_IMPL(A, B)   A ## B
  #define CONCATENATE(A, B)        CONCATENATE_IMPL(A, B)
#endif

#define CREATE_ENUM_STREAM_OPS3(functype, type, datatype) \
	functype QDataStream& operator << (QDataStream &out, type &e) { return out << (datatype&)e; } \
	functype QDataStream& operator >> (QDataStream &in, type &e) { in >> (datatype&)e; return in; }

#define CREATE_ENUM_STREAM_OPS2(type, datatype)          CREATE_ENUM_STREAM_OPS3(inline, type, datatype)
#define CREATE_ENUM_STREAM_OPS(type)                     CREATE_ENUM_STREAM_OPS2(type, quint32)

#define CREATE_ENUM_FRIEND_STREAM_OPS2(type, datatype)   CREATE_ENUM_STREAM_OPS3(friend inline , type, datatype)
#define CREATE_ENUM_FRIEND_STREAM_OPS(type)              CREATE_ENUM_FRIEND_STREAM_OPS2(type, quint32)

#endif // _MACROS_H_
