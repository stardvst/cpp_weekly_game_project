#include <iostream>
#include <array>
#include <map>

#include <spdlog/spdlog.h>
#include <imgui.h>
#include <imgui-SFML.h>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Graphics/CircleShape.hpp>
#include <docopt/docopt.h>

#include "Input.h"
#include "ImGuiHelpers.h"

static constexpr auto USAGE =
R"(C++ Weekly Game.
	Usage:
		game
		game -h | -help
	Options:
		-h --help	Show this screen.
		--width=<WIDTH>     Screen width in pixels [default: 1024].
		--height=<HEIGHT>   Screen height in pixels [default: 768].
		--scale=<SCALE>     Scaling factor [default: 2].
)";

template <class... Ts>
struct overloaded : Ts...
{
	using Ts::operator()...;
};

template <class... Ts>
overloaded(Ts...) -> overloaded<Ts...>;

int main(int argc, const char **argv)
{
	std::map<std::string, docopt::value> args = docopt::docopt(USAGE,
		{ std::next(argv), std::next(argv, argc) },
		true, // show help if requested
		"Game 0.0"); // version string

	const auto width = args["--width"].asLong();
	const auto height = args["--height"].asLong();
	const auto scale = args["--scale"].asLong();

	if (width < 0 || height < 0 || scale < 1 || scale > 5)
	{
		spdlog::error("Command line options are out of reasonable range.");
		for (auto const &arg : args)
			if (arg.second.isString())
				spdlog::info("Parameter set: {}='{}'", arg.first, arg.second.asString());
		abort();
	}

	// Use the default logger (stdout, multi-threaded, colored)
	spdlog::set_level(spdlog::level::debug);
	spdlog::info("Starting ImGui + SFML");

    sf::RenderWindow window(sf::VideoMode(static_cast<unsigned int>(width), static_cast<unsigned int>(height)), "ImGui + SFML = <3");
    window.setFramerateLimit(60);
    ImGui::SFML::Init(window);

	const auto scaleFactor = static_cast<float>(scale);
	ImGui::GetStyle().ScaleAllSizes(scaleFactor);
	ImGui::GetIO().FontGlobalScale = scaleFactor;

	constexpr std::array steps = { "Getting started",
		"Finding Errors As Soon As Possible",
		"Handling Command Line Parameters",
		"Reading SFML joystick states",
		"Displaying Joystick States",
		"Dealing With Game Events",
		"Reading SFML keyboard states",
		"Reading SFML mouse states",
		"C++20 so far",
		"Reading SFML input states",
		"Managing game state",
		"Making our game testable",
		"Making game state allocator-aware",
		"Add logging to game engine",
		"Draw a game map", "Dialog trees",
		"Porting from SFML to SDL"
	};

	std::array<bool, steps.size()> states{};

	Game::GameState gameState;

    sf::Clock deltaClock;
    while (window.isOpen())
	{
		std::vector<Game::GameState::Event> events;
		sf::Event event{};

        while (window.pollEvent(event))
		{
            ImGui::SFML::ProcessEvent(event);
			events.push_back(gameState.processEvent(event));
        }

		const auto timeElapsed = deltaClock.restart();
		ImGui::SFML::Update(window, timeElapsed);
		gameState.msElapsed = std::chrono::milliseconds{ timeElapsed.asMilliseconds() };

		bool joystickEvent = false;
		for (const auto &gameEvent : events)
		{
			std::visit(overloaded{
				[&](const Game::GameState::Joystick &)
			{
				joystickEvent = true;
				// move character or something?!
			},
				[&](const Game::GameState::CloseWindow &)
			{
				window.close();
			},
				[&](const std::nullopt_t &)
			{

			}
				}, gameEvent);
		}


		ImGui::Begin("The road-map");

		for (std::size_t index = 0; const auto &step : steps)
		{
			ImGui::Checkbox(fmt::format("{} : {}", index, step).c_str(), &states.at(index));
			++index;
		}

		ImGui::End();

		ImGui::Begin("Joystick");
		ImGui::TextUnformatted(fmt::format("JS Event: {}", joystickEvent).c_str());
		if (!gameState.joySticks.empty())
		{
			ImGuiHelper::Text("Joystick Event: {}", joystickEvent);
			for (std::size_t button = 0; button < gameState.joySticks[0].buttonCount; ++button)
			{
				ImGuiHelper::Text("{}: {}", button, gameState.joySticks[0].buttonState[button]);
			}

			for (std::size_t axis = 0; axis < sf::Joystick::AxisCount; ++axis)
			{
				ImGuiHelper::Text("{}: {}", Game::toString(
					static_cast<sf::Joystick::Axis>(axis)),
					gameState.joySticks[0].axisPosition[axis]);
			}
		}
		ImGui::End();

        window.clear();
        ImGui::SFML::Render(window);
        window.display();
    }

    ImGui::SFML::Shutdown();

	return 0;
}
