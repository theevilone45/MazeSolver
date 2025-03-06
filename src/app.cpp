#include <chrono>

#include "maze/app.hpp"
#include "maze/config.hpp"
#include "maze/events.hpp"
#include "maze/maze.hpp"
#include "maze/monads/identity.hpp"
#include "maze/renderer.hpp"

namespace maze::app {

auto run() -> void {
    const auto &cfg = ConfigService::get();
    auto window = render::makeWindow();
    auto mazeMonad =
        logic::makeEmptyMaze(cfg.columnsCount, cfg.rowsCount)
        .bind(logic::generateMaze);
    auto drawableMazeMonad =
        render::makeEmptyMaze(mazeMonad.get())
        .bind(render::setWalls, mazeMonad.get())
        .bind(render::updateMazeState, mazeMonad.get());

}
// auto maze = logic::generateMaze(
//     logic::makeEmptyMaze(cfg.columnsCount, cfg.rowsCount));
// auto drawableMaze = render::updateMazeState(
//     render::setWalls(render::makeEmptyMaze(maze),
//         maze),
//     maze);

// using Clock = std::chrono::high_resolution_clock;
// while (window.isOpen()) {
//     const auto t1 = Clock::now();
//     window.handleEvents(events::getOnClose(window),
//                         events::getOnMousePress(maze));

//     window.clear();
//     drawableMaze = render::updateMazeState(std::move(drawableMaze), maze);
//     draw(drawableMaze, window);
//     window.display();
//     const auto t2 = Clock::now();
//     [[maybe_unused]] const auto dt =
//         std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1)
//             .count();
//     std::println("Render time {}[ms]", dt);
//     // std::cout << "Render time: " << dt << "[ms]\n";
} // namespace maze::app
