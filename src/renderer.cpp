#include "maze/renderer.hpp"

#include "SFML/Graphics/Color.hpp"
#include "SFML/Graphics/Vertex.hpp"
#include "SFML/System/Vector2.hpp"

#include "maze/config.hpp"
#include "maze/maze.hpp"
#include "maze/monads/identity.hpp"

#include <cassert>
#include <cstddef>
#include <ranges>
#include <span>
#include <tuple>
#include <vector>

namespace maze::render {

/**
    Visual representation of cell vertices

       0 ______ 1/4
        |      |
        |      |
        |______|
      2/3       5

*/
using CellVertices = std::tuple<sf::Vertex, sf::Vertex, sf::Vertex, sf::Vertex,
                                sf::Vertex, sf::Vertex>;
/**
    Visual representation of walls vertices

     0/1 ______ 2/3
        |      |
        |      |
        |______|
     6/7        4/5

*/
using WallVertices = std::tuple<sf::Vertex, sf::Vertex, sf::Vertex, sf::Vertex,
                                sf::Vertex, sf::Vertex, sf::Vertex, sf::Vertex>;

namespace {

constexpr const size_t cWallVertexCount = 8;
constexpr const size_t cCellVertexCount = 6;

auto createCellVertices(float column, float row, float size,
                        sf::Color color) -> CellVertices {
    sf::Vertex a, b, c, d, e, f;
    a.position = {column * size, row * size};
    b.position = {(column + 1.f) * size, row * size};
    c.position = {column * size, (row + 1.f) * size};
    d.position = {column * size, (row + 1.f) * size};
    e.position = {(column + 1.f) * size, row * size};
    f.position = {(column + 1.f) * size, (row + 1.f) * size};
    a.color = color;
    b.color = color;
    c.color = color;
    d.color = color;
    e.color = color;
    f.color = color;
    return {a, b, c, d, e, f};
};

auto createWallVertices(float column, float row, float size,
                        sf::Color color) -> WallVertices {
    sf::Vertex a, b, c, d, e, f, g, h;
    a.position = h.position = {column * size, row * size};
    b.position = c.position = {(column + 1.f) * size, row * size};
    d.position = e.position = {(column + 1.f) * size, (row + 1.f) * size};
    f.position = g.position = {column * size, (row + 1.f) * size};

    a.color = color;
    b.color = color;
    c.color = color;
    d.color = color;
    e.color = color;
    f.color = color;
    g.color = color;
    h.color = color;

    return {a, b, c, d, e, f, g, h};
};

auto addCellVertices(std::vector<sf::Vertex> &&cells,
                     CellVertices &&vertices) -> std::vector<sf::Vertex> {
    cells.emplace_back(std::move(std::get<0>(vertices)));
    cells.emplace_back(std::move(std::get<1>(vertices)));
    cells.emplace_back(std::move(std::get<2>(vertices)));
    cells.emplace_back(std::move(std::get<3>(vertices)));
    cells.emplace_back(std::move(std::get<4>(vertices)));
    cells.emplace_back(std::move(std::get<5>(vertices)));

    return std::move(cells);
};

auto addWallVertices(std::vector<sf::Vertex> &&walls,
                     WallVertices &&vertices) -> std::vector<sf::Vertex> {
    walls.emplace_back(std::move(std::get<0>(vertices)));
    walls.emplace_back(std::move(std::get<1>(vertices)));
    walls.emplace_back(std::move(std::get<2>(vertices)));
    walls.emplace_back(std::move(std::get<3>(vertices)));
    walls.emplace_back(std::move(std::get<4>(vertices)));
    walls.emplace_back(std::move(std::get<5>(vertices)));
    walls.emplace_back(std::move(std::get<6>(vertices)));
    walls.emplace_back(std::move(std::get<7>(vertices)));
    return std::move(walls);
};

auto getStateColor(logic::CellState state) -> sf::Color {
    switch (state) {
    case logic::CellState::BEGIN:
        return sf::Color::Magenta;
    case logic::CellState::END:
        return sf::Color::Red;
    case logic::CellState::EMPTY:
        return sf::Color::Black;
    case logic::CellState::PATH:
        return sf::Color::Green;
    case logic::CellState::VISITED:
        return sf::Color::Yellow;
    case logic::CellState::NONE:
        return sf::Color::Transparent;
    };
    return sf::Color::Transparent;
}

} // namespace

auto makeWindow() -> sf::RenderWindow {
    const auto &cfg = ConfigService::get();
    const auto windowSize = sf::Vector2u{cfg.cellSize * (cfg.columnsCount + 1),
                                         cfg.cellSize * (cfg.rowsCount + 1)};
    return sf::RenderWindow(sf::VideoMode(windowSize), "MazeSolver");
}

auto makeEmptyMaze(const logic::Maze &maze) -> monads::Identity<DrawableMaze> {
    auto drawableMaze = DrawableMaze{};
    const auto &cfg = ConfigService::get();
    const auto cellSizeF = static_cast<float>(cfg.cellSize);
    const size_t cellsCount = getMazeCellCount(maze);
    drawableMaze.cells.reserve(cellsCount * cCellVertexCount);
    drawableMaze.walls.reserve(cellsCount * cWallVertexCount);
    for (const auto &cell : maze) {
        const auto columnF = static_cast<float>(cell.column);
        const auto rowF = static_cast<float>(cell.row);
        drawableMaze.cells = addCellVertices(
            std::move(drawableMaze.cells),
            createCellVertices(columnF, rowF, cellSizeF, sf::Color::Black));
        drawableMaze.walls =
            addWallVertices(std::move(drawableMaze.walls),
                            createWallVertices(columnF, rowF, cellSizeF,
                                               sf::Color::Transparent));
    }
    return monads::Identity(std::move(drawableMaze));
}

auto updateCellState(DrawableMaze &&drawableMaze, const logic::Cell &cell)
    -> monads::Identity<DrawableMaze> {
    const auto &cfg = ConfigService::get();
    const auto cellStartIdx =
        (cell.row * cfg.columnsCount + cell.column) * cCellVertexCount;

    auto cellVerticesView = std::span{drawableMaze.cells} |
                            std::views::drop(cellStartIdx) |
                            std::views::take(cCellVertexCount);
    for (auto &v : cellVerticesView) {
        v.color = getStateColor(cell.state);
    }

    return monads::Identity(std::move(drawableMaze));
};

auto updateMazeState(DrawableMaze &&drawableMaze, const logic::Maze &maze)
    -> monads::Identity<DrawableMaze> {
    auto intermediate = monads::Identity(std::move(drawableMaze));
    for (const auto &cell : maze) {
        intermediate.bind(updateCellState, cell);
    }
    return intermediate;
}

auto setWalls(DrawableMaze &&drawableMaze,
              const logic::Maze &maze) -> monads::Identity<DrawableMaze> {
    const auto &cfg = ConfigService::get();

    for (const auto &cell : maze) {
        const auto wallsStartIdx =
            (cell.row * cfg.columnsCount + cell.column) * cWallVertexCount;

        size_t idx = 0;
        // clang-format off
        auto wallVerticesView =
            std::span{drawableMaze.walls}
            | std::views::drop(wallsStartIdx)
            | std::views::take(cWallVertexCount)
            | std::views::chunk(2)
            | std::views::filter([&](const auto &) { return cell.walls[static_cast<logic::WallDirection>(idx++)]; })
            | std::views::join;
        // clang-format on

        for (auto &vertex : wallVerticesView) {
            vertex.color = sf::Color::White;
        }
    }

    return monads::Identity(std::move(drawableMaze));
}

namespace side_effects {

auto draw(const DrawableMaze &drawableMaze, sf::RenderWindow &window) -> void {
    const auto &cfg = ConfigService::get();
    const auto offset = static_cast<float>(cfg.cellSize) / 2.f;
    sf::Transform transform;
    transform.translate({offset, offset});
    window.draw(drawableMaze.cells.data(), drawableMaze.cells.size(),
                sf::PrimitiveType::Triangles, transform);
    window.draw(drawableMaze.walls.data(), drawableMaze.walls.size(),
                sf::PrimitiveType::Lines, transform);
}

} // namespace side_effects

} // namespace maze::render
