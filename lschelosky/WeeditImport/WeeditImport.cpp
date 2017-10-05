/*!
** \file    SerialImport.cpp
** \date    2017/02/22 08:00
** \brief   entry point for the weedit import console application.
** \author  A.Godinho (Woody)
**
** \version history
**          22 Feb 2017 - final production release.
**/

#include "stdafx.h"
#include "WeeditImport.h"
#include "xTools/xCommons.h"

/* #include <fstream> */
using std::fstream;

// -----------------------------------------------------------------------------

void
   WeeditImport::start(
      const string &PORT, 
      const ulong  SPEED
   )
{
   LOG_INFO( "Import started." );
   LOG_INFO( "Opening port " << PORT.c_str() << " @ " << SPEED << " bps." );

   /* get the filename. */
   const string DATAM_FILE( getOutputFile() );

   /* open the output file for write. */
   _ofs.open( DATAM_FILE.c_str(), fstream::in | fstream::out | fstream::app );
   if( !_ofs.is_open() )
      throw runtime_error( "Can't open the output file!" );

   /* open the serial port. */
   _serial.setPort( PORT );
   _serial.setBaudrate( SPEED );
   _serial.setTimeout( _TIMEOUT );
   _serial.open();
   _started = _serial.isOpen();
   if( !_started )
      throw runtime_error( "Can't open the specified port!" );

   /* wait until available. */
   //_serial.synchronize( cout );

   /* control variables. */
   shutdown = 0;
   x = 100;
   e = 100;
   f = 100;

   /* start the communication. */
   while( !shutdown )
   {
      /* *PX0, read variable parameters. */
      if( _serial.write( REQUEST_PX0 ) )
      {
         string px0( _serial.readline( RESPONSE_SIZE, RESPONSE_EOL ) );
         const stringMap map( PX0_map( px0 ) );
         x = getValue< int >( map, "X", 100 );
         e = getValue< int >( map, "E", 100 );
         f = getValue< int >( map, "F", 100 );
         LOG_DEBUG( "X=[" << x << "], E=[" << e << "], F=[" << f << "]" );
         LOG_DEBUG( "double=[" << toString( 123.123456, "%6.2f" ) << "]" );
      }

      /* *BX0, read nozzle activity. */
      if( _serial.write( REQUEST_BX0 ) )
      {
         string tmp( _serial.readline( RESPONSE_SIZE, RESPONSE_EXTRA ) );
         string bx0( _serial.readline( RESPONSE_SIZE, RESPONSE_EOL ) );
         BX0_report( bx0 );
      }

      string tmp( _serial.readline( RESPONSE_SIZE, RESPONSE_EXTRA ) );

      /* sleep, the serial timeout MUST BE GREATER THAN this delay. */
      xSleep( SLEEP_MILLIS );
   }

   stop();
}

void
   WeeditImport::stop()
      NOEXCEPTION
{
   if( _started )
   {
      if( _ofs.is_open() )
      {
         _ofs.flush();
         _ofs.close();
      }

      if( _serial.isOpen() )
      {
         _serial.purge();
         _serial.close();
      }

      LOG_INFO( " Import stopped." );
      _started = false;
   }
}

const string
   WeeditImport::getOutputFile()
{
   const string FOLDER( "c:\\autonomo\\web\\tmp" );
   const string NAME( "\\WEEDIT-DATA-" );
   const string DATE( localDay_ISO() );
   const string EXT( ".m" );

   /* Check the output folder, try to create. */
   if( !folderExists( FOLDER ) )
      if( !createFolder( FOLDER ) )
         throw runtime_error( "Can't create the output folder!" );

   string filename( FOLDER + NAME + DATE + EXT );

   /* IF the filename already exists append one tail char. */
   int i( 0 );
   while( fileExists( filename ) )
   {
      const char index( 'a' + i );
      filename = FOLDER + NAME + DATE + index + EXT;
      i ++;
   }

   return filename;
}

const stringMap
   WeeditImport::PX0_map(
      string& response
   )  NOEXCEPTION
{
   removeStr( response, RESPONSE_EOL );
   LOG_DEBUG( "*PX0 response [" << response << "]" );

   stringMap map;
   stringVector sv( split( response, ':' ) );
   if( sv.size() > 1 )
      if( sv[0] == CMD_PX0 )
      {
         string data = sv[1];
         map = parse( data, ',' );
      }
      else
         LOG_ERROR( "*PX0 ERROR, unexpected command id [" << sv[0] << "]" );
   else
      LOG_ERROR( "*PX0 ERROR, no command id!" );
   return map;
}

void
   WeeditImport::BX0_report(
      string& response
   )  NOEXCEPTION
{
   removeStr( response, RESPONSE_EOL );
   LOG_DEBUG( "*BX0 response [" << response << "]" );

   stringVector sv( split( response, ':' ) );
   if( sv.size() > 1 )
      if( sv[0] == CMD_BX0 )
      {
         string data = sv[1];
         BX0_details( data );
      }
      else
         LOG_ERROR( "*BX0 ERROR, unexpected command id [" << sv[0] << "]" );
   else
      LOG_ERROR( "*BX0 ERROR, no command id!" );
}

void
   WeeditImport::BX0_details(
      const string& response
   )  NOEXCEPTION
{
   const int L( response.length() );
   for( int i = 0; i < L; i ++ )
   {
      string type;
      const char cc( response.at( i ) );
      if( cc == '1' )
         type = "1";
      else if( cc == '0' )
         type = "0";
      if( !type.empty() )
      {
         const int number( ( ( i + 1 ) - ( ( L + 1 ) / 2 ) ) * 5 );
         _ofs
            << localTime_ISO() << ';'
            << number          << ';'
            << type            << REPORT_EOL;
      }
      else
         LOG_ERROR( "*BX0 ERROR, invalid char at [" << i << "]!" );
   }
}

// -----------------------------------------------------------------------------
// EOF.