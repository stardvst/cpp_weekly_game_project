#include <iostream>
#include <array>

#include <spdlog/spdlog.h>
#include <imgui.h>
#include <imgui-SFML.h>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Graphics/CircleShape.hpp>

int main([[maybe_unused]] int argc, [[maybe_unused]] const char **argv)
{
	// Use the default logger (stdout, multi-threaded, colored)
	spdlog::info("Starting ImGui + SFML");

    sf::RenderWindow window(sf::VideoMode(1024, 760), "ImGui + SFML = <3");
    window.setFramerateLimit(60);
    ImGui::SFML::Init(window);

	constexpr auto scaleFactor = 2.0;
	ImGui::GetStyle().ScaleAllSizes(scaleFactor);
	ImGui::GetIO().FontGlobalScale = scaleFactor;

	std::array<bool, 12> states{};

    sf::Clock deltaClock;
    while (window.isOpen()) {
		sf::Event event;
        while (window.pollEvent(event)) {
            ImGui::SFML::ProcessEvent(event);

            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }

        ImGui::SFML::Update(window, deltaClock.restart());

		ImGui::Begin("The road-map");

		int index = 0;
		for (const auto &step : { "The road-map",
			"Getting started",
			"Finding Errors As Soon As Possible",
			"Handling Command Line Parameters",
			"C++20 so far",
			"Reading SFML input states",
			"Managing game state",
			"Making our game testable",
			"Making game state allocator-aware",
			"Add logging to game engine",
			"Draw a game map", "Dialog trees",
			"Porting from SFML to SDL"
			})
		{
			ImGui::Checkbox(fmt::format("{} : {}", index, step).c_str(), &states[index]);
			++index;
		}

		ImGui::End();

        window.clear();
        ImGui::SFML::Render(window);
        window.display();
    }

    ImGui::SFML::Shutdown();

	return 0;
}
