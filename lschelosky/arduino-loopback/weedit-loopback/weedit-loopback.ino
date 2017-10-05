/*!
** \file    weedit-loopback.h
** \date    2017/02/22 08:00
** \brief   Weedit loopback test.
** \author  A.Godinho (Woody)
**
** \version history
**          22 Feb 2017 - final production release.
**/

#define DELAY_MILLIS              50

#define RESPONSE_EOL              '\x0D'
#define REQUEST_EOL               '\x0D'

#define READLINE_DELAY_MILLIS     50
#define READLINE_BUFFER_SIZE      64

void setup()
{
   Serial.begin( 38400 );
   while( !Serial )
   {
      ; // wait for serial port to connect. Needed for native USB port only.
   }
   response( "Weedit, Serial loopback test." );
}

inline
void response( const char* line )
{
   Serial.write( line );
   Serial.write( RESPONSE_EOL );

   delay( DELAY_MILLIS );
}

String readLine( const char EOL )
{
   char buffer[ READLINE_BUFFER_SIZE ];
   int  pos( 0 );

   while( !Serial.available() )
      delay( READLINE_DELAY_MILLIS );
   if( Serial.available() )
      pos = Serial.readBytesUntil( EOL, buffer, READLINE_BUFFER_SIZE-1 );

   String line;
   if( pos )
   {
      buffer[ pos ] = '\0';
      line = String( buffer );
   }
   return line;
}

void loop()
{
   String line( readLine( REQUEST_EOL ) );
   if( line == "*PX0" )
   {
      Serial.write( "*PX0:I11000105,X3,L290565,H5811,S58,U5941,A8906400,T30,V4090,D3107,P1989,C28,Z666\x0D" );
      Serial.write( "\x0D\x0A" );
   }
   else if( line == "*BX0" )
   {
      Serial.write( "*BX0:10110000001111101101\x0D" );
      Serial.write( "\x0D\x0A" );
   }
   else
   {
      Serial.write( "*UNKNOWN:UNKNOWN\x0D" );
      Serial.write( "\x0D\x0A" );
   }
}
