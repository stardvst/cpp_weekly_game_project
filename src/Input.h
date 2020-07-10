#pragma once

#include <variant>
#include <optional>

#include <SFML/Window/Joystick.hpp>

namespace Game
{

constexpr std::string_view toString(const sf::Joystick::Axis &axis)
{
	switch (axis)
	{
		case sf::Joystick::Axis::PovX:
			return "PovX";
		case sf::Joystick::Axis::PovY:
			return "PovY";
		case sf::Joystick::Axis::R:
			return "R";
		case sf::Joystick::Axis::U:
			return "U";
		case sf::Joystick::Axis::V:
			return "V";
		case sf::Joystick::Axis::X:
			return "X";
		case sf::Joystick::Axis::Y:
			return "Y";
		case sf::Joystick::Axis::Z:
			return "Z";
	}
	abort();
}

struct GameState
{

	struct Joystick
	{
		unsigned int id;
		unsigned int buttonCount;
		std::array<bool, sf::Joystick::ButtonCount> buttonState;
		std::array<float, sf::Joystick::AxisCount> axisPosition;
	};

	static void refreshJoystick(Joystick &js)
	{
		sf::Joystick::update();

		for (unsigned int btn = 0; btn < sf::Joystick::ButtonCount; ++btn)
			js.buttonState[btn] = sf::Joystick::isButtonPressed(js.id, btn);

		for (unsigned int axis = 0; axis < sf::Joystick::ButtonCount; ++axis)
			js.axisPosition[axis] = sf::Joystick::getAxisPosition(js.id, static_cast<sf::Joystick::Axis>(axis));
	}

	static Joystick loadJoystick(unsigned int id)
	{
		Joystick js{ id, sf::Joystick::getButtonCount(id), {}, {} };
		refreshJoystick(js);
		return js;
	}
	static Joystick &joystickById(std::vector<Joystick> &joysticks, unsigned int id)
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

	struct CloseWindow
	{
	};

	using Event = std::variant<Joystick, CloseWindow, std::nullopt_t>;
	Event processEvent(const sf::Event &event)
	{
		switch (event.type)
		{
			case sf::Event::Closed:
				return CloseWindow{};
			case sf::Event::JoystickButtonPressed:
			{
				auto &js = joystickById(joySticks, event.joystickButton.joystickId);
				js.buttonState[event.joystickButton.button] = true;
				return js;
			}
			case sf::Event::JoystickButtonReleased:
			{
				auto &js = joystickById(joySticks, event.joystickButton.joystickId);
				js.buttonState[event.joystickButton.button] = false;
				return js;
			}
			case sf::Event::JoystickMoved:
			{
				auto &js = joystickById(joySticks, event.joystickMove.joystickId);
				js.axisPosition[event.joystickMove.axis] = event.joystickMove.position;
				return js;
			}
			default:
				return std::nullopt;
		}
	}

	std::vector<Joystick> joySticks;
	std::chrono::milliseconds msElapsed;

};

}
