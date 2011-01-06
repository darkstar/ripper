CMAKE_MINIMUM_REQUIRED(VERSION 2.4)

INCLUDE(CheckIncludeFileCXX)
INCLUDE(CheckSymbolExists)

check_include_file_cxx(unistd.h HAVE_UNISTD_H)
check_include_file_cxx(sys/time.h HAVE_SYS_TIME_H)
check_include_file_cxx(sys/fcntl.h HAVE_SYS_FCNTL_H)
check_include_file_cxx(sys/mman.h HAVE_SYS_MMAN_H)
check_symbol_exists(madvise sys/mman.h HAVE_MADVISE)

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
		VOCRipper.cpp
		XMRipper.cpp)

ADD_EXECUTABLE(ripper ${OBJSOURCES})
SET_DIRECTORY_PROPERTIES(PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES "RIPPED_*")

IF(NOT MSVC)
SET_TARGET_PROPERTIES(ripper PROPERTIES COMPILE_FLAGS "-Wall -Wabi -Woverloaded-virtual -Wextra -Werror -g -pipe")
ELSE(NOT MSVC)
SET_TARGET_PROPERTIES(ripper PROPERTIES COMPILE_FLAGS "/wd4996")
ENDIF(NOT MSVC)

CONFIGURE_FILE("${PROJECT_SOURCE_DIR}/Config.h.in" "${PROJECT_BINARY_DIR}/Config.h")

INCLUDE_DIRECTORIES("${PROJECT_BINARY_DIR}")

