/*!
** \file    xSerialImpl-win.cpp
** \brief   Windows implementation of the Serial class interface.
** \license Open Source, The MIT License.
** \author  William Woodall <wjwwood@gmail.com>
** \author  John Harrison   <ash.gti@gmail.com>
** \version 0.1
** \review  A.Godinho (Woody)
** \date    2017/02/21 08:00
**/

#if defined( _WIN32 )

#include <sstream>

#include "xSerialImpl-win.h"

using std::string;
using std::wstring;
using std::stringstream;
using std::invalid_argument;
using serial::Serial;
using serial::Timeout;
using serial::bytesize_t;
using serial::parity_t;
using serial::stopbits_t;
using serial::flowcontrol_t;
using serial::SerialException;
using serial::PortNotOpenedException;
using serial::IOException;

/* only for synchronize. */
#include <iostream>
using std::cout;
using std::endl;

inline
wstring
_prefix_port_if_needed( const wstring &input )
{
  static wstring windows_com_port_prefix = L"\\\\.\\";
  if( input.compare( windows_com_port_prefix ) != 0 )
  {
    return windows_com_port_prefix + input;
  }
  return input;
}

Serial::SerialImpl::SerialImpl (
   const string        &port,
   const unsigned long baudrate,
   const bytesize_t    bytesize,
   const parity_t      parity,
   const stopbits_t    stopbits,
   const flowcontrol_t flowcontrol
)  :
   _port          ( port.begin(), port.end() ),
   _hSerial       ( INVALID_HANDLE_VALUE ),
   _is_open       ( false ),
   _baudrate      ( baudrate ),
   _parity        ( parity ),
   _bytesize      ( bytesize ),
   _stopbits      ( stopbits ),
   _flowcontrol   ( flowcontrol )
{
  _read_mutex = CreateMutex( NULL, false, NULL );
  _write_mutex = CreateMutex( NULL, false, NULL );
  if( !_port.empty() )
    open();
}

Serial::SerialImpl::~SerialImpl()
{
  this->close();
  CloseHandle( _write_mutex );
  CloseHandle( _read_mutex );
}

void
Serial::SerialImpl::open()
{
  if( _port.empty() )
    throw invalid_argument( "Empty port is invalid." );
  if( _is_open )
    throw SerialException( "Serial port already open." );

  // See: https://github.com/wjwwood/serial/issues/84
  wstring port_with_prefix( _prefix_port_if_needed( _port ) );
  LPCWSTR lp_port( port_with_prefix.c_str() );
  _hSerial = CreateFileW(
     lp_port,
     GENERIC_READ | GENERIC_WRITE,
     0,
     0,
     OPEN_EXISTING,
     FILE_ATTRIBUTE_NORMAL,
     0
  );

  if( _hSerial == INVALID_HANDLE_VALUE )
  {
    DWORD lastErr( GetLastError() );
	 stringstream ss;
    switch( lastErr )
    {
    case ERROR_FILE_NOT_FOUND:
      /* Use this->getPort to convert to a std::string. */
      ss << "Specified port, " << this->getPort() << ", does not exist.";
      THROW (IOException, ss.str().c_str());
    default:
      ss << "Unknown error opening the serial port: " << lastErr;
      THROW (IOException, ss.str().c_str());
    }
  }

  _is_open = true;
  reconfigurePort();
}

void
Serial::SerialImpl::reconfigurePort()
{
  /* Can only operate on a valid file descriptor. */
  if( _hSerial == INVALID_HANDLE_VALUE )
    THROW (IOException, "Invalid file descriptor, is the serial port open?");

  DCB dcbSerialParams = { 0 };
  dcbSerialParams.DCBlength = sizeof( dcbSerialParams );

  /* Check for error getting state. */
  if( !GetCommState( _hSerial, &dcbSerialParams ) )
    THROW (IOException, "Error getting the serial port state.");

  /* setup baud rate. */
  switch( _baudrate )
  {
#ifdef CBR_0
  case 0: dcbSerialParams.BaudRate = CBR_0; break;
#endif
#ifdef CBR_50
  case 50: dcbSerialParams.BaudRate = CBR_50; break;
#endif
#ifdef CBR_75
  case 75: dcbSerialParams.BaudRate = CBR_75; break;
#endif
#ifdef CBR_110
  case 110: dcbSerialParams.BaudRate = CBR_110; break;
#endif
#ifdef CBR_134
  case 134: dcbSerialParams.BaudRate = CBR_134; break;
#endif
#ifdef CBR_150
  case 150: dcbSerialParams.BaudRate = CBR_150; break;
#endif
#ifdef CBR_200
  case 200: dcbSerialParams.BaudRate = CBR_200; break;
#endif
#ifdef CBR_300
  case 300: dcbSerialParams.BaudRate = CBR_300; break;
#endif
#ifdef CBR_600
  case 600: dcbSerialParams.BaudRate = CBR_600; break;
#endif
#ifdef CBR_1200
  case 1200: dcbSerialParams.BaudRate = CBR_1200; break;
#endif
#ifdef CBR_1800
  case 1800: dcbSerialParams.BaudRate = CBR_1800; break;
#endif
#ifdef CBR_2400
  case 2400: dcbSerialParams.BaudRate = CBR_2400; break;
#endif
#ifdef CBR_4800
  case 4800: dcbSerialParams.BaudRate = CBR_4800; break;
#endif
#ifdef CBR_7200
  case 7200: dcbSerialParams.BaudRate = CBR_7200; break;
#endif
#ifdef CBR_9600
  case 9600: dcbSerialParams.BaudRate = CBR_9600; break;
#endif
#ifdef CBR_14400
  case 14400: dcbSerialParams.BaudRate = CBR_14400; break;
#endif
#ifdef CBR_19200
  case 19200: dcbSerialParams.BaudRate = CBR_19200; break;
#endif
#ifdef CBR_28800
  case 28800: dcbSerialParams.BaudRate = CBR_28800; break;
#endif
#ifdef CBR_57600
  case 57600: dcbSerialParams.BaudRate = CBR_57600; break;
#endif
#ifdef CBR_76800
  case 76800: dcbSerialParams.BaudRate = CBR_76800; break;
#endif
#ifdef CBR_38400
  case 38400: dcbSerialParams.BaudRate = CBR_38400; break;
#endif
#ifdef CBR_115200
  case 115200: dcbSerialParams.BaudRate = CBR_115200; break;
#endif
#ifdef CBR_128000
  case 128000: dcbSerialParams.BaudRate = CBR_128000; break;
#endif
#ifdef CBR_153600
  case 153600: dcbSerialParams.BaudRate = CBR_153600; break;
#endif
#ifdef CBR_230400
  case 230400: dcbSerialParams.BaudRate = CBR_230400; break;
#endif
#ifdef CBR_256000
  case 256000: dcbSerialParams.BaudRate = CBR_256000; break;
#endif
#ifdef CBR_460800
  case 460800: dcbSerialParams.BaudRate = CBR_460800; break;
#endif
#ifdef CBR_921600
  case 921600: dcbSerialParams.BaudRate = CBR_921600; break;
#endif
  default:
    /* Try to blindly assign it. */
    dcbSerialParams.BaudRate = _baudrate;
  }

  /* setup char len. */
  if( _bytesize == eightbits )
    dcbSerialParams.ByteSize = 8;
  else if( _bytesize == sevenbits )
    dcbSerialParams.ByteSize = 7;
  else if( _bytesize == sixbits )
    dcbSerialParams.ByteSize = 6;
  else if( _bytesize == fivebits )
    dcbSerialParams.ByteSize = 5;
  else
    throw invalid_argument( "Invalid byte size!" );

  /* setup stopbits. */
  if( _stopbits == stopbits_one )
    dcbSerialParams.StopBits = ONESTOPBIT;
  else if( _stopbits == stopbits_one_point_five )
    dcbSerialParams.StopBits = ONE5STOPBITS;
  else if( _stopbits == stopbits_two )
    dcbSerialParams.StopBits = TWOSTOPBITS;
  else
    throw invalid_argument( "Invalid stop bit!" );

  /* setup parity. */
  if( _parity == parity_none )
    dcbSerialParams.Parity = NOPARITY;
  else if( _parity == parity_even )
    dcbSerialParams.Parity = EVENPARITY;
  else if( _parity == parity_odd )
    dcbSerialParams.Parity = ODDPARITY;
  else if( _parity == parity_mark )
    dcbSerialParams.Parity = MARKPARITY;
  else if( _parity == parity_space )
    dcbSerialParams.Parity = SPACEPARITY;
  else
    throw invalid_argument( "Invalid parity!" );

  /* setup flowcontrol. */
  if( _flowcontrol == flowcontrol_none )
  {
    dcbSerialParams.fOutxCtsFlow = false;
    dcbSerialParams.fRtsControl = RTS_CONTROL_DISABLE;
    dcbSerialParams.fOutX = false;
    dcbSerialParams.fInX = false;
  }
  else if( _flowcontrol == flowcontrol_software )
  {
    dcbSerialParams.fOutxCtsFlow = false;
    dcbSerialParams.fRtsControl = RTS_CONTROL_DISABLE;
    dcbSerialParams.fOutX = true;
    dcbSerialParams.fInX = true;
  }
  else if( _flowcontrol == flowcontrol_hardware )
  {
    dcbSerialParams.fOutxCtsFlow = true;
    dcbSerialParams.fRtsControl = RTS_CONTROL_HANDSHAKE;
    dcbSerialParams.fOutX = false;
    dcbSerialParams.fInX = false;
  }
  else
     throw invalid_argument( "Invalid flow control!" );

  /*
   * Setting the DTR to Control_Enable ensures that the uController
   * is properly reset upon establishing a connection.
   */
  dcbSerialParams.fDtrControl = DTR_CONTROL_ENABLE;

  /* apply settings. */
  if( !SetCommState( _hSerial, &dcbSerialParams ) )
  {
    CloseHandle( _hSerial );
    THROW( IOException, "Error setting serial port parameters." );
  }

  /* Setup timeouts. */
  COMMTIMEOUTS timeouts = {0};
  timeouts.ReadIntervalTimeout = _timeout.inter_byte_timeout;
  timeouts.ReadTotalTimeoutConstant = _timeout.read_timeout_constant;
  timeouts.ReadTotalTimeoutMultiplier = _timeout.read_timeout_multiplier;
  timeouts.WriteTotalTimeoutConstant = _timeout.write_timeout_constant;
  timeouts.WriteTotalTimeoutMultiplier = _timeout.write_timeout_multiplier;
  if( !SetCommTimeouts( _hSerial, &timeouts ) )
    THROW( IOException, "Error setting timeouts." );

  purge();
  Sleep( 5 );
}

void
Serial::SerialImpl::close()
{
  if( _is_open )
  {
    if( _hSerial != INVALID_HANDLE_VALUE )
    {
      int ret( CloseHandle( _hSerial ) );
      if( ret == 0 )
      {
        stringstream ss;
        ss << "Error while closing serial port: " << GetLastError();
        THROW( IOException, ss.str().c_str() );
      }
      else
      {
        _hSerial = INVALID_HANDLE_VALUE;
      }
    }
    _is_open = false;
  }
}

const bool
Serial::SerialImpl::isOpen () const
{
  return _is_open;
}

const size_t
Serial::SerialImpl::available ()
{
  if( !_is_open )
    return 0;

  COMSTAT cs;
  if( !ClearCommError( _hSerial, NULL, &cs ) )
  {
    stringstream ss;
    ss << "Error while checking status of the serial port: " << GetLastError();
    THROW( IOException, ss.str().c_str() );
  }
  return static_cast< size_t >( cs.cbInQue );
}

const bool
Serial::SerialImpl::waitReadable( const uint32_t timeout )
{
  THROW( IOException, "waitReadable is not implemented on Windows." );
  return false;
}

void
Serial::SerialImpl::waitByteTimes( const size_t count )
{
  THROW( IOException, "waitByteTimes is not implemented on Windows." );
}

const size_t
Serial::SerialImpl::read( uint8_t *buf, const size_t size )
{
  if( !_is_open )
    throw PortNotOpenedException( "Serial::read" );

  DWORD bytes_read;
  if( !ReadFile( _hSerial, buf, static_cast< DWORD >( size ), &bytes_read, NULL ) )
  {
    stringstream ss;
    ss << "Error while reading from the serial port: " << GetLastError();
    THROW( IOException, ss.str().c_str() );
  }
  return ( size_t )( bytes_read );
}

const size_t
Serial::SerialImpl::write( const uint8_t *data, const size_t length )
{
  if( !_is_open )
    throw PortNotOpenedException( "Serial::write" );

  DWORD bytes_written;
  if( !WriteFile( _hSerial, data, static_cast< DWORD >( length ), &bytes_written, NULL ) )
  {
    stringstream ss;
    ss << "Error while writing to the serial port: " << GetLastError();
    THROW( IOException, ss.str().c_str() );
  }
  return ( size_t )( bytes_written );
}

void
Serial::SerialImpl::setPort( const string &port )
{
  _port = wstring( port.begin(), port.end() );
}

const string
Serial::SerialImpl::getPort() const
{
  return string( _port.begin(), _port.end() );
}

void
Serial::SerialImpl::setTimeout( const serial::Timeout& timeout )
{
  _timeout = timeout;
  if( _is_open )
    reconfigurePort();
}

const serial::Timeout
Serial::SerialImpl::getTimeout() const
{
  return _timeout;
}

void
Serial::SerialImpl::setBaudrate( const unsigned long baudrate )
{
  _baudrate = baudrate;
  if( _is_open )
    reconfigurePort();
}

const unsigned long
Serial::SerialImpl::getBaudrate() const
{
  return _baudrate;
}

void
Serial::SerialImpl::setBytesize( const serial::bytesize_t bytesize )
{
  _bytesize = bytesize;
  if( _is_open )
    reconfigurePort();
}

const serial::bytesize_t
Serial::SerialImpl::getBytesize () const
{
  return _bytesize;
}

void
Serial::SerialImpl::setParity( const serial::parity_t parity )
{
  _parity = parity;
  if( _is_open )
    reconfigurePort();
}

const serial::parity_t
Serial::SerialImpl::getParity() const
{
  return _parity;
}

void
Serial::SerialImpl::setStopbits( serial::stopbits_t stopbits )
{
  _stopbits = stopbits;
  if( _is_open )
    reconfigurePort();
}

const serial::stopbits_t
Serial::SerialImpl::getStopbits() const
{
  return _stopbits;
}

void
Serial::SerialImpl::setFlowcontrol( const serial::flowcontrol_t flowcontrol )
{
  _flowcontrol = flowcontrol;
  if( _is_open )
    reconfigurePort();
}

const serial::flowcontrol_t
Serial::SerialImpl::getFlowcontrol() const
{
  return _flowcontrol;
}

void
Serial::SerialImpl::flush ()
{
  if( !_is_open )
    throw PortNotOpenedException( "Serial::flush" );

  FlushFileBuffers( _hSerial );
}

void
Serial::SerialImpl::flushInput()
{
  THROW( IOException, "flushInput is not supported on Windows." );
}

void
Serial::SerialImpl::flushOutput()
{
  THROW( IOException, "flushOutput is not supported on Windows." );
}

void
Serial::SerialImpl::synchronize( ostream& out )
{
   if( !_is_open )
      throw PortNotOpenedException( "Serial::synchronize" );

   bool u( false );
   if( ( !available() ) && ( u = true ) )
      out << "Serial unavailable, please wait, purging serial buffer ";

   /* can't read while unavailable! */
   while( !available() )
   {
      flush();
      purge();
      close();

      Sleep( rand() % 100 + 250 );
      open();
      out << '.';
   }

   if( u )
      out << endl;
}

void
Serial::SerialImpl::purge()
{
   if( !_is_open )
      throw PortNotOpenedException( "Serial::purge" );

   PurgeComm( _hSerial,
      PURGE_TXABORT |
      PURGE_RXABORT |
      PURGE_TXCLEAR |
      PURGE_RXCLEAR );
}

void
Serial::SerialImpl::sendBreak ( const int duration )
{
  THROW( IOException, "sendBreak is not supported on Windows." );
}

void
Serial::SerialImpl::setBreak( const bool level )
{
  if( !_is_open  )
    throw PortNotOpenedException( "Serial::setBreak" );

  EscapeCommFunction( _hSerial, level? SETBREAK: CLRBREAK );
}

void
Serial::SerialImpl::setRTS( const bool level )
{
  if( !_is_open )
    throw PortNotOpenedException( "Serial::setRTS" );

  EscapeCommFunction( _hSerial, level? SETRTS: CLRRTS );
}

void
Serial::SerialImpl::setDTR( const bool level )
{
  if( !_is_open )
    throw PortNotOpenedException ("Serial::setDTR");

  EscapeCommFunction( _hSerial, level? SETDTR: CLRDTR );
}

const bool
Serial::SerialImpl::waitForChange()
{
  if( !_is_open )
    throw PortNotOpenedException ("Serial::waitForChange");

  DWORD dwCommEvent;

  /* Error setting communications mask? */
  if( !SetCommMask( _hSerial, EV_CTS | EV_DSR | EV_RING | EV_RLSD ) )
    return false;

  if( !WaitCommEvent( _hSerial, &dwCommEvent, NULL ) )
    /* An error occurred waiting for the event. */
    return false;
  else
    /* Event has occurred. */
    return true;
}

const bool
Serial::SerialImpl::getCTS() const
{
  if( !_is_open )
    throw PortNotOpenedException( "Serial::getCTS" );

  DWORD dwModemStatus;
  if( !GetCommModemStatus( _hSerial, &dwModemStatus ) )
    THROW( IOException, "Error getting the status of the CTS line." );

  return ( MS_CTS_ON & dwModemStatus ) != 0;
}

const bool
Serial::SerialImpl::getDSR() const
{
  if( !_is_open )
    throw PortNotOpenedException( "Serial::getDSR" );

  DWORD dwModemStatus;
  if( !GetCommModemStatus( _hSerial, &dwModemStatus ) )
    THROW( IOException, "Error getting the status of the DSR line." );

  return ( MS_DSR_ON & dwModemStatus ) != 0;
}

const bool
Serial::SerialImpl::getRI() const
{
  if( !_is_open )
    throw PortNotOpenedException( "Serial::getRI" );

  DWORD dwModemStatus;
  if( !GetCommModemStatus( _hSerial, &dwModemStatus ) )
    THROW( IOException, "Error getting the status of the RI line." );

  return ( MS_RING_ON & dwModemStatus ) != 0;
}

const bool
Serial::SerialImpl::getCD() const
{
  if( !_is_open )
    throw PortNotOpenedException( "Serial::getCD" );

  DWORD dwModemStatus;
  if( !GetCommModemStatus( _hSerial, &dwModemStatus ) )
    THROW( IOException, "Error getting the status of the CD line." );

  return ( MS_RLSD_ON & dwModemStatus ) != 0;
}

void
Serial::SerialImpl::readLock()
{
  if( WaitForSingleObject( _read_mutex, INFINITE ) != WAIT_OBJECT_0 )
    THROW( IOException, "Error claiming read mutex." );
}

void
Serial::SerialImpl::readUnlock()
{
  if( !ReleaseMutex( _read_mutex ) )
    THROW( IOException, "Error releasing read mutex." );
}

void
Serial::SerialImpl::writeLock()
{
  if( WaitForSingleObject( _write_mutex, INFINITE ) != WAIT_OBJECT_0 )
    THROW( IOException, "Error claiming write mutex." );
}

void
Serial::SerialImpl::writeUnlock()
{
  if( !ReleaseMutex( _write_mutex ) )
    THROW( IOException, "Error releasing write mutex." );
}

#endif // #if defined(_WIN32)

// EOF.