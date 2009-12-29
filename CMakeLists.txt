CMAKE_MINIMUM_REQUIRED(VERSION 2.4)

INCLUDE(CheckIncludeFileCXX)

check_include_file_cxx(unistd.h HAVE_UNISTD_H)
check_include_file_cxx(sys/time.h HAVE_SYS_TIME_H)
check_include_file_cxx(sys/fcntl.h HAVE_SYS_FCNTL_H)
check_include_file_cxx(sys/mman.h HAVE_SYS_MMAN_H)

SET(OBJSOURCES	Main.cpp
		File.cpp
		Time.cpp
		BMPRipper.cpp
		DDSRipper.cpp
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
SET_TARGET_PROPERTIES(ripper PROPERTIES COMPILE_FLAGS "-Wall -Wextra -Werror -g -pipe")
CONFIGURE_FILE(Config.h.in Config.h)

