/*!
** \file    WeeditImport.h
** \date    2017/02/22 08:00
** \brief   entry point for the weedit import console application.
** \author  A.Godinho (Woody)
**
** \version history
**          22 Feb 2017 - final production release.
**/

#ifndef __WEEDIT_IMPORT_H__
#define __WEEDIT_IMPORT_H__

//-----------------------------------------------------------------------------

#define USE_DEBUG

#define SERIAL_PORT           "COM6"
#define SERIAL_SPEED          38400
#define TIMEOUT_MILLIS        10000          /* 10 seconds. */
#define SLEEP_MILLIS          500

#define EOL_CR_C              "\x0D"         /* reference. */
#define EOL_LF_C              "\x0A"         /* reference. */
#define EOL_CR_LF_C           "\x0D\x0A"     /* reference. */

#define REQUEST_SIZE          128            /* bytes! */
#define REQUEST_EOL           EOL_CR_C
#define RESPONSE_SIZE         128            /* bytes! */
#define RESPONSE_EOL          EOL_CR_C

#define RESPONSE_EXTRA        EOL_CR_LF_C

#define REPORT_EOL            EOL_CR_C

#define CMD_PX0               string( "*PX0" )
#define CMD_BX0               string( "*BX0" )

#define REQUEST_PX0           CMD_PX0 + REQUEST_EOL
#define REQUEST_BX0           CMD_BX0 + REQUEST_EOL

//-----------------------------------------------------------------------------

#include "xTools/xCommons.h"
#include "xTools/xSerial.h"
using serial::Serial;
using serial::Timeout;

#include <fstream>
using std::ofstream;

//-----------------------------------------------------------------------------

class WeeditImport
{
public:

   /*!
    * Constructor.
    */
   WeeditImport():
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
   ~WeeditImport()
   {
      if( _started )
         stop();
   }

   /*!
    * Start the import.
    */
   void
      start(
         const string& PORT,
         const ulong   SPEED
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
    * Parse the response.
    */
   const stringMap
      WeeditImport::PX0_map(
         string& response
      )  NOEXCEPTION;

   /*!
    * Write the BX0 report.
    */
   void
      WeeditImport::BX0_report(
         string& response
      )  NOEXCEPTION;

   /*!
    * Write the BX0 report detail.
    */
   void
      WeeditImport::BX0_details(
      const string& response
      )  NOEXCEPTION;

private:
   bool      _started;
   Serial    _serial;
   ofstream  _ofs;

   const
   Timeout   _TIMEOUT;

   int       shutdown;
   int       x;
   int       e;
   int       f;
};

//-----------------------------------------------------------------------------

#endif /* __WEEDIT_IMPORT_H__ */

// EOF.