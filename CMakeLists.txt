cmake_minimum_required( VERSION 2.4 )
if( COMMAND cmake_policy )
	cmake_policy( SET CMP0003 NEW )
endif( COMMAND cmake_policy )

include( CheckFunctionExists )
include( CheckCXXCompilerFlag )

project( WADEXT )

IF( NOT CMAKE_BUILD_TYPE )
	SET( CMAKE_BUILD_TYPE Debug CACHE STRING
		"Choose the type of build, options are: Debug Release RelWithDebInfo MinSizeRel."
		FORCE )
ENDIF( NOT CMAKE_BUILD_TYPE )

set( SSE_MATTERS NO )

if( CMAKE_COMPILER_IS_GNUCXX )
	set( GPROF 0 CACHE BOOL "Enable profiling with gprof for Debug and RelWithDebInfo build types." )
	set( PROFILE 0 CACHE INT "1=Generate profile coverage info, 2=Use it" )
endif( CMAKE_COMPILER_IS_GNUCXX )

find_package( ZLIB )

if( MSVC )
	# Eliminate unreferenced functions and data
	# Perform identical COMDAT folding
	set( REL_LINKER_FLAGS "/opt:ref /opt:icf /nodefaultlib:msvcrt" )

	# String pooling
	# Function-level linking
	# Disable run-time type information
	set( ALL_C_FLAGS "/GF /Gy /GR-" )
	
	# Avoid CRT DLL dependancies in release builds
	set( REL_C_FLAGS "/MT" )

	# Disable warnings for unsecure CRT functions from VC8+
	if( MSVC_VERSION GREATER 1399 )
		set( ALL_C_FLAGS "${ALL_C_FLAGS} /wd4996" )
	endif( MSVC_VERSION GREATER 1399 )
	
	# The CMake configurations set /GR and /MD by default, which conflict with our settings.
	string(REPLACE "/MD " " " CMAKE_CXX_FLAGS_RELEASE ${CMAKE_CXX_FLAGS_RELEASE} )
	string(REPLACE "/MD " " " CMAKE_CXX_FLAGS_MINSIZEREL ${CMAKE_CXX_FLAGS_MINSIZEREL} )
	string(REPLACE "/MD " " " CMAKE_CXX_FLAGS_RELWITHDEBINFO ${CMAKE_CXX_FLAGS_RELWITHDEBINFO} )
	string(REPLACE "/MD " " " CMAKE_C_FLAGS_RELEASE ${CMAKE_C_FLAGS_RELEASE} )
	string(REPLACE "/MD " " " CMAKE_C_FLAGS_MINSIZEREL ${CMAKE_C_FLAGS_MINSIZEREL} )
	string(REPLACE "/MD " " " CMAKE_C_FLAGS_RELWITHDEBINFO ${CMAKE_C_FLAGS_RELWITHDEBINFO} )
	string(REPLACE " /GR" " " CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} )
endif( MSVC )

if( CMAKE_COMPILER_IS_GNUCXX )
	set( ALL_C_FLAGS "${ALL_C_FLAGS} -ffast-math -pipe" )
	if( GPROF )
		set( ALL_C_FLAGS "${ALL_C_FLAGS} -pg -g" )
	else( GPROF )
		set( REL_C_FLAGS "${REL_C_FLAGS} -fomit-frame-pointer" )
	endif( GPROF )
	if( PROFILE EQUAL 1 )
		message( STATUS "Generating profile coverage information" )
		set( ALL_C_FLAGS "${ALL_C_FLAGS} -fprofile-generate" )
		set( PROF_LIB "gcov" )
	elseif( PROFILE EQUAL 2 )
		message( STATUS "Using profile coverage information" )
		set( ALL_C_FLAGS "${ALL_C_FLAGS} -fprofile-use" )
	endif( PROFILE EQUAL 1 )
endif( CMAKE_COMPILER_IS_GNUCXX )

#if( WIN32 AND "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang" )
#	set( ALL_C_FLAGS "${ALL_C_FLAGS} -Wno-deprecated-declarations -Wno-format" )
#endif( WIN32 AND "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang" )

if( ZLIB_FOUND )
	message( STATUS "Using system zlib" )
else( ZLIB_FOUND )
	message( STATUS "Using internal zlib" )
	add_subdirectory( zlib )
	set( ZLIB_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/zlib )
	set( ZLIB_LIBRARIES z )
	set( ZLIB_LIBRARY z )
endif( ZLIB_FOUND )

CHECK_FUNCTION_EXISTS( stricmp STRICMP_EXISTS )
if( NOT STRICMP_EXISTS )
	add_definitions( -Dstricmp=strcasecmp )
endif( NOT STRICMP_EXISTS )

CHECK_FUNCTION_EXISTS( strnicmp STRNICMP_EXISTS )
if( NOT STRNICMP_EXISTS )
	add_definitions( -Dstrnicmp=strncasecmp )
endif( NOT STRNICMP_EXISTS )

set( WADEXT_LIBS "${ZLIB_LIBRARIES}" )

option( DEF_STRIP_NODES "Strip nodes from extracted maps by default." NO )
if( DEF_STRIP_NODES )
	if ( MSVC )
		set( ALL_C_FLAGS "${ALL_C_FLAGS} /DDEFAULT_STRIP" )
	else ( MSVC )
		set( ALL_C_FLAGS "${ALL_C_FLAGS} -DDEFAULT_STRIP" )
	endif( MSVC )
endif()

set( HEADERS
	fileformat.h
	resourcefile.h
	tarray.h
	wadext.h
	wadman.h
	)

set( SOURCES
	convert.cpp
	fileformat.cpp
	main.cpp
	wadext.cpp
	wadfile.cpp
)

set( CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} ${REL_LINKER_FLAGS}" )
set( CMAKE_EXE_LINKER_FLAGS_MINSIZEREL "${CMAKE_EXE_LINKER_FLAGS_MINSIZEREL} ${REL_LINKER_FLAGS}" )
set( CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO "${CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO} ${REL_LINKER_FLAGS}" )

set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${ALL_C_FLAGS}" )
set( CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} ${REL_C_FLAGS}" )
set( CMAKE_C_FLAGS_MINSIZEREL "${CMAKE_C_FLAGS_MINSIZEREL} ${REL_C_FLAGS}" )
set( CMAKE_C_FLAGS_RELWITHDEBINFO "${CMAKE_C_FLAGS_RELWITHDEBINFO} ${REL_C_FLAGS}" )
set( CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} ${DEB_C_FLAGS} -D_DEBUG" )

set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${ALL_C_FLAGS}" )
set( CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} ${REL_C_FLAGS}" )
set( CMAKE_CXX_FLAGS_MINSIZEREL "${CMAKE_CXX_FLAGS_MINSIZEREL} ${REL_C_FLAGS}" )
set( CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} ${REL_C_FLAGS}" )
set( CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} ${DEB_C_FLAGS} -D_DEBUG" )

add_executable( wadext ${SOURCES} ${HEADERS} )
target_link_libraries( wadext ${WADEXT_LIBS} ${PROF_LIB} )
include_directories( "${ZLIB_INCLUDE_DIR}" )
