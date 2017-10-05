========================================================================
    CONSOLE APPLICATION : WeatherImport Project Overview
========================================================================

This file contains a summary of what you will find in each of the files 
that make up your WeatherImport application.

WeatherImport.vcproj
    This is the visual studio 2008 project file.
    It contains information about the version of Visual C++ that genera-
	ted the file, and information about the platforms, configurations,
	and project features selected.

main.cpp
    This is the main application launcher file.
	here we do parse the command line parameters to
		call in the main class.

WeatherImport.h
    This is the main application include file.

WeatherImport.cpp
    This is the main application source file.

/////////////////////////////////////////////////////////////////////////////
Other standard files:

StdAfx.h, StdAfx.cpp
    These files are used to build a precompiled header (PCH) file
    named WeatherImport.pch and a precompiled types file named StdAfx.obj.

/////////////////////////////////////////////////////////////////////////////

Project,
   Properties,
      Configuration Properties,
         C/C++, 
            Preprocessor, 
               Preprocessor definitions:

   WIN32;_WIN32;NDEBUG;_CONSOLE

   define USE_SERIAL_ENUM AT PROJECT LEVEL (preprocessor directive)
      to enable the serial port enumeration option.
      
   define USE_MOCK at file level WeeditImport.cpp ONLY)
      to enable the mockup test.
      
   Original library issues:
   https://github.com/wjwwood/serial/issues/
      
   Serial Library
   Sample from arduino, purge before using comm.
   http://playground.arduino.cc/Interfacing/CPPWindows
   
   complete sample.
   http://stackoverflow.com/questions/18216940/how-do-i-correctly-use-commtimeouts-with-overlapped-io-mode-reading-from-a-seria
   
   PurgeComm
   https://msdn.microsoft.com/en-us/library/windows/desktop/aa363428(v=vs.85).aspx