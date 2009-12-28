CMAKE_MINIMUM_REQUIRED(VERSION 2.4)

SET(OBJSOURCES	Main.cpp
		BMPRipper.cpp
		DDSRipper.cpp
		FileWin32.cpp
		GIFRipper.cpp
		ICORipper.cpp
		IFFRipper.cpp
		JPEGRipper.cpp
		MIDIRipper.cpp
		MODRipper.cpp
		MPEGRipper.cpp
		PatternMatcher.cpp
		PDFRipper.cpp
		PVRRipper.cpp
		RIFFRipper.cpp
		RTFRipper.cpp
		VOCRipper.cpp)

ADD_EXECUTABLE(ripper ${OBJSOURCES})
SET_TARGET_PROPERTIES(ripper PROPERTIES COMPILE_FLAGS "-ansi -Wall -Wextra -Werror -g -pipe")

