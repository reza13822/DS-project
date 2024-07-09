#include <iostream>
#include <vector>
#include <fstream>
#include <chrono>
#include <stack>
#include <algorithm>
#include <cstdlib>
#include <ctime>


using namespace std;
using namespace chrono;

const int WIDTH = 20; // satr
const int HEIGHT = 10; // soton

struct Record // baray sabt record
{
    string name;
    int time;
    int stops;
    int point;

    bool operator<(const Record& other) const
    {
        if (time == other.time)
        {
            return stops < other.stops;
        }
        return time < other.time;
    }
};

class Maze
{
    private:

        vector<vector<char>> grid; // matris masir
        pair<int, int> playerPos; // mokhtasat player 
        pair<int, int> endPos; // mokhtasat noghte payan
        vector<Record> records; // vectory as record ha
        stack<pair<int, int>> previousMoves; // vector baraye save move ghabli

        void loadRecords()
        {
            ifstream file("records.txt");
            if (file.is_open())
            {
                Record record;
                while (file >> record.name >> record.time >> record.stops >> record.point)
                {
                    records.push_back(record); // read as file
                }
                file.close();
            }
        }

        void saveRecords()
        {
            ofstream file("records.txt");
            if (file.is_open())
            {
                for (const auto& record : records)
                {
                    file << record.name << ' ' << record.time << ' ' << record.stops << ' ' << record.point << endl; // write to file
                }
                file.close();
            }
        }

    public:
        Maze() //constructor
        {
            grid = vector<vector<char>>(HEIGHT, vector<char>(WIDTH, ' '));
        
            for (int i = 0; i < HEIGHT; ++i)
            {
                for (int j = 0; j < WIDTH; ++j)
                {
                    if (i == 0 || j == 0 || i == HEIGHT-1 || j == WIDTH-1 || (i % 2 == 0 && j % 2 == 0))
                    {
                        grid[i][j] = '#';
                    }
                }
            }
            playerPos = {1, 1};
            endPos = {HEIGHT-2, WIDTH-2};
            grid[playerPos.first][playerPos.second] = 'P';
            grid[endPos.first][endPos.second] = '*';

            loadRecords();
        }

        void display()
        {
            for (const auto& row : grid)
            {
                for (char cell : row)
                {
                    cout << cell << ' ';
                }
                cout << endl;
            }
        }

        bool movePlayer(char direction)
        {
            grid[playerPos.first][playerPos.second] = ' ';
            previousMoves.push(playerPos); // Push current position to stack
            switch (direction)
            {
                case 'w': playerPos.first--; break;
                case 's': playerPos.first++; break;
                case 'a': playerPos.second--; break;
                case 'd': playerPos.second++; break;
                default: break;
            }
            if (grid[playerPos.first][playerPos.second] == '#') 
            { // move eshtebah -> bargasht be posishen ghabli
                 switch (direction)
                {
                    case 'w': playerPos.first++; break;
                    case 's': playerPos.first--; break;
                    case 'a': playerPos.second++; break;
                    case 'd': playerPos.second--; break;
                    default: break;
                }
                grid[playerPos.first][playerPos.second] = 'P';
                previousMoves.pop(); 
                return false;
            }
            grid[playerPos.first][playerPos.second] = 'P';
            return true;
        }

        bool isFinished()
        {
            return playerPos == endPos;
        }

        int calculatePoints(int time, int stops)
        {
            if(time > 0 && time <= 10)
            {
                return (time *1000) - (stops * 3);
            }
            else if( time > 10 && time <= 25)
            {
                return (time * 8) - (stops * 5);
            }else
            {   
                return time - (stops * 5);
            }

        }


        void undoMove()
        {
            if (!previousMoves.empty())
            {
                grid[playerPos.first][playerPos.second] = ' ';
                playerPos = previousMoves.top();
                previousMoves.pop();
                grid[playerPos.first][playerPos.second] = 'P';
            }
        }

        void addRecord(const string& name, int time, int stops)
        {
            int points = calculatePoints(time, stops);
            records.push_back({name, time, stops, points});
            sort(records.begin(), records.end());
            saveRecords();
        }

        void showRecords()
        {
            cout << "Records:\n";
            for (const auto& record : records)
            {
                cout << "Name: " << record.name << endl << "Time: " << record.time << "s" << endl;
                cout << "Stops: " << record.stops << endl << "Points: " << record.point << endl;
                cout <<"______________________________________________\n";
            }
        }

};

int main()
{
    Maze maze;
    string playerName;
    int stops = 0;

    cout << "Enter your name: ";
    cin >> playerName;

    auto start = high_resolution_clock::now(); // return time now
    
    while (!maze.isFinished())
    {
        system("clear"); 
        maze.display();
        cout << "Use WASD to move (w=up, a=left, s=down, d=right), U to undo last move: ";
        char move;
        cin >> move;
        if (move == 'u')
        {
            maze.undoMove();
        } else if (!maze.movePlayer(move))
        {
            stops++; // +1 baraye move eshtebah
        }
    }

    auto end = high_resolution_clock::now(); // return time now
    int duration = duration_cast<seconds>(end - start).count(); // mohasebe time 

    maze.addRecord(playerName, duration, stops);

    cout << "Congratulations, " << playerName << "! You finished the maze in " << duration << " seconds with " << stops << " stops." << endl;

    maze.showRecords();

    return 0;
}
