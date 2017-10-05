/*!
** \file    xSerial.h
** \brief   Cross platform interface for interacting with Serial Ports.
** \author  William Woodall <wjwwood@gmail.com>
** \author  John Harrison   <ash.gti@gmail.com>
**          http://wjwwood.io/serial/
** \version 0.1
**
** \review  A.Godinho (Woody)
** \date    2017/02/21 08:00
**
** Copyright (c) 2014 Craig Lilley <cralilley@gmail.com>
** This software is made available under the terms of the MIT license.
** A copy of the license can be obtained from: http://opensource.org/licenses/MIT
**/

#include <algorithm>

#if !defined( _WIN32 ) && !defined( __OpenBSD__ ) && !defined( __FreeBSD__ )
#include <alloca.h>
#endif

#if defined( __MINGW32__ )
#define alloca __builtin_alloca
#endif

#include "xSerial.h"

#if defined( _WIN32 )
#include "xSerialImpl-win.h"
#else
#include "xSerialImpl-unix.h"
#endif

using std::invalid_argument;
using std::min;
using std::numeric_limits;
using std::vector;
using std::size_t;
using std::string;

using serial::Serial;
using serial::SerialException;
using serial::IOException;
using serial::bytesize_t;
using serial::parity_t;
using serial::stopbits_t;
using serial::flowcontrol_t;

/* disable 'strncopy' unsafe warning. */
#pragma warning( disable : 4996 )

class Serial::ScopedReadLock {
public:
  ScopedReadLock( SerialImpl *pimpl ) : _pimpl( pimpl )
  {
    this->_pimpl->readLock();
  }
  ~ScopedReadLock()
  {
    this->_pimpl->readUnlock();
  }

private:
  /* Disable copy constructors. */
  ScopedReadLock( const ScopedReadLock& );
  const ScopedReadLock& operator = (ScopedReadLock);

  SerialImpl *_pimpl;
};

class Serial::ScopedWriteLock {
public:
  ScopedWriteLock( SerialImpl *pimpl ) : _pimpl( pimpl )
  {
    this->_pimpl->writeLock();
  }
  ~ScopedWriteLock() {
    this->_pimpl->writeUnlock();
  }

private:
  /* Disable copy constructors. */
  ScopedWriteLock( const ScopedWriteLock& );
  const ScopedWriteLock& operator = (ScopedWriteLock);

  SerialImpl *_pimpl;
};

Serial::Serial(
  const string&          port,
  const uint32_t         baudrate,
  const serial::Timeout& timeout,
  const bytesize_t       bytesize,
  const parity_t         parity,
  const stopbits_t       stopbits,
  const flowcontrol_t    flowcontrol
):
  _pimpl( new SerialImpl( 
     port, baudrate, 
     bytesize, parity, stopbits, flowcontrol
  ))
{
  _pimpl->setTimeout( timeout );
}

Serial::~Serial()
{
  delete _pimpl;
}

void
Serial::open()
{
  _pimpl->open();
}

void
Serial::close()
{
  _pimpl->close();
}

const bool
Serial::isOpen() const
{
  return _pimpl->isOpen();
}

const size_t
Serial::available()
{
  return _pimpl->available();
}

const bool
Serial::waitReadable()
{
  serial::Timeout timeout( _pimpl->getTimeout() );
  return _pimpl->waitReadable( timeout.read_timeout_constant );
}

void
Serial::waitByteTimes( const size_t count )
{
  _pimpl->waitByteTimes( count );
}

const size_t
Serial::_read( uint8_t *buffer, const size_t size )
{
  return this->_pimpl->read( buffer, size );
}

const size_t
Serial::read( uint8_t *buffer, const size_t size )
{
  ScopedReadLock lock( this->_pimpl );
  return this->_pimpl->read( buffer, size );
}

const size_t
Serial::read( vector< uint8_t > &buffer, const size_t size )
{
  ScopedReadLock lock( this->_pimpl );
  uint8_t *buffer_( new uint8_t[ size ] );
  const size_t bytes_read( this->_pimpl->read( buffer_, size ) );
  buffer.insert( buffer.end (), buffer_, buffer_ + bytes_read );
  delete[] buffer_;
  return bytes_read;
}

const size_t
Serial::read ( string &buffer, const size_t size )
{
  ScopedReadLock lock( this->_pimpl );
  uint8_t *buffer_( new uint8_t[ size ] );
  const size_t bytes_read( this->_pimpl->read( buffer_, size ) );
  buffer.append( reinterpret_cast< const char* >( buffer_ ), bytes_read );
  delete[] buffer_;
  return bytes_read;
}

const string
Serial::read( const size_t size )
{
  string buffer;
  this->read( buffer, size );
  return buffer;
}

const size_t
Serial::readline( string &buffer, const size_t size, const string &eol )
{
  ScopedReadLock lock( this->_pimpl );
  const size_t eol_len( eol.length() );
  uint8_t *buffer_( static_cast< uint8_t* >
    ( alloca( size * sizeof( uint8_t ) ) ) );
  size_t read_so_far( 0 );
  while( true )
  {
    const size_t bytes_read( this->_read( buffer_ + read_so_far, 1 ) );
    read_so_far += bytes_read;
    if( bytes_read == 0 )
    {
      break; /* Timeout occured on reading 1 byte. */
    }
    if( string( reinterpret_cast< const char* >
    ( buffer_ + read_so_far - eol_len ), eol_len ) == eol )
    {
      break; /* EOL found. */
    }
    if( read_so_far == size )
    {
      break; /* Reached the maximum read length. */
    }
  }
  buffer.append( reinterpret_cast< const char* >( buffer_), read_so_far );
  return read_so_far;
}

const string
Serial::readline( const size_t size, const string &eol )
{
  string buffer;
  this->readline( buffer, size, eol );
  return buffer;
}

vector< string >
Serial::readlines( const size_t size, const string &eol )
{
  ScopedReadLock lock( this->_pimpl );
  vector< string > lines;
  const size_t eol_len( eol.length() );
  uint8_t *buffer_( static_cast< uint8_t* >
    (alloca( size * sizeof( uint8_t ) ) ) );
  size_t read_so_far( 0 );
  size_t start_of_line( 0 );
  while( read_so_far < size )
  {
    const size_t bytes_read( this->_read( buffer_+read_so_far, 1 ) );
    read_so_far += bytes_read;
    if( bytes_read == 0 )
    {
      if( start_of_line != read_so_far )
      {
        lines.push_back(
          string( reinterpret_cast< const char* >( buffer_ + start_of_line ),
            read_so_far - start_of_line ) );
      }
      break; /* Timeout occured on reading 1 byte. */
    }
    if( string( reinterpret_cast< const char* >
    ( buffer_ + read_so_far - eol_len ), eol_len ) == eol )
    {
      /* EOL found. */
      lines.push_back(
        string( reinterpret_cast< const char* >( buffer_ + start_of_line ),
          read_so_far - start_of_line ) );
      start_of_line = read_so_far;
    }
    if( read_so_far == size )
    {
      if( start_of_line != read_so_far )
      {
        lines.push_back(
          string( reinterpret_cast< const char* >( buffer_ + start_of_line ),
            read_so_far - start_of_line ) );
      }
      break; /* Reached the maximum read length. */
    }
  }
  return lines;
}

const size_t
Serial::write( const string &data )
{
  ScopedWriteLock lock( this->_pimpl );
  return this->_write( reinterpret_cast< const uint8_t* >
    ( data.c_str() ), data.length() );
}

const size_t
Serial::write( const vector< uint8_t > &data )
{
  ScopedWriteLock lock( this->_pimpl );
  return this->_write( &data[0], data.size() );
}

const size_t
Serial::write( const uint8_t *data, const size_t size )
{
  ScopedWriteLock lock( this->_pimpl );
  return this->_write( data, size );
}

const size_t
Serial::_write( const uint8_t *data, const size_t length )
{
  return _pimpl->write( data, length );
}

void
Serial::setPort( const string &port )
{
  ScopedReadLock rlock( this->_pimpl );
  ScopedWriteLock wlock( this->_pimpl );
  const bool was_open( _pimpl->isOpen() );
  if( was_open ) close();
  _pimpl->setPort( port );
  if( was_open ) open();
}

const string
Serial::getPort() const
{
  return _pimpl->getPort ();
}

void
Serial::setTimeout( const serial::Timeout& timeout )
{
  _pimpl->setTimeout( timeout );
}

const serial::Timeout
Serial::getTimeout() const {
  return _pimpl->getTimeout ();
}

void
Serial::setBaudrate( const uint32_t baudrate )
{
  _pimpl->setBaudrate( baudrate );
}

const uint32_t
Serial::getBaudrate() const
{
  return uint32_t( _pimpl->getBaudrate () );
}

void
Serial::setBytesize( const bytesize_t bytesize )
{
  _pimpl->setBytesize( bytesize );
}

const bytesize_t
Serial::getBytesize() const
{
  return _pimpl->getBytesize();
}

void
Serial::setParity( const parity_t parity )
{
  _pimpl->setParity( parity );
}

const parity_t
Serial::getParity() const
{
  return _pimpl->getParity();
}

void
Serial::setStopbits( const stopbits_t stopbits )
{
  _pimpl->setStopbits( stopbits );
}

const stopbits_t
Serial::getStopbits() const
{
  return _pimpl->getStopbits();
}

void
Serial::setFlowcontrol( const flowcontrol_t flowcontrol )
{
  _pimpl->setFlowcontrol( flowcontrol );
}

const flowcontrol_t
Serial::getFlowcontrol() const
{
  return _pimpl->getFlowcontrol();
}

void Serial::flush ()
{
  ScopedReadLock rlock( this->_pimpl );
  ScopedWriteLock wlock( this->_pimpl );
  _pimpl->flush();
}

void Serial::flushInput()
{
  ScopedReadLock lock( this->_pimpl );
  _pimpl->flushInput();
}

void Serial::flushOutput()
{
  ScopedWriteLock lock( this->_pimpl );
  _pimpl->flushOutput();
}

void
Serial::synchronize( ostream& out )
{
   ScopedReadLock rlock( this->_pimpl );
   ScopedWriteLock lock( this->_pimpl );
   _pimpl->synchronize( out );
}

void
Serial::purge()
{
   ScopedReadLock rlock( this->_pimpl );
   ScopedWriteLock lock( this->_pimpl );
   _pimpl->purge();
}

void
Serial::dump( ostream& out, const int COUNT )
{
   for( int i = 0; i < COUNT; i ++ )
   {
      const string SS( this->read( 1 ) );
      if( !SS.empty() )
      {
         const char CC( SS.at( 0 ) );
         const int  CI( (int) CC );
         string CO;
         if( CI > 31 && CI < 128 )
            CO = CC;
         else
            CO = "***";

         char buf[ 32 ];
         sprintf( buf, "%02d - [%02X] = '%s'", i, CI, CO.c_str() );
         string line( buf );

         out << line << '\n';
      }
   }
}

void
Serial::sendBreak( const int duration )
{
  _pimpl->sendBreak( duration );
}

void
Serial::setBreak( const bool level )
{
  _pimpl->setBreak( level );
}

void
Serial::setRTS( const bool level )
{
  _pimpl->setRTS( level );
}

void
Serial::setDTR( const bool level )
{
  _pimpl->setDTR( level );
}

const bool
Serial::waitForChange()
{
  return _pimpl->waitForChange();
}

const bool
Serial::getCTS() const
{
  return _pimpl->getCTS();
}

const bool
Serial::getDSR() const
{
  return _pimpl->getDSR();
}

const bool
Serial::getRI() const
{
  return _pimpl->getRI();
}

const bool
Serial::getCD() const
{
  return _pimpl->getCD();
}

// EOF.