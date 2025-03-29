#include "maze/config.hpp"
#include "maze/app.hpp"

int main(int argc, char** argv) {
    maze::ConfigService::provide(maze::getCliConfig(argc, argv));
    maze::app::run();
}
