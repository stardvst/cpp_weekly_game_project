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

struct Joystick
{
	unsigned int id;
	std::string name;
	std::array<bool, sf::Joystick::ButtonCount> buttonState;
	std::array<float, sf::Joystick::AxisCount> axisPosition;
};

Joystick loadJoystick(unsigned int id)
{
	const auto identification = sf::Joystick::getIdentification(id);
	return Joystick{ id, static_cast<std::string>(identification.name), {}, {} };
}

Joystick &joystickById(std::vector<Joystick> &joysticks, unsigned int id)
{
	auto joystick = std::find_if(begin(joysticks), end(joysticks),
		[id](const auto &j)
	{
		return j.id == id;
	});

	if (joystick == joysticks.end()) [[unlikely]]
	{
		joysticks.emplace_back(loadJoystick(id));
		return joysticks.back();
	}
	else [[likely]]
	{
		return *joystick;
	}
}

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

	std::vector<Joystick> joySticks;
	bool joystickEvent{ false };

    sf::Clock deltaClock;
    while (window.isOpen())
	{
		sf::Event event{};
        while (window.pollEvent(event))
		{
            ImGui::SFML::ProcessEvent(event);

			switch (event.type)
			{
				case sf::Event::Closed:
					window.close();
					break;

				case sf::Event::JoystickConnected:
				{
					joystickEvent = true;
					break;
				}

				case sf::Event::JoystickDisconnected:
				{
					joystickEvent = true;
					break;
				}

				case sf::Event::JoystickButtonPressed:
				{
					joystickEvent = true;
					auto &js = joystickById(joySticks, event.joystickButton.joystickId);
					js.buttonState[event.joystickButton.button] = true;
					break;
				}

				case sf::Event::JoystickButtonReleased:
				{
					joystickEvent = true;
					auto &js = joystickById(joySticks, event.joystickButton.joystickId);
					js.buttonState[event.joystickButton.button] = false;
					break;
				}

				case sf::Event::JoystickMoved:
				{
					joystickEvent = true;
					auto &js = joystickById(joySticks, event.joystickMove.joystickId);
					js.axisPosition[event.joystickMove.axis] = event.joystickMove.position;
					break;
				}

				default:
					spdlog::trace("Unhandled event type");
			}
        }

        ImGui::SFML::Update(window, deltaClock.restart());

		ImGui::Begin("The road-map");

		for (std::size_t index = 0; const auto &step : steps)
		{
			ImGui::Checkbox(fmt::format("{} : {}", index, step).c_str(), &states.at(index));
			++index;
		}

		ImGui::End();

		ImGui::Begin("Joystick");
		ImGui::TextUnformatted(fmt::format("JS Event: {}", joystickEvent).c_str());
		if (!joySticks.empty())
		{
			for (std::size_t button = 0; button < sf::Joystick::ButtonCount; ++button)
			{
				ImGui::TextUnformatted(fmt::format("{}: {}", button, joySticks[0].buttonState[button]).c_str());
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
