/*
 * BattleState.cpp
 *
 *  Created on: Jun 16, 2019
 *      Author: Nikostreet11
 */

#include "BattleState.hpp"

// Constructor / Destructor
BattleState::BattleState(
		StateData& stateData/*,
		std::vector<std::shared_ptr<Character>> party,
		std::vector<std::shared_ptr<Character>> enemies*/) :
		State(stateData)/*,
		party(party),
		enemies(enemies)*/
{
	initVariables();
	initDeferredRendering();
	initBindings();
	initFonts();
	initTextures();
	initBackground();
	initCharacters();
	initActiveQueue();
	initDialogueMenu();
	initActionMenu(Empty);
	initPauseMenu();
	initMarkers();
}

BattleState::~BattleState()
{
}

// Functions
void BattleState::endState()
{
	std::cout << "Ending BattleState" << std::endl;
	ended = true;
}

void BattleState::update(const float& dt)
{
	updateMousePositions();
	updateInput(dt);

	if (!paused)
	{
		// Unpaused update
		updateBattleInput(dt);
		//dialogueMenu->update();
		//actionMenu->update();
	}
	else
	{
		// Paused update
		updatePauseMenu();
	}
}

void BattleState::updateInput(const float& dt)
{
	if (keybinds.at("PAUSE").isPressed())
	{
		// Key pressed
		if (!paused)
			pauseState();
		else
			unpauseState();
	}
}

void BattleState::updateBattleInput(const float& dt)
{
	switch (phase)
	{

	case Begin:

		if (keybinds["CONFIRM"].isPressed())
		{
			dialogueMenu->setNextLine();

			if (dialogueMenu->isEnded())
			{
				changePhase(ActionSelection);
			}
		}
		break;

	case ActionSelection:

		if (keybinds["CONFIRM"].isPressed())
		{
			std::string entry = actionMenu->getSelectedEntry();

			if (entry == "MAGIC" || entry == "OBJECT")
			{
				updateActionMenu(entry);
			}
			else if (entry == "FLEE")
			{
				endState();
			}
			else
			{
				action = active->getAction(entry);
				changePhase(TargetSelection);
			}
		}

		if (keybinds["SELECT_UP"].isPressed())
		{
			actionMenu->moveMarker(Direction::Up);
			initDialogueMenu();
		}

		if (keybinds["SELECT_LEFT"].isPressed())
		{
			actionMenu->moveMarker(Direction::Left);
			initDialogueMenu();
		}

		if (keybinds["SELECT_DOWN"].isPressed())
		{
			actionMenu->moveMarker(Direction::Down);
			initDialogueMenu();
		}

		if (keybinds["SELECT_RIGHT"].isPressed())
		{
			actionMenu->moveMarker(Direction::Right);
			initDialogueMenu();
		}
		break;

	case TargetSelection:

		if (keybinds["CONFIRM"].isPressed())
		{
			target = targets[actionMenu->getIndex()];
			changePhase(Results);
		}

		if (keybinds["SELECT_UP"].isPressed())
		{
			actionMenu->moveMarker(Direction::Up);
			size_t index = actionMenu->getIndex();
			targetMarker.setPosition(targets[index]->getPosition());
			targetMarker.move(
					targets[index]->getSize().x / 2.f - targetMarker.getSize() / 2,
					-targetMarker.getSize() * 0.8f);
		}

		if (keybinds["SELECT_LEFT"].isPressed())
		{
			actionMenu->moveMarker(Direction::Left);
			size_t index = actionMenu->getIndex();
			targetMarker.setPosition(targets[index]->getPosition());
			targetMarker.move(
					targets[index]->getSize().x / 2.f - targetMarker.getSize() / 2,
					-targetMarker.getSize() * 0.8f);
		}

		if (keybinds["SELECT_DOWN"].isPressed())
		{
			actionMenu->moveMarker(Direction::Down);
			size_t index = actionMenu->getIndex();
			targetMarker.setPosition(targets[index]->getPosition());
			targetMarker.move(
					targets[index]->getSize().x / 2.f - targetMarker.getSize() / 2,
					-targetMarker.getSize() * 0.8f);
		}

		if (keybinds["SELECT_RIGHT"].isPressed())
		{
			actionMenu->moveMarker(Direction::Right);
			size_t index = actionMenu->getIndex();
			targetMarker.setPosition(targets[index]->getPosition());
			targetMarker.move(
					targets[index]->getSize().x / 2.f - targetMarker.getSize() / 2,
					-targetMarker.getSize() * 0.8f);
		}
		break;

	case Results:

		if (keybinds["CONFIRM"].isPressed())
		{
			changePhase(ActionSelection);
		}
		break;

	case End:

		if (keybinds["CONFIRM"].isPressed())
		{
			endState();
		}
		break;
	}
}

void BattleState::updateActionMenu(const std::string& entry)
{
	if (entry == "MAGIC")
	{
		initActionMenu(MagicMenu);
	}
	else if (entry == "OBJECT")
	{
		initActionMenu(ObjectMenu);
	}
}

void BattleState::updatePauseMenu()
{
	pauseMenu->update(mousePosWindow);

	if (pauseMenu->isButtonPressed("QUIT"))
		endState();
}

void BattleState::render(std::shared_ptr<sf::RenderTarget> target)
{
	if (!target)
		target = window;

	target->setView(target->getDefaultView());

	renderTexture.clear();

	renderTexture.draw(background);
	renderCharacters(renderTexture);
	dialogueMenu->render(renderTexture);

	if (phase != Begin)
	{
		actionMenu->render(renderTexture);
		activeMarker.render(renderTexture);
	}

	if (phase == TargetSelection || phase == Results)
	{
		targetMarker.render(renderTexture);
	}

	if (paused)
	{
		// Pause menu render
		pauseMenu->render(renderTexture);
	}

	renderTexture.display();

	target->draw(renderSprite);
}

void BattleState::renderCharacters(sf::RenderTarget& target)
{
	for (auto &enemy : enemies)
	{
		enemy->render(target);
	}

	for (auto &hero : party)
	{
		hero->render(target);
	}
}

// Internal
void BattleState::changePhase(Phase phase)
{
	switch (phase)
	{

	case Begin:

		initActionMenu(Empty);
		break;

	case ActionSelection:

		if (this->phase == Begin)
		{
			active = activeQueue[0];
		}
		else
		{
			selectNextActive();
		}

		updateActiveMarker();
		initActionMenu(MainActions);
		break;

	case TargetSelection:
		// TODO: put in a separate function
		targets.clear();

		if (menu == ObjectMenu)
		{
			targets.insert(targets.end(), party.begin(), party.end());
			targets.insert(targets.end(), enemies.begin(), enemies.end());
		}
		else
		{
			targets.insert(targets.end(), enemies.begin(), enemies.end());
			targets.insert(targets.end(), party.begin(), party.end());
		}

		initActionMenu(TargetMenu);

		targetMarker.setPosition(targets[0]->getPosition());
		targetMarker.move(
				targets[0]->getSize().x / 2.f - targetMarker.getSize() / 2,
				-targetMarker.getSize() * 0.8f);
		break;

	case Results:

		results = action->use(active, target);
		break;

	case End:
		endState();
	}

	this->phase = phase;
	initDialogueMenu();
}

// Initialization
void BattleState::initVariables()
{
	activeIndex = 0;
	phase = BattleState::Begin;
}

void BattleState::initDeferredRendering()
{
	renderTexture.create(
			graphicsSettings->resolution.width,
			graphicsSettings->resolution.height);

	renderSprite.setTexture(renderTexture.getTexture());
	renderSprite.setTextureRect(sf::IntRect(
			0,
			0,
			graphicsSettings->resolution.width,
			graphicsSettings->resolution.height));
}

void BattleState::initBindings()
{
	std::ifstream ifs("Config/Keybinds/BattleState.ini");

	if (ifs.is_open())
	{
		std::string action, key;

		while (ifs >> action >> key)
		{
			keybinds[action].setType(InputButton::Type::keyboardKey);
			keybinds[action].setCode((*supportedKeys)[key]);
		}
	}

	ifs.close();
}

void BattleState::initFonts()
{
	font = std::make_shared<sf::Font>();

	if (!font->loadFromFile("Fonts/Arial.ttf"))
	{
		throw("ERROR::MAINMENUSTATE::COULD_NOT_LOAD_FONT");
	}
}

void BattleState::initTextures()
{
	textures["BACKGROUNDS"] = std::make_shared<sf::Texture>();

	if (!textures["BACKGROUNDS"]->loadFromFile(
			"Images/Battle/Backgrounds/Backgrounds.png"))
	{
		throw "ERROR::BATTLESTATE::UNABLE_TO_LOAD_BACKGROUND_TEXTURE";
	}

	textures["WARRIOR"] = std::make_shared<sf::Texture>();

	if (!textures["WARRIOR"]->loadFromFile(
			"Images/Battle/Characters/Warrior.png"))
	{
		throw "ERROR::BATTLESTATE::UNABLE_TO_LOAD_WARRIOR_TEXTURE";
	}

	textures["THIEF"] = std::make_shared<sf::Texture>();

	if (!textures["THIEF"]->loadFromFile(
			"Images/Battle/Characters/Thief.png"))
	{
		throw "ERROR::BATTLESTATE::UNABLE_TO_LOAD_THIEF_TEXTURE";
	}

	textures["BLACK_MAGE"] = std::make_shared<sf::Texture>();

	if (!textures["BLACK_MAGE"]->loadFromFile(
			"Images/Battle/Characters/BlackMage.png"))
	{
		throw "ERROR::BATTLESTATE::UNABLE_TO_LOAD_BLACK_MAGE_TEXTURE";
	}

	textures["WHITE_MAGE"] = std::make_shared<sf::Texture>();

	if (!textures["WHITE_MAGE"]->loadFromFile(
			"Images/Battle/Characters/WhiteMage.png"))
	{
		throw "ERROR::BATTLESTATE::UNABLE_TO_LOAD_WHITE_MAGE_TEXTURE";
	}

	textures["FOES"] = std::make_shared<sf::Texture>();

	if (!textures["FOES"]->loadFromFile(
			"Images/Battle/Enemies/Foes.png"))
	{
		throw "ERROR::BATTLESTATE::UNABLE_TO_LOAD_FOES_TEXTURE";
	}
}

void BattleState::initBackground()
{
	background.setTexture(*textures["BACKGROUNDS"]);
	background.setTextureRect(sf::IntRect(
			2,
			2 * 15 + 312 * 7,
			512,
			312));
	background.setScale(
			graphicsSettings->resolution.width / 512.f,
			graphicsSettings->resolution.height / 312.f);
}

void BattleState::initCharacters()
{
	// TODO: rework with the map factory method
	for (unsigned index = 0; index < 4; index++)
	{
		enemies.push_back(std::move(std::make_shared<Monster>(
						Monster::Werewolf,
						*textures["FOES"])));
	}

	for (unsigned index = 0; index < 4; index++)
	{
		enemies.push_back(std::move(std::make_shared<Monster>(
						Monster::GigasWorm,
						*textures["FOES"])));
	}

	// TODO: rework with the party getters
	party.push_back(std::move(std::make_shared<Human>(
			Human::Warrior,
			*textures["WARRIOR"])));
	party.push_back(std::move(std::make_shared<Human>(
			Human::Thief,
			*textures["THIEF"])));
	party.push_back(std::move(std::make_shared<Human>(
			Human::WhiteMage,
			*textures["WHITE_MAGE"])));
	party.push_back(std::move(std::make_shared<Human>(
			Human::BlackMage,
			*textures["BLACK_MAGE"])));

	sf::Vector2f leftStart(700.f, 180.f);
	sf::Vector2f leftStep(-80.f, 100.f);

	for (size_t index = 0; index < enemies.size(); index++)
	{
		enemies[index]->setPosition(sf::Vector2f(
				leftStart.x + leftStep.x * index,
				leftStart.y + leftStep.y * (index % 4)));
		enemies[index]->setSize(sf::Vector2f(200, 200));
	}

	sf::Vector2f rightStart(1200, 200);
	sf::Vector2f rightStep(80, 100);

	for (std::size_t index = 0; index < party.size(); index++)
	{
		party[index]->setPosition(sf::Vector2f(
				rightStart.x + rightStep.x * index,
				rightStart.y + rightStep.y * (index % 4) + 20.f));
		party[index]->setSize(sf::Vector2f(160, 160));
	}

}

void BattleState::initActiveQueue()
{
	// TODO: rework based on Characters stats
	// TODO: add enemies
	activeQueue = party;
}

void BattleState::initDialogueMenu()
{
	std::vector<std::string> dialogue;
	std::stringstream stringStream;

	switch (phase)
	{

	case Begin:
		dialogue = {"Wild foes appeared!", "Battle starts!!!"};
		break;

	case ActionSelection:
			if (actionMenu->getSelectedEntry() == "ATTACK")
			{
				dialogue = {"Physical attack."};
			}
			else if (actionMenu->getSelectedEntry() == "MAGIC")
			{
				dialogue = {"Mostly elemental attacks."};
			}
			else if (actionMenu->getSelectedEntry() == "OBJECT")
			{
				dialogue = {"Utility objects, various effects."};
			}
			else if (actionMenu->getSelectedEntry() == "FLEE")
			{
				dialogue = {"Try to flee from the battlefield."};
			}
			else
			{
				dialogue = {""}; // = active->getAction(actionMenu->getSelectedEntry())->getDescription();
			}
		break;

	case TargetSelection:
		dialogue = {"Select the target"};
		break;

	case Results:
		stringStream << "Damage dealt: " << results->target.dHealth << " HP!";
		dialogue = {stringStream.str()};
		break;

	case End:
		dialogue = {"<< battle results >>"};
		break;

	}


	if (phase == Begin)
	{
		dialogueMenu.reset(new gui::Dialogue(
				{1700, 250},
				{40, 50},
				50,
				dialogue,
				font));
	}
	else
	{
		dialogueMenu.reset(new gui::Dialogue(
				{1000, 250},
				{40, 50},
				50,
				dialogue,
				font));
	}

	dialogueMenu->setPosition(100, 750);
}

void BattleState::initActionMenu(ActionMenu menu)
{
	switch (menu)
	{

	case Empty:

		actionMenu.reset(new gui::Selection(
				sf::Vector2f(1200, 750),
				sf::Vector2f(600, 250),
				sf::Vector2f(40, 50),
				sf::Vector2u(2, 3),
				sf::Vector2u(2, 2),
				45,
				font));
		break;

	case MainActions:

		actionMenu.reset(new gui::Selection(
				sf::Vector2f(1200, 750),
				sf::Vector2f(600, 250),
				sf::Vector2f(40, 50),
				sf::Vector2u(2, 3),
				sf::Vector2u(2, 2),
				45,
				font));

		actionMenu->addEntry("ATTACK");
		actionMenu->addEntry("MAGIC");
		actionMenu->addEntry("OBJECT");
		actionMenu->addEntry("FLEE");
		break;

	case MagicMenu:

		actionMenu.reset(new gui::Selection(
				sf::Vector2f(1200, 750),
				sf::Vector2f(600, 250),
				sf::Vector2f(30, 60),
				sf::Vector2u(2, 3),
				sf::Vector2u(2, 2),
				40,
				font));

		actionMenu->addEntry("Fire");
		actionMenu->addEntry("Blizzard");
		actionMenu->addEntry("Aero");
		actionMenu->addEntry("Thunder");
		actionMenu->addEntry("Cure");
		actionMenu->addEntry("Heal");
		break;

	case ObjectMenu:

		actionMenu.reset(new gui::Selection(
				sf::Vector2f(1200, 750),
				sf::Vector2f(600, 250),
				sf::Vector2f(30, 60),
				sf::Vector2u(2, 3),
				sf::Vector2u(2, 2),
				40,
				font));

		actionMenu->addEntry("Potion");
		actionMenu->addEntry("Ether");
		actionMenu->addEntry("Phoenix down");
		actionMenu->addEntry("Antidote");
		actionMenu->addEntry("Megapotion");
		actionMenu->addEntry("Megaether");
		break;

	case TargetMenu:

		actionMenu.reset(new gui::Selection(
				sf::Vector2f(1200, 750),
				sf::Vector2f(600, 250),
				sf::Vector2f(60, 40),
				sf::Vector2u(1, 12),
				sf::Vector2u(1, 3),
				40,
				font));

		for (auto &target : targets)
		{
			actionMenu->addEntry(target->getName());
		}
	}

	this->menu = menu;
}

void BattleState::initPauseMenu()
{
	pauseMenu.reset(new gui::PauseMenu(window, font));

	pauseMenu->addButton("QUIT", 800.f, "Quit");
}

void BattleState::selectNextActive()
{
	if (activeIndex < activeQueue.size() - 1)
	{
		activeIndex++;
	}
	else
	{
		activeIndex = 0;
	}

	active = activeQueue[activeIndex];
}

void BattleState::updateActiveMarker()
{
	activeMarker.setPosition(active->getPosition());
	activeMarker.move(
			active->getSize().x / 2.f - activeMarker.getSize() / 2,
			-activeMarker.getSize() * 0.8f);
}

void BattleState::initMarkers()
{
	activeMarker.setSize(40);
	activeMarker.setOrigin(sf::Vector2f(0, activeMarker.getSize()));
	activeMarker.setRotation(90);
	activeMarker.setColor(sf::Color(255, 255, 255, 200));

	targetMarker.setSize(40);
	targetMarker.setOrigin(sf::Vector2f(0, targetMarker.getSize()));
	targetMarker.setRotation(90);
	targetMarker.setColor(sf::Color(255, 255, 255, 200));
}
