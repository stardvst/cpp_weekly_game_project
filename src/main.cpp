#include <iostream>

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

	bool state0{ false };
	bool state1{ false };
	bool state2{ false };
	bool state3{ false };
	bool state4{ false };
	bool state5{ false };
	bool state6{ false };
	bool state7{ false };
	bool state8{ false };
	bool state9{ false };
	bool state10{ false };

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
		ImGui::Checkbox("0: The road-map", &state0);
		ImGui::Checkbox("1: Getting started", &state1);
		ImGui::Checkbox("2: C++20 so far", &state2);
		ImGui::Checkbox("3: Reading SFML input states", &state3);
		ImGui::Checkbox("4: Managing game state", &state4);
		ImGui::Checkbox("5: Making our game testable", &state5);
		ImGui::Checkbox("6: Making game state allocator-aware", &state6);
		ImGui::Checkbox("7: Add logging to game engine", &state7);
		ImGui::Checkbox("8: Draw a game map", &state8);
		ImGui::Checkbox("9: Dialog trees", &state9);
		ImGui::Checkbox("10: Porting from SFML to SDL", &state10);

        ImGui::End();

        window.clear();
        ImGui::SFML::Render(window);
        window.display();
    }

    ImGui::SFML::Shutdown();

	return 0;
}
