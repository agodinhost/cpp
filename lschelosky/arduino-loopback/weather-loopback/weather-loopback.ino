/*!
** \file    weather-loopback.h
** \date    2017/02/22 08:00
** \brief   Weather Station loopback test.
** \author  A.Godinho (Woody)
**
** \version history
**          22 Feb 2017 - final production release.
**/

#define DEFAULT_SPEED             4800
#define SEND_DELAY_MILLIS         200

#define READLINE_DELAY_MILLIS     50
#define READLINE_BUFFER_SIZE      64

#define EOL_CR_C                  "\x0D"
#define EOL_LF_C                  "\x0A"
#define EOL_CR_LF_C               "\x0D\x0A"

#define RESPONSE_EOL              EOL_CR_C
#define REQUEST_EOL               EOL_CR_C

__attribute__( ( noinline ) )
void setup()
{
   Serial.begin( DEFAULT_SPEED );
   /* wait for serial port to connect. Needed for native USB port only. */
   while( !Serial )
      delay( READLINE_DELAY_MILLIS );
   send( "Weather Station, Serial loopback test." );
}

__attribute__( ( noinline ) )
const String
   getTimestamp()
{
   // @see https://github.com/PaulStoffregen/Time
   //#include "Time.h"
   //#include "RTClib.h"
   /*
   char  buf[ 30 ];
   const time_t t( now() );

   const int y( year(   t ) );
   const int m( month(  t ) );
   const int d( day(    t ) );
   const int h( hour(   t ) );
   const int i( minute( t ) );
   const int s( second( t ) );

   sprintf( buf, "%d-%d-%d %02d:%02d:%02d", y, m, d, h, i, s );
   const String timestamp( buf )
   return timestamp;
   */

   return "2016-03-01 23:59 \x0A";
}

// -fno-inline-small-functions


__attribute__( ( noinline ) )
void send( char* line )
{
   Serial.write( "2016-03-01 23:59 \x0A" );
   Serial.write( line );
   Serial.write( REQUEST_EOL );
   delay( SEND_DELAY_MILLIS );
}

__attribute__( ( noinline ) )
String readLine( const char* EOL )
{
   int  pos( 0 );
   char buf[ READLINE_BUFFER_SIZE ];

   while( !Serial.available() )
      delay( READLINE_DELAY_MILLIS );
   if( Serial.available() )
      pos = Serial.readBytesUntil( EOL, buf, READLINE_BUFFER_SIZE-1 );

   String line;
   if( pos )
   {
      buf[ pos ] = '\0';
      line = String( buf );
   }
   return line;
}

__attribute__( ( noinline ) )
void loop()
{
   /*
   String line( readLine( '\x0D' ) );
   if( line == "teste" )
   send( "2016-08-27 10:20 $WIMDA,30.2269,I,1.0236,B,13.8,C,,,45.9,,2.3,C,80.6,T,69.7,M,1.2,N,0.6,M*53" );
   */

   send( "$WIMDA,29.8430,I,1.0106,B,22.0,C,,,78.3,,18.0,C,260.3,T,249.9,M,1.8,N,0.9,M*6F" );
   send( "$WIMDA,29.8430,I,1.0106,B,22.0,C,,,78.3,,18.0,C,250.1,T,239.7,M,2.0,N,1.0,M*64" );
   send( "$GPRMC,055936.40,A,2823.0122,S,15018.3935,E,1.3,330.1,020317,10.4,E,A*1A" );
   send( "$GPGSA,A,3,21,15,29,18,10,24,16,20,27,26,,,1.6,0.9,1.3*38" );
   send( "$HCHDG,344.0,0.0,E,10.4,E*74" );
   send( "$WIMWD,255.7,T,245.3,M,1.2,N,0.6,M*5A" );
   send( "$GPRMC,055910.40,A,2823.0067,S,15018.4156,E,4.0,240.1,020317,10.4,E,A*14" );
   send( "$HCHDG,250.3,0.0,E,10.4,E*73" );
   send( "$WIMDA,29.8548,I,1.0110,B,22.0,C,,,78.0,,17.9,C,232.8,T,222.4,M,0.6,N,0.3,M*6A" );
   send( "$WIMDA,29.8637,I,1.0113,B,21.9,C,,,78.3,,17.9,C,148.6,T,138.2,M,3.7,N,1.9,M*6C" );
   send( "$WIMWV,349.7,R,3.9,N,A*20" );
   send( "$WIMWD,162.0,T,151.6,M,1.9,N,1.0,M*55" );
   send( "$HCHDG,345.2,0.0,E,10.4,E*77" );
   send( "$GPGSA,A,3,21,15,29,18,10,24,16,20,27,26,,,1.6,0.9,1.3*38" );
   send( "$HCHDG,273.7,0.0,E,10.4,E*76" );
   send( "$WIMDA,29.8696,I,1.0115,B,22.0,C,,,77.9,,17.9,C,127.1,T,116.7,M,1.7,N,0.9,M*6A" );
   send( "$HCHDG,254.1,0.0,E,10.4,E*75" );
   send( "$GPGSA,A,3,21,15,29,18,10,24,16,20,27,26,,,1.6,0.9,1.3*38" );
   send( "$HCHDG,345.2,0.0,E,10.4,E*77" );
   send( "$HCHDG,251.4,0.0,E,10.4,E*75" );
   send( "$GPGSA,A,3,21,15,29,18,10,24,16,20,27,26,,,1.6,0.9,1.3*38" );
   send( "$GPRMC,055955.40,A,2823.0126,S,15018.3928,E,0.0,217.6,020317,10.4,E,A*16" );
   send( "$HCHDG,196.5,0.0,E,10.4,E*7C" );
   send( "$WIMWV,356.6,R,2.8,N,A*2F" );
   send( "$GPRMC,055928.40,A,2823.0143,S,15018.3964,E,3.1,271.6,020317,10.4,E,A*15" );
   send( "$HCHDG,345.3,0.0,E,10.4,E*76" );
   send( "$WIMDA,29.8666,I,1.0114,B,21.9,C,,,78.2,,17.9,C,165.6,T,155.2,M,3.0,N,1.5,M*61" );
   send( "$HCHDG,200.1,0.0,E,10.4,E*74" );
   send( "$WIMDA,29.8430,I,1.0106,B,22.0,C,,,78.356.3,T,245.9,M,1.8,N,0.9,M*66" );
   send( "$WIMWV,354.5,R,2.9,N,A*2F" );
   send( "$HCHDG,197.0,0.0,E,10.4,E*78" );
   send( "$WIMWV,312.0,R,3.7,N,A*27" );
   send( "$HCHDG,196.9,0.0,E,10.4,E*70" );
   send( "$GPRMC,055858.40,A,2823.0018,S,15018.4275,E,1.8,246.4,020317,10.4,E,A*1D" );

}
