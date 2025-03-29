#ifndef INCLUDE_MAZE_SOLVERS_GENERIC_SOLVER_HPP
#define INCLUDE_MAZE_SOLVERS_GENERIC_SOLVER_HPP

#include "../common/types.hpp"
#include "../monads/identity.hpp"

namespace maze::solvers {

class GenericSolver {
public:
    virtual auto
    getNextMazeState(common::Maze &&maze) -> monads::Identity<common::Maze> = 0;
    virtual auto
    markSolution(common::Maze &&maze) -> monads::Identity<common::Maze> = 0;
    virtual auto isSolved() const -> bool = 0;

    virtual ~GenericSolver() {};
};

} // namespace maze::solvers

#endif
