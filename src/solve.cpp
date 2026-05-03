#include"Game.h"
#include <algorithm>
#include <map>
#include <string>

#define MAX_ITERATIONS 10

namespace tms{
    void Game::solve(){
        while(solveIteration());
    }
    void Game::debugPrintGroups(const std::vector<SolveCell>& cells, const std::vector<Group>& groups){
        for(int i = 0; i < groups.size(); i++){
            std::cerr << "Group " << i << ": minMines = " << groups[i].minMines << ", maxMines = " << groups[i].maxMines << ", cells = ";
            for(int j : groups[i].cellIndices){
                std::cerr << "(" << cells[j].x << ", " << cells[j].y << ") ";
            }
            std::cerr << std::endl;
        }
    }
    bool Game::solveIteration(){
        std::vector<SolveCell> cells = getSolveCells();
        std::vector<Group> groups = getGroups(cells);

        bool addedMineCount = false;
        int iteration = 0;
        while(true){
            iteration++;
            if(iteration > MAX_ITERATIONS){
                return false;
            }
            // debugPrintGroups(cells, groups);
            if(makeObviusMoves(cells, groups)){
                return true;
                std::cerr << "Made obvius move in iteration " << iteration << std::endl;
            }
            if(!makeNewGroups(cells, groups)){
                if(!addedMineCount && cells.size() < 10){
                    addedMineCount = true;
                    addMineCountGroup(cells, groups);
                }else{
                    return false;
                }
            }
        }
        

        return 0;
    }
    std::vector<Game::SolveCell> Game::getSolveCells(){
        std::vector<SolveCell> solveCells;
        for(int i = 0; i < height; i++){
            for(int j = 0; j < width; j++){
                if(!cells[j][i].isRevealed && !cells[j][i].isFlagged){
                    SolveCell cell;
                    cell.x = i;
                    cell.y = j;
                    solveCells.push_back(cell);
                }
            }
        }
        return solveCells;
    }
    std::vector<Game::Group> Game::getGroups(std::vector<SolveCell>& cells){
        std::vector<Group> groups;
        std::map<std::pair<int,int>, SolveCell*> cellMap;
        for(int i = 0; i < cells.size(); i++){
            cellMap[{cells[i].x, cells[i].y}] = &cells[i];
        }
        for(int i = 0; i < height; i++){
            for(int j = 0; j < width; j++){
                if(this->cells[j][i].isRevealed && countAdjacentUnRevealed(i, j) > 0){
                    Group group;
                    group.minMines = countAdjacentMines(i, j) - countAdjacentFlags(i, j);
                    group.maxMines = group.minMines;
                    for(int k = -1; k <= 1; k++){
                        for(int l = -1; l <= 1; l++){
                            if(i + k < 0 || i + k >= height || j + l < 0 || j + l >= width || !(k || l) 
                                    || this->cells[j+l][i+k].isFlagged == true ||  this->cells[j+l][i+k].isRevealed == true) 
                                continue;
                            
                            auto it = cellMap.find({i + k, j + l});
                            if(it != cellMap.end()){
                                group.cellIndices.push_back(it->second - cells.data());
                                it->second->groups.push_back(groups.size());
                            }
                            else{
                                std::string errorMessage;
                                errorMessage += "Error: Cell at (" + std::to_string(i+k) + ", " + std::to_string(j+l) + ") not found in cellMap.\n";
                                errorMessage += "i = " + std::to_string(i) + ", j = " + std::to_string(j) + "\n";
                                errorMessage += "isFlagged = " + std::to_string(this->cells[j+l][i+k].isFlagged) + ", isRevealed = " + std::to_string(this->cells[j+l][i+k].isRevealed) + "\n";
                                throw std::runtime_error(errorMessage);
                            }
                        }
                    }
                    groups.push_back(group);
                }
            }
        }
        return groups;
    }
    bool Game::makeObviusMoves(std::vector<SolveCell>& cells, std::vector<Group>& groups){
        bool madeMove = false;
        for(int i = 0; i < groups.size(); i++){
            if(groups[i].maxMines == 0){
                for(int j : groups[i].cellIndices){
                    sweep(cells[j].x, cells[j].y);
                    madeMove = true;
                }
            }
            else if(groups[i].minMines == groups[i].cellIndices.size()){
                for(int j : groups[i].cellIndices){
                    setFlag(cells[j].x, cells[j].y, true);
                    madeMove = true;
                }
            }
        }
        return madeMove;
    }
    bool Game::makeNewGroups(std::vector<SolveCell>& cells, std::vector<Group>& groups){
        mergeIdenticalGroups(cells, groups);
        std::vector<Group> prevGroups = groups;
        for(int currentGroupIndex = 0; currentGroupIndex < groups.size(); currentGroupIndex++){
            std::vector<int> adjacentGroups;
            for(int i : groups[currentGroupIndex].cellIndices){
                for(int g : cells[i].groups){
                    if(g > currentGroupIndex) adjacentGroups.push_back(g);
                }
            }
            std::sort(adjacentGroups.begin(), adjacentGroups.end());
            adjacentGroups.erase(std::unique(adjacentGroups.begin(), adjacentGroups.end()), adjacentGroups.end());

            for(int otherGroupIndex : adjacentGroups){
                makeNewGroupsFromOverlap(cells, groups, currentGroupIndex, otherGroupIndex);
            }
        }
        mergeIdenticalGroups(cells, groups);
        return groups != prevGroups;
    }
    void Game::mergeIdenticalGroups(std::vector<SolveCell>& cells, std::vector<Group>& groups){
        for(Group& group : groups){
            std::sort(group.cellIndices.begin(), group.cellIndices.end());
        }
        sort(groups.begin(), groups.end(), [](const Group& a, const Group& b){
            return a.cellIndices < b.cellIndices;
        });
        std::vector<Group> mergedGroups(1, groups.front());
        for(int i = 1; i < groups.size(); i++){
            if(groups[i].cellIndices == mergedGroups.back().cellIndices){
                mergedGroups.back().minMines = std::max(mergedGroups.back().minMines, groups[i].minMines);
                mergedGroups.back().maxMines = std::min(mergedGroups.back().maxMines, groups[i].maxMines);
            }
            else{
                mergedGroups.push_back(groups[i]);
            }
        }
        groups = mergedGroups;
        fixCellGroups(cells, groups);
    }
    void Game::fixCellGroups(std::vector<SolveCell>& cells, const std::vector<Group>& groups){
        for(SolveCell& cell : cells){
            cell.groups.clear();
        }
        for(int i = 0; i < groups.size(); i++){
            // if(groups[i].cellIndices.size() > 10){
            //     std::string errorMessage;
            //     errorMessage += "Error: Group " + std::to_string(i) + " has too many cells (" + std::to_string(groups[i].cellIndices.size()) + ").\n";
            //     errorMessage += "minMines = " + std::to_string(groups[i].minMines) + ", maxMines = " + std::to_string(groups[i].maxMines) + "\n";
            //     errorMessage += "Cells: ";
            //     for(int j : groups[i].cellIndices){
            //         errorMessage += "(" + std::to_string(cells[j].x) + ", " + std::to_string(cells[j].y) + ") ";
            //     }
            //     throw std::runtime_error(errorMessage);
            // }
            for(int j : groups[i].cellIndices){
                cells[j].groups.push_back(i);
            }
        }
    }
    void Game::addMineCountGroup(std::vector<SolveCell>& cells, std::vector<Group>& groups){
        std::cerr << "\n";
        std::cerr << "Adding mine count group with " << flagsLeft << " mines left and " << cells.size() << " cells" << std::endl;
        Group group;
        int flagsLeft = countFlagsLeft();
        group.minMines = flagsLeft;
        group.maxMines = flagsLeft;
        for(int i = 0; i < cells.size(); i++){
            group.cellIndices.push_back(i);
        }
        groups.push_back(group);
        fixCellGroups(cells, groups);
    }
    void Game::makeNewGroupsFromOverlap(std::vector<SolveCell>& cells, std::vector<Group>& groups, int groupIndex1, int newGroupIndex){
        Group& group1 = groups[groupIndex1];
        Group& group2 = groups[newGroupIndex];
        std::vector<int> only1, only2, intersection, all;
        
        all = group1.cellIndices;
        all.insert(all.end(), group2.cellIndices.begin(), group2.cellIndices.end());
        std::sort(all.begin(), all.end());
        all.erase(std::unique(all.begin(), all.end()), all.end());

        for(int i : group1.cellIndices){
            if(!std::binary_search(group2.cellIndices.begin(), group2.cellIndices.end(), i) ){
                only1.push_back(i);
            }
            else{
                intersection.push_back(i);
            }
        }
        for(int i : group2.cellIndices){
            if(!std::binary_search(group1.cellIndices.begin(), group1.cellIndices.end(), i) ){
                only2.push_back(i);
            }
        }

        Group only1Group;
        only1Group.cellIndices = only1;
        Group only2Group;
        only2Group.cellIndices = only2;

        Group intersectionGroup;
        intersectionGroup.cellIndices = intersection;
        intersectionGroup.minMines = std::max({ 0, 
                group1.minMines - ((int)group1.cellIndices.size() - (int)intersection.size()),
                group2.minMines - ((int)group2.cellIndices.size() - (int)intersection.size()) 
            });
        only1Group.maxMines = std::min(group1.maxMines, group1.maxMines - intersectionGroup.minMines);
        only2Group.maxMines = std::min(group2.maxMines, group2.maxMines - intersectionGroup.minMines);

        intersectionGroup.maxMines = std::min({group1.maxMines, group2.maxMines, (int)intersection.size()});
        
        only1Group.minMines = std::max(0, group1.minMines - intersectionGroup.maxMines);
        only2Group.minMines = std::max(0, group2.minMines - intersectionGroup.maxMines);
        
        if(intersectionGroup.minMines > intersectionGroup.maxMines || only1Group.minMines > only1Group.maxMines || only2Group.minMines > only2Group.maxMines){
            std::string errorMessage;
            errorMessage += "Error: Invalid group created from overlap.\n";
            errorMessage += "group1: minMines = " + std::to_string(group1.minMines) + ", maxMines = " + std::to_string(group1.maxMines) + ", cells = ";
            for(int j : group1.cellIndices){
                errorMessage += "(" + std::to_string(cells[j].x) + ", " + std::to_string(cells[j].y) + ") ";
            }
            errorMessage += "\ngroup2: minMines = " + std::to_string(group2.minMines) + ", maxMines = " + std::to_string(group2.maxMines) + ", cells = ";
            for(int j : group2.cellIndices){
                errorMessage += "(" + std::to_string(cells[j].x) + ", " + std::to_string(cells[j].y) + ") ";
            }
            errorMessage += "\nintersectionGroup: minMines = " + std::to_string(intersectionGroup.minMines) + ", maxMines = " + std::to_string(intersectionGroup.maxMines) + ", cells = ";
            for(int j : intersectionGroup.cellIndices){
                errorMessage += "(" + std::to_string(cells[j].x) + ", " + std::to_string(cells[j].y) + ") ";
            }
            errorMessage += "\nonly1Group: minMines = " + std::to_string(only1Group.minMines) + ", maxMines = " + std::to_string(only1Group.maxMines) + ", cells = ";
            for(int j : only1Group.cellIndices){
                errorMessage += "(" + std::to_string(cells[j].x) + ", " + std::to_string(cells[j].y) + ") ";
            }
            errorMessage += "\nonly2Group: minMines = " + std::to_string(only2Group.minMines) + ", maxMines = " + std::to_string(only2Group.maxMines) + ", cells = ";
            for(int j : only2Group.cellIndices){
                errorMessage += "(" + std::to_string(cells[j].x) + ", " + std::to_string(cells[j].y) + ") ";
            }
            throw std::runtime_error(errorMessage);
        }
        // std::cerr << "Created new groups: \n" << to_string(group1,cells) << "\n" << to_string(group2, cells) << "\n" << to_string(intersectionGroup, cells) << "\n" << to_string(only1Group, cells) << "\n" << to_string(only2Group, cells) <<"\n"<< std::endl;

        groups.push_back(intersectionGroup);
        if(!only1Group.cellIndices.empty()){
            groups.push_back(only1Group);
        }
        if(!only2Group.cellIndices.empty()){
            groups.push_back(only2Group);
        }
    }
    std::string Game::to_string(Group& group, const std::vector<SolveCell>& cells){
        std::string result = "Group: minMines = " + std::to_string(group.minMines) + ", maxMines = " + std::to_string(group.maxMines) + ", cells = ";
        for(int j : group.cellIndices){
            result += "(" + std::to_string(cells[j].x) + ", " + std::to_string(cells[j].y) + ") ";
        }
        return result;
    }
}
