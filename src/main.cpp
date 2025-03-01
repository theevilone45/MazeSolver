#include "maze/config.hpp"
#include "maze/app.hpp"

int main() {
    maze::ConfigService::provide(maze::getDefaultConfig());
    maze::app::run();
}
