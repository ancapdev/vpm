
###
### Sources, headers, directories and libs
###
set(header_directory "include/yaml-cpp/")

file(GLOB sources "src/[a-zA-Z]*.cpp")
file(GLOB public_headers "include/yaml-cpp/[a-zA-Z]*.h")
file(GLOB private_headers "src/[a-zA-Z]*.h")

add_definitions(-DYAML_CPP_NO_CONTRIB)

include_directories(include)
include_directories(src)

###
### Library
###
add_library(yaml-cpp
	${sources}
	${public_headers}
	${private_headers}
)
