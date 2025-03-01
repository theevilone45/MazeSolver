# Declare SFML as an external dependency.
FetchContent_Declare(
    SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG        3.0.0
)

# Download and add SFML to your build.
FetchContent_MakeAvailable(SFML)
