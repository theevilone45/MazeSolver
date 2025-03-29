#ifndef INCLUDE_MAZE_SOLVERS_BFS_HPP
#define INCLUDE_MAZE_SOLVERS_BFS_HPP

#include "../common/types.hpp"
#include "../monads/identity.hpp"
#include "generic_solver.hpp"

#include <queue>
#include <memory>
#include <unordered_map>

namespace maze::solvers::bfs {

class BfsSolver : public GenericSolver {
    public:
    auto getNextMazeState(common::Maze &&maze) -> monads::Identity<common::Maze> override;
    auto markSolution(common::Maze &&maze) -> monads::Identity<common::Maze> override;
    auto isSolved() const -> bool override;

    private:
    std::queue<common::CellCoords> mCellQueue;
    bool mSolved = false;
    common::CellCoords mStart;
    common::CellCoords mEnd;
    std::unordered_map<common::CellCoords, common::CellCoords> mParentMap;

    private:
    friend auto initSolver(const common::CellCoords &begin) -> std::unique_ptr<GenericSolver>;
};

auto initSolver(const common::CellCoords &begin) -> std::unique_ptr<GenericSolver>;

} // namespace maze::solvers::bfs

#endif
