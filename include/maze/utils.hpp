#ifndef INCLUDE_MAZE_IDENTITY_HPP
#define INCLUDE_MAZE_IDENTITY_HPP

#include "common/types.hpp"
#include "monads/identity.hpp"

#include <functional>
#include <optional>

namespace maze::utils {

// template <typename F>
// concept Predicate =
//     std::invocable<F, const common::Maze &, const common::CellCoords &, const
//     common::CellCoords&,
//                    common::WallDirection> &&
//     std::same_as<
//         std::invoke_result_t<F, const common::Maze &,
//                              const common::CellCoords &, const
//                              common::CellCoords&, common::WallDirection>,
//         bool>;

using Predicate =
    std::function<bool(const common::Maze &, const common::CellCoords &,
                       const common::CellCoords &, common::WallDirection)>;

auto isWallPresent(const common::Maze &maze, const common::CellCoords &first,
                   common::WallDirection direction) -> bool;
auto getOpositeDirection(const common::WallDirection &direction)
    -> common::WallDirection;
auto getNextCellCoords(
    const common::CellCoords &current, const common::Maze &maze,
    common::WallDirection direction) -> std::optional<common::CellCoords>;

auto reserveNeighbors(std::vector<common::Neighbor> &&neighbors, size_t amount)
    -> monads::Identity<std::vector<common::Neighbor>>;

auto addNeighborIf(common::NeighborCollection &&neighbors,
                   const common::Maze &maze, common::WallDirection direction,
                   const common::CellCoords &current, Predicate predicate)
    -> monads::Identity<common::NeighborCollection>;

auto changeCellStateInMaze(
    common::Maze &&maze, const common::CellCoords &coords,
    common::CellState state) -> monads::Identity<common::Maze>;

auto changeState(common::Cell &&cell,
                 common::CellState state) -> monads::Identity<common::Cell>;

} // namespace maze::utils

#endif
