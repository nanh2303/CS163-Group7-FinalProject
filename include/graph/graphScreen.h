#pragma once

#include "graph/graphCore.h"
#include "core/screen.h"

#include <functional>
#include <memory>

#include <SFML/Graphics.hpp>

class GraphScreen : public Screen {
private:
	std::function<void(std::unique_ptr<Screen>)> onChangeScreen;

	Graph graph;

	int addEdgeU = 0;
	int addEdgeV = 1;
	int addEdgeW = 1;

	int removeVertexId = 0;
	int removeEdgeU = 0;
	int removeEdgeV = 0;

	int updateEdgeU = 0;
	int updateEdgeV = 0;
	int updateNewW = 1;

	int searchId = 0;

	int randomN = 5;
	int randomM = 7;
	int randomWMin = 1;
	int randomWMax = 9;

	int dijkstraSource = 0;

	char filepathBuffer[256] = "data/graph_sample.txt";

	bool autoPlay = true;

	sf::Sprite backgroundSprite;

	bool isPlaying = false;
	int currentStep = 0;
	int totalSteps = 0;
	float playbackSpeed = 1.0f;
	float timer = 0.0f;

	void startAnimation();

public:
	GraphScreen(std::function<void(std::unique_ptr<Screen>)> changeScreenCallback);

	void update(sf::RenderWindow& window, sf::Time deltaTime) override;
	void render(sf::RenderWindow& window) override;
};
