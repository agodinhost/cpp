/*!
** \file    xTypes.h
** \date    2014/12/17 08:00
** \brief   xTools basic types, definition.
** \author  A.Godinho (Woody)
**/

#ifndef __XTOOLS_XTYPES_H__
#define __XTOOLS_XTYPES_H__

/*!
** Platform check.
** ----------------------------------------------------------------------------
** Microsoft Visual Studio:
** ----------------------------------------------------------------------------
**    MSVC++ 14.0 _MSC_VER == 1900 (Visual Studio 2015)
**    MSVC++ 12.0 _MSC_VER == 1800 (Visual Studio 2013)
**    MSVC++ 11.0 _MSC_VER == 1700 (Visual Studio 2012)
**    MSVC++ 10.0 _MSC_VER == 1600 (Visual Studio 2010)
**    MSVC++ 9.0  _MSC_VER == 1500 (Visual Studio 2008)
**    MSVC++ 8.0  _MSC_VER == 1400 (Visual Studio 2005)
**    MSVC++ 7.1  _MSC_VER == 1310 (Visual Studio 2003)
**    MSVC++ 7.0  _MSC_VER == 1300
**    MSVC++ 6.0  _MSC_VER == 1200
**    MSVC++ 5.0  _MSC_VER == 1100
** ----------------------------------------------------------------------------
**/
#if defined( _MSC_VER )
#   if( _MSC_VER < 1500 )
#      error "This file REQUIRES, at least, VS2008!"
#   endif
#   pragma once
#   define NOINLINE
#   define NOINLINE2    __attribute__( ( __noinline__ ) )
#elif defined( __GNUC__ )
#   define GNUC_VERSION ( __GNUC__ * 1000 + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__ )
#   if ( GNUC_VERSION < 4803 )
#      error "This file REQUIRES, at least, g++ 4.8.3!"
#   endif
#   define NOINLINE     noinline
#   define NOINLINE2
#else
#   if __cplusplus >= 199711L
#      error "C++11. There is no support for your compiler!"
#   else
#      error "NOT C++11. There is no support for your compiler!"
#   endif
#   define NOINLINE
#   define NOINLINE2
#endif

/*!
** CXX11 AND CXX3 conditional defines.
** ----------------------------------------------------------------------------
**/
#if __cplusplus >= 201103L
#   define CXX11
#elif __cplusplus >= 199711L
#   define CXX3
#else
#   error "Unknown C++ Version!"
#endif

/*!
** C++ 11 noexcept statement.
** ----------------------------------------------------------------------------
**/
#if defined( USE_NOEXCEPTION ) && defined( CXX11 )
#  define NOEXCEPTION  noexcept
#else
#  define NOEXCEPTION
#endif

/*!
** Exception list.
** ----------------------------------------------------------------------------
**/
#define THROW_EXCEPTION

/*****************************************************************************/
/* Common Includes.                                                          */
/*****************************************************************************/
//
#include <exception>
using std::exception;

#include <stdexcept>
using std::runtime_error;

#include <string>
using std::string;
using std::locale;

#include <iostream>
using std::cout;
using std::cerr;
using std::endl;
using std::hex;

using std::ifstream;
using std::ostream;
using std::ofstream;
using std::ostringstream;

#include <vector>
using std::vector;

#include <map>
using std::map;

/*****************************************************************************/
/* Common shortcut types.                                                    */
/*****************************************************************************/
/* Type     Bytes Min               Max                                      */
/*****************************************************************************/
/* bool     1     True/False        True/False                               */
/* byte     1     0                 255                                      */
/* schar    1     -128              127                                      */
/* char     1     0                 255                                      */
/* short    2     -32,768           32,767                                   */
/* ushort   2     0                 65,535                                   */
/* int      4     -2,147,483,648    2,147,483,647                            */
/* uint     4     0                 4,294,967,295                            */
/* long     8     -2,147,483,648    2,147,483,647                            */
/* ulong    8     0                 4,294,967,295                            */
/* float    4     1.8E-38           3.4E+38                                  */
/* double   8     2.2E-308          1.8E+308                                 */
/*****************************************************************************/
//
#if defined( CXX11 )
/*! C++ 11 alias. */

using uchar        = unsigned char;    /* WARNING, use with caution, uchar = char */
using byte         = unsigned char;    /* 0 to 255 */
using schar        = signed char;      /* -128 to 127 */
using ushort       = unsigned short;   /* 0 to 65,535 */
using uint         = unsigned int;     /* 0 to 4,294,967,295 */
using ulong        = unsigned long;    /* 0 to 4294967295 */
using stringVector = vector< string >;
using uintVector   = vector< uint >;
using stringMap    = map< string, string >;

#else
/*! priot to C++ 11, there is no alias available. */

typedef unsigned char                  /* WARNING, use with caution, uchar = char */
uchar;

typedef unsigned char                  /* 0 to 255 */
byte;

typedef signed char                    /* -128 to 127 */
schar;

typedef unsigned short                 /* 0 to 65,535 */
ushort;

typedef unsigned int                   /* 0 to 4,294,967,295 */
uint;

typedef unsigned long                  /* 0 to 4294967295 */
ulong;

typedef vector< string >
stringVector;

typedef vector< uint >
uintVector;

typedef map< string, string >
stringMap;

#endif

/*****************************************************************************/
/* Log and debug.                                                            */
/*****************************************************************************/

#if defined( USE_DEBUG )
#define LOG_DEBUG( msg )         cout << msg << endl
#else
#define LOG_DEBUG( msg )
#endif

#define LOG_ERROR( msg )         cout << msg << endl
#define LOG_INFO(  msg )         cout << msg << endl

#endif /* __XTOOLS_XTYPES_H__ */

// EOF.