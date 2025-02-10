#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <fstream>
#include <sstream>
#include <iostream>
#include "Camera.h"
#include "Map.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Character.h"
#include "Adventurer.h"
#include "Monster.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Warrior.h"
#include "Mage.h"
#include "Sniper.h"

#include "Goblin.h"


#include <chrono>
#include <thread>
#include <cmath>
#include <algorithm>


double mouseX, mouseY;


std::string readFile(const char* filePath) {
    std::string content;
    std::ifstream fileStream(filePath, std::ios::in);

    if (!fileStream.is_open()) {
        std::cerr << "Failed to read file: " << filePath << std::endl;
        return "";
    }

    std::string line = "";
    while (!fileStream.eof()) {
        std::getline(fileStream, line);
        content.append(line + "\n");
    }
    fileStream.close();
    return content;
}


int main()
{
    // ---------- Initialisation de GLFW et création de la fenêtre ----------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow* window = glfwCreateWindow(800, 600, "Chessboard Map", NULL, NULL);
    if (window == NULL)
    {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, [](GLFWwindow* window, int width, int height) {
        glViewport(0, 0, width, height);
        });


    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // ---------- Compilation des shaders ----------
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    std::string vertexShaderSourceStr = readFile("shader.vert");
    const char* vertexShaderSource = vertexShaderSourceStr.c_str();
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    std::string fragmentShaderSourceStr = readFile("shader.frag");
    const char* fragmentShaderSource = fragmentShaderSourceStr.c_str();
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }


    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // ---------- Configuration de la caméra - Vue 2.5D ----------
    Camera camera(glm::vec3(25.0f, 7.0f, 7.0f),
        glm::vec3(0.0f, 0.0f, 7.0f),
        glm::vec3(0.0f, 1.0f, 0.0f));

    // ---------- Création de la carte ----------
    Map chessboardMap(16, 1.0f);


    // ---------- Création des personnages - Guerriers, Mages, Snipers et Goblins - Placés sur les côtés opposés ----------
    std::vector<Adventurer*> adventurers;
    std::vector<Monster*> monsters;

    int adventurerStartX = 2;
    int monsterStartX = 13;
    int startRowAdventurers = 5;
    int startRowMonsters = 5;

    // Création d'un Aventurier Guerrier
    adventurers.push_back(new Warrior(adventurerStartX, startRowAdventurers, &chessboardMap));
    chessboardMap.addCharacterToMap(adventurers.back(), adventurerStartX, startRowAdventurers);

    // Création d'un Aventurier Mage
    adventurers.push_back(new Mage(adventurerStartX + 2, startRowAdventurers, &chessboardMap));
    chessboardMap.addCharacterToMap(adventurers.back(), adventurerStartX + 2, startRowAdventurers);

    // Création d'un Aventurier Sniper
    adventurers.push_back(new Sniper(adventurerStartX + 4, startRowAdventurers, &chessboardMap));
    chessboardMap.addCharacterToMap(adventurers.back(), adventurerStartX + 4, startRowAdventurers);

    // Création de trois Monstres Goblins
    for (int i = 0; i < 3; ++i) {
        monsters.push_back(new Goblin(monsterStartX, startRowMonsters + i * 2, &chessboardMap));
        chessboardMap.addCharacterToMap(monsters.back(), monsterStartX, startRowMonsters + i * 2);
    }


    glEnable(GL_DEPTH_TEST);


    bool gameRunning = true;
    bool adventurersTurn = true;
    auto lastTurnTime = std::chrono::steady_clock::now();


    // ---------- Boucle de rendu ----------
    while (!glfwWindowShouldClose(window) && gameRunning)
    {
        // ---------- Input ----------
        glfwPollEvents();

        // ---------- Rendu ----------
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Configuration des uniformes de shader - Lumière directionnelle
        glUseProgram(shaderProgram);
        glUniform3fv(glGetUniformLocation(shaderProgram, "lightColor"), 1, &glm::vec3(1.0f, 1.0f, 1.0f)[0]);
        glm::vec3 lightDirection = glm::normalize(glm::vec3(0.2f, -0.8f, -0.6f));
        glUniform3fv(glGetUniformLocation(shaderProgram, "lightDir"), 1, &lightDirection[0]);

        // Rendu de la carte
        chessboardMap.render(shaderProgram, camera);

        // Rendu des personnages (appel polymorphe) - RENDU SEULEMENT DES PERSONNAGES VIVANTS
        for (Adventurer* adventurer : adventurers) {
            adventurer->render(shaderProgram, camera);
        }
        for (Monster* monster : monsters) {
            monster->render(shaderProgram, camera);
        }

        glfwSwapBuffers(window);


        // ---------- Logique de combat au tour par tour ----------
        auto currentTime = std::chrono::steady_clock::now();
        auto timeElapsed = std::chrono::duration_cast<std::chrono::seconds>(currentTime - lastTurnTime).count();

        if (timeElapsed >= 2) { // Délai de 2 secondes par tour
            if (adventurersTurn) {
                std::cout << "** Tour des Aventuriers **" << std::endl;
                std::vector<Monster*> targetedMonsters;

                for (Adventurer* adventurer : adventurers) {
                    if (!adventurer->isAlive()) continue;

                    Monster* closestMonster = nullptr;
                    float minDistance = 100000.0f;

                    for (Monster* monster : monsters) {
                        if (!monster->isAlive() || std::find(targetedMonsters.begin(), targetedMonsters.end(), monster) != targetedMonsters.end()) continue;

                        float distance = std::abs(adventurer->gridX - monster->gridX) + std::abs(adventurer->gridZ - monster->gridZ);
                        if (distance < minDistance) {
                            minDistance = distance;
                            closestMonster = monster;
                        }
                    }

                    if (closestMonster) {
                        targetedMonsters.push_back(closestMonster);
                        // Déplacement avec minDistanceToTarget défini sur adventurer->attackRange
                        adventurer->moveTowards(closestMonster->gridX, closestMonster->gridZ, adventurer->attackRange - 1);
                    }

                    for (Monster* monster : targetedMonsters) {
                        if (!monster->isAlive()) continue;
                        if (std::abs(adventurer->gridX - monster->gridX) <= adventurer->attackRange &&
                            std::abs(adventurer->gridZ - monster->gridZ) <= adventurer->attackRange) {
                            std::cout << adventurer->getClassName() << " à (" << adventurer->gridX << ", " << adventurer->gridZ << ") attaque " << monster->getClassName() << " à (" << monster->gridX << ", " << monster->gridZ << ")" << std::endl;
                            monster->takeDamage(adventurer->baseDamage, adventurer);
                        }
                    }
                }
                adventurersTurn = false;
            }
            else { // Tour des Monstres
                std::cout << "** Tour des Monstres **" << std::endl;
                for (Monster* monster : monsters) {
                    if (!monster->isAlive()) continue;

                    // Logique de riposte des monstres - Utilisation des méthodes publiques
                    if (monster->shouldRetaliate()) {
                        monster->retaliate();
                    }

                    for (Adventurer* adventurer : adventurers) {
                        if (!adventurer->isAlive()) continue;
                        if (std::abs(monster->gridX - adventurer->gridX) <= monster->attackRange &&
                            std::abs(monster->gridZ - adventurer->gridZ) <= monster->attackRange) {
                            std::cout << monster->getClassName() << " à (" << monster->gridX << ", " << monster->gridZ << ") attaque " << adventurer->getClassName() << " à (" << adventurer->gridX << ", " << adventurer->gridZ << ")" << std::endl;
                            adventurer->takeDamage(monster->baseDamage, monster);
                        }
                    }
                }
                adventurersTurn = true;
            }
            lastTurnTime = currentTime;
        }


        // ---------- Suppression des personnages morts (Visuel) ----------
        std::vector<Adventurer*> nextAdventurers;
        for (Adventurer* adventurer : adventurers) {
            if (adventurer->isAlive()) {
                nextAdventurers.push_back(adventurer);
            }
            else {
                chessboardMap.removeCharacterFromMap(adventurer, adventurer->getGridX(), adventurer->getGridZ());
                delete adventurer;
            }
        }
        adventurers = nextAdventurers;

        std::vector<Monster*> nextMonsters;
        for (Monster* monster : monsters) {
            if (monster->isAlive()) {
                nextMonsters.push_back(monster);
            }
            else {
                // **SUPPRESSION DU PERSONNAGE DE LA CARTE AVANT DE LE SUPPRIMER - AJOUT DE LA SUPPRESSION DE LA CARTE**
                chessboardMap.removeCharacterFromMap(monster, monster->getGridX(), monster->getGridZ());
                delete monster;
            }
        }
        monsters = nextMonsters;

        if (adventurers.empty() || monsters.empty()) {
            gameRunning = false;
            if (adventurers.empty()) {
                std::cout << "Les Monstres Gagnent !" << std::endl;
            }
            else {
                std::cout << "Les Aventuriers Gagnent !" << std::endl;
            }
        }

        // Rendu des personnages (appel polymorphe) - RENDU SEULEMENT DES PERSONNAGES VIVANTS
        for (Adventurer* adventurer : adventurers) {
            adventurer->render(shaderProgram, camera);
        }
        for (Monster* monster : monsters) {
            monster->render(shaderProgram, camera);
        }
    }

    // ---------- Nettoyage ----------
    glDeleteProgram(shaderProgram);
    glfwTerminate();
    return 0;
}