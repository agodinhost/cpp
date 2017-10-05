/*!
** \file    xCommons.cpp
** \date    2017/02/22 08:00
** \brief   xTools, common functions, implementation.
** \author  A.Godinho (Woody)
**
** \version history
**          22 Feb 2017 - final production release.
**/

/* disable 'strncopy' unsafe warning. */
#pragma warning( disable : 4996 )

/* disable 'createFolder' performance warning. */
#pragma warning( disable : 4800 )

#include "xCommons.h"

#include <iostream>

#include <sstream>
using std::stringstream;

/* file system includes. */
#include <sys/types.h>
#include <sys/stat.h>

/* sleep includes. */
#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

#include <time.h>

//-----------------------------------------------------------------------------

namespace xTools
{
   /*
    * \format: [YYYY-MM-DD]
    */
   const string
      localDay_ISO()
         NOEXCEPTION
   {
      /* windows only. */
      SYSTEMTIME st;
      GetLocalTime( &st );
      //GetSystemTime( &st ); /* UTC time */

      char bf[ 11 ];
      sprintf_s(
         bf, sizeof( bf ),
         "%d-%02d-%02d", 
         st.wYear,
         st.wMonth, 
         st.wDay
      );

      return string( bf );
   }

   /*
    * \format: [YYYY-MM-DD;HH:MM:SS.LL]
    */
   const string
      localTime_ISO()
         NOEXCEPTION
   {
      /* windows only. */
      SYSTEMTIME st;
      GetLocalTime( &st );
      //GetSystemTime( &st ); /* UTC time */

      char bf[ 24 ];
      sprintf_s(
         bf, sizeof( bf ),
         "%4d-%02d-%02d;%02d:%02d:%02d.%2d", 
         st.wYear,
         st.wMonth, 
         st.wDay,                      
         st.wHour, 
         st.wMinute, 
         st.wSecond,
         st.wMilliseconds/10
      );

      return string( bf );
   }

   /* this is not a REAL timestamp. */
   const string
      getTimestamp() 
         NOEXCEPTION
   {
      /*
       * MSDN QueryPerformanceCounter documentation
       * https://msdn.microsoft.com/en-us/library/windows/desktop/ms644904(v=vs.85).aspx
       * https://msdn.microsoft.com/en-us/library/windows/desktop/dn553408(v=vs.85).aspx
       *
       * !WARNING!
       * http://coherent-labs.com/blog/timestamps-for-performance-measurements/
       * http://www.decompile.com/cpp/faq/windows_timer_api.htm
       */

      /* 
       * The availability of the QueryPerformanceCounter is dependent 
       * on the system's hardware - it's indicated by s_qpc_available.
       */
      static LARGE_INTEGER s_frequency;
      static BOOL s_qpc_available( QueryPerformanceFrequency( &s_frequency ) );
      if( s_qpc_available )
      {
         LARGE_INTEGER counter;
         QueryPerformanceCounter( &counter );

         /*
          * to get millis use:
          * ( 1000LL * counter.QuadPart ) / s_frequency.QuadPart
          */

         /*
          * Specific customer math.
          */
         long long timestamp( counter.QuadPart / s_frequency.QuadPart * 1000LL );
         return toString( timestamp );
      }
      else
         throw exception( "QueryPerformanceCounter NOT AVAILABLE!" );
   }

   const bool
      folderExists(
      const string& SOURCE
      )  NOEXCEPTION
   {
      char source[ 512 ];
      strncpy( source, SOURCE.c_str(), sizeof( source ) );
      source[ sizeof( source ) - 1 ] = 0;

      //TODO
      //char* lastSlash = strrchr( source, '\\' );

      bool b( false );
      struct stat st;
      if( stat( source, &st ) != -1 ) 
         b = ( st.st_mode & S_IFDIR ) == S_IFDIR;

      return b;
   }

   inline
   const bool TryCreateDirectory( char* path )
   {
      char* p;
      bool  b;

      if( !( b = CreateDirectory( path, NULL ) ) &&
          !( b = NULL == ( p = strrchr( path, '\\' ) ) ) )
      {
         size_t i;

         ( p = strncpy( ( char* )malloc( 1 + i ), path, i = p - path ) )[ i ] = '\0';
         b = TryCreateDirectory( p );
         free( p );
         b = b ? CreateDirectory( path, NULL ) : false;
      }

      return b;
   }

   const bool
      createFolder(
      const string& SOURCE
      )  NOEXCEPTION
   {
      TryCreateDirectory( const_cast< char* >( SOURCE.c_str() ) );
      return folderExists( SOURCE );
   }

   const bool
      fileExists(
      const string& SOURCE
      )  NOEXCEPTION
   {
      char source[ 512 ];
      strncpy( source, SOURCE.c_str(), sizeof( source ) );
      source[ sizeof( source ) - 1 ] = 0;

      bool b( false );
      struct stat st;
      if( stat( source, &st ) != -1 ) 
         b = ( st.st_mode & S_IFREG ) == S_IFREG;

      return b;
   }

   void
      xSleep(
      const ulong millis
      )  NOEXCEPTION
   {
      Sleep( millis );           /* 100 ms. */
   }

   const bool split( const string& STR, stringVector& out, const char DEL, const uint& MIN ) NOEXCEPTION
   {
      stringstream ss( STR );

      string token;
      while( std::getline( ss, token, DEL ) )
         out.push_back( token );

      return out.size() >= MIN;
   }

   stringVector split( const string& STR, const char DEL ) NOEXCEPTION
   {
      stringVector out;
      try
      {
         split( STR, out, DEL );
      }
      catch( ... )
      {
         // Can't split, does nothing!
      }
      return out;
   }

   const bool parse( const string& STR, stringMap& out, const char DEL, const uint& MIN ) NOEXCEPTION
   {
      if( !STR.empty() )
      {
         const stringVector SV( split( STR, DEL ) );
         const int SZ( SV.size() );
         for( int i = 0; i < SZ; i ++ )
         {
            const string TUPLE( SV[i] );
            if( !TUPLE.empty() )
            {
               const string key( TUPLE.substr( 0, 1 ) );
               const string val( TUPLE.substr( 1, string::npos ) );
               out[ key ] = val;
            }
         }
      }

      return out.size() >= MIN;
   }

   const stringMap parse( const string& STR, const char DEL, const uint& MIN ) NOEXCEPTION
   {
      stringMap out;
      parse( STR, out, DEL, MIN );
      return out;
   }

   void
      removeStr(
            string& str,
      const string  REMOVE
      )  NOEXCEPTION
   { 
      string::size_type n = REMOVE.length();
      for(
         string::size_type i = str.find( REMOVE );
         i != string::npos;
         i = str.find( REMOVE )
      )
         str.erase( i, n );
   }

   bool
      replaceStr(
               string& str,
         const string& FROM,
         const string& TO
      ) NOEXCEPTION
   {
      size_t pos( str.find( FROM ) );
      const bool FOUND( pos != string::npos );
      if( FOUND )
         str.replace( pos, FROM.length(), TO );
      return FOUND;
   }
}

// EOF.