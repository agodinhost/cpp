/*!
** \file    main.cpp
** \date    2017/02/22 08:00
** \brief   entry point for the weedit import console application.
** \author  A.Godinho (Woody)
**
** \version history
**          22 Feb 2017 - final production release.
**/

#include "stdafx.h"
#include "WeeditImport.h"
WeeditImport* _import;
#include <csignal>

// -----------------------------------------------------------------------------

const int usageList()
{
   LOG_INFO( "Usage: " );
#if defined( USE_SERIAL_LIST )
   LOG_INFO( "\tWeeditImport -e" );
#endif
   LOG_INFO( "\tWeeditImport <port> <speed>" );

   return EXIT_SUCCESS;
}

void sigint_handler( const int sig )
{
   if( _import )
   {
      _import->stop();
      delete _import;
      _import = NULL;
   }

   if( sig != 999 )
      LOG_DEBUG( " Import aborted ( Ctrl + C )." );
   exit( EXIT_SUCCESS );
}

/* -- Project main. */
int _tmain( int argc, char* argv[] )
{
   string arg1( SERIAL_PORT );

   if( argc > 1 )
   {
      arg1 = string( argv[1] );

      if( arg1 == "-h" )
         return usageList();

      #if defined( USE_SERIAL_LIST )
      if( arg1 == "-e" )
         return serialList( cout );
      #endif
   }

   string port(  arg1 );
   ulong  speed( SERIAL_SPEED );
   if( argc > 2 )
      speed = stringTo< ulong >( argv[2], SERIAL_SPEED );

   int retCode( EXIT_FAILURE );

   if( signal( SIGINT, sigint_handler ) == SIG_ERR )
      LOG_ERROR( "fatal error, can't install SIGINT handler!" );
   else
      try
      {
         _import = new WeeditImport();
         _import->start( port, speed );
      }
      catch( const exception &e )
      {
         LOG_ERROR( e.what() );
         sigint_handler( 999 );
      }

   return retCode;
}

// -----------------------------------------------------------------------------

// EOF.