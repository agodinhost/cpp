/*!
** \file    SerialImport.cpp
** \date    2017/02/22 08:00
** \brief   entry point for the weather station import console application.
** \author  A.Godinho (Woody)
**
** \version history
**          22 Feb 2017 - final production release.
**/

#include "stdafx.h"
#include "WeatherImport.h"
#include "xTools/xCommons.h"

/* #include <fstream> */
using std::fstream;

// -----------------------------------------------------------------------------

void
   WeatherImport::start(
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
   _serial.synchronize( cout );

   /* start the communication. */
   while( !_shutdown )
   {
      string line( _serial.readline( RESPONSE_SIZE, RESPONSE_EOL ) );
      const stringVector data( parseNMEA( WIMDA, line ) ) ;
      if( data.size() )
      {
         weatherReport( data );

         /* sleep, the serial timeout MUST BE GREATER THAN this delay. */
         xSleep( SLEEP_MILLIS );
      }
      else
         /* sleep, the serial timeout MUST BE GREATER THAN this delay. */
         xSleep( SMALL_SLEEP_MILLIS );
   }

   stop();
}

void
   WeatherImport::stop()
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
   WeatherImport::getOutputFile()
{
   const string FOLDER( "c:\\autonomo\\web\\tmp" );
   const string NAME( "\\WeatherStation" );
   const string EXT( ".m" );

   /* Check the output folder, try to create. */
   if( !folderExists( FOLDER ) )
      if( !createFolder( FOLDER ) )
         throw runtime_error( "Can't create the output folder!" );

   return FOLDER + NAME + EXT;
}

const stringVector
   WeatherImport::parseNMEA(
      const string& protocol,
            string& response
   )  NOEXCEPTION
{
   stringVector data;

   /*
    * Line format:
    * "2016-08-27 10:20\t$WIMDA,30.2269,I,1.0236,B,13.8,C,,,45.9,,2.3,C,80.6,T,69.7,M,1.2,N,0.6,M*53"
    */
   removeStr( response, RESPONSE_EOL );

   /*
    * FIX for the double $ that the device is sending sometimes.
    * Sample data to test this fix:
    * "$$GPGSA,A,3,12,5,29,24,21,2,25,20,31,,,,1.7,1.0,1.4*3D"
    */
   replaceStr( response, "$$", "$" );

   LOG_DEBUG( "response [" << response << "]" );

   stringVector sv1( split( response, NMEA_START_DEL ) );
   if( sv1.size() > 1 )
   {
      stringVector sv2( split( sv1[1], NMEA_DATA_DEL ) );
      if( sv2.size() && sv2[0] == protocol )
      {
         sv2.erase( sv2.begin() );
         if( sv2.size() >= 20 )
            data = sv2;
         else
            LOG_ERROR( "parse NMEA, invalid column count, ignoring line!" );
      }
      else
         LOG_DEBUG( "parse NMEA, ignoring protocol [" << sv2[0] << "]" );
   }
   else
      LOG_ERROR( "parse NMEA, invalid line format!" );

   return data;
}

/*!
 * 30.2239,I,1.0235,B,13.8,C,,,45.9,,2.3,C,73.0,T,62.1,M,1.0,N,0.5,M*53
 * 0       1 2      3 4    5   8    9    11     13     15    17    19
 *                           6       10    12     14     16    18
 *                            7
 */
void
   WeatherImport::weatherReport(
      const stringVector& data
   )  NOEXCEPTION
{
   const float barPressBar(    stringTo< float >( data[2],  -999 ) );
   const float airTemp(        stringTo< float >( data[4],  -999 ) );
   const float relHumid(       stringTo< float >( data[8],  -999 ) );
   const float windDegTrue(    stringTo< float >( data[12], -999 ) );
   const float windSpeedMetre( stringTo< float >( data[18], -999 ) );

   _ofs
      << barPressBar    << ';'
      << airTemp        << ';'
      << relHumid       << ';'
      << windDegTrue    << ';'
      << windSpeedMetre << ';'
      << getTimestamp() << REPORT_EOL;
}

// -----------------------------------------------------------------------------
// EOF.