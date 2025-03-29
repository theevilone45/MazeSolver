#ifndef INCLUDE_MAZE_MAZE_HPP
#define INCLUDE_MAZE_MAZE_HPP

#include <cstddef>

#include "common/types.hpp"
#include "monads/identity.hpp"

namespace maze::init {

auto makeCell(size_t column, size_t row) -> monads::Identity<common::Cell>;

auto addWall(common::Cell &&cell,
             common::WallDirection direction) -> monads::Identity<common::Cell>;

auto removeWall(common::Cell &&cell, common::WallDirection direction)
    -> monads::Identity<common::Cell>;

auto makeEmptyMaze(size_t width,
                   size_t height) -> monads::Identity<common::Maze>;

auto getMazeCellCount(const common::Maze &maze) -> size_t;

auto generateMaze(common::Maze &&maze) -> monads::Identity<common::Maze>;

} // namespace maze::logic

#endif
