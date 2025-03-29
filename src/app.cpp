#include "maze/app.hpp"

#include <chrono>
#include <memory>
#include <expected>

#include "maze/common/types.hpp"
#include "maze/config.hpp"
#include "maze/debug.hpp"
#include "maze/events.hpp"
#include "maze/maze.hpp"
#include "maze/monads/identity.hpp"
#include "maze/renderer.hpp"
#include "maze/solvers/bfs.hpp"
#include "maze/solvers/dfs.hpp"
#include "maze/solvers/generic_solver.hpp"

namespace maze::app {

auto makeMazeMonad() -> monads::Identity<common::Maze> {
    const auto &cfg = ConfigService::get();
    return init::makeEmptyMaze(cfg.columnsCount, cfg.rowsCount)
        .bind(init::generateMaze)
        .bind(utils::changeCellStateInMaze, common::CellCoords{0, 0},
              common::CellState::BEGIN)
        .bind(utils::changeCellStateInMaze,
              common::CellCoords{cfg.columnsCount - 1, cfg.rowsCount - 1},
              common::CellState::END);
}

auto makeDrawableMazeMonad(const common::Maze &maze)
    -> monads::Identity<render::DrawableMaze> {
    return render::makeEmptyMaze(maze)
        .bind(render::setWalls, maze)
        .bind(render::updateMazeState, maze);
}

auto makeSolver() -> std::expected< std::unique_ptr<solvers::GenericSolver>, std::string> {
    const auto& cfg = ConfigService::get();
    switch(cfg.solver) {
        case maze::SolverType::DFS:
            return solvers::dfs::initSolver(common::CellCoords{0, 0});
        case maze::SolverType::BFS:
            return solvers::bfs::initSolver(common::CellCoords{0,0});
        default:
            return std::unexpected{"Not implemented yet"};
    }
}

auto run() -> void {
    using Clock = std::chrono::high_resolution_clock;

    auto window = render::makeWindow();
    auto mazeMonad = makeMazeMonad();
    auto drawableMazeMonad = makeDrawableMazeMonad(mazeMonad.get());
    auto solver = makeSolver();

    if(!solver) {
        LOG(std::format("Error: while initializing solver: {}", solver.error()));
        return;
    }

    while (window.isOpen()) {
        const auto t1 = Clock::now();

        window.handleEvents(events::getOnClose(window));
        window.clear();

        mazeMonad = mazeMonad.bind([&solver](common::Maze &&maze) {
            return (*solver)->getNextMazeState(std::move(maze));
        });
        drawableMazeMonad =
            drawableMazeMonad.bind(render::updateMazeState, mazeMonad.get());

        render::side_effects::draw(drawableMazeMonad.get(), window);
        window.display();

        const auto t2 = Clock::now();
        const auto dt =
            std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1)
                .count();
        LOG(std::format("render time {}[ms]", dt));
    }
}

} // namespace maze::app
