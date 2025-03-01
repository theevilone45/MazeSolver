#ifndef INCLUDE_MAZE_EVENTS_HPP
#define INCLUDE_MAZE_EVENTS_HPP

#include <print>

#include "maze.hpp"
#include "maze/config.hpp"

#include "SFML/Graphics/Transform.hpp"
#include "SFML/Graphics/RenderWindow.hpp"
#include "SFML/System/Vector2.hpp"
#include "SFML/Window/Event.hpp"
#include "SFML/Window/Mouse.hpp"

namespace maze::events {

    auto getOnClose(sf::RenderWindow &window) {
        return [&window](const sf::Event::Closed &) {
            std::println("{}", __func__);
            window.close();
        };
    }

    auto getOnMousePress(logic::Maze &maze) {
        return [&maze](const sf::Event::MouseButtonPressed &e) {
            std::println("{}", __func__);
            const auto &cfg = ConfigService::get();
            const auto offset = static_cast<float>(cfg.cellSize) / 2.f;
            auto mousePosF = sf::Vector2f({static_cast<float>(e.position.x),
                                           static_cast<float>(e.position.y)});
            sf::Transform transform;
            transform.translate({-offset, -offset});
            mousePosF = transform.transformPoint(std::move(mousePosF));
            const auto column = (static_cast<size_t>(mousePosF.x) / cfg.cellSize);
            const auto row = (static_cast<size_t>(mousePosF.y) / cfg.cellSize);

            if (column >= cfg.columnsCount || row >= cfg.rowsCount) {
                return;
            }

            if (e.button == sf::Mouse::Button::Left) {
                std::println("begin: {} {}", column, row);
                maze = logic::addBeginCell(std::move(maze), {column, row});
                return;
            }
            if (e.button == sf::Mouse::Button::Right) {
                std::println("end: {} {}", column, row);
                maze = logic::addEndCell(std::move(maze), {column, row});
            }
        };
    }

}; // namespace maze::events

#endif
