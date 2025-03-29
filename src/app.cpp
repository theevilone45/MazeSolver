#include "maze/app.hpp"

#include <chrono>

#include "maze/common/types.hpp"
#include "maze/config.hpp"
#include "maze/events.hpp"
#include "maze/maze.hpp"
#include "maze/monads/identity.hpp"
#include "maze/renderer.hpp"
#include "maze/solvers/dfs.hpp"

namespace maze::app {

auto run() -> void {

    const auto &cfg = ConfigService::get();
    auto window = render::makeWindow();

    // auto maze = logic::generateMaze(
    //     logic::makeEmptyMaze(cfg.columnsCount, cfg.rowsCount));
    // auto drawableMaze = render::updateMazeState(
    //     render::setWalls(render::makeEmptyMaze(maze),
    //         maze),
    //     maze);

    auto mazeMonad =
        init::makeEmptyMaze(cfg.columnsCount, cfg.rowsCount)
            .bind(init::generateMaze)
            .bind(utils::changeCellStateInMaze, common::CellCoords{0, 0},
                  common::CellState::BEGIN)
            .bind(utils::changeCellStateInMaze,
                  common::CellCoords{cfg.columnsCount - 1, cfg.rowsCount - 1},
                  common::CellState::END);

    auto drawableMazeMonad =
        render::makeEmptyMaze(mazeMonad.get())
            .bind(render::setWalls, mazeMonad.get())
            .bind(render::updateMazeState, mazeMonad.get());

    using Clock = std::chrono::high_resolution_clock;
    auto solver = solvers::dfs::initSolution(common::CellCoords{0, 0});
    while (window.isOpen()) {
        const auto t1 = Clock::now();
        window.handleEvents(events::getOnClose(window));

        window.clear();

        mazeMonad = mazeMonad.bind([&solver](common::Maze &&maze) {
            return solver.getNextMazeState(std::move(maze));
        });

        drawableMazeMonad =
            drawableMazeMonad.bind(render::updateMazeState, mazeMonad.get());
        //     // drawableMaze =
        //     render::updateMazeState(std::move(drawableMaze),
        //     // maze);
        // drawableMazeMonad =
        //     drawableMazeMonad.bind(render::updateMazeState, mazeMonad.get());
        render::side_effects::draw(drawableMazeMonad.get(), window);
        window.display();
        const auto t2 = Clock::now();
        [[maybe_unused]] const auto dt =
            std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1)
                .count();
        // std::println("Render time {}[ms]", dt);
        // std::cout << "Render time: " << dt << "[ms]\n";
    }
}

} // namespace maze::app
