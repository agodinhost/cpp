/*!
** \file    WeatherImport.h
** \date    2017/02/22 08:00
** \brief   entry point for the weather station import console application.
** \author  A.Godinho (Woody)
**
** \version history
**          22 Feb 2017 - final production release.
**/

#ifndef __WEATHER_IMPORT_H__
#define __WEATHER_IMPORT_H__

//-----------------------------------------------------------------------------

#define USE_DEBUG

#define SERIAL_PORT           "COM6"
#define SERIAL_SPEED          4800
#define TIMEOUT_MILLIS        10000          /* 10 seconds. */
#define SLEEP_MILLIS          500
#define SMALL_SLEEP_MILLIS    200

#define EOL_CR_C              "\x0D"         /* reference. */
#define EOL_LF_C              "\x0A"         /* reference. */
#define EOL_CR_LF_C           "\x0D\x0A"     /* reference. */

#define NMEA_START_DEL        '$'
#define NMEA_DATA_DEL         ','

#define REQUEST_SIZE          128            /* bytes! */
#define REQUEST_EOL           EOL_CR_C
#define RESPONSE_SIZE         128            /* bytes! */
#define RESPONSE_EOL          EOL_CR_C

#define WIMDA                 string( "WIMDA" )

#define REPORT_EOL            EOL_CR_LF_C

//-----------------------------------------------------------------------------

#include "xTools/xCommons.h"
#include "xTools/xSerial.h"
using serial::Serial;
using serial::Timeout;

#include <fstream>
using std::ofstream;

//-----------------------------------------------------------------------------

class WeatherImport
{
public:

   /*!
    * Constructor.
    */
   WeatherImport():
      _shutdown( 0 ),
      _started(  false ),
      _serial(   ),
      _ofs(      ),
      _TIMEOUT(  Timeout::simpleTimeout( TIMEOUT_MILLIS ) )
   {
      /* Nothing. */
   }

   /*!
    * Destructor.
    */
   ~WeatherImport()
   {
      if( _started )
         stop();
   }

   /*!
    * Start the import.
    */
   void
      start(
         const string &PORT,
         const ulong  SPEED
      );

   /*!
    * Stop the import.
    */
   void
      stop()
         NOEXCEPTION;

private:

   /*!
    * Create the output data.m file.
    */
   const string
      getOutputFile();

   /*!
    * Parse the NMEA string.
    */
   const stringVector
      parseNMEA(
      const string& protocol,
            string& response
      )  NOEXCEPTION;

   /*!
    * Write the weather report file.
    */
   void
      weatherReport(
      const stringVector& data
      )  NOEXCEPTION;

private:
   int       _shutdown;
   bool      _started;
   Serial    _serial;
   ofstream  _ofs;

   const
   Timeout   _TIMEOUT;
};

//-----------------------------------------------------------------------------

#endif /* __WEATHER_IMPORT_H__ */

// EOF.