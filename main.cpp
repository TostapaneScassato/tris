#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <ctime>
#include <ctype.h>
using namespace std;

// preprocessor
#ifdef __linux__
   #include <unistd.h>
   #include <thread>
   #include <time.h>

   #define Tdown  "┬"
   #define Tright "├"
   #define Tup    "┴"
   #define Tleft  "┤"

   //#define BottomLeft   "└"
   //#define BottomRight  "┘"
   //#define TopLeft      "┌"
   //#define TopRight     "┐"
   #define BottomLeft   "╰"
   #define BottomRight  "╯"
   #define TopLeft      "╭"
   #define TopRight     "╮"

   #define Vertical     "│"
   #define Orizzontal   "─"
   #define Cross        "┼"

   #define SYS_PAUSE() system("read -n 1 -p 'Press any key to continue...'");
   #define SYS_CLEAR() system("clear");
   #define SYS_SLEEP(duration_ms) std::this_thread::sleep_for(std::chrono::milliseconds(duration_ms));
#elif _WIN32
   #define byte win_byte_override
   #include <windows.h>
   #undef byte
   
   #define Tdown  char(194) // 194	├	Box Drawings Light Down And Horizontal
   #define Tright char(195) // 195	├	Box Drawings Light Vertical And Right
   #define Tup    char(193) // 193	┴	Box Drawings Light Up And Horizontal
   #define Tleft  char(180) // 180	┤	Box Drawings Light Vertical And Left
   
   #define BottomLeft  char(192) // 192	└	Box Drawings Light Up And Right
   #define BottomRight char(217) // 217	┘	Box Drawings Light Up And Left
   #define TopLeft     char(218) // 218	┌	Box Drawings Light Down And Right 
   #define TopRight    char(191) // 191	┐	Box Drawings Light Down And Left
   
   #define Vertical   char(179) // 179	│	Box Drawings Light Vertical
   #define Orizzontal char(196) // 196	─	Box Drawings Light Horizontal
   #define Cross      char(197) // 197	┼	Box Drawings Light Vertical And Horizontal

   #define SYS_PAUSE() system("pause");
   #define SYS_CLEAR() system("cls");
   #define SYS_SLEEP(duration_ms) Sleep(duration_ms);
#endif

// prototypes
bool cinGood();
void wait();

// constants
int intChoice;


// strutture
typedef struct {

   string date_wday, date_month, date_day, date_time, date_year;
   char loadedMatrix[9];
   int score1, score2, mode, move, difficulty;
} savedGame;

// constants
bool IN_ERROR = false;
const string SAVE_FILE = "savedGames.txt";

// classes
class Tris {
protected:
   char matrix[3][3]; // X or O
   int mode; //          1 -> bot;  2 -> human
   int difficulty=1; //  1 -> easy; 2 -> hard
   int player1=0, player2=0, move=0;
public:
   // constructors
   Tris() {
      /*
         the game has only one starting configuration: clear matrix and both scores to 0.
         therefore in this constructor I just have to clear the matrix of any dirty memory
         and reset any eventual left-over scores.
      */
      this->clearMatrix();
      this->resetScore();
      this->resetMoves();
   }
   // methods

   /* getters */
   /*int getPlayerScore(int player) { // Either 1 or 2
      return (player == 1)? player1 : player2;
   }*/
   int getDifficulty() { return this->difficulty; }
   int getMode()       { return this->mode; }
   int getMoves()      { return this->move; }

   /* setters */
   void makeMove(int player) {
      
      int pos;
      while (true) {
         do {
            cout << "Insert the cell number (1..9): ";   cin >> pos;
            if (!cinGood()) continue;
            
         } while (pos > 9 || pos < 1);
         
         pos--;   // account for user readability

         if (*(&matrix[0][0] + pos) != ' ') {
            cout << "[error] the cell is already occupied!" << endl;
            continue;
         }
         break;
      }

      *(&matrix[0][0] + pos) = (player == 1)? 'X' : 'O';
   }
   void botMove() {
      int pos;

      if (this->difficulty == 1) {  // completely random
         do {
            pos = rand()%9;
            
         } while (*(&matrix[0][0] + pos) != ' ');
         
         *(&matrix[0][0] + pos) = 'O';

      } else {//!\ ATTENTION /!\    I unfortunately had to use the help of Artificial Intelligence for this else statement. I am NOT proud of that :(

         pos = -1; // set to impossible value

         // if I can win, win
         for (int k = 0; k < 9; k++) {
            int row = k / 3;
            int col = k % 3;

            if (matrix[row][col] == ' ') {
               matrix[row][col] = 'O'; // simulate a possible move

               bool victory =
                        (matrix[row][ 0 ] == matrix[row][ 1 ] && matrix[row][ 0 ] == matrix[row][ 2 ] && matrix[row][ 2 ] != ' ') || // won in a row
                        (matrix[ 0 ][col] == matrix[ 1 ][col] && matrix[ 0 ][col] == matrix[ 2 ][col] && matrix[ 2 ][col] != ' ') || // won in a col
                        (matrix[ 0 ][ 0 ] == matrix[ 1 ][ 1 ] && matrix[ 0 ][ 0 ] == matrix[ 2 ][ 2 ] && matrix[ 2 ][ 2 ] != ' ') || // won on main diagonal
                        (matrix[ 0 ][ 2 ] == matrix[ 1 ][ 1 ] && matrix[ 0 ][ 2 ] == matrix[ 2 ][ 0 ] && matrix[ 2 ][ 0 ] != ' ');   // won on secondary diagonal

               matrix[row][col] = ' '; // just in case I didn't win, undo

               if (victory) {
                  pos = k;
                  break;
               }
            }
         }

         // if the player can win, block them
         if (pos == -1) {
            for (int k = 0; k < 9; k++) {
               int row = k / 3;
               int col = k % 3;

               if (matrix[row][col] == ' ') {
                  matrix[row][col] = 'X'; // simulate a possible move

                  bool victory =
                           (matrix[row][ 0 ] == matrix[row][ 1 ] && matrix[row][ 0 ] == matrix[row][ 2 ] && matrix[row][ 0 ] != ' ') || // won in a row
                           (matrix[ 0 ][col] == matrix[ 1 ][col] && matrix[ 0 ][col] == matrix[ 2 ][col] && matrix[ 0 ][col] != ' ') || // won in a col
                           (matrix[ 0 ][ 0 ] == matrix[ 1 ][ 1 ] && matrix[ 0 ][ 0 ] == matrix[ 2 ][ 2 ] && matrix[ 0 ][ 0 ] != ' ') || // won on main diagonal
                           (matrix[ 0 ][ 2 ] == matrix[ 1 ][ 1 ] && matrix[ 0 ][ 2 ] == matrix[ 2 ][ 0 ] && matrix[ 0 ][ 2 ] != ' ');   // won on secondary diagonal

                  matrix[row][col] = ' '; // I can't place another player's symbol

                  if (victory) {
                     pos = k;
                     break;
                  }
               }
            }
         }

         // take the center
         if (pos == -1 && (matrix[1][1] == ' ')) {
            pos = 4;
         }

         // take the corners
         if (pos == -1) {
            int corners[] = {0, 2, 6, 8};

            for (int i = 0; i < 4; i++) {
               int k = corners[i];

               if (matrix[k/3][k%3] == ' ') {
                  pos = k;
                  break;
               }
            }
         }

         // I'm cooked, just choose randomly
         if (pos == -1) {
            do {
               pos = rand()%9;
               
            } while (*(&matrix[0][0] + pos) != ' ');         
         }

         *(&matrix[0][0] + pos) = 'O';
      }
   }
   /*
   void incrementScore(int player) { // Either 1 or 2
      if (player == 3) return;
      (player == 1)? player1++ : player2++;
   }
   */
   void incrementMove() {
      this->move++;
   }
   void toggleDifficulty() {
      this->difficulty = (this->difficulty == 1)? 2 : 1;
   }
   void setMode(int mode) {
      (mode == 1)? this->mode = 1 : this->mode = 2;
   }

   void clearMatrix() {
      for (int i = 0; i < 3; i++) {
         for (int j = 0; j < 3; j++) {
            matrix[i][j] = ' ';
         }
      }
   }

   void resetScore() {
      player1 = 0;
      player2 = 0;
   }
   void resetMoves() {
      this->move = 0;
   }

   /* Return table:
      0 -> nothing;
      1 -> player 1 won;
      2 -> player 2 won;
      3 -> tie */
   int hasWon() {

      if ((matrix[0][0] == matrix[0][1]) && (matrix[0][0] == matrix[0][2]) && matrix[0][0] != ' ') { return (matrix[0][0] == 'X')? 1 : 2; }   // column 1
      if ((matrix[1][0] == matrix[1][1]) && (matrix[1][0] == matrix[1][2]) && matrix[1][0] != ' ') { return (matrix[1][0] == 'X')? 1 : 2; }   // column 2
      if ((matrix[2][0] == matrix[2][1]) && (matrix[2][0] == matrix[2][2]) && matrix[2][0] != ' ') { return (matrix[2][0] == 'X')? 1 : 2; }   // column 3
      if ((matrix[0][0] == matrix[1][0]) && (matrix[0][0] == matrix[2][0]) && matrix[0][0] != ' ') { return (matrix[0][0] == 'X')? 1 : 2; }   // row    1
      if ((matrix[0][1] == matrix[1][1]) && (matrix[0][1] == matrix[2][1]) && matrix[0][1] != ' ') { return (matrix[0][1] == 'X')? 1 : 2; }   // row    2
      if ((matrix[0][2] == matrix[1][2]) && (matrix[0][2] == matrix[2][2]) && matrix[0][2] != ' ') { return (matrix[0][2] == 'X')? 1 : 2; }   // row    3
      if ((matrix[0][0] == matrix[1][1]) && (matrix[0][0] == matrix[2][2]) && matrix[0][0] != ' ') { return (matrix[0][0] == 'X')? 1 : 2; }   // main diagonal
      if ((matrix[2][0] == matrix[1][1]) && (matrix[2][0] == matrix[0][2]) && matrix[2][0] != ' ') { return (matrix[2][0] == 'X')? 1 : 2; }   // secondary diagonal

      int flag=0;
      for (int i = 0; i < 3; i++) {
         for (int j = 0; j < 3; j++) {
            if (matrix[i][j] == ' ') {
               flag = 1;
               break;
            }
         }  if (flag) break;
      }  if (!flag) return 3;

      return 0;   // no one won, return 'false'
   }

   bool saveCurrentGame() {

      ofstream fout(SAVE_FILE, ios_base::app);

      if (!fout.is_open()) {
         cout << "[attention] could not find or create '" << SAVE_FILE << "'"; wait();
         return 0;
      }
      
      time_t timestamp; time(&timestamp); // get current time and date
      fout << ctime(&timestamp);

      for (int y = 0; y < 3; y++) {
         for (int x = 0; x < 3; x++) {
            (this->matrix[x][y] == ' ')? fout << 'E' : fout << this->matrix[x][y]; // if cell is empty, write down 'E'
            fout << " ";
         }
      }  fout << endl;
      
      fout << this->player1 << " " << this->player2 << " " << this->mode << " " << this->difficulty  << " " << this->move << endl << endl << endl;

      fout.close();
      return 1;
   }
   bool loadSavedGame() {
      ifstream fin(SAVE_FILE);

      if (!fin.is_open()) {
         cout << "[error] could not find '" << SAVE_FILE << "'"; wait();
         return 0;
      }

      int savedGames=0;
      savedGame games[999];

      while (fin >> games[savedGames].date_wday) { // read the first vaule to make sure there is actually something to read
         
         fin >> games[savedGames].date_month;      fin >> games[savedGames].date_day;
         fin >> games[savedGames].date_time;       fin >> games[savedGames].date_year;
         
         fin >> games[savedGames].loadedMatrix[0]; fin >> games[savedGames].loadedMatrix[1]; fin >> games[savedGames].loadedMatrix[2];
         fin >> games[savedGames].loadedMatrix[3]; fin >> games[savedGames].loadedMatrix[4]; fin >> games[savedGames].loadedMatrix[5];
         fin >> games[savedGames].loadedMatrix[6]; fin >> games[savedGames].loadedMatrix[7]; fin >> games[savedGames].loadedMatrix[8];
         
         fin >> games[savedGames].score1;
         fin >> games[savedGames].score2;
         fin >> games[savedGames].mode;
         fin >> games[savedGames].difficulty;
         fin >> games[savedGames].move;

         if (games[savedGames].mode == this->mode) savedGames++;

         if (savedGames > 999) {
            cout << "[attention] you have somehow saved more than a THOUSAND games. Go touch some grass"; wait();
            return 0;
         }
      }

      fin.close();

      SYS_CLEAR();
      if (!savedGames) {
         cout << "There are no saved games"; wait();
         return 0;
      }

      cout << "Found a total of " << savedGames << " saved ";
      (savedGames == 1)? cout << "game:" : cout << "games:"; cout << endl;

      for (int i = 0; i < savedGames; i++) {
         cout << "ID. " << right << setw(2) << i+1 << left << endl
         << "    Saved on " << games[i].date_day << " " << games[i].date_month << " " << games[i].date_year << "  " << games[i].date_time << endl
         << "    Mode: "; (games[i].mode == 1)? cout << "bot" : cout << "human"; cout << "  Scores: " << games[i].score1 << " v " << games[i].score2;
         cout << endl << endl;
      }
      
      do {
         intChoice = -1;
         cout << "Which game would you like to load? (1.." << savedGames << " - 0 to exit): ";
         cin >> intChoice;

         if (!cinGood()) continue;

      } while (intChoice < 0 || intChoice > savedGames);

      if (!intChoice) return 0;
      intChoice--;
      
      cout << "Loading the selected game"; wait();

      this->matrix[0][0] = (games[intChoice].loadedMatrix[0] == 'E')? ' ' : games[intChoice].loadedMatrix[0];
      this->matrix[1][0] = (games[intChoice].loadedMatrix[1] == 'E')? ' ' : games[intChoice].loadedMatrix[1];
      this->matrix[2][0] = (games[intChoice].loadedMatrix[2] == 'E')? ' ' : games[intChoice].loadedMatrix[2];
      this->matrix[0][1] = (games[intChoice].loadedMatrix[3] == 'E')? ' ' : games[intChoice].loadedMatrix[3];
      this->matrix[1][1] = (games[intChoice].loadedMatrix[4] == 'E')? ' ' : games[intChoice].loadedMatrix[4];
      this->matrix[2][1] = (games[intChoice].loadedMatrix[5] == 'E')? ' ' : games[intChoice].loadedMatrix[5];
      this->matrix[0][2] = (games[intChoice].loadedMatrix[6] == 'E')? ' ' : games[intChoice].loadedMatrix[6];
      this->matrix[1][2] = (games[intChoice].loadedMatrix[7] == 'E')? ' ' : games[intChoice].loadedMatrix[7];
      this->matrix[2][2] = (games[intChoice].loadedMatrix[8] == 'E')? ' ' : games[intChoice].loadedMatrix[8];

      this->player1      = games[intChoice].score1;
      this->player2      = games[intChoice].score2;
      this->difficulty   = games[intChoice].difficulty;
      this->move         = games[intChoice].move;

      return 1;
   }

   // printers
   void print() {
      cout << "Playing against a ";
      (this->mode == 1)? cout << "bot..." : cout << "human..."; cout << endl << endl;
      cout << "Move " << this->move << endl;
      /*cout << "Current points: | Move " << this->move << endl;
      cout << "- Player 1: " << this->player1 << endl;
      (this->mode == 1)? cout << "     - Bot: " : cout << "- Player 2: ";
      cout << this->player2 << endl << endl;*/
      
      cout << TopLeft << Orizzontal << Orizzontal << Orizzontal << Tdown << Orizzontal << Orizzontal << Orizzontal << Tdown << Orizzontal << Orizzontal << Orizzontal << TopRight << endl;
      cout << Vertical << " " << this->matrix[0][0] << " " << Vertical << " " << this->matrix[0][1] << " " << Vertical << " " << this->matrix[0][2] << " " << Vertical << endl;
      cout << Tright << Orizzontal << Orizzontal << Orizzontal << Cross << Orizzontal << Orizzontal << Orizzontal << Cross << Orizzontal << Orizzontal << Orizzontal << Tleft << endl;
      cout << Vertical << " " << this->matrix[1][0] << " " << Vertical << " " << this->matrix[1][1] << " " << Vertical << " " << this->matrix[1][2] << " " << Vertical << endl;
      cout << Tright << Orizzontal << Orizzontal << Orizzontal << Cross << Orizzontal << Orizzontal << Orizzontal << Cross << Orizzontal << Orizzontal << Orizzontal << Tleft << endl;
      cout << Vertical << " " << this->matrix[2][0] << " " << Vertical << " " << this->matrix[2][1] << " " << Vertical << " " << this->matrix[2][2] << " " << Vertical << endl;
      cout << BottomLeft << Orizzontal << Orizzontal << Orizzontal << Tup << Orizzontal << Orizzontal << Orizzontal << Tup << Orizzontal << Orizzontal << Orizzontal << BottomRight << endl << endl;
   }
};

// prototypes v2
void play(Tris &match);

// main function
int main() {
   SYS_CLEAR();
   srand(time(NULL));

   int menuChoice;
   int mode; // 1 -> bot; 2 -> human
   Tris match;

   do {
      cout << "Would you rather play against a bot (1) or a human (2)? ";
      cin >> mode;

      if (!cinGood()) continue;

   } while (mode != 1 && mode != 2);
   match.setMode(mode);

   do {
      SYS_CLEAR();

      cout << "Playing against a "; (mode == 1)? cout << "bot" : cout << "human"; cout << endl;
      cout << "What would you like to do?" << endl;
      cout << "[1] Start a new game;" << endl;
      cout << "[2] Load a saved game;" << endl;
      if (mode == 1) cout << "[3] Change the difficulty (currently: ";
      (match.getDifficulty() == 1)? cout << "1 - easy);" << endl : cout << "2 - hard);" << endl;
      cout << "[0] Exit the game." << endl << "> ";

      cin >> menuChoice;
      if (!cinGood()) { 
         menuChoice = -1;
         continue;
      }

      switch (menuChoice) {
      case 1:
         match.clearMatrix();
         match.resetMoves();
         play(match);
         break;

      case 2:
         if (match.loadSavedGame()) {
            cout << "Game loaded succesfully"; wait();
            play(match);
         }
         break;

      case 3:
         if (mode == 1) match.toggleDifficulty();
         else { cout << "[error] please choose a valid option"; wait(); }
         break;

      case 0:
         cout << "Bye bye!!" << endl;
         return 0;
         break;
      
      default:
         cout << "[error] please choose a valid option"; wait();
         break;
      }
   } while (menuChoice != 0);
   
}

// functions
bool cinGood() {
   if (!cin.good()) {
      cin.clear();
      cin.ignore(9999, '\n');
      return false;
   } else return true;
}
void wait() {
   SYS_SLEEP(300);
   cerr << ".";
   SYS_SLEEP(300);
   cerr << ".";
   SYS_SLEEP(300);
   cerr << ".";
   SYS_SLEEP(300);
   cout << endl;
}

void play(Tris &match) {
   int menuChoice, result;
   
   if (match.getMode() == 1) {   // PLAYING AGAINST A BOT
      do {
         SYS_CLEAR();
         match.print();
      
         cout << "What would you like to do?" << endl;
         cout << "[1] Make a move;" << endl;
         cout << "[2] Save the current match;" << endl;
         cout << "[0] Exit the match (all un-saved progress will be lost!)." << endl << "> ";

         cin >> menuChoice;
         if (!cinGood()) { 
            menuChoice = -1;
            continue;
         }

         switch (menuChoice) {
         case 1:
         
            match.makeMove(1);
         
            result = match.hasWon();
            if (result) {
               SYS_CLEAR();
               match.print();
               
               cout << endl;
               if (result == 1) cout << "Player 1 has won";
               if (result == 2) cout << "The bot has won";
               if (result == 3) cout << "There has been a tie";
               wait();

               SYS_PAUSE();
               return;
            }

            match.incrementMove();

            SYS_CLEAR();
            match.print();

            cout << "The bot is thinking"; wait();
            match.botMove();

            result = match.hasWon();
            if (result) {
               SYS_CLEAR();
               match.print();
               
               cout << endl;
               if (result == 1) cout << "Player 1 has won";
               if (result == 2) cout << "The bot has won";
               if (result == 3) cout << "There has been a tie";
               wait();

               SYS_PAUSE();
               return;
            }

            break;

         case 2:
            if (match.saveCurrentGame()) {
               cout << "Game saved succesfully"; wait();
               break;
            } else { cout << "[error] could not save the match"; wait(); }
            break;

         case 0:
            return;
         
         default:
            cout << "[error] please choose a valid option"; wait();
            break;
         }
      } while (menuChoice != 0);

   } else if (match.getMode() == 2) {  // PLAYING AGAINST A HUMAN
      do {
         SYS_CLEAR();
         match.print();
      
         cout << "What would you like to do?" << endl;
         cout << "[1] Make a move;" << endl;
         cout << "[2] Save the current match;" << endl;
         cout << "[0] Exit the match (all un-saved progress will be lost!)." << endl << "> ";

         cin >> menuChoice;
         if (!cinGood()) { 
            menuChoice = -1;
            continue;
         }

         switch (menuChoice) {
         case 1:
         
            if (match.getMoves()%2) {
               cout << "Player 2's move:" << endl;
               match.makeMove(2);
            } else {
               cout << "Player 1's move:" << endl;
               match.makeMove(1);
            }
            
            result = match.hasWon();
            if (result) {
               SYS_CLEAR();
               match.print();

               cout << endl;
               if (result == 1) cout << "Player 1 has won";
               if (result == 2) cout << "Player 2 has won";
               if (result == 3) cout << "There has been a tie";
               wait();

               SYS_PAUSE();
               return;
            }
            
            match.incrementMove();

            break;
            
         case 2:
            if (match.saveCurrentGame()) {
               cout << "Game saved succesfully"; wait();
               break;
            } else { cout << "[error] could not save the match"; wait(); }
            break;

         case 0:
            return;
         
         default:
            cout << "[error] please choose a valid option"; wait();
            break;
         }
      } while (menuChoice != 0);

   } else {
      cout << "[error] could not determine the playing mode"; wait();
      exit(1);
   }
}
