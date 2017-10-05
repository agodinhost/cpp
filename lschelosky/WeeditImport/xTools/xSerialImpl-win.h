/*!
** \file    xSerialImpl-win.h
** \brief   Windows implementation of the Serial class interface.
** \license Open Source, The MIT License.
** \author  William Woodall <wjwwood@gmail.com>
** \author  John Harrison   <ash.gti@gmail.com>
** \version 0.1
** \review  A.Godinho (Woody)
** \date    2017/02/21 08:00
**/

#if defined(_WIN32)

#ifndef SERIAL_IMPL_WINDOWS_H
#define SERIAL_IMPL_WINDOWS_H

#include "xSerial.h"

#include "windows.h"

namespace serial {

using std::string;
using std::wstring;
using std::invalid_argument;

using serial::SerialException;
using serial::IOException;

class serial::Serial::SerialImpl
{
public:

  SerialImpl(
    const string          &port,
    const unsigned long   baudrate,
    const bytesize_t      bytesize,
    const parity_t        parity,
    const stopbits_t      stopbits,
    const flowcontrol_t   flowcontrol
  );

  virtual ~SerialImpl ();

  void
    open();

  void
    close();

  const bool
    isOpen() const;

  const size_t
    available();
  
  const bool
    waitReadable( const uint32_t timeout );

  void
    waitByteTimes( const size_t count );

  const size_t
    read( uint8_t *buf, const size_t size = 1 );

  const size_t
    write( const uint8_t *data, const size_t length );

  void
    flush();

  void
    flushInput();

  void
    flushOutput();

  void
    synchronize( ostream& out );

  void
    purge();

  void
    sendBreak( const int duration );

  void
    setBreak( const bool level );

  void
    setRTS( const bool level );

  void
    setDTR( const bool level );

  const bool
    waitForChange();

  const bool
    getCTS() const;

  const bool
    getDSR() const;

  const bool
    getRI() const;

  const bool
    getCD() const;

  void
    setPort( const string &port );

  const string
    getPort() const;

  void
    setTimeout( const Timeout& timeout );

  const Timeout
    getTimeout() const;

  void
    setBaudrate( const unsigned long baudrate );

  const unsigned long
    getBaudrate() const;

  void
    setBytesize( const bytesize_t bytesize );

  const bytesize_t
    getBytesize () const;

  void
    setParity( const parity_t parity );

  const parity_t
    getParity() const;

  void
    setStopbits( const stopbits_t stopbits );

  const stopbits_t
    getStopbits() const;

  void
    setFlowcontrol( const flowcontrol_t flowcontrol );

  const flowcontrol_t
    getFlowcontrol() const;

  void
    readLock();

  void
    readUnlock();

  void
    writeLock();

  void
    writeUnlock();

protected:
  void reconfigurePort ();

private:
  wstring         _port;         /* Path to the file descriptor. */
  HANDLE          _hSerial;      /* Serial port handler. */
  bool            _is_open;

  unsigned long   _baudrate;     /* Baudrate. */
  parity_t        _parity;       /* Parity. */
  bytesize_t      _bytesize;     /* Size of the bytes. */
  stopbits_t      _stopbits;     /* Stop Bits. */
  flowcontrol_t   _flowcontrol;  /* Flow Control. */
  Timeout         _timeout;      /* Timeout for read operations. */

  HANDLE          _read_mutex;   /* Mutex to lock the read functions. */
  HANDLE          _write_mutex;  /* Mutex to lock the write functions. */
};

}

#endif // SERIAL_IMPL_WINDOWS_H

#endif // if defined(_WIN32)

// EOF.