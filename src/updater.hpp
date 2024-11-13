#ifndef UPDATER_HPP
#define UPDATER_HPP

#include <vector>
#include <set>
#include <mutex>
#include <string>
#include <sstream>
#include <fstream>
#include "renderer.hpp"

struct Cell{
    int x;
    int y;

    Cell(int x, int y){
        this->x = x;
        this->y = y;
    }
};

bool operator<(const Cell& cell1, const Cell& cell2);

class Updater{
    public:
    inline static int COLUMNS = 80;
    inline static int ROWS = 80;
    inline static std::set<Cell> grid;
    inline static std::vector<int> vertices;
    inline static std::vector<unsigned int> indices;

    // Updater();
    static std::set<Cell> grid_from_file(std::string file_name);
    static void insertCell(const Cell& cell, const std::set<Cell>& grid, std::set<Cell>& next_grid);
    static void nextGeneration(std::set<Cell>& grid);
    static std::vector<int> make_vertices(std::set<Cell>& grid);
    static std::vector<unsigned int> make_indices(std::vector<int>& vertices);
    static void update();
};

#endif