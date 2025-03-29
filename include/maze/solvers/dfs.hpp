#ifndef INCLUDE_MAZE_SOLVERS_DFS_HPP
#define INCLUDE_MAZE_SOLVERS_DFS_HPP

#include "../common/types.hpp"
#include "../monads/identity.hpp"
#include "generic_solver.hpp"

#include <memory>
#include <stack>

namespace maze::solvers::dfs {

class DfsSolver : public GenericSolver {
    public:
    auto getNextMazeState(common::Maze &&maze) -> monads::Identity<common::Maze> override;
    auto markSolution(common::Maze &&maze) -> monads::Identity<common::Maze> override;
    auto isSolved() const -> bool override;

private:
    std::stack<common::CellCoords> mCellStack;
    bool mSolved = false;

private:
    friend auto initSolver(const common::CellCoords &begin)
        -> std::unique_ptr<GenericSolver>;
};

auto initSolver(const common::CellCoords &begin)
    -> std::unique_ptr<GenericSolver>;

}; // namespace maze::solvers::dfs

#endif
