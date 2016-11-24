#include "myLib.h"
#include <stdlib.h>

unsigned short *videoBuffer = (unsigned short *)0x6000000;

void setPixel(int row, int col, unsigned short color)
{
	videoBuffer[OFFSET(row, col, 240)] = color;
}

void drawRect(int row, int col, int height, int width, volatile u16 color)
{
	for(int r=0; r<height; r++) {
		DMA[3].src = &color;
		DMA[3].dst = videoBuffer + OFFSET(row+r, col, 240);
		DMA[3].cnt = width | DMA_SOURCE_FIXED | DMA_ON;
	}
}

void delay(int n)
{
	volatile int x = 0;
	for(int i=0; i<50*n; i++)
	{
		x = x + 1;
	}
}

void waitForVblank()
{
	while(SCANLINECOUNTER > 160);
	while(SCANLINECOUNTER<160);
}

void drawBar(BAR a) {
	if (a.bar_size > 0) {
		if (a.orientation == 0) {
			drawRect(a.position, 0, a.bar_size, a.gap_position, a.color);
			int startNext = a.gap_position + a.gap_size;
			drawRect(a.position, startNext, a.bar_size, 240 - startNext, a.color);
		} else {
			drawRect(0, a.position, a.gap_position, a.bar_size, a.color);
			int startNext = a.gap_position + a.gap_size;
			drawRect(startNext, a.position, 160 - a.gap_position, a.bar_size, a.color);
		}

	}
}
void drawSnake(volatile int row[], volatile int col[], u16 color) {
	
	for (int i = 0; i < 6; i++) {
		drawRect(row[i],col[i],SNAKESIZE,SNAKESIZE,color);
	}

}

void drawImage3(int r, int c, int width, int height, const u16* image) { 
	/*for (int i = 0; i < height; i ++) {
		for (int j = 0; j < width; j++) {
			videoBuffer[(r+i)*240 + j + c] = image[r*width + c];
		}
	}*/
		for (int i = 0; i <height; i++) {
			DMA[3].src = image + i*width;
			DMA[3].dst = videoBuffer + (r+i)*240 + c;
			DMA[3].cnt = width | DMA_ON;

		}

	}


	int collideEnemy(volatile int playerRow[], volatile int playerCol[], ENEMY enemy[]) {
		int L1Y;
		int L1X;
		int R1Y;
		int R1X;
		int answer;
		for(int j = 0; j < ENEMYNUMBER; j++) {
			int L2Y = enemy[j].row;
			int L2X = enemy[j].col;
			int R2Y = enemy[j].row + ENEMYSIZE;
			int R2X = enemy[j].col + ENEMYSIZE;
			for (signed i = SNAKELENGTH - 1; i >= 0; i--) {
				L1Y = playerRow[i];
				L1X = playerCol[i];
				R1Y = playerRow[i] + SNAKESIZE;
				R1X = playerCol[i] + SNAKESIZE;
				collide(L1X,L1Y,R1X,R1Y,L2X,L2Y,R2X,R2Y, answer);
				if (answer == 1) {
					return 1;
				}
			}
		}
		return 0;
	}

	int collideBars(volatile int playerRow[], volatile int playerCol[], BAR bars[]) {
		int L1Y;
		int L1X;
		int R1Y;
		int R1X;
		int answer;
		int L2Y;
		int L2X;
		int R2Y;
		int R2X;
		for (signed i = SNAKELENGTH - 1; i >= 0; i--) {
			L1Y = playerRow[i];
			L1X = playerCol[i];
			R1Y = playerRow[i] + SNAKESIZE;
			R1X = playerCol[i] + SNAKESIZE;

			L2Y = bars[0].position;
			L2X = 0;
			R2Y = bars[0].position + bars[0].bar_size;
			R2X = bars[0].gap_position;

			collide(L1X,L1Y,R1X,R1Y,L2X,L2Y,R2X,R2Y,answer);
			if (answer == 1) {
				return 1;
			} else {
				L2Y = bars[0].position;
				L2X = bars[0].gap_position + bars[0].gap_size;
				R2Y = bars[0].position + bars[0].bar_size;
				R2X = 240;
				collide(L1X,L1Y,R1X,R1Y,L2X,L2Y,R2X,R2Y,answer);
				if (answer == 1) {
					return 1;
				} else {
					L2Y = 0;
					L2X = bars[1].position;
					R2Y = bars[1].gap_position;
					R2X = bars[1].position + bars[1].bar_size;
					collide(L1X,L1Y,R1X,R1Y,L2X,L2Y,R2X,R2Y,answer);
					if (answer == 1) {
						return 1;
					} else {
						L2Y = bars[1].gap_position + bars[1].gap_size;
						L2X = bars[1].position;
						R2Y = 160;
						R2X = bars[1].position + bars[1].bar_size;
						collide(L1X,L1Y,R1X,R1Y,L2X,L2Y,R2X,R2Y,answer);
						if (answer == 1) {
							return 1;
						}
					}
				}
			}
		}
		return 0;
	}

	void moveBars(BAR bars[]) {
		for (int i = 0; i < 2; i++) {
			if (bars[i].orientation == 0) {
				if (bars[i].bar_size == 0) {
					bars[i].gap_position = rand()%(240 - bars[i].gap_size - 7) + 3;
					if (rand()%10 > 4) {
						bars[i].position = 0;
						bars[i].direction = 1;
					} else {
						bars[i].position = 160 - BARSPEED;
						bars[i].direction = -1;
					}
					bars[i].bar_size = BARSPEED;
				}


				bars[i].position = bars[i].position + BARSPEED*bars[i].direction;

				if (bars[i].position + bars[i].bar_size > 159) {
					bars[i].bar_size = MAX(160 - bars[i].position,0);
					bars[i].position = 160 - bars[i].bar_size;
				} else if (bars[i].position < 0) {
					bars[i].bar_size = MAX(bars[i].bar_size + bars[i].position, 0);
					bars[i].position = 0;
				} else if (bars[i].bar_size < BARSIZE) {
					bars[i].position = bars[i].position - BARSPEED*bars[i].direction;
					bars[i].bar_size = MAX(bars[i].bar_size + BARSPEED, BARSIZE);
				}

			} else {
				if (bars[i].bar_size == 0) {
					bars[i].gap_position = rand()%(160 - bars[i].gap_size - 7) + 3;
					if (rand()%10 > 4) {
						bars[i].position = 0;
						bars[i].direction = 1;
					} else {
						bars[i].position = 238 - BARSPEED;
						bars[i].direction = -1;
					}
					bars[i].bar_size = BARSPEED;
				}


				bars[i].position = bars[i].position + BARSPEED*bars[i].direction;
				if (bars[i].position + bars[i].bar_size > 239) {
					bars[i].bar_size = MAX(239 - bars[i].position,0);
					bars[i].position = 239- bars[i].bar_size;
				} else if (bars[i].position < 0) {
					bars[i].bar_size = MAX(bars[i].bar_size + bars[i].position, 0);
					bars[i].position = 0;
				} else if (bars[i].bar_size < BARSIZE) {
					bars[i].position = bars[i].position - BARSPEED*bars[i].direction;
					bars[i].bar_size = MAX(bars[i].bar_size + BARSPEED, BARSIZE);
				}

			}
		}
	}

	void updateEnemyPos(ENEMY enemy[], ENEMY oldEnemy[]) {
		for (int i = 0; i < ENEMYNUMBER; i++) {
			if (enemy[i].exist == 0) {
				int randQuad = rand() % 4;
				int randRow = rand() % QUADROWSIZE;
				int randCol = rand() % QUADCOLSIZE;
				if (randQuad == 0) {
					enemy[i].row = randRow;
					enemy[i].col = randCol;
					enemy[i].rowDir = 1;
					enemy[i].colDir = 1;
					enemy[i].exist = 1;
				} else if (randQuad == 1) {
					enemy[i].row = randRow;
					enemy[i].col = 240 - QUADCOLSIZE + randCol;
					enemy[i].rowDir = 1;
					enemy[i].colDir = -1;
					enemy[i].exist = 1;
				} else if(randQuad == 2) {
					enemy[i].row = 160 - QUADROWSIZE + randRow;
					enemy[i].col = randCol;
					enemy[i].rowDir = -1;
					enemy[i].colDir = 1;
					enemy[i].exist = 1;
				} else if (randQuad == 3) {
					enemy[i].row = 160 - QUADROWSIZE + randRow;
					enemy[i].col = 240 - QUADCOLSIZE + randCol;
					enemy[i].rowDir = -1;
					enemy[i].colDir = -1;
					enemy[i].exist = 1;
				}

			}

			oldEnemy[i] = enemy[i];


			if (enemy[i].row > 159 || enemy[i].row < 0 || enemy[i].col < 0 || enemy[i].col > 239) {
				enemy[i].exist = 0;
			} else {
				enemy[i].row = enemy[i].row + ENEMYSPEED*enemy[i].rowDir;
				enemy[i].col = enemy[i].col + ENEMYSPEED*enemy[i].colDir;
			}
		}

	}

	void updateSnakePos(volatile int playerRow[], volatile int playerCol[], volatile int *last) {
		volatile unsigned int i;
		if (KEY_DOWN_NOW(BUTTON_UP) || KEY_DOWN_NOW(BUTTON_DOWN) || KEY_DOWN_NOW(BUTTON_LEFT) || KEY_DOWN_NOW(BUTTON_RIGHT)) {
			*last = 0;
		}

		if (KEY_DOWN_NOW(BUTTON_UP) || *last == 1) {
			for (i = 0; i < (SNAKELENGTH - SNAKESPEED); i++) {
				playerRow[i] = playerRow[i+SNAKESPEED];
				playerCol[i] = playerCol[i+SNAKESPEED];
			}
			for (i = 0; i < SNAKESPEED; i++) {
				playerRow[i + SNAKELENGTH - SNAKESPEED] = playerRow[SNAKELENGTH-1] - (i+1)*SNAKESIZE;
				playerCol[i + SNAKELENGTH - SNAKESPEED] = playerCol[SNAKELENGTH-1];
			}
			*last = 1;
		} else if (KEY_DOWN_NOW(BUTTON_DOWN) || *last == 2) {
			for (i = 0; i < (SNAKELENGTH - SNAKESPEED); i++) {
				playerRow[i] = playerRow[i+SNAKESPEED];
				playerCol[i] = playerCol[i+SNAKESPEED];
			}
			for (i = 0; i < SNAKESPEED; i++) {
				playerRow[i + SNAKELENGTH - SNAKESPEED] = playerRow[SNAKELENGTH-1] + (i+1)*SNAKESIZE;
				playerCol[i + SNAKELENGTH - SNAKESPEED] = playerCol[SNAKELENGTH-1];
			}
			*last = 2;
		} else if (KEY_DOWN_NOW(BUTTON_LEFT) || *last == 3) {
			for (i = 0; i < (SNAKELENGTH - SNAKESPEED); i++) {
				playerRow[i] = playerRow[i+SNAKESPEED];
				playerCol[i] = playerCol[i+SNAKESPEED];
			}
			for (i = 0; i < SNAKESPEED; i++) {
				playerRow[i + SNAKELENGTH - SNAKESPEED] = playerRow[SNAKELENGTH-1];
				playerCol[i + SNAKELENGTH - SNAKESPEED] = playerCol[SNAKELENGTH-1] - (i+1)*SNAKESIZE;
			}
			*last = 3;
		} else if (KEY_DOWN_NOW(BUTTON_RIGHT) || *last == 4) {
			for (i = 0; i < (SNAKELENGTH - SNAKESPEED); i++) {
				playerRow[i] = playerRow[i+SNAKESPEED];
				playerCol[i] = playerCol[i+SNAKESPEED];
			}
			for (i = 0; i < SNAKESPEED; i++) {
				playerRow[i + SNAKELENGTH - SNAKESPEED] = playerRow[SNAKELENGTH-1];
				playerCol[i + SNAKELENGTH - SNAKESPEED] = playerCol[SNAKELENGTH-1] + (i+1)*SNAKESIZE;
			}
			*last = 4;
		}
	}

	void drawOldEnemy(ENEMY enemy[], u16 color) {
		for (int i = 0; i < ENEMYNUMBER; i++) {
			drawRect(enemy[i].row,enemy[i].col,ENEMYSIZE, ENEMYSIZE, color);
		}
	}

	void drawNewEnemy(ENEMY enemy[], u16 color) {
		for (int i = 0; i < ENEMYNUMBER; i++) {
			if (enemy[i].exist == 1) {
				drawRect(enemy[i].row,enemy[i].col,ENEMYSIZE, ENEMYSIZE, color);
			}
		}
	}