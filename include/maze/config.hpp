#ifndef INCLUDE_MAZE_CONFIG_HPP
#define INCLUDE_MAZE_CONFIG_HPP

#include <cstdint>
#include <argparse/argparse.hpp>

namespace maze {

enum class SolverType {
    DFS,
    BFS,
    A_STAR,
    DIJKSTRA
};

struct Config {
    uint32_t cellSize;
    uint32_t columnsCount; // width
    uint32_t rowsCount;    // height
    uint32_t maxFps;
    SolverType solver;
};

auto getDefaultConfig() -> Config;
auto getCliConfig(int argc, char **argv) -> Config;

class ConfigService {
public:
    static auto provide(Config &&cfg) -> void;
    static auto get() -> const Config &;

private:
    inline static Config config {};
};

} // namespace maze

#endif
