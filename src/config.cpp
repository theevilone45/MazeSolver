#include "maze/config.hpp"
#include "argparse/argparse.hpp"

#include <algorithm>
#include <iostream>
#include <iterator>
#include <unordered_map>
#include <utility>

namespace maze {

namespace {

constexpr const uint32_t cDefaultCellSize{20};
constexpr const uint32_t cDefaultColumnCount{1920 / cDefaultCellSize};
constexpr const uint32_t cDefaultRowsCount{1080 / cDefaultCellSize};
constexpr const uint32_t cDefaultMaxFps{60};
constexpr const SolverType cDefaultSolver{SolverType::DFS};

constexpr const char *cCellSizeArgName = "--size";
constexpr const char *cColumnCountArgName = "--columns";
constexpr const char *cRowCountArgName = "--rows";
constexpr const char *cMaxFpsArgName = "--fps";
constexpr const char *cSolverArgName = "--solver";

const std::unordered_map<std::string, SolverType> cSolverStringMap = {
    {"dfs", SolverType::DFS},
    {"bfs", SolverType::BFS},
    {"a*", SolverType::A_STAR},
    {"dijkstra", SolverType::DIJKSTRA},
};

} // namespace

std::ostream &operator<<(std::ostream &os, const Config &config) {
    os << std::format("size={}, columns={}, rows={}, fps={}, solver={}",
                      config.cellSize, config.columnsCount, config.rowsCount,
                      config.maxFps, std::to_underlying(config.solver));
    return os;
}

auto getDefaultConfig() -> Config {
    return Config{.cellSize = cDefaultCellSize,
                  .columnsCount = cDefaultColumnCount,
                  .rowsCount = cDefaultRowsCount,
                  .maxFps = cDefaultMaxFps,
                  .solver = cDefaultSolver};
}
auto getCliConfig(int argc, char **argv) -> Config {
    auto config = Config();
    argparse::ArgumentParser parser("Maze Solver");
    parser.add_argument("-s", cCellSizeArgName)
        .help("setup cell size")
        .default_value(cDefaultCellSize)
        .store_into(config.cellSize);

    parser.add_argument("-c", cColumnCountArgName)
        .help("setup columns count")
        .default_value(cDefaultColumnCount)
        .store_into(config.columnsCount);

    parser.add_argument("-r", cRowCountArgName)
        .help("setup rows count")
        .default_value(cDefaultRowsCount)
        .store_into(config.rowsCount);

    parser.add_argument("-f", cMaxFpsArgName)
        .help("setup max fps")
        .default_value(cDefaultMaxFps)
        .store_into(config.maxFps);

    std::string solverStr;
    parser.add_argument("-t", cSolverArgName)
        .help("setup solver")
        .default_value("dfs")
        .choices("dfs", "bfs", "a*", "dijkstra")
        .store_into(solverStr);

    parser.parse_args(argc, argv);

    std::cout << "config: " << config;
    config.solver = cSolverStringMap.at(solverStr);

    return config; // for now
}

auto ConfigService::provide(Config &&cfg) -> void { config = std::move(cfg); }
auto ConfigService::get() -> const Config & { return config; }

} // namespace maze
