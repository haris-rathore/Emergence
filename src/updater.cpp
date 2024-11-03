#include "updater.hpp"

bool operator<(const Cell& cell1, const Cell& cell2){
    if(cell1.y < cell2.y || (cell1.y == cell2.y && cell1.x < cell2.x)){
        return true;
    }
    else{
        return false;
    }
}

void Updater::insertCell(const Cell& cell, const std::set<Cell>& grid, std::set<Cell>& next_grid){
    int neighbors = 0;
    if(grid.find(Cell(cell.x - 1, cell.y + 1)) != grid.end()){
        neighbors++;
    }
    if(grid.find(Cell(cell.x, cell.y + 1)) != grid.end()){
        neighbors++;
    }
    if(grid.find(Cell(cell.x + 1, cell.y + 1)) != grid.end()){
        neighbors++;
    }
    if(grid.find(Cell(cell.x - 1, cell.y)) != grid.end()){
        neighbors++;
    }
    if(grid.find(Cell(cell.x + 1, cell.y)) != grid.end()){
        neighbors++;
    }
    if(grid.find(Cell(cell.x - 1, cell.y - 1)) != grid.end()){
        neighbors++;
    }
    if(grid.find(Cell(cell.x, cell.y - 1)) != grid.end()){
        neighbors++;
    }
    if(grid.find(Cell(cell.x + 1, cell.y - 1)) != grid.end()){
        neighbors++;
    }
    if(grid.find(cell) != grid.end()){
        if(neighbors == 2 || neighbors == 3){
            next_grid.insert(cell);
        }
    }
    else{
        if(neighbors == 3){
            next_grid.insert(cell);
        }
    }
}

void Updater::nextGeneration(std::set<Cell>& grid){
    std::set<Cell> next_grid;
    for(const Cell& cell : grid){
        insertCell(cell, grid, next_grid);
        insertCell(Cell(cell.x - 1, cell.y + 1), grid, next_grid);
        insertCell(Cell(cell.x, cell.y + 1), grid, next_grid);
        insertCell(Cell(cell.x + 1, cell.y + 1), grid, next_grid);
        insertCell(Cell(cell.x - 1, cell.y), grid, next_grid);
        insertCell(Cell(cell.x + 1, cell.y), grid, next_grid);
        insertCell(Cell(cell.x - 1, cell.y - 1), grid, next_grid);
        insertCell(Cell(cell.x, cell.y - 1), grid, next_grid);
        insertCell(Cell(cell.x + 1, cell.y - 1), grid, next_grid);
    }
    grid = next_grid;
}

std::vector<int> Updater::make_vertices(std::set<Cell>& grid){
    std::vector<int> vertices;
    for(const Cell& cell : grid){
        vertices.push_back(cell.x);
        vertices.push_back(cell.y);
        vertices.push_back(cell.x + 1);
        vertices.push_back(cell.y);
        vertices.push_back(cell.x);
        vertices.push_back(cell.y + 1);
        vertices.push_back(cell.x + 1);
        vertices.push_back(cell.y + 1);
    }
    return vertices;
}

std::vector<unsigned int> Updater::make_indices(std::vector<int>& vertices){
    std::vector<unsigned int> indices;
    for(int i = 0; i < vertices.size(); i+=4){
        indices.push_back(i);
        indices.push_back(i + 1);
        indices.push_back(i + 2);
        indices.push_back(i + 1);
        indices.push_back(i + 2);
        indices.push_back(i + 3);
    }
    return indices;
}

void Updater::update(){
    while(Renderer::running){
        if(Application::Queue.size() < 10){
            nextGeneration(grid);
            render_data to_push;
            to_push.vertices = make_vertices(grid);
            to_push.indices = make_indices(vertices);
            Application::Queue.push(to_push);
        }
    }
}