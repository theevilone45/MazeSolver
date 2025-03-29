#ifndef INCLUDE_MAZE_SOLVERS_DFS_HPP
#define INCLUDE_MAZE_SOLVERS_DFS_HPP

#include "../common/types.hpp"
#include "../monads/identity.hpp"

#include <stack>

namespace maze::solvers::dfs {

class DfsSolution {
    public:
    auto getNextMazeState(common::Maze &&maze) -> monads::Identity<common::Maze>;
    auto markSolution(common::Maze &&maze) -> monads::Identity<common::Maze>;
    auto isSolved() const -> bool;

private:
    std::stack<common::CellCoords> mCellStack;
    bool mSolved = false;

private:
    friend auto initSolution(const common::CellCoords &begin)
        -> DfsSolution;
};

auto initSolution(const common::CellCoords &begin)
    -> DfsSolution;

}; // namespace maze::solvers::dfs

#endif
