#include "maze/config.hpp"

#include <utility>

namespace maze {

namespace {

constexpr const uint32_t cDefaultCellSize{20};
constexpr const uint32_t cDefaultColumnCount{1920/cDefaultCellSize};
constexpr const uint32_t cDefaultRowsCount{1080/cDefaultCellSize};
constexpr const uint32_t cDefaultMaxFps{120};

} // namespace

auto getDefaultConfig() -> Config {
    return Config{.cellSize = cDefaultCellSize,
                  .columnsCount = cDefaultColumnCount,
                  .rowsCount = cDefaultRowsCount,
                  .maxFps = cDefaultMaxFps};
}
auto getCliConfig(int, char **) -> Config {
    return getDefaultConfig(); // for now
}

auto ConfigService::provide(Config &&cfg) -> void {
    config = std::move(cfg);
}
auto ConfigService::get() -> const Config & {
    return config;
}

} // namespace maze
