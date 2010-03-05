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

ADD_LIBRARY(${myTarget}         STATIC EXCLUDE_FROM_ALL ${mySources})
SET_TARGET_PROPERTIES(${myTarget} PROPERTIES COMPILE_FLAGS "${PIC_FLAG}")

ADD_LIBRARY(${myTargetStreflop} STATIC EXCLUDE_FROM_ALL ${mySources})
TARGET_LINK_LIBRARIES(${myTargetStreflop} streflop)
SET_TARGET_PROPERTIES(${myTargetStreflop} PROPERTIES COMPILE_FLAGS "-DUSING_STREFLOP")
SET_TARGET_PROPERTIES(${myTargetStreflop} PROPERTIES COMPILE_FLAGS "${PIC_FLAG}")
