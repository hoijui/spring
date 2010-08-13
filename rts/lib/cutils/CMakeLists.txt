### C Utils helper library
#

set(myName            "cutils")
set(myTarget          "${myName}")
set(myTargetStreflop  "${myName}-streflop")

set(mySources
	"SharedLibrary.c"
	"SimpleLog.c"
	"SSkirmishAISpecifier.cpp"
	"Util.c"
	)

SET(myLibs "")
IF    (NOT WIN32)
	SET(myLibs ${myLibs} dl)
ENDIF (NOT WIN32)

ADD_LIBRARY(${myTarget}         STATIC EXCLUDE_FROM_ALL ${mySources})
TARGET_LINK_LIBRARIES(${myTarget} ${myLibs})
SET_TARGET_PROPERTIES(${myTarget} PROPERTIES COMPILE_FLAGS "${PIC_FLAG}")

ADD_LIBRARY(${myTargetStreflop} STATIC EXCLUDE_FROM_ALL ${mySources})
TARGET_LINK_LIBRARIES(${myTargetStreflop} ${myLibs} streflop)
SET_TARGET_PROPERTIES(${myTargetStreflop} PROPERTIES COMPILE_FLAGS "-DUSING_STREFLOP")
SET_TARGET_PROPERTIES(${myTargetStreflop} PROPERTIES COMPILE_FLAGS "${PIC_FLAG}")
