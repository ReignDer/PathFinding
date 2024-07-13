#include <iostream>
#include <string>
#include <vector>
#include <queue>
#include <fstream>

class Spelunky;

class Map {
private:
	std::fstream file;
	std::string text;
	int column;
	int row;
	std::queue <char>mapQueue;
	std::vector<std::vector<char>>mapMatrix;
	std::pair<int, int> exitCoordinate;

public:

	Map(std::string strName) {
		this->file.open(strName);
	}

	void readFile() {
		//this->file >> this->row;
		//std::cout << this->row << '\n';

		if (getline(this->file, this->text)) {
			for (int i = 0; i < this->text.size(); i++) {
				mapQueue.push(text[i]);
				this->column++;
			}
			this->row++;
		}

		//this->file >> this->column;
		//std::cout << this->column << '\n';

		while (getline(this->file, this->text)) {
			//std::cout << text << '\n';
			for (int i = 0; i < this->text.size(); i++) {
				mapQueue.push(text[i]);
			}
			this->row++;
		}
		file.close();

	}

	//Decided to make a 2D Array instead of a list
	void createMap(Spelunky& S);

	int getRow() {
		return this->row;
	}

	int getCol() {
		return this->column;
	}

	std::vector<std::vector<char>> getMapMatrix() {
		return this->mapMatrix;
	}

	std::pair<int, int> getExit() {
		return this->exitCoordinate;
	}

};

class Spelunky {
private:
	std::pair<int, int> coordinate;
	std::pair<int, int> spelunky;

	std::queue<std::pair<int, std::pair<int, int>>> priorityQ;
	std::vector<std::vector<int>>cost;
	std::vector<std::vector<bool>>visited;
	std::vector<std::vector<std::pair<int, int>>> parentNode;


	int currentCellX;
	int currentCellY;
	bool end = false;

	int damage = 0;
	int treasure = 0;
	int steps = 0;

	//directional vectors
	int dRow[4] = { 1,-1,0,0 };
	int dCol[4] = { 0,0,1,-1 };

public:
	Spelunky() {}

	void updateTreasureAndDamage(std::vector<std::vector<char>>tempMatrix, std::pair<int,int>parent) {
		if (tempMatrix[parent.first][parent.second] == 'm') {
			this->damage += 20;
		}

		if (tempMatrix[parent.first][parent.second] == 't') {
			this->treasure += 10;
		}
	}

	int distanceValue(std::pair<int,int> node, std::pair<int,int> exit) {
		return abs(node.first - exit.first) + abs(node.second - exit.second);
	}


	bool isValid(Map *map, std::vector<std::vector<bool>>visited, int currentCellX, int currentCellY) {
		std::vector<std::vector<char>>tempMatrix = map->getMapMatrix();

		//Out of Bounds
		if (currentCellX < 0 || currentCellY < 0
			|| currentCellX >= map->getRow() || currentCellY >= map->getCol())
			return false;
		//Visited
		if (visited[currentCellX][currentCellY])
			return false;
		//Impassible
		if (tempMatrix[currentCellX][currentCellY] == '*')
			return false;

		/*if(tempMatrix[currentCellX][currentCellY] == 'm')
			return false;*/

		//Not visited and passable
		return true;
	}

	void AStarSearchWithoutDMG(Map *map) {
		std::vector<std::vector<char>>tempMatrix = map->getMapMatrix();

		this->visited.resize(map->getRow(), std::vector<bool>(map->getCol()));
		this->parentNode.resize(map->getRow(), std::vector<std::pair<int,int>>(map->getCol()));
		this->cost.resize(map->getRow(), std::vector<int>(map->getCol(), INT_MAX));


		//Initialize visited to false
		for (int i = 0; i < map->getRow(); i++) {
			for (int j = 0; j < map->getCol(); j++) {
				visited[i][j] = false;
			}
		}


		//Starting position
		this->priorityQ.push({ 0, this->coordinate});
		
		visited[coordinate.first][coordinate.second] = true;
		cost[coordinate.first][coordinate.second] = 0;

		while (!this->priorityQ.empty() && !end) {
			std::pair<int, std::pair<int,int>> cell = priorityQ.front();
			int x = cell.second.first;
			int y = cell.second.second;
			int c = -cell.first;

			priorityQ.pop();
			//std::cout << tempMatrix[x][y];

			if (x == map->getExit().first && y == map->getExit().second) { //End traversal
				spelunky = { x,y }; //Position of Bomberman
				end = true;
			}

			for (int i = 0; i < 4; i++) {
				currentCellX = x + dRow[i]; //Check cell above or below
				currentCellY = y + dCol[i]; // Check cell to the right or left
				int newCost = c + 1;


				if (isValid(map, visited, currentCellX, currentCellY)) {

					if (tempMatrix[currentCellX][currentCellY] == 'm') {
						cost[currentCellX][currentCellY] = 100;
						newCost = distanceValue({ currentCellX, currentCellY }, map->getExit()) + cost[currentCellX][currentCellY];
					}
						
					else if(tempMatrix[currentCellX][currentCellY] == 't')
						newCost = distanceValue({ currentCellX, currentCellY }, map->getExit()) - 10;
					else
						newCost = distanceValue({ currentCellX, currentCellY }, map->getExit());

					if (newCost < cost[currentCellX][currentCellY]) {
						cost[currentCellX][currentCellY] = newCost;
						priorityQ.push({ -newCost, {currentCellX,currentCellY } });//queue current cell
						//std::cout << "XY: (" << currentCellX << " " << currentCellY << ") - " << newCost << " - " << cost[currentCellX][currentCellY] << '\n';
						visited[currentCellX][currentCellY] = true; // current cell is visited
						parentNode[currentCellX][currentCellY] = { x,y }; //Parent of current cell
						//std::cout << "Parent Node: " << parentNode[8][6].first << "," << parentNode[8][6].second << '\n';
					}
					
				}
					
			}
		}
	}

	void AStarSearchWithDMG(Map* map) {
		std::vector<std::vector<char>>tempMatrix = map->getMapMatrix();

		this->visited.resize(map->getRow(), std::vector<bool>(map->getCol()));
		this->parentNode.resize(map->getRow(), std::vector<std::pair<int, int>>(map->getCol()));
		this->cost.resize(map->getRow(), std::vector<int>(map->getCol(), INT_MAX));


		//Initialize visited to false
		for (int i = 0; i < map->getRow(); i++) {
			for (int j = 0; j < map->getCol(); j++) {
				visited[i][j] = false;
			}
		}


		//Starting position
		this->priorityQ.push({ 0, this->coordinate });

		visited[coordinate.first][coordinate.second] = true;
		cost[coordinate.first][coordinate.second] = 0;

		while (!this->priorityQ.empty() && !end) {
			std::pair<int, std::pair<int, int>> cell = priorityQ.front();
			int x = cell.second.first;
			int y = cell.second.second;
			int c = -cell.first;

			priorityQ.pop();
			//std::cout << tempMatrix[x][y];

			if (x == map->getExit().first && y == map->getExit().second) { //End traversal
				spelunky = { x,y }; //Position of Bomberman
				end = true;
			}

			for (int i = 0; i < 4; i++) {
				currentCellX = x + dRow[i]; //Check cell above or below
				currentCellY = y + dCol[i]; // Check cell to the right or left
				int newCost = c + 1;


				if (isValid(map, visited, currentCellX, currentCellY)) {

					if (tempMatrix[currentCellX][currentCellY] == 'm') {
						newCost = distanceValue({ currentCellX, currentCellY }, map->getExit()) + 100;
					}

					else if (tempMatrix[currentCellX][currentCellY] == 't')
						newCost = distanceValue({ currentCellX, currentCellY }, map->getExit()) - 10;
					else
						newCost = distanceValue({ currentCellX, currentCellY }, map->getExit());

					if (newCost < cost[currentCellX][currentCellY]) {
						cost[currentCellX][currentCellY] = newCost;
						priorityQ.push({ -newCost, {currentCellX,currentCellY } });//queue current cell
						//std::cout << "XY: (" << currentCellX << " " << currentCellY << ") - " << newCost << " - " << cost[currentCellX][currentCellY] << '\n';
						visited[currentCellX][currentCellY] = true; // current cell is visited
						parentNode[currentCellX][currentCellY] = { x,y }; //Parent of current cell
						//std::cout << "Parent Node: " << parentNode[8][6].first << "," << parentNode[8][6].second << '\n';
					}

				}

			}
		}
	}

	void displayPath(Map* map) {
		std::pair<int, int> current = spelunky;
		std::pair<int, int> parent = parentNode[current.first][current.second];
		std::vector<std::vector<char>>tempMatrix = map->getMapMatrix();


		while (parentNode[current.first][current.second] != this->coordinate) {

			parent = parentNode[current.first][current.second];

			if (parent.first > current.first) {
				updateTreasureAndDamage(tempMatrix,parent);
				tempMatrix[parent.first][parent.second] = 'X';
				this->steps++;
			}
			else if (parent.first < current.first) {
				updateTreasureAndDamage(tempMatrix,parent);
				tempMatrix[parent.first][parent.second] = 'X';
				this->steps++;
			}
			else if (parent.second > current.second) {
				updateTreasureAndDamage(tempMatrix,parent);
				tempMatrix[parent.first][parent.second] = 'X';
				this->steps++;
			}
			else if (parent.second < current.second) {
				updateTreasureAndDamage(tempMatrix,parent);
				tempMatrix[parent.first][parent.second] = 'X';
				this->steps++;
			}

			current = parent;
		}

		//Final step
		parent = parentNode[current.first][current.second];
		if (parent.first > current.first) {
			this->steps++;
		}
		else if (parent.first < current.first) {
			this->steps++;
		}
		else if (parent.second > current.second) {
			this->steps++;
		}
		else if (parent.second < current.second) {
			this->steps++;
		}

		std::ofstream fileout("spelunky_out.txt");

		std::cout << "2.) Optimal route from start to exit with damage" << '\n';

		fileout << "2.) Optimal route from start to exit with damage"<<'\n';

		//Print map
		for (int i = 0 ; i< map->getRow(); i++) {
			for (int j = 0; j < map->getCol(); j++) {
				std::cout << tempMatrix[i][j];
				fileout << tempMatrix[i][j];
			}
			std::cout << '\n';
			fileout << '\n';
		}

		fileout << "Steps: " << this->steps << '\n';
		std::cout << "Steps: " << this->steps << '\n';
		fileout << "Treasures obtained: " << this->treasure << '\n';
		std::cout << "Treasures obtained: " << this->treasure << '\n';
		fileout << "Damage: " << this->damage << "hp" << '\n';
		std::cout << "Damage: " << this->damage << "hp" << '\n' <<'\n';


		fileout.close();

	}

	void setCoordinate(int row, int column) {
		this->coordinate.first = row;
		this->coordinate.second = column;
	}

};

void Map::createMap(Spelunky& S) {
	this->mapMatrix.resize(this->row, std::vector<char>(this->column));

	for (int i = 0; i < this->row; i++) {
		for (int j = 0; j < this->column; j++) {
			this->mapMatrix[i][j] = mapQueue.front();

			if (this->mapMatrix[i][j] == 'S') {
				S.setCoordinate(i, j);
			}

			if (this->mapMatrix[i][j] == 'E') {
				this->exitCoordinate.first = i;
				this->exitCoordinate.second = j;
			}

			this->mapQueue.pop();
		}
	}

	//Print map
	/*for (int i = 0; i < this->row; i++) {
		for (int j = 0; j < this->column; j++)
			std::cout << this->mapMatrix[i][j];
		std::cout << '\n';
	}*/
}

int main() {

	Map map("Spelunky.txt");
	Spelunky S;
	map.readFile();
	map.createMap(S);
	S.AStarSearchWithDMG(&map);
	S.displayPath(&map);
	
	//There are 2 possible shortest path form E to B

	return 0;
}