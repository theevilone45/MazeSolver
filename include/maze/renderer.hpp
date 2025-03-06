#ifndef INCLUDE_MAZE_RENDERER_HPP
#define INCLUDE_MAZE_RENDERER_HPP

#include "SFML/Graphics/RenderWindow.hpp"
#include "SFML/Graphics/Vertex.hpp"

#include "maze.hpp"
#include "monads/identity.hpp"

#include <vector>

namespace maze::render {

struct DrawableMaze {
    std::vector<sf::Vertex> cells;
    std::vector<sf::Vertex> walls;
};

auto makeWindow() -> sf::RenderWindow;

auto makeEmptyMaze(const logic::Maze &maze) -> monads::Identity<DrawableMaze>;
auto updateMaze(DrawableMaze &&drawableMaze,
                const logic::Maze &maze) -> monads::Identity<DrawableMaze>;
auto updateCellState(DrawableMaze &&drawableMaze,
                     const logic::Cell &cell) -> monads::Identity<DrawableMaze>;
auto updateMazeState(DrawableMaze &&drawableMaze,
                     const logic::Maze &maze) -> monads::Identity<DrawableMaze>;
auto setWalls(DrawableMaze &&drawableMaze,
              const logic::Maze &maze) -> monads::Identity<DrawableMaze>;

namespace side_effects {
    auto draw(const DrawableMaze &drawableMaze, sf::RenderWindow &window) -> void;
} // namespace side_effects

} // namespace maze::render

#endif
