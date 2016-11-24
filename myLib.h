typedef unsigned int u32;
typedef unsigned short u16;

#define REG_DISPCTL *(unsigned short *)0x4000000
#define MODE3 3
#define BG2_ENABLE (1<<10)

extern unsigned short *videoBuffer; 

#define COLOR(r,g,b) ((r) |  (g)<<5 |  (b)<<10)
#define RED COLOR(31,0,0)
#define GREEN COLOR(0,31,0)
#define BLUE COLOR(0,0,31)
#define WHITE COLOR(31,31,31)
#define GRAY COLOR(15,15,15)
#define BLACK 0

#define OFFSET(r,c,rowlen) ((r)*(rowlen) + (c))

// Buttons

#define BUTTON_SELECT	(1<<2)
#define BUTTON_START	(1<<3)
#define BUTTON_RIGHT	(1<<4)
#define BUTTON_LEFT		(1<<5)
#define BUTTON_UP		(1<<6)
#define BUTTON_DOWN		(1<<7)

#define KEY_DOWN_NOW(key)  (~(BUTTONS) & key)

#define BUTTONS *(volatile unsigned int *)0x4000130

#define SCANLINECOUNTER *(volatile short *)0x4000006



typedef struct
{
	const volatile void *src;
	void *dst;
	unsigned int cnt;
} DMA_CONTROLLER;

#define DMA ((volatile DMA_CONTROLLER *) 0x040000B0)

// Defines

#define DMA_SOURCE_INCREMENT (0 << 23)
#define DMA_SOURCE_FIXED (2 << 23)
#define DMA_ON (1 << 31)


//STRUCTURES

typedef struct 
{
	int position;
	int gap_position;
	int gap_size;
	int bar_size;
	int orientation; //0 = horizontal
	volatile unsigned short color;
	int direction; //1 for down or right, -1 for up or left
} BAR;

typedef struct 
{
	int row;
	int col;
	int hit;
	int count;
	volatile u16 color;
} FOOD;

typedef struct
{
	int row;
	int col;
	int rowDir;
	int colDir;
	int exist;
} ENEMY;

//MACROS
#define SNAKECOLOR RED
#define SNAKESIZE 4
#define NUMBAR 3
#define BARSPEED 2
#define MAX(a,b) ((a)>=(b) ? (a) : (b))
#define MIN(a,b) ((a)<=(b) ? (a) : (b))
#define BARSIZE 4
#define SNAKESPEED 1
#define SNAKELENGTH 6
#define FOODSIZE 7

#define ENEMYNUMBER 2
#define ENEMYSIZE 4
#define ENEMYSPEED 2
#define ENEMYCOLOR BLUE
#define QUADROWSIZE 30
#define QUADCOLSIZE 60

#define BGCOLOR GRAY
#define collide(L1X,L1Y,R1X,R1Y,L2X,L2Y,R2X,R2Y,answer) ((answer) = ((L1X) < (R2X) && (R1X) > (L2X) && (L1Y) < (R2Y) && (R1Y) > (L2Y)) ? 1 : 0)
				
// Prototype
void setPixel(int , int , unsigned short );
void drawRect(int row, int col, int height, int width, unsigned short color);
void delay(int);
void waitForVblank();
void drawBar(BAR a);
void drawSnake(volatile int row[], volatile int col[], u16 color);
void drawImage3(int r, int c, int width, int height, const u16* image);
int collideBars(volatile int playerRow[], volatile int playerCol[], BAR bars[]);
void moveBars(BAR bars[]);
int collideEnemy(volatile int playerRow[], volatile int playerCol[], ENEMY enemy[]);
void updateEnemyPos(ENEMY enemy[], ENEMY oldEnemy[]);
void updateSnakePos(volatile int playerRow[], volatile int playerCol[], volatile int *last);
void drawOldEnemy(ENEMY enemy[], u16 color);
void drawNewEnemy(ENEMY enemy[], u16 color);
