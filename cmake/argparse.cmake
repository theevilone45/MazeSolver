FetchContent_Declare(
    argparse
    GIT_REPOSITORY https://github.com/p-ranav/argparse.git
    GIT_TAG v3.1
)

FetchContent_MakeAvailable(argparse)

set_target_properties(argparse
    PROPERTIES
    POSITION_INDEPENDENT_CODE ON
)
