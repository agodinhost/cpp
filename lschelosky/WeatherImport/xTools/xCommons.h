/*!
** \file    xCommons.h
** \date    2017/02/22 08:00
** \brief   xTools, common functions, definition.
** \author  A.Godinho (Woody)
**
** \version history
**          22 Feb 2017 - final production release.
**/

#ifndef __XTOOLS_XCOMMONS_H__
#define __XTOOLS_XCOMMONS_H__

//-----------------------------------------------------------------------------

#include "xTypes.h"

//-----------------------------------------------------------------------------

namespace xTools
{
   const string
      localDay_ISO()
         NOEXCEPTION;

   const string
      localTime_ISO()
         NOEXCEPTION;

   /* this is not a REAL timestamp. */
   const string
      getTimestamp()
         NOEXCEPTION;

   const bool
      folderExists(
      const string& SOURCE
      )  NOEXCEPTION;

   const bool
      createFolder(
      const string& SOURCE
      )  NOEXCEPTION;

   const bool
      fileExists(
      const string& SOURCE
      )  NOEXCEPTION;

   void
      xSleep(
      const ulong milis
      )  NOEXCEPTION;

   const bool
      split(
      const string& STR,
      stringVector& VEC,
      const char    DEL = ':',
      const uint&   MIN = 2
      )  NOEXCEPTION;

   stringVector
      split(
      const string& STR,
      const char    DEL = ':'
      )  NOEXCEPTION;

   const bool
      parse(
      const string& STR,
      stringMap&    out,
      const char    DEL = ',',
      const uint&   MIN = 1
      )  NOEXCEPTION;

   const stringMap
      parse(
      const string& STR,
      const char    DEL = ',',
      const uint&   MIN = 1
      )  NOEXCEPTION;

   /*!
    * Type to string conversion with format.
    */
   template< typename T >
      const string 
         toString( const T& VAL, const string& format )
         NOEXCEPTION
         {
            char buf[ 48 ];
            sprintf_s(
               buf, sizeof( buf ),
               format.c_str(), 
               VAL
            );

            const string STR( buf );
            return STR;
         }
      
   /*!
    * Type to string conversion.
    */
   template< typename T >
      const string 
         toString( const T& VAL )
         NOEXCEPTION
         {
            std::ostringstream os;
            os << VAL;
            return os.str();
         }

   /*!
    * EXCEPTION SAFE string to number cast.
    */
   template< typename T >
      const T
         stringTo(
         const string& str,
         const T       defVal
         )  NOEXCEPTION
         {
            T val( defVal );
            if( !str.empty() )
            {
               std::istringstream is( str );
               T val2;
               is >> val2;
               if( is )
                  val = val2;
            }
            return val;
         }

   /*!
    * EXCEPTION SAFE string to number cast.
    */
   template< typename T >
      const T
         getValue(
         const stringMap& map,
         const string&    key,
         const T          defVal
         )  NOEXCEPTION
         {
            T val( defVal );
            stringMap::const_iterator it( map.find( key ) );
            if( it != map.end() )
            {
               //string str( it->second );
               return stringTo< T >( it->second, defVal );
            }
            return val;
         }

   void
      removeStr(
            string& str,
      const string  REMOVE
      )  NOEXCEPTION;

   bool
      replaceStr(
            string& str,
      const string& FROM,
      const string& TO
      ) NOEXCEPTION;

}

//-----------------------------------------------------------------------------

using xTools::localDay_ISO;
using xTools::localTime_ISO;
using xTools::getTimestamp;
using xTools::folderExists;
using xTools::createFolder;
using xTools::fileExists;
using xTools::xSleep;
using xTools::split;
using xTools::parse;
using xTools::toString;
using xTools::stringTo;
using xTools::getValue;
using xTools::removeStr;
using xTools::replaceStr;

#endif /* __XTOOLS_XCOMMONS_H__ */

//-----------------------------------------------------------------------------

// EOF.