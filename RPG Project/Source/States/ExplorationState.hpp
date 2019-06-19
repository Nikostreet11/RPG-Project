/*
 * GameState.h
 *
 *  Created on: Apr 9, 2019
 *      Author: nicop
 */

#ifndef STATES_EXPLORATIONSTATE_HPP_
#define STATES_EXPLORATIONSTATE_HPP_

#include "State.hpp"

#include "..\GUI\PauseMenu.hpp"
#include "..\Map\TileMap.hpp"
#include "..\Entities\Player.hpp"

class ExplorationState :
		public State
{
public:
	// Constructors / Destructors
	explicit ExplorationState(StateData& stateData);
	virtual ~ExplorationState();

	// Functions
	virtual void endState();

	virtual void update(const float& dt);
	virtual void updateCamera(const float& dt);
	virtual void updateInput(const float& dt);
	virtual void updatePlayerInput(const float& dt);
	virtual void updatePauseMenu();
	virtual void render(std::shared_ptr<sf::RenderTarget> target = nullptr);

	// Getters / Setters

private:
	// Initialization functions
	void initDeferredRendering();
	void initCamera();
	void initBindings();
	void initFonts();
	void initTextures();
	void initPlayers();
	void initTileMap();
	void initPauseMenu();

	// Resources
	std::shared_ptr<sf::View> camera;
	sf::RenderTexture renderTexture;
	sf::Sprite renderSprite;
	std::shared_ptr<Player> player;
	std::unique_ptr<TileMap> tileMap;
	std::unique_ptr<gui::PauseMenu> pauseMenu;

	std::shared_ptr<sf::Font> font;

};

#endif /* STATES_EXPLORATIONSTATE_HPP_ */
