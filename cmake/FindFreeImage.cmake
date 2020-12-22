# - Try to find FreeImage
# Once done, this will define
#
# FreeImage_FOUND - system has FreeImage
# FreeImage_INCLUDE_DIR - the FreeImage include directories
# FreeImage_LIBRARIES - link these to use FreeImage
FIND_PATH( FreeImage_INCLUDE_DIR FreeImage/FreeImage.h
	/usr/include
	/usr/local/include
	/opt/local/include
	"${CMAKE_SOURCE_DIR}/include"
)
FIND_LIBRARY( FreeImage_LIBRARY FreeImage
	/usr/lib64
	/usr/lib
	/usr/local/lib
	/opt/local/lib
	"${CMAKE_SOURCE_DIR}/libraries"
)

IF(FreeImage_INCLUDE_DIR AND FreeImage_LIBRARY)
	SET( FreeImage_FOUND TRUE )
	SET( FreeImage_LIBRARIES ${FreeImage_LIBRARY} )
ENDIF(FreeImage_INCLUDE_DIR AND FreeImage_LIBRARY)
IF(FreeImage_FOUND)
	IF(NOT FreeImage_FIND_QUIETLY)
	MESSAGE(STATUS "Found FreeImage: ${FreeImage_LIBRARY}")
	ENDIF(NOT FreeImage_FIND_QUIETLY)
ELSE(FreeImage_FOUND)
	IF(FreeImage_FIND_REQUIRED)
	MESSAGE(FATAL_ERROR "Could not find libFreeImage")
	ENDIF(FreeImage_FIND_REQUIRED)
ENDIF(FreeImage_FOUND)
