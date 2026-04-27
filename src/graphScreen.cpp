#include "graphScreen.h"
#include "fileDialog.h"
#include "assetManager.h"
#include "drawingUtils.h"
#include "menuScreen.h"
#include "theme.h"
#include "imgui.h"
#include "iostream"
#include <cmath>
#include <cstdio>
#include <string>

namespace {

static sf::Vector2f layoutVertex(int index, int n, float cx, float cy, float orbitRadius) {
	if (n <= 0) {
		return { cx, cy };
	}
	const float PI = 3.14159265f;
	float ang = -PI * 0.5f + (2.0f * PI * static_cast<float>(index)) / static_cast<float>(n);
	return { cx + orbitRadius * std::cos(ang), cy + orbitRadius * std::sin(ang) };
}

} // namespace

GraphScreen::GraphScreen(std::function<void(std::unique_ptr<Screen>)> changeScreenCallback)
	: onChangeScreen(std::move(changeScreenCallback)) {
	sf::Texture& bgTexture = AssetManager::getInstance().getTexture("main_bg");
	backgroundSprite.setTexture(bgTexture);
	backgroundSprite.setColor(sf::Color(255, 255, 255, 150));
}

void GraphScreen::startAnimation() {
	if (graph.hasFrames()) {
		totalSteps = static_cast<int>(graph.getFrames().size()) - 1;
		currentStep = 0;
		timer = 0.0f;
		isPlaying = autoPlay;
	}
}

void GraphScreen::update(sf::RenderWindow& window, sf::Time deltaTime) {
	if (isPlaying && totalSteps > 0) {
		timer += deltaTime.asSeconds() * playbackSpeed;
		if (timer >= 1.0f) {
			timer = 0.0f;
			if (currentStep < totalSteps) {
				currentStep++;
			} else {
				isPlaying = false;
			}
		}
	}

	ImGui::SetNextWindowSizeConstraints(
		ImVec2(250.0f, window.getSize().y - 20.0f),
		ImVec2(800.0f, window.getSize().y - 20.0f)
	);
	ImGui::SetNextWindowSize(ImVec2(Theme::ControlPanelWidth, window.getSize().y - 20.0f), ImGuiCond_Once);
	ImGui::SetNextWindowPos(ImVec2(10.0f, 10.0f), ImGuiCond_Always);

	ImGui::Begin("Graph controls (Dijkstra)", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove);
	Theme::ControlPanelWidth = ImGui::GetWindowWidth();

	if (ImGui::Button("<- Back to Menu")) {
		onChangeScreen(std::make_unique<MenuScreen>(onChangeScreen));
		ImGui::End();
		return;
	}

	ImGui::Separator();
	ImGui::Text("Vertices: %d", graph.getVertexCount());
	ImGui::Spacing();

	ImGui::Text("Initialization:");
	if (ImGui::CollapsingHeader("Initialize", ImGuiTreeNodeFlags_DefaultOpen)) {
		if (ImGui::Button("Empty graph", ImVec2(ImGui::GetContentRegionAvail().x, 0))) {
			graph.initEmpty();
			currentStep = 0;
			totalSteps = 0;
			isPlaying = false;
		}

		ImGui::Spacing();
		ImGui::InputInt("Random vertices", &randomN);
		ImGui::InputInt("Random edges", &randomM);
		ImGui::InputInt("Weight min", &randomWMin);
		ImGui::InputInt("Weight max", &randomWMax);
		if (ImGui::Button("Generate random", ImVec2(ImGui::GetContentRegionAvail().x, 0))) {
			graph.initRandom(randomN, randomM, randomWMin, randomWMax);
			startAnimation();
		}

		
		// Input From file
		ImGui::Spacing();

		ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - 90.0f);
		ImGui::InputText("##filepath", filepathBuffer, IM_ARRAYSIZE(filepathBuffer));

		ImGui::SameLine();

		if (ImGui::Button("Browse", ImVec2(85.0f, 0))) {
		#ifdef _WIN32
			std::string selectedFile = openFileDialog();
			if (!selectedFile.empty()) {
				strncpy(filepathBuffer, selectedFile.c_str(), IM_ARRAYSIZE(filepathBuffer) - 1);
				filepathBuffer[IM_ARRAYSIZE(filepathBuffer) - 1] = '\0';
			}
		#endif
		}

		if (ImGui::Button("Load from file", ImVec2(ImGui::GetContentRegionAvail().x, 0))) {
			if (strlen(filepathBuffer) == 0) {
				std::cout << "Please enter file path\n";
			} else {
				graph.initFromFile(filepathBuffer);
				startAnimation();
			}
		}
		ImGui::TextWrapped("File format: one directed edge per line: u v weight (non-negative integers).");
	}

	if (ImGui::CollapsingHeader("Add", ImGuiTreeNodeFlags_DefaultOpen)) {
		if (ImGui::Button("Add vertex", ImVec2(ImGui::GetContentRegionAvail().x, 0))) {
			graph.addVertex();
			startAnimation();
		}
		ImGui::InputInt("Edge u new", &addEdgeU);
		ImGui::InputInt("Edge v new", &addEdgeV);
		ImGui::InputInt("Weight", &addEdgeW);
		if (ImGui::Button("Add / update edge", ImVec2(ImGui::GetContentRegionAvail().x, 0))) {
			graph.addEdge(addEdgeU, addEdgeV, addEdgeW);
			startAnimation();
		}
	}

	if (ImGui::CollapsingHeader("Delete", ImGuiTreeNodeFlags_DefaultOpen)) {
		ImGui::InputInt("Delete vertex id", &removeVertexId);
		if (ImGui::Button("Delete vertex", ImVec2(ImGui::GetContentRegionAvail().x, 0))) {
			graph.removeVertex(removeVertexId);
			startAnimation();
		}
		ImGui::InputInt("Delete edge u", &removeEdgeU);
		ImGui::InputInt("Delete edge v", &removeEdgeV);
		if (ImGui::Button("Delete edge", ImVec2(ImGui::GetContentRegionAvail().x, 0))) {
			graph.removeEdge(removeEdgeU, removeEdgeV);
			startAnimation();
		}
	}

	if (ImGui::CollapsingHeader("Update", ImGuiTreeNodeFlags_DefaultOpen)) {
		ImGui::InputInt("Edge u", &updateEdgeU);
		ImGui::InputInt("Edge v", &updateEdgeV);
		ImGui::InputInt("New weight", &updateNewW);
		if (ImGui::Button("Update edge weight", ImVec2(ImGui::GetContentRegionAvail().x, 0))) {
			graph.updateEdgeWeight(updateEdgeU, updateEdgeV, updateNewW);
			startAnimation();
		}
	}

	if (ImGui::CollapsingHeader("Search", ImGuiTreeNodeFlags_DefaultOpen)) {
		ImGui::InputInt("Vertex id", &searchId);
		if (ImGui::Button("Highlight vertex", ImVec2(ImGui::GetContentRegionAvail().x, 0))) {
			graph.searchVertex(searchId);
			startAnimation();
		}
	}

	if (ImGui::CollapsingHeader("Algorithms", ImGuiTreeNodeFlags_DefaultOpen)) {
		ImGui::InputInt("Dijkstra source", &dijkstraSource);
		ImGui::Checkbox("Auto-play after run", &autoPlay);
		if (ImGui::Button("Run Dijkstra", ImVec2(ImGui::GetContentRegionAvail().x, 0))) {
			graph.dijkstra(dijkstraSource);
			startAnimation();
		}
	}

	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();

	if (ImGui::CollapsingHeader("Appearance Settings", ImGuiTreeNodeFlags_DefaultOpen)) {
		ImGui::Text("Node sizes");
		ImGui::SliderFloat("Circle radius", &Theme::NodeRadius, 18.0f, 60.0f);

		ImGui::Spacing();
		ImGui::Text("Color");
		float color[3] = { Theme::NodeDefault.r / 255.0f, Theme::NodeDefault.g / 255.0f, Theme::NodeDefault.b / 255.0f };
		if (ImGui::ColorEdit3("Default node", color)) {
			Theme::NodeDefault.r = static_cast<sf::Uint8>(color[0] * 255.0f);
			Theme::NodeDefault.g = static_cast<sf::Uint8>(color[1] * 255.0f);
			Theme::NodeDefault.b = static_cast<sf::Uint8>(color[2] * 255.0f);
		}
	}

	ImGui::End();

	float bottomBarHeight = 120.0f;
	float middleCanvasWidth = static_cast<float>(window.getSize().x) - Theme::ControlPanelWidth - Theme::CodePanelWidth;

	ImGui::SetNextWindowPos(ImVec2(Theme::ControlPanelWidth, static_cast<float>(window.getSize().y) - bottomBarHeight));
	ImGui::SetNextWindowSize(ImVec2(middleCanvasWidth, bottomBarHeight));

	ImGui::Begin("Playback controls", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBackground);

	ImVec2 btnSize(45, 30);
	float styleSpacing = ImGui::GetStyle().ItemSpacing.x;
	float totalTopRowWidth = (btnSize.x * 4.0f) + 120.0f + 50.0f + (styleSpacing * 4.0f);
	ImGui::SetCursorPosX((middleCanvasWidth - totalTopRowWidth) * 0.5f);

	if (ImGui::Button(u8"\uf048", btnSize)) {
		isPlaying = false;
		if (currentStep > 0) {
			currentStep--;
		}
	}
	ImGui::SameLine();

	if (isPlaying) {
		if (ImGui::Button(u8"\uf04c", btnSize)) {
			isPlaying = false;
		}
	} else {
		if (ImGui::Button(u8"\uf04b", btnSize)) {
			isPlaying = true;
		}
	}
	ImGui::SameLine();

	if (ImGui::Button(u8"\uf051", btnSize)) {
		isPlaying = false;
		if (currentStep < totalSteps) {
			currentStep++;
		}
	}
	ImGui::SameLine();

	if (ImGui::Button(u8"\uf050", btnSize)) {
		isPlaying = false;
		currentStep = totalSteps;
	}

	ImGui::SameLine();
	ImGui::SetNextItemWidth(120.0f);
	ImGui::SliderFloat("Speed", &playbackSpeed, 0.5f, 3.0f, "%.1fx");

	ImGui::Spacing();
	ImGui::Spacing();

	float progressBarWidth = 400.0f;
	ImGui::SetCursorPosX((middleCanvasWidth - progressBarWidth) * 0.5f);
	float progress = (totalSteps > 0) ? static_cast<float>(currentStep) / static_cast<float>(totalSteps) : 0.0f;
	char progressText[32];
	std::snprintf(progressText, sizeof(progressText), "%d%%", static_cast<int>(progress * 100.0f));
	ImGui::ProgressBar(progress, ImVec2(progressBarWidth, 30), progressText);

	ImGui::End();

	if (graph.hasFrames()) {
		const auto& frames = graph.getFrames();
		if (!frames.empty() && currentStep >= 0 && currentStep < static_cast<int>(frames.size())) {
			ImGui::SetNextWindowSizeConstraints(
				ImVec2(250.0f, 200.0f),
				ImVec2(800.0f, static_cast<float>(window.getSize().y) - 20.0f)
			);
			ImGui::SetNextWindowPos(
				ImVec2(static_cast<float>(window.getSize().x) - Theme::CodePanelWidth - 10.0f, 10.0f),
				ImGuiCond_Always
			);
			ImGui::SetNextWindowSize(ImVec2(Theme::CodePanelWidth, 300.0f), ImGuiCond_Once);

			ImGui::Begin("Execution Steps", nullptr, ImGuiWindowFlags_NoCollapse);
			Theme::CodePanelWidth = ImGui::GetWindowWidth();

			const auto& code = graph.getPseudoCode();
			int activeLine = frames[static_cast<size_t>(currentStep)].activeLineOfCode;

			ImGui::TextWrapped("Action: %s", frames[static_cast<size_t>(currentStep)].explanation.c_str());
			ImGui::Separator();
			ImGui::Spacing();

			if (!code.empty()) {
				for (size_t i = 0; i < code.size(); ++i) {
					if (static_cast<int>(i) == activeLine) {
						ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%s", code[i].c_str());
					} else {
						ImGui::Text("%s", code[i].c_str());
					}
				}
			} else {
				ImGui::TextDisabled("(No pseudo-code for this operation.)");
			}

			ImGui::End();
		}
	}
}

void GraphScreen::render(sf::RenderWindow& window) {
	window.draw(backgroundSprite);

	if (!graph.hasFrames()) {
		return;
	}
	if (currentStep < 0 || currentStep > totalSteps) {
		return;
	}

	const auto& frames = graph.getFrames();
	if (currentStep >= static_cast<int>(frames.size())) {
		return;
	}

	const auto& frame = frames[static_cast<size_t>(currentStep)];
	int n = static_cast<int>(frame.vertices.size());
	if (n == 0) {
		return;
	}

	float middleLeft = Theme::ControlPanelWidth;
	float middleWidth = static_cast<float>(window.getSize().x) - Theme::ControlPanelWidth - Theme::CodePanelWidth;
	float cx = middleLeft + middleWidth * 0.5f;
	float cy = static_cast<float>(window.getSize().y) * 0.5f;

	float orbit = std::min(middleWidth, static_cast<float>(window.getSize().y)) * 0.33f;
	if (orbit < Theme::NodeRadius * 3.0f) {
		orbit = Theme::NodeRadius * 3.0f;
	}

	std::vector<sf::Vector2f> pos(static_cast<size_t>(n));
	for (int i = 0; i < n; ++i) {
		pos[static_cast<size_t>(i)] = layoutVertex(i, n, cx, cy, orbit);
	}

	for (const auto& e : frame.edges) {
		if (e.u < 0 || e.v < 0 || e.u >= n || e.v >= n) {
			continue;
		}
		sf::Vector2f a = pos[static_cast<size_t>(e.u)];
		sf::Vector2f b = pos[static_cast<size_t>(e.v)];
		DrawingUtils::drawDirectedWeightedEdge(
			window,
			a,
			b,
			std::to_string(e.weight),
			Theme::LinkDefault,
			e.highlight
		);
	}

	for (int i = 0; i < n; ++i) {
		const auto& vs = frame.vertices[static_cast<size_t>(i)];
		sf::Vector2f p = pos[static_cast<size_t>(i)];

		sf::Color nodeColor = Theme::NodeDefault;
		if (vs.isSearchHit) {
			nodeColor = Theme::NodeFound;
		} else if (vs.highlighted) {
			nodeColor = Theme::NodeHighlight;
		} else if (vs.visited) {
			nodeColor = Theme::NodeFound;
		}

		std::string label = std::to_string(vs.id);
		if (vs.dist != GRAPH_INF) {
			label += "\n";
			label += std::to_string(vs.dist);
		}

		DrawingUtils::drawCircleNode(window, p, label, nodeColor);
	}
}
