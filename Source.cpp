#include "olcPixelGameEngine.h"
static int screenW = 410, screenH = 300;
enum GAMESTAGE { START, PLAY, SCORE, EXIT };

class Example : public olc::PixelGameEngine
{
public:

	int m_x = 0, m_y = 0, 
		BoardDist = 10, BoardSize = 6, LineLength = 20, n_LinesLeft = 40, n_Boxes = 16, n_p1Boxes = 0, n_p2Boxes = 0;
	bool m_mouseReleased = false, Playerturn = true, hasPlayed = false;
	GAMESTAGE l_gamestage = START;
	






	//this struct is used to see if we mouse over a line and tell the system if we are.
	struct lineId {
		int id = 0, x = 0, y = 0;
		bool isHorizontal = false , isHit = false, isMarked = false;
	};




	// this class is made to store all board infomation and handle hit tests with lines as well as calculate boxes.
	class m_board {
	private:
		int n_Dots = 4, l_boxSize = 5, l_offSet = 0;
		//vectors are used so we can change the size of the board
		std::vector<int> l_boxes;
		std::vector<bool> l_vLines;
		std::vector<bool> l_hLines;

	public:

		//SET UP THE BOARD, 
		//Making boxes as integers (to store player id value if won), horizontal lines and vertical lines as booleans to see if they have been drawn.
		m_board(int size, int boxPixelSize, int offSet) : n_Dots(size), l_boxSize(boxPixelSize), l_offSet(offSet){
			for (int i = 0; i < n_Dots; i++) {
				for (int k = 0; k < n_Dots; k++) {
					if (i < n_Dots - 1) {
						//as long as we are not at the last i, add a non marked line to the horizontal lines.
						l_hLines.push_back(0);
						if (k < n_Dots - 1) {
							//as long as we are not at the last i or last k, add a box with the int value 0 (no player id).
							l_boxes.push_back(0);
						}
					}

					if (k < n_Dots - 1) {
						//as long as we are not at the last k, add a non marked line to the vertical lines.
						l_vLines.push_back(0);
					}

				}
			}
		}

		bool clear() {
			n_Dots = 0;
			l_boxSize = 0;
			l_offSet = 0;
			l_boxes.clear();
			l_hLines.clear();
			l_vLines.clear();
			return true;
		}

		// Get teh size of the board
		int getSize() {
			return n_Dots;
		}

		int getLineLength() {
			return l_boxSize;
		}

		//See if any lines are moused over (Hit test) and return the line Id if it is.
		bool getLineHit(int xPos, int yPos, lineId &hitLine) {
			

			//See if the mouse is within the board, if not, it is not over any lines.
			if (xPos < l_boxSize * (n_Dots) && yPos < l_boxSize * (n_Dots)) {
				int LineX = hitLine.x * l_boxSize + l_offSet;
				int LineY = hitLine.y * l_boxSize + l_offSet;
				if (hitLine.isHorizontal && xPos < LineX + l_boxSize - 5 && xPos > LineX + 5 && yPos < LineY + 3 && yPos > LineY - 3) {
					return true;
				}
				else if (!hitLine.isHorizontal && yPos < LineY + l_boxSize - 5 && yPos > LineY + 5 && xPos < LineX + 3 && xPos > LineX - 3) {
					return true;
				}
			}

			return false;
		}
		bool isEnclosed(int boxId) {
			int linesOk = 0;
			int id1, id2, id3, id4, boxX = 0, boxY = 0, count = boxId;
			while (count >= (n_Dots - 1)) {
				count -= (n_Dots - 1);
				boxY += 1;
			}
			boxX = count;

			id1 = boxY * (n_Dots - 1) + boxX;
			id2 = (boxY+1) * (n_Dots - 1) + boxX;
			id3 = boxX * (n_Dots - 1) + boxY;
			id4 = (boxX+1) * (n_Dots - 1) + boxY;


			if (l_hLines.at(id1)) linesOk += 1;
			if (l_hLines.at(id2)) linesOk += 1;
			if (l_vLines.at(id3)) linesOk += 1;
			if (l_vLines.at(id4)) linesOk += 1;

			if (linesOk == 4) return true;
			return false;
		}

		//Lets see if we make a new box
		bool makesBox(lineId lineToTest, bool &player, int &l_p1Boxes, int &l_p2Boxes) {
			int boxesSet = 0;
			int box1Id;
			int box2Id;

			if (lineToTest.isHorizontal) {
				box1Id = lineToTest.y * (n_Dots - 1) + lineToTest.x;
				box2Id = (lineToTest.y - 1) * (n_Dots - 1) + lineToTest.x;
			}
			else {
				box1Id = lineToTest.y * (n_Dots - 1) + lineToTest.x;
				box2Id = (lineToTest.y) * (n_Dots - 1) + (lineToTest.x - 1);
			}

			if ((lineToTest.isHorizontal && lineToTest.y < n_Dots - 1 && lineToTest.y > 0) || (!lineToTest.isHorizontal && lineToTest.x < n_Dots - 1 && lineToTest.x > 0) ){
				if (isEnclosed(box1Id)) {
					l_boxes.at(box1Id) = (int)!player + 1;
					boxesSet += 1;
				}

				if (isEnclosed(box2Id)) {
					l_boxes.at(box2Id) = (int)!player + 1;
					boxesSet += 1;
				}
				
			}
			else if ((lineToTest.isHorizontal && lineToTest.y < n_Dots - 1) ||  (!lineToTest.isHorizontal && lineToTest.x < n_Dots - 1)){
				if (isEnclosed(box1Id)) {
					l_boxes.at(box1Id) = (int)!player + 1;
					boxesSet += 1;
				}
			}
			else if ((lineToTest.isHorizontal && lineToTest.y > 0) || (!lineToTest.isHorizontal && lineToTest.x > 0)) {
				if (isEnclosed(box2Id)) {
					l_boxes.at(box2Id) = (int)!player + 1;
					boxesSet += 1;
				}
			}
			if (player) l_p1Boxes += boxesSet;
			else l_p2Boxes += boxesSet;

			if (boxesSet > 0) return true;
			return false;
		}

		bool Update(lineId &LineToDraw, olc::Pixel &col, int mouseX, int mouseY, bool &isPressed, int &n_lines, bool &player, int &l_p1Boxes, int &l_p2Boxes) {
			int id = LineToDraw.id;
			bool isMarked = false;
			if(LineToDraw.isHorizontal) LineToDraw.isMarked = l_hLines.at(LineToDraw.id);
			else LineToDraw.isMarked = l_vLines.at(LineToDraw.id);

			isMarked = LineToDraw.isMarked;
			if (getLineHit(mouseX, mouseY, LineToDraw)) {
				if (isMarked) {
					col.r = 255;
					col.g = 0;
					col.b = 0;
					return false;
				}
				else {
					if (isPressed) {
						col.r = 200;
						col.g = 50;
						col.b = 200;
						if (LineToDraw.isHorizontal)l_hLines.at(LineToDraw.id) = true;
						else l_vLines.at(LineToDraw.id) = true;
						isPressed = false;
						n_lines -= 1;
						if (makesBox(LineToDraw, player, l_p1Boxes, l_p2Boxes)) return false;
						return true;
					}
					else {
						col.r = 0;
						col.g = 255;
						col.b = 0;
						return false;
					}
				}
			}
			else if (isMarked) {
				col.r = 255;
				col.g = 255;
				col.b = 255;
				return false;
			}
			else {
				col.r = 0;
				col.g = 0;
				col.b = 0;
			}
			return false;
		}

		bool isFieldMarked(int x, int y, olc::Pixel &col) {
			if (l_boxes.at(y * (n_Dots - 1) + x) == 0) {
				col.r = 100;
				col.g = 100;
				col.b = 100;
			}
			else if (l_boxes.at(y * (n_Dots - 1) + x) == 1) {
				col.r = 220;
				col.g = 80;
				col.b = 80;
			}
			else if (l_boxes.at(y * (n_Dots - 1) + x) == 2) {
				col.r = 80;
				col.g = 220;
				col.b = 80;
			}
			
			return true;
		}

	};









	Example()
	{
		sAppName = "Dots and Lines";
	}

	void SetUpBoard(int l_boardSize) {
		BoardSize = l_boardSize;
		if (hasPlayed)l_board->clear();
		l_board = new m_board(BoardSize, LineLength, BoardDist);
		n_LinesLeft = BoardSize * (BoardSize - 1) * 2;
		n_Boxes = (BoardSize - 1) * (BoardSize - 1);
		n_p1Boxes = 0;
		n_p2Boxes = 0;
		hasPlayed = true;
	}


	bool RunGame() {
		m_x = GetMouseX();
		m_y = GetMouseY();
		m_mouseReleased = false;
		if (GetMouse(0).bReleased) m_mouseReleased = true;

		//clear the screen
		Clear(olc::DARK_BLUE);
		//Lets update and draw the board

		SetPixelMode(olc::Pixel::NORMAL);
		int boardSize = l_board->getSize();
		int LineLength = l_board->getLineLength();
		olc::Pixel col;
		col.r = 0;
		col.g = 0;
		col.b = 0;
		col.a = 0;
		for (int x = 0; x < boardSize; x++) {
			for (int y = 0; y < boardSize; y++) {

				//Draw and update the horizontal lines
				if (x < boardSize - 1) {
					lineId lineToTest;
					lineToTest.id = y * (boardSize - 1) + x;
					lineToTest.x = x;
					lineToTest.y = y;
					lineToTest.isHorizontal = true;

					if (l_board->Update(lineToTest, col, m_x, m_y, m_mouseReleased, n_LinesLeft, Playerturn, n_p1Boxes, n_p2Boxes)) Playerturn = !Playerturn;
					DrawLine(x * LineLength + BoardDist, y * LineLength + BoardDist, (x + 1) * LineLength + BoardDist, y* LineLength + BoardDist, olc::Pixel(col.r, col.g, col.b));

				}
				//Draw and update the vertical lines
				if (y < boardSize - 1) {
					lineId lineToTest;
					lineToTest.id = x * (boardSize - 1) + y;
					lineToTest.x = x;
					lineToTest.y = y;
					lineToTest.isHorizontal = false;

					if (l_board->Update(lineToTest, col, m_x, m_y, m_mouseReleased, n_LinesLeft, Playerturn, n_p1Boxes, n_p2Boxes)) Playerturn = !Playerturn;
					DrawLine(x * LineLength + BoardDist, y * LineLength + BoardDist, x * LineLength + BoardDist, (y + 1)* LineLength + BoardDist, olc::Pixel(col.r, col.g, col.b));


					//If a box is marked, draw it in its team color
					if (x < boardSize - 1) {
						if (l_board->isFieldMarked(x, y, col)) FillRect(x * LineLength + BoardDist + 3, y * LineLength + BoardDist + 3, LineLength - 5, LineLength - 5, olc::Pixel(col.r, col.g, col.b));
					}
				}



				//Draw all the dots
				FillCircle(x * LineLength + BoardDist, y * LineLength + BoardDist, 2, olc::WHITE);
			}
		}

		//lets draw some information:
		std::string printOutLines("Number of lines left: ");
		printOutLines += std::to_string(n_LinesLeft);

		std::string printOutBoxes("Number of boxes left: ");
		printOutBoxes += std::to_string(n_Boxes - (n_p1Boxes + n_p2Boxes));

		std::string printOutTurn("Player ");
		if (Playerturn) printOutTurn += std::to_string(1);
		else printOutTurn += std::to_string(2);
		printOutTurn += "'s turn";

		std::string printOutP1Points("Player 1:");
		printOutP1Points += std::to_string(n_p1Boxes);
		std::string printOutP2Points("Player 2:");
		printOutP2Points += std::to_string(n_p2Boxes);

		DrawString(boardSize * LineLength, BoardDist, printOutLines, olc::WHITE, 1);
		DrawString(boardSize * LineLength, BoardDist * 2, printOutBoxes, olc::WHITE, 1);
		if (Playerturn) DrawString(boardSize * LineLength, BoardDist * 3, printOutTurn, olc::DARK_RED, 1);
		else  DrawString(boardSize * LineLength, BoardDist * 3, printOutTurn, olc::DARK_GREEN, 1);
		DrawString(boardSize * LineLength, BoardDist * 5, printOutP1Points, olc::DARK_RED, 1);
		DrawString(boardSize * LineLength, BoardDist * 6, printOutP2Points, olc::DARK_GREEN, 1);

		std::string printOutMessage("\n\n\n\n\nPress R to restart \n\nor ESCAPE to exit");
		DrawString(boardSize * LineLength, BoardDist * 10, printOutMessage, olc::WHITE, 1);
		if (GetKey(olc::Key::ESCAPE).bPressed) {
			l_gamestage = EXIT;
		}

		//Cheat key, R to go to SCORE screen
		if (GetKey(olc::Key::R).bPressed) l_gamestage = SCORE;
		else if(n_LinesLeft == 0) l_gamestage = SCORE;
		return false;
	}

	bool SetUpGame() {
		//clear the screen
		Clear(olc::DARK_BLUE);
		//Lets update and draw the board

		SetPixelMode(olc::Pixel::NORMAL);

		std::string LineToPrint("CHOSE BOARD SIZE");

		DrawString(20, 20, LineToPrint, olc::WHITE, 1);
		if (GetKey(olc::Key::UP).bPressed) BoardSize-= 2;
		else if (GetKey(olc::Key::DOWN).bPressed) BoardSize+= 2;
		if (BoardSize < 4) BoardSize = 10;
		else if (BoardSize > 10) BoardSize = 4;
		FillRect(18, 38 + (16 * ((BoardSize - 4)/2)), 130, 11, olc::DARK_GREY);
		std::string LineToPrintSizes(" 4 By 4 dots \n\n 6 By 6 dots \n\n 8 By 8 dots\n\n 10 By 10 dots\n\n\n\n Press SPACE to play.\n\n or ESCAPE to exit");
		DrawString(20, 40, LineToPrintSizes, olc::WHITE, 1);

		DrawRect(18, 38 + (16*((BoardSize - 4) / 2)), 130, 11, olc::WHITE);
		
		if (GetKey(olc::Key::SPACE).bReleased) return true;
		if (GetKey(olc::Key::ESCAPE).bPressed) {
			l_gamestage = EXIT;
			return false;
		}
		return false;
	}


	bool ShowScore() {
		//clear the screen
		Clear(olc::DARK_BLUE);
		//Lets update and draw the board

		SetPixelMode(olc::Pixel::NORMAL);
		std::string printOutWinner("Player ");
		if(n_p1Boxes > n_p2Boxes) printOutWinner += std::to_string(1) + " Wins with a total of " + std::to_string(n_p1Boxes) + " Boxes \n\n\n\n Press SPACE to restart!\n\n or ESCAPE to exit";
		else printOutWinner += std::to_string(2) + " Wins with a total of " + std::to_string(n_p2Boxes) + " Boxes \n\n\n\n Press SPACE to restart!\n\n or ESCAPE to exit";
		DrawString(20, 40, printOutWinner, olc::WHITE, 1);
		if (GetKey(olc::Key::ESCAPE).bPressed) {
			l_gamestage = EXIT;
			return false;
		}
		if (GetKey(olc::Key::SPACE).bReleased) return true;
		return false;
	}
public:

	m_board *l_board;

	bool OnUserCreate() override
	{
		// Called once at the start, so create things here
		
		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{
		
		if (l_gamestage == START) if (SetUpGame()) {
			SetUpBoard(BoardSize);
			l_gamestage = PLAY;
		}
		if (l_gamestage == PLAY) if (RunGame()) {
			l_gamestage = SCORE;
		}
		if (l_gamestage == SCORE) if(ShowScore()) {
			l_gamestage = START;
		}
		
		if(l_gamestage == EXIT) return false;
		return true;
	}
};


int main()
{
	Example demo;
	if (demo.Construct(screenW, screenH, 2, 2))
		demo.Start();

	return 0;
}
