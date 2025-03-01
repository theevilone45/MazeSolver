#ifndef INCLUDE_MAZE_RENDERER_HPP
#define INCLUDE_MAZE_RENDERER_HPP

#include "SFML/Graphics/RenderWindow.hpp"
#include "SFML/Graphics/Vertex.hpp"

#include "maze/maze.hpp"

#include <vector>

namespace maze::render {

struct DrawableMaze {
    std::vector<sf::Vertex> cells;
    std::vector<sf::Vertex> walls;
};

auto makeWindow() -> sf::RenderWindow;
auto makeEmptyMaze(DrawableMaze &&drawableMaze,
                   const logic::Maze &maze) -> DrawableMaze;
auto updateMaze(DrawableMaze &&drawableMaze,
                const logic::Maze &maze) -> DrawableMaze;
auto updateCellState(DrawableMaze &&drawableMaze,
                     const logic::Cell &cell) -> DrawableMaze;
auto updateMazeState(DrawableMaze &&drawableMaze, const logic::Maze& maze) -> DrawableMaze;
auto setWalls(DrawableMaze &&drawableMaze,
              const logic::Maze &maze) -> DrawableMaze;
auto draw(const DrawableMaze &drawableMaze, sf::RenderWindow &window) -> void;
auto handleEvents() -> void;

} // namespace maze::render

#endif
