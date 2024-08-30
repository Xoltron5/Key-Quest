/**
 * Game Title: Key Quest
 * Description:
 * Key Quest is a 2D adventure game where the player navigates through various levels, 
 * collecting keys while avoiding obstacles like spikes and enemies. Each level presents 
 * unique challenges and requires the player to maneuver strategically to survive and progress.
 * The game features multiple difficulty levels, including a special Nightmare mode for advanced players.
 * The player's journey is depicted through a simple yet engaging pixel art style, and the game 
 * incorporates basic sound effects to enhance the gaming experience. The objective is to 
 * collect all keys in a level to unlock the door to the next stage, all while managing 
 * a limited number of lives represented by hearts.
 *
 * Developed by: [Denis & Cillian]
 * Date: [25/11/23]
 * Course: T857
 * Year: 1
 */

#include <stm32f031x6.h> // Include the STM32F0xx Standard Peripheral Library
#include "display.h" // Include the display header for screen operations
#include "sound.h" // Include the sound header for audio functionalities
#include "musical_notes.h" // Include definitions for musical notes
#include "prbs.h" // Include the pseudo-random binary sequence header
#include "serial.h" // Include the serial communication header for data transmission and logging functionalities


// Preprocessor directives defining musical notes for different game levels
#define LEVEL_1_MUSIC 10
#define LEVEL_2_MUSIC 16
#define LEVEL_3_MUSIC 24

// Define the number of characters to be used for text display on screen
#define NUM_OF_CHAR 5

// Define the total number of difficulties and badges in the game
#define DIFFICULTY_AMOUNT 4
#define BADGES_AMOUNT 4

// Define the number of keys and random locations for player in level 1
#define NUM_OF_KEYS_1_DISPLAYED 1
#define NUMOF_KEYS_1 1
#define NUM_OF_RANDOM_LOCATIONS 2

// Define the number of keys, spikes, and random locations for player in level 2
#define NUM_OF_KEYS_2_DISPLAYED 2
#define NUMOF_KEYS_2 2
#define NUMOF_SPIKES_2 2

// Define the number of keys, spikes, enemies, and random locations for player in level 3
#define NUM_OF_KEYS_3_DISPLAYED 3
#define NUMOF_KEYS_3 3
#define NUMOF_SPIKES_3 3
#define NUM_OF_ENEMY_3 2

void RightButtonPressed(uint16_t* x,int* hmoved, int* hinverted);
void LeftButtonPressed(uint16_t* x,int* hmoved, int* hinverted);
void UpButtonPressed(uint16_t* y,int* vmoved, int* vinverted);
void DownButtonPressed(uint16_t* y,int* vmoved, int* vinverted);
void initClock(void);
void initSysTick(void);
void SysTick_Handler(void);
void delay(volatile uint32_t dly);
void setupIO();
int isInside(uint16_t x1, uint16_t y1, uint16_t w, uint16_t h, uint16_t px, uint16_t py);
void enablePullUp(GPIO_TypeDef *Port, uint32_t BitNumber);
void pinMode(GPIO_TypeDef *Port, uint32_t BitNumber, uint32_t Mode);
void intro(void);
void main_menu(void);
void gameover(int *start_game, int *current_difficulty_choice,int *difficulty);
int Level1_Start(uint16_t x,uint16_t y,uint16_t oldx,uint16_t oldy,uint16_t* px,uint16_t* py,int hearts_used,int difficulty);
int Level2_Start(uint16_t x,uint16_t y,uint16_t oldx,uint16_t oldy,uint16_t* px,uint16_t* py,int hearts_used,int difficulty);
int Level3_Start(uint16_t x,uint16_t y,uint16_t oldx,uint16_t oldy,uint16_t* px,uint16_t* py,int hearts_used,int difficulty);
void play_music(int [],int);
void Difficulty_choice(int* difficulty,int choice,int *hearts_used);
void Difficulty_Display(int difficulty);
void Difficulty_Nightmare(int* difficulty,int choice,int *hearts_used);
void gameend(int *start_game, int *current_difficulty_choice,int *difficulty);

// Red LED that tells you, that you are not in a level and the game is running. 
void RedOn(void);
void RedOff(void);

// Green LED that tells you, that you are in a level and the game is running. 
void GreenOn();
void GreenOff();
// Serial Communication
void serial_log(char log[]);
volatile uint32_t milliseconds;  // Global variable to store milliseconds for timing
volatile uint32_t milliseconds_timer;  // Timer variable for specific timing tasks
uint32_t max_time = 60000;  // Maximum time limit in milliseconds (1 minute)
int timer = 60;  // Timer variable for Nightmare Mode, set to 60 seconds

// knight_animation1: Array to store the first animation frame of the knight sprite
const uint16_t knight_animation1[] =
{
	0,0,0,0,34617,34617,34617,34617,34617,0,0,
	0,0,0,0,34617,43866,4492,4492,54437,54437,
	34617,0,0,0,0,0,34617,43866,4492,34617,34617,
	34617,34617,0,0,0,0,0,34617,43866,4492,4492,34617,
	54437,34617,0,0,0,0,0,34617,43866,4492,4492,34617,54437,
	34617,0,0,0,34617,34617,34617,34617,34617,34617,34617,34617,
	34617,34617,0,34617,65288,65288,65288,65288,65288,65288,4492,
	34617,54437,54437,34617,34617,65288,65288,65288,65288,65288,
	65288,4492,4492,34617,54437,34617,34617,65288,65288,16135,16135,
	65288,65288,4492,4492,34617,54437,34617,34617,65288,65288,16135,
	16135,65288,65288,4492,4492,34617,54437,34617,34617,65288,65288,
	65288,65288,65288,65288,4492,4492,34617,34617,34617,0,34617,65288,
	65288,65288,65288,34617,43866,4492,4492,34617,0,0,0,34617,65288,65288,
	34617,34617,34617,4492,4492,34617,0,0,0,0,34617,34617,34617,0,0,34617,
	4492,34617,0,0,0,0,34617,43866,34617,0,0,34617,4492,34617,0,0,0,0,34617,
	43866,0,0,0,0,4492,34617,0,
};

// knight_animation2: Array to store the second animation frame of the knight sprite
const uint16_t	knight_animation2[] = 
{
	0,0,0,0,34617,34617,34617,
	34617,34617,0,0,0,0,0,0,34617,
	43866,4492,4492,54437,54437,34617,
	0,0,0,0,0,34617,43866,4492,34617,34617,
	34617,34617,0,0,0,0,0,34617,43866,4492,
	4492,34617,54437,34617,0,0,0,0,0,34617,43866,
	4492,4492,34617,54437,34617,0,0,0,0,34617,34617,
	34617,34617,34617,34617,34617,34617,34617,0,0,34617,
	65288,65288,65288,65288,65288,65288,34617,54437,54437,
	34617,0,34617,65288,65288,65288,65288,65288,65288,4492,
	34617,54437,34617,0,34617,65288,65288,16135,16135,65288,
	65288,4492,34617,54437,54437,0,34617,65288,65288,16135,16135,
	65288,65288,4492,34617,34617,34617,0,34617,65288,65288,65288,
	65288,65288,65288,4492,34617,0,0,0,0,34617,65288,65288,65288,
	65288,34617,4492,4492,34617,0,0,0,0,34617,65288,65288,34617,
	43866,4492,4492,34617,0,0,0,0,34617,34617,34617,0,0,0,34617,
	4492,34617,0,0,0,34617,43866,34617,0,0,0,34617,4492,34617,0,
	0,34617,43866,34617,0,0,0,0,0,0,0,
};

// knight_animation3: Array to store the third animation frame of the knight sprite
const uint16_t knight_animation3[] = 
{
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,34617,
	34617,34617,34617,0,0,0,0,0,0,0,34617,34617,
	34617,34617,34617,34617,0,0,0,0,4492,4492,34617,
	34617,34617,34617,34617,34617,4492,4492,0,0,34617,
	34617,34617,34617,34617,34617,34617,34617,34617,34617,
	0,0,4492,4492,34617,34617,34617,34617,34617,34617,4492,
	4492,0,0,0,0,0,34617,34617,34617,34617,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

const uint16_t spike[] =
{
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,50737,
	0,0,0,0,0,0,0,0,0,0,0,50737,0,0,0,0,0,0,0,0,0,0,0,50737,50737,
	0,0,0,0,0,0,0,0,0,50737,65535,50737,0,0,0,0,0,0,0,0,0,50737,65535,
	50737,0,0,0,0,0,0,0,0,0,50737,65535,50737,50737,0,0,0,0,0,0,0,0,50737,
	65535,50737,50737,0,0,0,0,0,0,0,50737,65535,20612,50737,50737,0,0,0,0,
	0,0,0,50737,65535,20612,50737,50737,0,0,0,0,0,0,0,50737,65535,20612,43866,
	50737,50737,0,0,0,0,0,0,50737,65535,20612,43866,50737,50737,0,0,0,0,0,50737,
	65535,20612,20612,43866,50737,50737,0,0,0,0,0,50737,65535,20612,43866,43866,
	50737,50737,50737,0,0,0,0,50737,65535,20612,43866,43866,43866,43866,50737,0,0,0,
};

// spike: Array to store the graphical representation of a spike obstacle
const uint16_t nightmare_spike[] =
{
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,7960,0,0,0,
	0,0,0,0,0,0,0,0,7960,0,0,0,0,0,0,0,0,0,0,0,7960,7960,0,0,0,0,0,0,0,
	0,0,7960,7960,7960,0,0,0,0,0,0,0,0,0,7960,7960,50737,0,0,0,0,0,0,0,
	0,0,50737,7960,50737,7960,0,0,0,0,0,0,0,0,50737,7960,50737,50737,0,
	0,0,0,0,0,0,50737,7960,20612,50737,50737,0,0,0,0,0,0,0,50737,7960,
	20612,50737,50737,0,0,0,0,0,0,0,50737,7960,20612,43866,50737,50737,
	0,0,0,0,0,0,50737,7960,20612,43866,50737,50737,0,0,0,0,0,50737,7960,
	20612,20612,43866,50737,50737,0,0,0,0,0,50737,7960,20612,43866,43866,
	50737,50737,50737,0,0,0,0,50737,7960,20612,43866,43866,43866,43866,50737
	,0,0,0,
};

// heart: Array to represent a heart (life) in the game
const uint16_t heart[] =
{
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,7936,7936,0,
	0,0,7936,7936,0,0,0,0,7936,7936,7936,7936,0,7936,7936,7936,7936,0,0,
	0,7936,56253,7936,7936,7936,7936,7936,7936,7936,0,0,0,7936,7936,7936,
	7936,7936,7936,7936,7936,7936,0,0,0,7936,7936,7936,7936,7936,7936,7936,
	7936,7936,0,0,0,7936,7936,7936,7936,7936,7936,7936,7936,7936,0,0,0,0,7936,
	7936,7936,7936,7936,7936,7936,0,0,0,0,0,0,7936,7936,7936,7936,7936,0,0,0,0,
	0,0,0,0,7936,7936,7936,0,0,0,0,0,0,0,0,0,0,7936,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,
};

// nightmare_heart: Heart representation for Nightmare Mode
const uint16_t nightmare_heart[] = 
{
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,12192,12192,0,0,0,
	12192,12192,0,0,0,0,12192,12192,12192,12192,0,12192,12192,12192,12192,0,
	0,0,12192,56253,12192,12192,12192,12192,12192,12192,12192,0,0,0,12192,
	12192,12192,12192,12192,12192,12192,12192,12192,0,0,0,12192,12192,12192,
	12192,12192,12192,12192,12192,12192,0,0,0,12192,12192,12192,12192,12192,
	12192,12192,12192,12192,0,0,0,0,12192,12192,12192,12192,12192,12192,12192,
	0,0,0,0,0,0,12192,12192,12192,12192,12192,0,0,0,0,0,0,0,0,12192,12192,12192,
	0,0,0,0,0,0,0,0,0,0,12192,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

// hearts_empty: Array to represent an empty heart (lost life)
const uint16_t hearts_empty[] =
{
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,44395,44395,0,0,0,
	44395,44395,0,0,0,0,44395,44395,44395,44395,0,44395,44395,44395,44395,0,
	0,0,44395,56253,44395,44395,44395,44395,44395,44395,44395,0,0,0,44395,44395,
	44395,44395,44395,44395,44395,44395,44395,0,0,0,44395,44395,44395,44395,44395,
	44395,44395,44395,44395,0,0,0,44395,44395,44395,44395,44395,44395,44395,44395,
	44395,0,0,0,0,44395,44395,44395,44395,44395,44395,44395,0,0,0,0,0,0,44395,44395,
	44395,44395,44395,0,0,0,0,0,0,0,0,44395,44395,44395,0,0,0,0,0,0,0,0,0,0,44395,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

// key: Array to represent a key in Level 1
const uint16_t key[] =
{
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,24326,7943,0,0,0,0,0,
	0,0,0,0,0,24326,7943,0,0,0,0,0,0,0,0,0,0,24326,7943,0,0,0,0,0,0,0,0,0,0,24326,
	7943,7943,7943,0,0,0,0,0,0,0,0,24326,7943,7943,0,0,0,0,0,0,0,0,0,24326,7943,0,0,
	0,0,0,0,0,0,0,0,24326,7943,0,0,0,0,0,0,0,0,0,0,24326,7943,0,0,0,0,0,0,0,0,0,0,
	24326,7943,0,0,0,0,0,0,0,0,0,0,24326,7943,0,0,0,0,0,0,0,0,0,7943,0,0,7943,0,0,
	0,0,0,0,0,0,24326,0,0,7943,0,0,0,0,0,0,0,0,24326,7943,7943,24326,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,
};

// taken_key: Array to represent a collected key
const uint16_t taken_key[] =
{
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,26954,44395,0,0,0,
	0,0,0,0,0,0,0,26954,44395,0,0,0,0,0,0,0,0,0,0,26954,44395,0,0,0,0,0,0,0,0,0,
	0,26954,44395,44395,44395,0,0,0,0,0,0,0,0,26954,44395,44395,0,0,0,0,0,0,0,0,
	0,26954,44395,0,0,0,0,0,0,0,0,0,0,26954,44395,0,0,0,0,0,0,0,0,0,0,26954,44395,
	0,0,0,0,0,0,0,0,0,0,26954,44395,0,0,0,0,0,0,0,0,0,0,26954,44395,0,0,0,0,0,0,0,
	0,0,44395,0,0,44395,0,0,0,0,0,0,0,0,26954,0,0,44395,0,0,0,0,0,0,0,0,26954,26954,
	44395,26954,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

// door: Array to represent a door in the game
const uint16_t door[] =
{
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,18233,18233,
	18233,18233,0,0,0,0,0,0,0,18233,37640,60672,37640,
	37640,18233,0,0,0,0,0,18233,60672,37640,60672,60672,
	37640,37640,18233,0,0,0,18233,60672,60672,37640,60672,
	60672,37640,60672,37640,18233,0,0,18233,37640,60672,37640,
	60672,37640,37640,60672,37640,18233,0,0,18233,60672,60672,
	37640,60672,60672,37640,37640,37640,18233,0,0,18233,44395,
	44395,44395,44395,44395,44395,44395,44395,18233,0,0,18233,
	44395,44395,44395,44395,44395,44395,44395,44395,18233,0,0,
	18233,60672,60672,37640,37640,60672,37640,60672,37640,18233,
	0,0,18233,60672,60672,37640,60672,60672,37640,60672,37640,18233,
	0,0,18233,60672,37640,37640,60672,60672,37640,60672,37640,18233,
	0,0,18233,44395,44395,44395,44395,44395,44395,24326,44395,18233,
	0,0,18233,44395,44395,44395,44395,44395,44395,24326,44395,18233,
	0,0,18233,37640,60672,37640,60672,60672,37640,37640,37640,18233,
	0,0,18233,60672,60672,37640,37640,60672,37640,60672,37640,18233,
	0,0,18233,18233,18233,18233,18233,18233,18233,18233,18233,18233,
	0,0,0,0,0,0,0,0,0,0,0,0,0,
};

// skeleton_run: Array for the running animation of a skeleton enemy
const uint16_t skeleton_run[] =
{
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,10306,0,0,0,0,0,39374,39374,39374,39374,
	39374,0,10306,27218,27218,0,0,39374,65535,65535,65535,65535,65535,
	0,10306,27218,27218,0,0,39374,65535,0,65535,0,65535,0,10306,27218,
	10306,0,0,39374,65535,65535,39374,65535,65535,0,51977,10306,0,0,0,
	0,0,39374,39374,39374,39374,0,51977,0,0,0,0,39374,65535,65535,65535,
	39374,0,0,4114,0,0,0,0,39374,65535,0,0,39374,39374,0,4114,0,0,0,0,
	39374,65535,65535,65535,39374,39374,39374,4114,0,0,0,0,39374,65535,
	0,0,39374,0,0,4114,0,0,0,0,0,65535,65535,39374,39374,0,0,4114,0,0,0,
	0,65535,65535,0,0,0,39374,0,4114,0,0,0,65535,0,0,0,0,0,0,0,4114,0,0,0,
};

// skeleton_attack1: First attack frame of the skeleton enemy
const uint16_t skeleton_attack1[] =
{
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,39374,39374,39374,39374,39374,0,0,0,
	10306,0,0,39374,65535,65535,65535,65535,65535,0,0,0,10306,0,0,39374,
	65535,0,65535,0,65535,0,0,10306,27218,27218,27218,39374,65535,65535,
	39374,65535,65535,0,0,10306,27218,27218,0,0,0,39374,39374,39374,39374,
	0,51977,10306,27218,10306,0,39374,65535,65535,65535,39374,0,51977,51977,
	0,0,0,0,39374,39374,0,0,39374,39374,4114,0,0,0,0,0,0,39374,39374,65535,
	39374,4114,4114,0,0,0,0,0,0,65535,39374,39374,39374,4114,0,0,0,0,0,0,0,
	65535,65535,39374,4114,4114,0,0,0,0,0,0,0,65535,0,0,4114,0,0,0,0,0,0,0,0,
	39374,0,4114,4114,0,0,0,0,0,0,0,
};

// skeleton_attack2: Second attack frame of the skeleton enemy
const uint16_t skeleton_attack2[] =
{
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,39374,39374,39374,39374,39374,0,0,0,
	0,0,0,39374,65535,65535,65535,65535,65535,0,0,0,0,0,0,39374,65535,0,
	65535,0,65535,0,0,0,0,0,0,39374,65535,65535,39374,65535,65535,0,0,0,
	0,0,0,0,0,39374,39374,39374,39374,0,0,0,0,0,0,39374,65535,65535,65535,
	39374,0,0,0,0,0,0,0,0,39374,0,0,39374,39374,0,0,0,0,0,4114,4114,4114,
	65535,4114,4114,65535,51977,10306,10306,10306,10306,0,0,65535,39374,
	39374,39374,0,10306,27218,27218,27218,0,0,0,65535,65535,39374,39374,0,
	0,10306,27218,27218,0,0,0,65535,0,0,39374,0,0,0,0,0,0,0,0,39374,0,0,
	39374,0,0,0,0,0,0,
};

// night_skeleton_run: Running animation for the skeleton in Nightmare Mode
const uint16_t night_skeleton_run[] = 
{
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,10306,0,0,0,0,0,30168,30168,30168,30168,30168,
	0,10306,27218,27218,0,0,30168,48123,48123,48123,48123,48123,0,10306,
	27218,27218,0,0,30168,48123,7960,48123,7960,48123,0,10306,27218,10306,
	0,0,30168,48123,48123,30168,48123,48123,0,51977,10306,0,0,0,0,0,30168,
	30168,30168,30168,0,51977,0,0,0,0,30168,48123,48123,48123,30168,0,0,4114,
	0,0,0,0,30168,48123,0,0,30168,30168,0,4114,0,0,0,0,30168,48123,48123,48123,
	30168,30168,30168,4114,0,0,0,0,30168,48123,0,0,30168,0,0,4114,0,0,0,0,0,48123,
	48123,30168,30168,0,0,4114,0,0,0,0,48123,48123,0,0,0,30168,0,4114,0,0,0,48123,
	0,0,0,0,0,0,0,4114,0,0,0,
};

// night_skeleton_attack1: First attack frame of the skeleton in Nightmare Mode
const uint16_t night_skeleton_attack1[] = 
{
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,30168,30168,30168,30168,30168,0,0,0,10306,
	0,0,30168,48123,48123,48123,48123,48123,0,0,0,
	10306,0,0,30168,48123,7960,48123,7960,48123,0,0,
	10306,27218,27218,27218,30168,48123,48123,30168,
	48123,48123,0,0,10306,27218,27218,0,0,0,30168,30168,
	30168,30168,0,51977,10306,27218,10306,0,30168,48123,
	48123,48123,30168,0,51977,51977,0,0,0,0,30168,30168,
	0,0,30168,30168,4114,0,0,0,0,0,0,30168,30168,48123,
	30168,4114,4114,0,0,0,0,0,0,48123,30168,30168,30168,
	4114,0,0,0,0,0,0,0,48123,48123,30168,4114,4114,0,0,0,
	0,0,0,0,48123,0,0,4114,0,0,0,0,0,0,0,0,30168,0,4114,
	4114,0,0,0,0,0,0,0,
};

// night_skeleton_attack2: Second attack frame of the skeleton in Nightmare Mode
const uint16_t night_skeleton_attack2[] = 
{
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	30168,30168,30168,30168,30168,0,0,0,0,0,0,30168,48123,
	48123,48123,48123,48123,0,0,0,0,0,0,30168,48123,7960,
	48123,7960,48123,0,0,0,0,0,0,30168,48123,48123,30168,
	48123,48123,0,0,0,0,0,0,0,0,30168,30168,30168,30168,0,
	0,0,0,0,0,30168,48123,48123,48123,30168,0,0,0,0,0,0,0,
	0,30168,0,0,30168,30168,0,0,0,0,0,4114,4114,4114,48123,
	4114,4114,48123,51977,10306,10306,10306,10306,0,0,48123,
	30168,30168,30168,0,10306,27218,27218,27218,0,0,0,48123,
	48123,30168,30168,0,0,10306,27218,27218,0,0,0,48123,0,0,
	30168,0,0,0,0,0,0,0,0,30168,0,0,30168,0,0,0,0,0,0,
};

// easy_skull: Skull representation for the easy difficulty level
const uint16_t easy_skull[] =
{
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	14005,39374,39374,39374,39374,14005,0,0,0,0,0,14005,
	14005,14005,14005,14005,14005,14005,14005,0,0,0,14005,
	14005,14005,14005,14005,14005,14005,14005,14005,14005,
	0,0,14005,14005,14005,14005,14005,14005,14005,14005,
	14005,14005,0,0,14005,0,0,0,14005,14005,0,0,0,14005,0,
	0,14005,0,40966,40966,14005,14005,40966,40966,0,14005,0,
	0,39374,0,40966,40966,14005,14005,40966,40966,0,39374,0,
	0,0,14005,14005,14005,0,0,14005,14005,14005,0,0,0,0,14005,
	14005,14005,14005,14005,14005,14005,14005,0,0,0,0,14005,
	14005,14005,14005,14005,14005,14005,14005,0,0,0,0,14005,
	14005,14005,14005,14005,14005,14005,14005,0,0,0,0,0,39374,
	14005,14005,14005,14005,39374,0,0,0,0,0,0,0,14005,39374,39374,
	14005,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

// normal_skull: Skull representation for the normal difficulty level
const uint16_t normal_skull[] =
{
	0,0,0,0,0,0,0,0,0,0,0,0,0,11627,0,0,0,0,0,0,0,0,11627,0,11627,11627,
	11627,14005,39374,39374,39374,39374,14005,11627,11627,11627,0,11627,
	14005,14005,14005,14005,14005,14005,14005,14005,11627,0,0,14005,14005,
	14005,14005,14005,14005,14005,14005,14005,14005,0,0,14005,14005,14005,
	14005,14005,14005,14005,14005,14005,14005,0,0,14005,0,0,0,14005,14005,
	0,0,0,14005,0,0,14005,0,15950,15950,14005,14005,15950,15950,0,14005,0,
	0,39374,0,15950,15950,14005,14005,15950,15950,0,39374,0,0,0,14005,14005,
	14005,0,0,14005,14005,14005,0,0,0,0,14005,14005,14005,14005,14005,14005,
	14005,14005,0,0,0,0,14005,14005,14005,14005,14005,14005,14005,14005,0,0,
	0,0,14005,14005,14005,14005,14005,14005,14005,14005,0,0,0,0,0,39374,14005,
	14005,14005,14005,39374,0,0,0,0,0,0,0,14005,39374,39374,14005,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,
};

// hard_skull: Skull representation for the hard difficulty level
const uint16_t hard_skull[] =
{
	11627,0,0,0,0,0,0,0,0,0,0,11627,11627,11627,0,0,0,0,0,0,0,0,
	11627,11627,11627,11627,11627,14005,39374,39374,39374,39374,
	14005,11627,11627,11627,0,11627,14005,14005,14005,14005,14005,
	14005,14005,14005,11627,0,0,14005,14005,14005,14005,14005,14005,
	14005,14005,14005,14005,0,0,14005,14005,14005,14005,14005,14005,
	14005,14005,14005,14005,0,0,14005,0,0,0,14005,14005,0,0,0,14005,
	0,0,14005,0,40705,40705,14005,14005,40705,40705,0,14005,0,0,39374,
	0,40705,40705,14005,14005,40705,40705,0,39374,0,0,0,14005,14005,
	14005,0,0,14005,14005,14005,0,0,0,0,14005,14005,14005,14005,14005,
	14005,14005,14005,0,0,0,0,14005,14005,14005,14005,14005,14005,14005,
	14005,0,0,0,0,14005,14005,14005,14005,14005,14005,14005,14005,0,0,0,
	0,0,39374,14005,14005,14005,14005,39374,0,0,0,0,0,0,0,14005,39374,39374,
	14005,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

// nightmare_skull: Skull representation for the Nightmare Mode
const uint16_t nightmare_skull[] =
{
	11627,0,0,0,0,0,0,0,0,0,0,11627,11627,11627,0,0,0,0,0,
	0,0,0,11627,11627,11627,11627,11627,30168,30961,30961,
	30961,30961,30168,11627,11627,11627,0,11627,30168,30168,
	30168,30168,30168,30168,30168,30168,11627,0,0,30168,30168,
	30168,30168,30168,30168,30168,30168,30168,30168,0,0,30168,
	30168,30168,30168,30168,30168,30168,30168,30168,30168,0,0,
	30168,0,0,0,30168,30168,0,0,0,30168,0,0,30168,0,7960,7960,
	30168,30168,7960,7960,0,30168,0,0,30961,0,7960,7960,30168,
	30168,7960,7960,0,30961,0,0,0,30168,30168,30168,0,0,30168,
	30168,30168,0,0,0,0,30168,30168,30168,30168,30168,30168,30168,
	30168,0,0,0,0,30168,30168,30168,30168,30168,30168,30168,30168,0,
	0,0,0,30168,30168,30168,30168,30168,30168,30168,30168,0,0,0,0,
	0,30961,30168,30168,30168,30168,30961,0,0,0,0,0,0,0,30168,30961,
	30961,30168,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

int music_flag = 0;  // Flag to control music playback
int current_level = 1;  // Variable to track the current game level
int start_movement = 0;  // Flag to start player movement
int badges[BADGES_AMOUNT] = {0,0,0,0};  // Array to store badge status for player achievements
char timer_string[NUM_OF_CHAR];  // String to display timer information

// Nightmare Mode 
int nightmare_enabled = 0;
int nightmare_flag = 0;

//Seed for picking random player locations.
uint32_t seed = 0; 

int main() 
{
    // Initialize variables
    int start_game = 0;
    int intro_seen = 0;
    int hinverted = 0; // Horizontal inversion flag
    int vinverted = 0; // Vertical inversion flag
    int toggle = 0; // Toggle for animation
    int hmoved = 0; // Horizontal movement flag
    int vmoved = 0; // Vertical movement flag
    int difficulty = 0; // Difficulty level
    int current_diff_choice = 0; // Current difficulty choice
    int num_of_hearts = 0; // Number of hearts (lives)
    char system_log[] = "Game has been intialised";
    uint16_t x = 53; // Player's X position
    uint16_t y = 125; // Player's Y position
    uint16_t oldx = x; // Previous X position
    uint16_t oldy = y; // Previous Y position
    uint16_t oldx_OG = x; // Original X position
    uint16_t oldy_OG = y; // Original Y position

    // Initialize system components
    initClock();
    initSysTick();
    setupIO();
    initSound();
    initSerial();

    // Log system initialization
    serial_log(system_log);

    // Indicate the game is running and not in a level
    RedOn();

    // Main game loop
    while(1) 
	{
        // Display intro if not seen
        if (intro_seen == 0) {
            intro();
            start_game = 1;
            intro_seen = 1;
        }
        // Display main menu after intro
        if (start_game == 0 && intro_seen == 1) {
            main_menu();
            x = oldx_OG;
            y = oldy_OG;
            start_game = 1;
        }

        // Difficulty selection
        Difficulty_choice(&difficulty, current_diff_choice, &num_of_hearts);
        if (nightmare_enabled == 1 && difficulty == 3) {
            Difficulty_Nightmare(&difficulty, current_diff_choice, &num_of_hearts);
        }

	// Level handling
	switch (current_level) 
	{
		case 1:
		{
			start_movement = 1;
			int game = Level1_Start(x,y,oldx_OG,oldy_OG,&x,&y,num_of_hearts,difficulty);
			if (game == 1)
			{
				gameover(&start_game,&current_diff_choice,&difficulty);
			}
			game = 0;
			break;
		}
		case 2:
		{
			start_movement = 1;
			int game = Level2_Start(x,y,oldx_OG,oldy_OG,&x,&y,num_of_hearts,difficulty);
			if (game == 1)
			{
				gameover(&start_game,&current_diff_choice,&difficulty);
			}
			game = 0;
			break;
		}
		case 3:
		{
			start_movement = 1;
			int game = Level3_Start(x,y,oldx_OG,oldy_OG,&x,&y,num_of_hearts,difficulty);
			if (game == 1)
			{
				gameover(&start_game,&current_diff_choice,&difficulty);
			}
			game = 0;
			break;
		}
		case 4:
		{
			gameend(&start_game,&current_diff_choice,&difficulty);
		}
		default:
		{
			break;
		}
	}

        // Handle player movement and input
        if (start_game == 1) {
            hmoved = vmoved = 0;
            hinverted = vinverted = 0;
            RightButtonPressed(&x, &hmoved, &hinverted);
            LeftButtonPressed(&x, &hmoved, &hinverted);
            UpButtonPressed(&y, &vmoved, &vinverted);
            DownButtonPressed(&y, &vmoved, &vinverted);

            if (vmoved || hmoved) {
                // Redraw only if there has been movement to reduce flicker
                fillRectangle(oldx, oldy, 12, 16, 0);
                oldx = x;
                oldy = y;
                if (hmoved) {
                    // Alternate between knight animations for horizontal movement
                    if (toggle)
                        putImage(x, y, 12, 16, knight_animation1, hinverted, 0);
                    else
                        putImage(x, y, 12, 16, knight_animation2, hinverted, 0);
                    
                    toggle = toggle ^ 1;
                } else {
                    // Use a different animation for vertical movement
                    putImage(x, y, 12, 16, knight_animation3, 0, vinverted);
                }
            }
        }

        delay(30); // Short delay for game loop timing
    }
    return 0;
}

void initSysTick(void)
{
	SysTick->LOAD = 48000;
	SysTick->CTRL = 7;
	SysTick->VAL = 10;
	__asm(" cpsie i "); // enable interrupts
}
void SysTick_Handler(void)
{
	milliseconds++;
	milliseconds_timer++;
}
void initClock(void)
{
// This is potentially a dangerous function as it could
// result in a system with an invalid clock signal - result: a stuck system
	// Set the PLL up
	// First ensure PLL is disabled
	RCC->CR &= ~(1u<<24);
	while( (RCC->CR & (1 <<25))); // wait for PLL ready to be cleared
        
// Warning here: if system clock is greater than 24MHz then wait-state(s) need to be
// inserted into Flash memory interface
				
	FLASH->ACR |= (1 << 0);
	FLASH->ACR &=~((1u << 2) | (1u<<1));
	// Turn on FLASH prefetch buffer
	FLASH->ACR |= (1 << 4);
	// set PLL multiplier to 12 (yielding 48MHz)
	RCC->CFGR &= ~((1u<<21) | (1u<<20) | (1u<<19) | (1u<<18));
	RCC->CFGR |= ((1<<21) | (1<<19) ); 

	// Need to limit ADC clock to below 14MHz so will change ADC prescaler to 4
	RCC->CFGR |= (1<<14);

	// and turn the PLL back on again
	RCC->CR |= (1<<24);        
	// set PLL as system clock source 
	RCC->CFGR |= (1<<1);
}
void delay(volatile uint32_t dly)
{
	uint32_t end_time = dly + milliseconds;
	while(milliseconds != end_time)
		__asm(" wfi "); // sleep
}

void enablePullUp(GPIO_TypeDef *Port, uint32_t BitNumber)
{
	Port->PUPDR = Port->PUPDR &~(3u << BitNumber*2); // clear pull-up resistor bits
	Port->PUPDR = Port->PUPDR | (1u << BitNumber*2); // set pull-up bit
}
void pinMode(GPIO_TypeDef *Port, uint32_t BitNumber, uint32_t Mode)
{
	uint32_t mode_value = Port->MODER;
	Mode = Mode << (2 * BitNumber);
	mode_value = mode_value & ~(3u << (BitNumber * 2));
	mode_value = mode_value | Mode;
	Port->MODER = mode_value;
}
int isInside(uint16_t x1, uint16_t y1, uint16_t w, uint16_t h, uint16_t px, uint16_t py)
{
	// checks to see if point px,py is within the rectange defined by x,y,w,h
	uint16_t x2,y2;
	x2 = x1+w;
	y2 = y1+h;
	int rvalue = 0;
	if ( (px >= x1) && (px <= x2))
	{
		// ok, x constraint met
		if ( (py >= y1) && (py <= y2))
			rvalue = 1;
	}
	return rvalue;
}

void setupIO()
{
	RCC->AHBENR |= (1 << 18) + (1 << 17); // enable Ports A and B
	display_begin();
	pinMode(GPIOB,4,0);
	pinMode(GPIOB,5,0);
	pinMode(GPIOA,8,0);
	pinMode(GPIOA,11,0);
	pinMode(GPIOB,3,1); // Make GPIOB 3 a output pin (This is used for the RED LED)
	pinMode(GPIOB,0,1); // Make GPIOB 0 a output pin (This is used for the Green LED)
	enablePullUp(GPIOB,4);
	enablePullUp(GPIOB,5);
	enablePullUp(GPIOA,11);
	enablePullUp(GPIOA,8);
}
// Function for handling right button press. Moves the player to the right.
void RightButtonPressed(uint16_t* x,int* hmoved, int* hinverted) {
    if ((GPIOB->IDR & (1 << 4))==0) { // Check if the right button is pressed
        if (*x < 110) { // Ensure the player doesn't move out of the screen bounds
            *x = (*x + 1); // Move the player to the right
            *hmoved = 1; // Flag to indicate horizontal movement
            *hinverted = 0; // Flag to manage sprite inversion (flipping)
        }                        
    }
}

// Function for handling left button press. Moves the player to the left.
void LeftButtonPressed(uint16_t* x,int* hmoved, int* hinverted) {
    if ((GPIOB->IDR & (1 << 5))==0) { // Check if the left button is pressed
        if (*x > 10) { // Ensure the player doesn't move out of the screen bounds
            *x = (*x - 1); // Move the player to the left
            *hmoved = 1; // Flag to indicate horizontal movement
            *hinverted = 1; // Flag to manage sprite inversion (flipping)
        }                        
    }
}

// Function for handling up button press. Moves the player up.
void UpButtonPressed(uint16_t* y,int* vmoved, int* vinverted) {
    if ((GPIOA->IDR & (1 << 11)) == 0) { // Check if the up button is pressed
        if (*y < 140) { // Ensure the player doesn't move out of the screen bounds
            *y = (*y + 1); // Move the player up
            *vmoved = 1; // Flag to indicate vertical movement
            *vinverted = 0; // Flag to manage sprite inversion (unused in vertical movement)
        }
    }
}

// Function for handling down button press. Moves the player down.
void DownButtonPressed(uint16_t* y,int* vmoved, int* vinverted) {
    if ((GPIOA->IDR & (1 << 8)) == 0) { // Check if the down button is pressed
        if (*y > 32) { // Ensure the player doesn't move out of the screen bounds
            *y = (*y - 1); // Move the player down
            *vmoved = 1; // Flag to indicate vertical movement
            *vinverted = 1; // Flag to manage sprite inversion (unused in vertical movement)
        }
    }
}

// Starts and handles the logic for Level 1 of the game
int Level1_Start(uint16_t x, uint16_t y,uint16_t oldx, uint16_t oldy,uint16_t *px,uint16_t *py,int hearts_used,int difficulty)
{
    // Define various logs for different events in the level
    char level1_started_log[] = "Level 1 Started!";
    char level1_complete_log[] = "Level 1 Complete!";
    char key_pickup_log[] = "Key Found";
    char died_spike_log[] = "Player died to spike";

    static int start_game = 0; // Flag to check if the game has started
    static int heart_gone = 0; // Counter for the number of lost hearts
    static int key_pickup = 0; // Flag to check if the key has been picked up

    // Arrays to store the positions of hearts and key on the screen
    int heart_location_x[3] = {85,100,115};
    int heart_location_y[3] = {6,6,6};
    char num_of_hearts[NUM_OF_CHAR]; // Buffer to store number of hearts as a string
    int player_location_x[NUM_OF_RANDOM_LOCATIONS] = {53,100}; // Initial player positions
    int player_location_y[NUM_OF_RANDOM_LOCATIONS] = {125,80};
    uint32_t random_location_picker = random(0,1); // Randomly pick a starting location

    // Key pickup and door position information
    int pickup_key_location_x[3] = {10,20,100};
    int pickup_key_location_y[3] = {35,130,50};
    int key_location_x[NUM_OF_KEYS_1_DISPLAYED] = {5};
    int key_location_y[NUM_OF_KEYS_1_DISPLAYED] = {6};
    char key_display[NUM_OF_CHAR]; // Buffer for displaying key count

    // Spike position information
    int spike_location_x = 10;
    int spike_location_y = 60;

    // Door position information
    int door_location_x = 115;
    int door_location_y = 90;

    // Array containing the musical notes for level 1
    int notes[LEVEL_1_MUSIC] = {C4,D4,E4,G4,E4,D4,C4,G4,E4,C4};

    // More detailed logic goes here...
    // This includes rendering the level layout, handling player movements and interactions
    // Checking conditions for level completion, key pickups, and updating the player's health status

    // Return an integer to indicate the completion or continuation of the level
	while (start_game == 0)
	{
		delay(100);
		printTextX2("Level 1", 25, 20, RGBToWord(255,255,255), 0);
		printText("Difficulty ", 10, 50, RGBToWord(255,255,255), 0);
		Difficulty_Display(difficulty);
		printText("Collect ", 15, 65, RGBToWord(255,255,255), 0);
		sprintf(key_display,"%d",NUMOF_KEYS_1);
		printText(key_display,70,65,RGBToWord(255,255,255),0);
		putImage(80,60,12,16,key,0,0);
		sprintf(num_of_hearts,"%d",hearts_used);
		printText(num_of_hearts,20,80,RGBToWord(255,255,255),0);
		putImage(30,75,12,16,heart,0,0);
		printText("Beware of:",15,100,RGBToWord(255,255,255),0);

		if (difficulty == DIFFICULTY_AMOUNT)
		{
			putImage(90,95,12,16,nightmare_spike,0,0);
		}
		else
		{
			putImage(90,95,12,16,spike,0,0);
		}
		printText("<-- AND -->", 20, 120, RGBToWord(255,255,255), 0);

		if ((GPIOB->IDR & (1 << 4)) == 0 && (GPIOB->IDR & (1 << 5)) == 0) // right pressed
		{	
			start_game = 1;		
			fillRectangle(0,0,128,160,RGBToWord(0,0,0));

			while (!(GPIOB->IDR & (1 << 4))&& !(GPIOB->IDR & (1 << 5)))
			{
				seed++;
			}
			

			while (1)
			{
				// Display the Keys
				for (int i = 0; i < NUM_OF_KEYS_1_DISPLAYED; i++)
				{
					putImage(key_location_x[i],key_location_y[i],12,16,key,0,0);
				}
				
				// Display the hearts
				for (int i = 0; i < hearts_used; i++)
				{
					if (difficulty == DIFFICULTY_AMOUNT)
					{
						putImage(heart_location_x[i],heart_location_y[i],12,16,nightmare_heart,0,0);
					}
					else
					{
						putImage(heart_location_x[i],heart_location_y[i],12,16,heart,0,0);
					}
				}

				fillRectangle(2,25,168,1,RGBToWord(255,255,255));

				for (int i = 0; i < NUMOF_KEYS_1; i++)
				{
					putImage(pickup_key_location_x[i],pickup_key_location_y[i],12,16,key,0,0);
				}
				putImage(x,y,12,16,knight_animation1,0,0);
				// We turn red off since we are in a level now. 
				RedOff();
				// Green LED tells you the game is running and we are in a level.
				GreenOn();
				serial_log(level1_started_log);
				break;
			}							
		}
	}
	if (difficulty == DIFFICULTY_AMOUNT)
	{
		if (timer < 10)
		{
			fillRectangle(55,12,12,12,RGBToWord(0,0,0));
			sprintf(timer_string,"%d",timer);
			printText(timer_string,55,12,RGBToWord(255,0,0),0);
		}
		else
		{
			sprintf(timer_string,"%d",timer);
			printText(timer_string,55,12,RGBToWord(255,255,255),0);
		}
		if (max_time <= 0)
		{
			start_game = 0;
			heart_gone = 0;
			key_pickup = 0;
			return 1;
		}
		if (milliseconds_timer >= 1000)
		{
			max_time = max_time - 1000;
			milliseconds_timer = 0;
			timer--;
		}
		milliseconds_timer++;
	}
	if (music_flag == 0)
	{
		play_music(notes,LEVEL_1_MUSIC);
	}
	// Check if Player can go through door and finish level.
	if (isInside(door_location_x,door_location_y,12,16,x,y) || isInside(door_location_x,door_location_y,12,16,x+12,y) || isInside(door_location_x,door_location_y,12,16,x,y+16) || isInside(door_location_x,door_location_y,12,16,x+12,y+16))
	{
		// Checks if all keys have been obtained 
		if (key_pickup == NUMOF_KEYS_1)
		{
			// We turn Green off since we are not in a level now. 
			GreenOff();
			// Red LED tells you the game is running and we are not in a level.
			RedOn();
			playNote(0);
			music_flag = 1;
			int tmp = hearts_used - heart_gone;
			char currentHeart[NUM_OF_CHAR];
			sprintf(currentHeart,"%d",tmp);

			fillRectangle(0,0,128,160,RGBToWord(0,0,0));
			printTextX2("Level 1", 25, 20, RGBToWord(255,255,255), 0);
			printTextX2("Complete!", 15, 40, RGBToWord(255,255,255), 0);
			printText("Hearts Left", 5, 70, RGBToWord(255,255,255), 0);
			printText(currentHeart,88,70,RGBToWord(255,255,255),0);
			putImage(100,63,12,16,heart,0,0);
			printText("<--", 5, 90, RGBToWord(255,255,255), 0);
			serial_log(level1_complete_log);
			while (1)
			{
				if ((GPIOB->IDR & (1 << 5))==0) // left pressed
				{
					start_game = 0;
					heart_gone = 0;
					key_pickup = 0;
					start_movement = 0;
					fillRectangle(0,0,128,160,RGBToWord(0,0,0));
					break;
				}
			}
			// Move onto the next level
			current_level++;
			// Clear the current screen
		}
	}
	// Key pickup check
	if (key_pickup == 0)
	{
		for (int i = 0; i < NUMOF_KEYS_1; i++)
		{
			if (isInside(pickup_key_location_x[i],pickup_key_location_y[i],12,16,x,y) || isInside(pickup_key_location_x[i],pickup_key_location_y[i],12,16,x+12,y) || isInside(pickup_key_location_x[i],pickup_key_location_y[i],12,16,x,y+16) || isInside(pickup_key_location_x[i],pickup_key_location_y[i],12,16,x+12,y+16))
			{
				putImage(pickup_key_location_x[i],pickup_key_location_y[i],12,16,taken_key,0,0);
				putImage(key_location_x[key_pickup],key_location_y[key_pickup],12,16,taken_key,0,0);
				key_pickup++;
				serial_log(key_pickup_log);
			}
		}	
	}
	else
	{
		if (key_pickup >= 1)
		{
			for (int i = 0; i < NUMOF_KEYS_1; i++)
			{
				if (!(isInside(pickup_key_location_x[i],pickup_key_location_y[i],12,16,x,y) || isInside(pickup_key_location_x[i],pickup_key_location_y[i],12,16,x+12,y) || isInside(pickup_key_location_x[i],pickup_key_location_y[i],12,16,x,y+16) || isInside(pickup_key_location_x[i],pickup_key_location_y[i],12,16,x+12,y+16)))
				{
					putImage(pickup_key_location_x[i],pickup_key_location_y[i],12,16,taken_key,0,0);
				}
			}
		}
	}
	// Draws the spike
	if (difficulty == DIFFICULTY_AMOUNT)
	{
		putImage(spike_location_x,spike_location_y,12,16,nightmare_spike,0,0);
	}
	else
	{
		putImage(spike_location_x,spike_location_y,12,16,spike,0,0);
	}

	// Draws the Door
	putImage(door_location_x,door_location_y,12,16,door,0,0);

	// Detects if user hit the spike 
	if ((isInside(spike_location_x,spike_location_y,12,16,x,y) || isInside(spike_location_x,spike_location_y,12,16,x+12,y) || isInside(spike_location_x,spike_location_y,12,16,x,y+16) || isInside(spike_location_x,spike_location_y,12,16,x+12,y+16)))
	{
		serial_log(died_spike_log);
		playNote(0);
		music_flag = 1;
		// The player has been hit so we automatically punish him by setting him back to the original positoon.
		if (difficulty == DIFFICULTY_AMOUNT)
		{
			putImage(spike_location_x,spike_location_y,12,16,nightmare_spike,0,0);
		}
		else
		{
			putImage(spike_location_x,spike_location_y,12,16,spike,0,0);
		}
		// Delete current positoon of player
		fillRectangle(x,y,12,16,RGBToWord(0,0,0));
		// Respawn the player back at the Original Position.
		*px = player_location_x[random_location_picker];
		*py = player_location_y[random_location_picker];
		
		// Player loses a heart.
		putImage(heart_location_x[heart_gone],heart_location_y[heart_gone],12,16,hearts_empty,0,0);
		heart_gone++;
		delay(1500);
		putImage(*px,*py,12,16,knight_animation1,0,0);
		music_flag = 0;
	}	
	if (heart_gone == hearts_used)
	{
		start_game = 0;
		heart_gone = 0;
		key_pickup = 0;
		return 1;
	}
}

// Starts and handles the logic for Level 2 of the game
int Level2_Start(uint16_t x, uint16_t y, uint16_t oldx, uint16_t oldy, uint16_t *px, uint16_t *py, int hearts_used, int difficulty) {
    // Logging and status variables for different events in the level
    char level2_started_log[] = "Level 2 Started!";
    char level2_complete_log[] = "Level 2 Complete!";
    char died_spike_log[] = "Player died to spike";
    char died_skeleton_log[] = "Player died to skeleton";
    char key_pickup_log[] = "Key Found";

    static int start_game = 0; // Flag to manage the start status of the game
    static int clear_screen = 0; // Flag to determine if the screen should be cleared
    static int heart_gone = 0; // Track the number of hearts lost
    static int key_pickup[NUMOF_KEYS_2] = {0, 0}; // Track if keys have been picked up
    static int amount_keys = 0; // Total number of keys picked up

    // Arrays for storing positions of hearts, keys, and player's spawn points
    int heart_location_x[3] = {85, 100, 115};
    int heart_location_y[3] = {6, 6, 6};
    char num_of_hearts[NUM_OF_CHAR]; // Buffer to store number of hearts as a string
    int player_location_x[NUM_OF_RANDOM_LOCATIONS] = {5, 80}; // Player spawn points
    int player_location_y[NUM_OF_RANDOM_LOCATIONS] = {125, 120};
    uint32_t random_location_picker = random(0, 1); // Randomly picking a starting location

    // Key pickup locations
    int pickup_key_location_x[NUMOF_KEYS_2] = {10, 100};
    int pickup_key_location_y[NUMOF_KEYS_2] = {35, 35};
    int key_location_x[NUM_OF_KEYS_2_DISPLAYED] = {5, 20};
    int key_location_y[NUM_OF_KEYS_2_DISPLAYED] = {6, 6};
    char key_display[NUM_OF_CHAR]; // Buffer for displaying key count

    // Spike locations
    int spike_location_x[NUMOF_SPIKES_2] = {10, 90};
    int spike_location_y[NUMOF_SPIKES_2] = {55, 37};

    // Enemy (skeleton) positions
    int enemy_x_left = 30;
    int enemy_x_right = 115;
    static int enemy_current_pos_x = 75;
    static int enemy_current_pos_y = 65;
    static int toggle = 0; // Toggle for enemy movement direction

    // Door position
    int door_location_x = 115;
    int door_location_y = 130;

    // Array containing the musical notes for level 2
    int notes[LEVEL_2_MUSIC] = {G4, B4, D5, G5, D5, B4, G4, A4, B4, G4, B4, D5, G5, D5, B4, G4};

    // Rest of the function contains the game logic for Level 2
    // Similar to Level 1 but with additional challenges like more keys, spikes, and an enemy (skeleton)
    // This includes rendering the level, handling player movements and interactions
    // Checking conditions for level completion, key pickups, and updating the player's health status

    // Return an integer to indicate the completion or continuation of the level


	while (clear_screen == 0)
	{
		fillRectangle(0,0,128,160,RGBToWord(0,0,0));
		// New Character positions
		*px = player_location_x[random_location_picker];
		*py = player_location_y[random_location_picker];
		clear_screen++;
	}

	while (start_game == 0)
	{
		printTextX2("Level 2", 25, 20, RGBToWord(255,255,255), 0);
		printText("Difficulty ", 10, 50, RGBToWord(255,255,255), 0);
		Difficulty_Display(difficulty);
		printText("Collect ", 15, 65, RGBToWord(255,255,255), 0);
		sprintf(key_display,"%d",NUMOF_KEYS_2);
		printText(key_display,70,65,RGBToWord(255,255,255),0);
		putImage(80,60,12,16,key,0,0);
		sprintf(num_of_hearts,"%d",hearts_used);
		printText(num_of_hearts,20,80,RGBToWord(255,255,255),0);
		putImage(30,75,12,16,heart,0,0);
		printText("Beware of:",15,100,RGBToWord(255,255,255),0);
		if (difficulty == DIFFICULTY_AMOUNT)
		{
			putImage(90,95,12,16,nightmare_spike,0,0);
			putImage(110,95,12,16,night_skeleton_run,0,0);
		}
		else
		{
			putImage(90,95,12,16,spike,0,0);
			putImage(110,95,12,16,skeleton_run,0,0);
		}
		printText("<-- AND -->", 20, 120, RGBToWord(255,255,255), 0);

		if ((GPIOB->IDR & (1 << 4)) == 0 && (GPIOB->IDR & (1 << 5)) == 0) // right pressed
		{	
			start_game = 1;		
			fillRectangle(0,0,128,160,RGBToWord(0,0,0));
			while (1)
			{
				// Display the Keys
				for (int i = 0; i < NUM_OF_KEYS_2_DISPLAYED; i++)
				{
					putImage(key_location_x[i],key_location_y[i],12,16,key,0,0);
				}
				
				// Display the hearts
				for (int i = 0; i < hearts_used; i++)
				{
					if (difficulty == DIFFICULTY_AMOUNT)
					{
						putImage(heart_location_x[i],heart_location_y[i],12,16,nightmare_heart,0,0);					
					}
					else
					{
						putImage(heart_location_x[i],heart_location_y[i],12,16,heart,0,0);
					}
				}

				fillRectangle(2,25,168,1,RGBToWord(255,255,255));

				for (int i = 0; i < NUMOF_KEYS_2; i++)
				{
					putImage(pickup_key_location_x[i],pickup_key_location_y[i],12,16,key,0,0);
				}
				music_flag = 0;
				// We turn red off since we are in a level now. 
				RedOff();
				// Green LED tells you the game is running and we are in a level.
				GreenOn();
				serial_log(level2_started_log);
				break;
			}							
		}
	}
	if (difficulty == DIFFICULTY_AMOUNT)
	{
		if (timer < 10)
		{
			fillRectangle(55,12,12,12,RGBToWord(0,0,0));
			sprintf(timer_string,"%d",timer);
			printText(timer_string,55,12,RGBToWord(255,0,0),0);
		}
		else
		{
			sprintf(timer_string,"%d",timer);
			printText(timer_string,55,12,RGBToWord(255,255,255),0);
		}
		if (max_time <= 0)
		{
			start_game = 0;
			heart_gone = 0;
			for (int i = 0; i < NUMOF_KEYS_2; i++)
			{
				key_pickup[i] = 0;
			}
			return 1;
		}
		if (milliseconds_timer >= 1000)
		{
			max_time = max_time - 1000;
			milliseconds_timer = 0;
			timer--;
		}
		milliseconds_timer++;
	}
	if (music_flag == 0)
	{
		play_music(notes,LEVEL_2_MUSIC);
	}

	// Check if Player can go through door and finish level.
	if (isInside(door_location_x,door_location_y,12,16,x,y) || isInside(door_location_x,door_location_y,12,16,x+12,y) || isInside(door_location_x,door_location_y,12,16,x,y+16) || isInside(door_location_x,door_location_y,12,16,x+12,y+16))
	{
		// Checks if all keys have been obtained 
		if (amount_keys == NUMOF_KEYS_2)
		{
			// We turn Green off since we are not in a level now. 
			GreenOff();
			// Red LED tells you the game is running and we are not in a level.
			RedOn();
			serial_log(level2_complete_log);
			playNote(0);
			music_flag = 1;
			int tmp = hearts_used - heart_gone;
			char currentHeart[NUM_OF_CHAR];
			sprintf(currentHeart,"%d",tmp);

			fillRectangle(0,0,128,160,RGBToWord(0,0,0));
			printTextX2("Level 2", 25, 20, RGBToWord(255,255,255), 0);
			printTextX2("Complete!", 15, 40, RGBToWord(255,255,255), 0);
			printText("Hearts Left", 5, 70, RGBToWord(255,255,255), 0);
			printText(currentHeart,88,70,RGBToWord(255,255,255),0);
			putImage(100,63,12,16,heart,0,0);
			printText("<--", 5, 90, RGBToWord(255,255,255), 0);
			while (1)
			{
				if ((GPIOB->IDR & (1 << 5))==0) // left pressed
				{
					start_game = 0;
					clear_screen = 0;
					heart_gone = 0;
					amount_keys = 0;
					enemy_current_pos_x = 75;
					enemy_current_pos_y = 65;
					toggle = 0;
					for (int i = 0; i < NUMOF_KEYS_2; i++)
					{
						key_pickup[i] = 0;
					}
					amount_keys = 0;
					start_movement = 0;
					music_flag = 0;
					break;
				}
			}
			// Move onto the next level
			current_level++;
			// Clear the current screen
			fillRectangle(0,0,128,160,RGBToWord(0,0,0));
		}
	}
	// Key pickup check
	for (int i = 0; i < NUMOF_KEYS_2; i++)
	{
		if (key_pickup[i] == 0)
		{
			if (isInside(pickup_key_location_x[i],pickup_key_location_y[i],12,16,x,y) || isInside(pickup_key_location_x[i],pickup_key_location_y[i],12,16,x+12,y) || isInside(pickup_key_location_x[i],pickup_key_location_y[i],12,16,x,y+16) || isInside(pickup_key_location_x[i],pickup_key_location_y[i],12,16,x+12,y+16))
			{
				putImage(pickup_key_location_x[i],pickup_key_location_y[i],12,16,taken_key,0,0);
				putImage(key_location_x[amount_keys],key_location_y[amount_keys],12,16,taken_key,0,0);
				key_pickup[i] = 1;
				amount_keys++;
				serial_log(key_pickup_log);
			}
		}
		if (key_pickup[i] == 1)
		{
			if (!(isInside(pickup_key_location_x[i],pickup_key_location_y[i],12,16,x,y) || isInside(pickup_key_location_x[i],pickup_key_location_y[i],12,16,x+12,y) || isInside(pickup_key_location_x[i],pickup_key_location_y[i],12,16,x,y+16) || isInside(pickup_key_location_x[i],pickup_key_location_y[i],12,16,x+12,y+16)))
			{
				putImage(pickup_key_location_x[i],pickup_key_location_y[i],12,16,taken_key,0,0);
			}
		}
	}

	// Draws the Door
	putImage(door_location_x,door_location_y,12,16,door,0,0);

	// Draws the Enemy
	if (enemy_current_pos_x != enemy_x_right && toggle == 0)
	{
		if (difficulty == DIFFICULTY_AMOUNT)
		{
			putImage(enemy_current_pos_x,enemy_current_pos_y,12,16,night_skeleton_run,0,0);
		}
		else
		{
			putImage(enemy_current_pos_x,enemy_current_pos_y,12,16,skeleton_run,0,0);
		}
		drawRectangle(enemy_current_pos_x,enemy_current_pos_y,12,16,RGBToWord(0,0,0));
		enemy_current_pos_x++;
		if (difficulty == DIFFICULTY_AMOUNT)
		{
			putImage(enemy_current_pos_x,enemy_current_pos_y,12,16,night_skeleton_run,0,0);
		}
		else
		{
			putImage(enemy_current_pos_x,enemy_current_pos_y,12,16,skeleton_run,0,0);
		}
	}
	else
	{
		toggle = 1;
	}
	if (enemy_current_pos_x != enemy_x_left && toggle == 1)
	{
		if (difficulty == DIFFICULTY_AMOUNT)
		{
			putImage(enemy_current_pos_x,enemy_current_pos_y,12,16,night_skeleton_run,1,0);
		}
		else
		{
			putImage(enemy_current_pos_x,enemy_current_pos_y,12,16,skeleton_run,1,0);
		}
		drawRectangle(enemy_current_pos_x,enemy_current_pos_y,12,16,RGBToWord(0,0,0));
		enemy_current_pos_x--;
		if (difficulty == DIFFICULTY_AMOUNT)
		{
			putImage(enemy_current_pos_x,enemy_current_pos_y,12,16,night_skeleton_run,1,0);
		}
		else
		{
			putImage(enemy_current_pos_x,enemy_current_pos_y,12,16,skeleton_run,1,0);
		}
	}
	else
	{
		toggle = 0; 
	}

	// Check to see if the player is hit by the enemy 
	if ((isInside(enemy_current_pos_x,enemy_current_pos_y,12,16,x,y) || isInside(enemy_current_pos_x,enemy_current_pos_y,12,16,x+12,y) || isInside(enemy_current_pos_x,enemy_current_pos_y,12,16,x,y+16) || isInside(enemy_current_pos_x,enemy_current_pos_y,12,16,x+12,y+16)))
	{
		serial_log(died_skeleton_log);
		if (toggle == 1)
		{
			if (difficulty == DIFFICULTY_AMOUNT)
			{
				putImage(enemy_current_pos_x,enemy_current_pos_y,12,16,night_skeleton_attack1,1,0);

			}
			else
			{
				putImage(enemy_current_pos_x,enemy_current_pos_y,12,16,skeleton_attack1,1,0);
			}
		}
		else
		{
			if (difficulty == DIFFICULTY_AMOUNT)
			{
				putImage(enemy_current_pos_x,enemy_current_pos_y,12,16,night_skeleton_attack1,0,0);

			}
			else
			{
				putImage(enemy_current_pos_x,enemy_current_pos_y,12,16,skeleton_attack1,0,0);
			}
		}
		// The player has been hit so we automatically punish him by setting him back to the original positoon.
		// Delete current positoon of player
		fillRectangle(x,y,12,16,RGBToWord(0,0,0));

		// Respawn the player back at the Original Position.
		*px = player_location_x[random_location_picker];
		*py = player_location_y[random_location_picker];
		
		playNote(0);
		music_flag = 1;
		// Player loses a heart.
		putImage(heart_location_x[heart_gone],heart_location_y[heart_gone],12,16,hearts_empty,0,0);
		heart_gone++;
		delay(1500);
		music_flag = 0;
		putImage(*px,*py,12,16,knight_animation1,0,0);
	}	
	// Draws the spike
	for (int i = 0; i < NUMOF_SPIKES_2; i++)
	{
		if (difficulty == DIFFICULTY_AMOUNT)
		{
			putImage(spike_location_x[i],spike_location_y[i],12,16,nightmare_spike,0,0);
		}
		else
		{
			putImage(spike_location_x[i],spike_location_y[i],12,16,spike,0,0);
		}
		if ((isInside(spike_location_x[i],spike_location_y[i],12,16,x,y) || isInside(spike_location_x[i],spike_location_y[i],12,16,x+12,y) || isInside(spike_location_x[i],spike_location_y[i],12,16,x,y+16) || isInside(spike_location_x[i],spike_location_y[i],12,16,x+12,y+16)))
		{
			serial_log(died_spike_log);
			playNote(0);
			// The player has been hit so we automatically punish him by setting him back to the original positoon.
			if (difficulty == DIFFICULTY_AMOUNT)
			{
				putImage(spike_location_x[i],spike_location_y[i],12,16,nightmare_spike,0,0);
			}
			else
			{
				putImage(spike_location_x[i],spike_location_y[i],12,16,spike,0,0);
			}
			//putImage(spike_location_x[0],spike_location_y[0],12,16,spike_level1,0,0);
			// Delete current positoon of player
			fillRectangle(x,y,12,16,RGBToWord(0,0,0));
			// Respawn the player back at the Original Position.
			*px = 5;
			*py = 125;
			
			// Player loses a heart.
			putImage(heart_location_x[heart_gone],heart_location_y[heart_gone],12,16,hearts_empty,0,0);
			heart_gone++;
			delay(1500);
			music_flag = 0;
			putImage(*px,*py,12,16,knight_animation1,0,0);
		}	
	}

	if (heart_gone == hearts_used)
	{
		start_game = 0;
		clear_screen = 0;
		heart_gone = 0;
		for (int i = 0; i < NUMOF_KEYS_2; i++)
		{
			key_pickup[i] = 0;
		}
		amount_keys = 0;
		enemy_current_pos_x = 75;
		enemy_current_pos_y = 65;
		toggle = 0;
		return 1;
	}
}

// Function to start and handle Level 3 logic in the game
int Level3_Start(uint16_t x, uint16_t y, uint16_t oldx, uint16_t oldy, uint16_t *px, uint16_t *py, int hearts_used, int difficulty) {
    // Logging messages for different events within the level
    char level3_started_log[] = "Level 3 Started!";
    char level3_complete_log[] = "Level 3 Complete!";
    char died_spike_log[] = "Player died to spike";
    char died_skeleton_log[] = "Player died to skeleton";
    char key_pickup_log[] = "Key Found";

    static int start_game = 0; // Flag to check if the game level has started
    static int clear_screen = 0; // Flag to determine if the screen needs clearing
    static int key_pickup[NUMOF_KEYS_3] = {0, 0, 0}; // Array to track picked-up keys
    static int amount_keys = 0; // Total number of keys picked up
    static int heart_gone = 0; // Number of hearts lost

    // Arrays for storing positions of hearts on the screen
    int heart_location_x[3] = {85, 100, 115};
    int heart_location_y[3] = {6, 6, 6};
    char num_of_hearts[NUM_OF_CHAR]; // Buffer to store number of hearts as a string
    int player_location_x[NUM_OF_RANDOM_LOCATIONS] = {110, 115}; // Player spawn points
    int player_location_y[NUM_OF_RANDOM_LOCATIONS] = {40, 40};
    uint32_t random_location_picker = random(0, 1); // Randomly picking a starting location

    // Key pickup positions
    int pickup_key_location_x[NUMOF_KEYS_3] = {5, 5, 90};
    int pickup_key_location_y[NUMOF_KEYS_3] = {140, 95, 140};
    int key_location_x[NUM_OF_KEYS_3_DISPLAYED] = {5, 20, 35};
    int key_location_y[NUM_OF_KEYS_3_DISPLAYED] = {6, 6, 6};
    char key_display[NUM_OF_CHAR]; // Buffer for displaying key count

    // Spike positions
    int spike_location_x[NUMOF_SPIKES_3] = {25, 30, 100};
    int spike_location_y[NUMOF_SPIKES_3] = {140, 40, 80};

    // Enemy (skeleton) positions and movement toggles
    int enemy_x_left[NUM_OF_ENEMY_3] = {10, 10};
    int enemy_x_right[NUM_OF_ENEMY_3] = {90, 90};
    static int enemy_current_pos_x[NUM_OF_ENEMY_3] = {75, 75};
    static int enemy_current_pos_y[NUM_OF_ENEMY_3] = {75, 110};
    static int toggle[NUM_OF_ENEMY_3] = {0, 0}; // Toggle for enemy movement direction
    int pos_x_old[NUM_OF_ENEMY_3] = {75, 75}; // Previous positions of enemies
    int pos_y_old[NUM_OF_ENEMY_3] = {75, 110};

    // Door position
    int door_location_x = 5; 
    int door_location_y = 45;

    // Array containing the musical notes for level 3
    int notes[LEVEL_3_MUSIC] = {A4, C5, E5, A5, G5, E5, C5, A4, B4, D5, F5, B5, A5, F5, D5, B4, E4, G4, B4, E5, D5, B4, G4, E4};

    // Rest of the function contains the game logic for Level 3
    // This includes rendering the level layout, handling player movements and interactions
    // Checking conditions for level completion, key pickups, and updating the player's health status
    // Additional complexity includes more enemies and different key and spike positions

    // Return an integer to indicate the completion or continuation of the level
	
	while (clear_screen == 0)
	{
		fillRectangle(0,0,128,160,RGBToWord(0,0,0));
		// New Character positions
		*px = player_location_x[random_location_picker];
		*py = player_location_y[random_location_picker];
		clear_screen++;
	}
	while (start_game == 0)
	{
		printTextX2("Level 3", 25, 20, RGBToWord(255,255,255), 0);
		printText("Difficulty ", 10, 50, RGBToWord(255,255,255), 0);
		Difficulty_Display(difficulty);
		printText("Collect ", 15, 65, RGBToWord(255,255,255), 0);
		sprintf(key_display,"%d",NUMOF_KEYS_3);
		printText(key_display,70,65,RGBToWord(255,255,255),0);
		putImage(80,60,12,16,key,0,0);
		sprintf(num_of_hearts,"%d",hearts_used);
		printText(num_of_hearts,20,80,RGBToWord(255,255,255),0);
		putImage(30,75,12,16,heart,0,0);
		printText("Beware of:",15,100,RGBToWord(255,255,255),0);
		if (difficulty == DIFFICULTY_AMOUNT)
		{
			putImage(90,95,12,16,nightmare_spike,0,0);
			putImage(110,95,12,16,night_skeleton_run,0,0);
		}
		else
		{
			putImage(90,95,12,16,spike,0,0);
			putImage(110,95,12,16,skeleton_run,0,0);
		}
		printText("<-- AND -->", 20, 120, RGBToWord(255,255,255), 0);

		if ((GPIOB->IDR & (1 << 4)) == 0 && (GPIOB->IDR & (1 << 5)) == 0) // right pressed
		{	
			start_game = 1;		
			fillRectangle(0,0,128,160,RGBToWord(0,0,0));
			while (1)
			{
				// Display the Keys
				for (int i = 0; i < NUM_OF_KEYS_3_DISPLAYED; i++)
				{
					putImage(key_location_x[i],key_location_y[i],12,16,key,0,0);
				}
				
				// Display the hearts
				for (int i = 0; i < hearts_used; i++)
				{
					if (difficulty == DIFFICULTY_AMOUNT)
					{
						putImage(heart_location_x[i],heart_location_y[i],12,16,nightmare_heart,0,0);					
					}
					else
					{
						putImage(heart_location_x[i],heart_location_y[i],12,16,heart,0,0);
					}
				}

				fillRectangle(2,25,168,1,RGBToWord(255,255,255));

				for (int i = 0; i < NUMOF_KEYS_3; i++)
				{
					putImage(pickup_key_location_x[i],pickup_key_location_y[i],12,16,key,0,0);
				}
				// We turn red off since we are in a level now. 
				RedOff();
				// Green LED tells you the game is running and we are in a level.
				GreenOn();
				serial_log(level3_started_log);
				break;
			}							
		}
	}
	if (difficulty == DIFFICULTY_AMOUNT)
	{
		if (timer < 10)
		{
			fillRectangle(55,12,12,12,RGBToWord(0,0,0));
			sprintf(timer_string,"%d",timer);
			printText(timer_string,55,12,RGBToWord(255,0,0),0);
		}
		else
		{
			sprintf(timer_string,"%d",timer);
			printText(timer_string,55,12,RGBToWord(255,255,255),0);
		}
		if (max_time <= 0)
		{
			start_game = 0;
			heart_gone = 0;
			for (int i = 0; i < NUMOF_KEYS_3; i++)
			{
				key_pickup[i] = 0;
			}
			return 1;
		}
		if (milliseconds_timer >= 1000)
		{
			max_time = max_time - 1000;
			milliseconds_timer = 0;
			timer--;
		}
		milliseconds_timer++;
	}
	if (music_flag == 0)
	{
		play_music(notes,LEVEL_3_MUSIC);
	}
	// Check if Player can go through door and finish level.
	if (isInside(door_location_x,door_location_y,12,16,x,y) || isInside(door_location_x,door_location_y,12,16,x+12,y) || isInside(door_location_x,door_location_y,12,16,x,y+16) || isInside(door_location_x,door_location_y,12,16,x+12,y+16))
	{
		// Checks if all keys have been obtained 
		if (amount_keys == NUMOF_KEYS_3)
		{
			// We turn Green off since we are not in a level now. 
			GreenOff();
			// Red LED tells you the game is running and we are not in a level.
			RedOn();
			serial_log(level3_complete_log);
			playNote(0);
			music_flag = 1;
			int tmp = hearts_used - heart_gone;
			char currentHeart[NUM_OF_CHAR];
			sprintf(currentHeart,"%d",tmp);

			fillRectangle(0,0,128,160,RGBToWord(0,0,0));
			printTextX2("Level 3", 25, 20, RGBToWord(255,255,255), 0);
			printTextX2("Complete!", 15, 40, RGBToWord(255,255,255), 0);
			printText("Hearts Left", 5, 70, RGBToWord(255,255,255), 0);
			printText(currentHeart,88,70,RGBToWord(255,255,255),0);
			putImage(100,63,12,16,heart,0,0);
			printText("<--", 5, 90, RGBToWord(255,255,255), 0);
			while (1)
			{
				if ((GPIOB->IDR & (1 << 5))==0) // left pressed
				{
					start_game = 0;
					clear_screen = 0;
					amount_keys = 0;
					heart_gone = 0;
					for (int i = 0; i < NUMOF_KEYS_3; i++)
					{
						key_pickup[i] = 0;
						toggle[i] = 0;
						enemy_current_pos_x[i] = pos_x_old[i];
						enemy_current_pos_y[i] = pos_y_old[i];
						start_movement = 0;
					}
					break;
				}
			}
			// Move onto the next level
			current_level++;
			// Clear the current screen
			fillRectangle(0,0,128,160,RGBToWord(0,0,0));
		}
	}
	// Key pickup check
	for (int i = 0; i < NUMOF_KEYS_3; i++)
	{
		if (key_pickup[i] == 0)
		{
			if (isInside(pickup_key_location_x[i],pickup_key_location_y[i],12,16,x,y) || isInside(pickup_key_location_x[i],pickup_key_location_y[i],12,16,x+12,y) || isInside(pickup_key_location_x[i],pickup_key_location_y[i],12,16,x,y+16) || isInside(pickup_key_location_x[i],pickup_key_location_y[i],12,16,x+12,y+16))
			{
				putImage(pickup_key_location_x[i],pickup_key_location_y[i],12,16,taken_key,0,0);
				putImage(key_location_x[amount_keys],key_location_y[amount_keys],12,16,taken_key,0,0);
				key_pickup[i] = 1;
				amount_keys++;
				serial_log(key_pickup_log);
			}
		}
		if (key_pickup[i] == 1)
		{
			if (!(isInside(pickup_key_location_x[i],pickup_key_location_y[i],12,16,x,y) || isInside(pickup_key_location_x[i],pickup_key_location_y[i],12,16,x+12,y) || isInside(pickup_key_location_x[i],pickup_key_location_y[i],12,16,x,y+16) || isInside(pickup_key_location_x[i],pickup_key_location_y[i],12,16,x+12,y+16)))
			{
				putImage(pickup_key_location_x[i],pickup_key_location_y[i],12,16,taken_key,0,0);
			}
		}
	}

	// Draws the Door
	putImage(door_location_x,door_location_y,12,16,door,0,0);

	// Draws the Enemy
	for (int i = 0; i < NUM_OF_ENEMY_3; i++)
	{
		if (enemy_current_pos_x[i] != enemy_x_right[i] && toggle[i] == 0)
		{
			if (difficulty == DIFFICULTY_AMOUNT)
			{
				putImage(enemy_current_pos_x[i],enemy_current_pos_y[i],12,16,night_skeleton_run,0,0);
			}
			else
			{
				putImage(enemy_current_pos_x[i],enemy_current_pos_y[i],12,16,skeleton_run,0,0);
			}
			drawRectangle(enemy_current_pos_x[i],enemy_current_pos_y[i],12,16,RGBToWord(0,0,0));
			enemy_current_pos_x[i]++;
			if (difficulty == DIFFICULTY_AMOUNT)
			{
				putImage(enemy_current_pos_x[i],enemy_current_pos_y[i],12,16,night_skeleton_run,0,0);
			}
			else
			{
				putImage(enemy_current_pos_x[i],enemy_current_pos_y[i],12,16,skeleton_run,0,0);
			}		
		}
		else
		{
			toggle[i] = 1;
		}
		if (enemy_current_pos_x[i] != enemy_x_left[i] && toggle[i] == 1)
		{
			if (difficulty == DIFFICULTY_AMOUNT)
			{
				putImage(enemy_current_pos_x[i],enemy_current_pos_y[i],12,16,night_skeleton_run,1,0);
			}
			else
			{
				putImage(enemy_current_pos_x[i],enemy_current_pos_y[i],12,16,skeleton_run,1,0);
			}
			drawRectangle(enemy_current_pos_x[i],enemy_current_pos_y[i],12,16,RGBToWord(0,0,0));
			enemy_current_pos_x[i]--;
			if (difficulty == DIFFICULTY_AMOUNT)
			{
				putImage(enemy_current_pos_x[i],enemy_current_pos_y[i],12,16,night_skeleton_run,1,0);
			}
			else
			{
				putImage(enemy_current_pos_x[i],enemy_current_pos_y[i],12,16,skeleton_run,1,0);
			}
		}
		else
		{
			toggle[i] = 0; 
		}

		// Check to see if the player is hit by the enemy 
		if ((isInside(enemy_current_pos_x[i],enemy_current_pos_y[i],12,16,x,y+5) || isInside(enemy_current_pos_x[i],enemy_current_pos_y[i],12,16,x+12,y+5) || isInside(enemy_current_pos_x[i],enemy_current_pos_y[i],12,16,x+5,y+11) || isInside(enemy_current_pos_x[i],enemy_current_pos_y[i],12,16,x+7,y+11)))
		{
			serial_log(died_skeleton_log);
			if (toggle[i] == 1)
			{
				if (difficulty == DIFFICULTY_AMOUNT)
				{
					putImage(enemy_current_pos_x[i],enemy_current_pos_y[i],12,16,night_skeleton_attack2,1,0);
				}
				else
				{
					putImage(enemy_current_pos_x[i],enemy_current_pos_y[i],12,16,skeleton_attack2,1,0);
				}
			}
			else
			{
				if (difficulty == DIFFICULTY_AMOUNT)
				{
					putImage(enemy_current_pos_x[i],enemy_current_pos_y[i],12,16,night_skeleton_attack2,0,0);
				}
				else
				{
					putImage(enemy_current_pos_x[i],enemy_current_pos_y[i],12,16,skeleton_attack2,0,0);
				}
			}
			playNote(0);
			music_flag = 1;
			fillRectangle(x,y,12,16,RGBToWord(0,0,0));

			// Respawn the player back at the Original Position.
			*px = pickup_key_location_x[random_location_picker];
			*py = pickup_key_location_y[random_location_picker];
			
			// Player loses a heart.
			putImage(heart_location_x[heart_gone],heart_location_y[heart_gone],12,16,hearts_empty,0,0);
			heart_gone++;
			delay(1500);
			putImage(*px,*py,12,16,knight_animation1,0,0);
			music_flag = 0;
		}	
	}
	// Draws the spike
	for (int i = 0; i < NUMOF_SPIKES_3; i++)
	{
		if (difficulty == DIFFICULTY_AMOUNT)
		{
			putImage(spike_location_x[i],spike_location_y[i],12,16,nightmare_spike,0,0);
		}
		else
		{
			putImage(spike_location_x[i],spike_location_y[i],12,16,spike,0,0);
		}
		if ((isInside(spike_location_x[i],spike_location_y[i],12,16,x,y) || isInside(spike_location_x[i],spike_location_y[i],12,16,x+12,y) || isInside(spike_location_x[i],spike_location_y[i],12,16,x,y+16) || isInside(spike_location_x[i],spike_location_y[i],12,16,x+12,y+16)))
		{
			serial_log(died_spike_log);
			playNote(0);
			music_flag = 1;
			// The player has been hit so we automatically punish him by setting him back to the original positoon.
			if (difficulty == DIFFICULTY_AMOUNT)
			{
				putImage(spike_location_x[i],spike_location_y[i],12,16,nightmare_spike,0,0);
			}
			else
			{
				putImage(spike_location_x[i],spike_location_y[i],12,16,spike,0,0);
			}
			fillRectangle(x,y,12,16,RGBToWord(0,0,0));
			// Respawn the player back at the Original Position.
			*px = 115;
			*py = 45;
			// Player loses a heart.
			putImage(heart_location_x[heart_gone],heart_location_y[heart_gone],12,16,hearts_empty,0,0);
			heart_gone++;
			delay(1500);
			putImage(*px,*py,12,16,knight_animation1,0,0);
			music_flag = 0;
		}	
	}

	if (heart_gone == hearts_used)
	{
		start_game = 0;
		clear_screen = 0;
		amount_keys = 0;
		heart_gone = 0;
		for (int i = 0; i < NUMOF_KEYS_3; i++)
		{
			key_pickup[i] = 0;
			toggle[i] = 0;
			enemy_current_pos_x[i] = pos_x_old[i];
			enemy_current_pos_y[i] = pos_y_old[i];
		}
		return 1;
	}
}

// Function to handle the game over scenario
void gameover(int *start_game, int *current_difficulty_choice, int *difficulty) {
    int press = 0; // Variable to detect button press

    // Turn off the green LED and turn on the red LED
    GreenOff(); // Indicate that the player is not in a level
    RedOn(); // Indicate that the game is running but not in a level

    playNote(0); // Stop any ongoing music or sounds
    fillRectangle(0, 0, 128, 164, RGBToWord(0, 0, 0)); // Clear the screen

    // Display "You Lost!" message on the screen
    printTextX2("You", 40, 20, RGBToWord(255, 0, 0), 0); // "You" in red
    printTextX2("Lost!", 40, 50, RGBToWord(255, 0, 0), 0); // "Lost!" in red
    printText("|", 110, 130, RGBToWord(255, 255, 255), 0); // Display a vertical line
    printText("V", 110, 140, RGBToWord(255, 255, 255), 0); // Display a "V" to indicate a button press option

    // Loop to wait for player input to acknowledge the game over
    while (press == 0) {
        if ((GPIOA->IDR & (1 << 11)) == 0) { // If right button is pressed
            fillRectangle(0, 0, 128, 160, RGBToWord(0, 0, 0)); // Clear the screen
            press = 1; // Set the press flag
            *start_game = 0; // Reset the game start flag
            *difficulty = 0; // Reset the difficulty
            *current_difficulty_choice = 0; // Reset the difficulty choice
            current_level = 1; // Reset the current level to 1
            start_movement = 0; // Reset the start movement flag
            nightmare_flag = 0; // Reset the nightmare mode flag
            music_flag = 0; // Reset the music flag
            max_time = 60000; // Reset the maximum time to 60,000 milliseconds
            timer = 60; // Reset the timer back to 60 seconds
            milliseconds_timer = 0; // Reset the milliseconds timer
            seed = 0; // Reset the seed
            delay(500); // Short delay before continuing
        }
    }
}

// Function to set the game's difficulty level and number of hearts (lives) based on the player's choice
void Difficulty_choice(int* difficulty, int choice, int* hearts_used) {
    if ((*difficulty) == 0) { // Check if the difficulty is not yet set
        delay(100); // Short delay for timing purposes
        fillRectangle(0, 0, 128, 160, RGBToWord(0, 0, 0)); // Clear the screen

        // Display difficulty level options
        printTextX2("Difficulty", 5, 5, RGBToWord(255, 255, 255), 0); // Display the text "Difficulty"
        printTextX2("Easy", 40, 25, RGBToWord(0, 255, 0), 0); // Display the text "Easy" in green
        putImage(99, 25, 12, 16, easy_skull, 0, 0); // Display the Easy Mode Skull image

        // Display hearts for Easy mode
        for (int i = 0; i < 3; i++) {
            putImage(42 + 15 * i, 40, 12, 16, heart, 0, 0); // Display four heart images
        }
        printText("<--", 53, 58, RGBToWord(255, 255, 255), 0); // Display left arrow for selection

        // Repeat similar process for Normal and Hard modes
        printTextX2("Normal", 30, 65, RGBToWord(255, 165, 0), 0); // Normal mode in orange
        putImage(100, 65, 12, 16, normal_skull, 0, 0); // Normal Mode Skull image
        for (int i = 0; i < 2; i++) {
            putImage(50 + 15 * i, 80, 12, 16, heart, 0, 0); // Display two heart images
        }
        printText("^", 60, 100, RGBToWord(255, 255, 255), 0); // Display up arrow for selection
		printText("|", 60, 105, RGBToWord(255,255,255), 0); // Displays the pipe for selection 
        printTextX2("Hard", 40, 115, RGBToWord(255, 0, 0), 0); // Hard mode in red
        putImage(100, 115, 12, 16, hard_skull, 0, 0); // Hard Mode Skull image
        putImage(58, 130, 12, 16, heart, 0, 0); // Display one heart image
        printText("-->", 55, 148, RGBToWord(255, 255, 255), 0); // Display right arrow for selection

        // Loop to wait for player's input to select difficulty
        while (choice == 0) {
            if ((GPIOB->IDR & (1 << 5)) == 0) { // If left button pressed, choose Easy
                *difficulty = 1;
                *hearts_used = 3;
                choice = 1;
                fillRectangle(0, 0, 128, 160, RGBToWord(0, 0, 0)); // Clear screen
            } else if ((GPIOA->IDR & (1 << 8)) == 0) { // If up button pressed, choose Normal
                *difficulty = 2;
                *hearts_used = 2;
                choice = 1;
                fillRectangle(0, 0, 128, 160, RGBToWord(0, 0, 0)); // Clear screen
            } else if ((GPIOB->IDR & (1 << 4)) == 0) { // If right button pressed, choose Hard
                *difficulty = 3;
                *hearts_used = 1;
                choice = 1;
                fillRectangle(0, 0, 128, 160, RGBToWord(0, 0, 0)); // Clear screen
            }
        }
    }
}

// Function to display the current difficulty level on the screen
void Difficulty_Display(int difficulty) {
    // Use a switch statement to handle different difficulty levels
    switch(difficulty) {
        case 1: // If the difficulty level is 'Easy'
            printText("Easy", 85, 50, RGBToWord(0, 255, 0), 0); // Display "Easy" in green color
            break; // Exit switch statement

        case 2: // If the difficulty level is 'Normal'
            printText("Normal", 85, 50, RGBToWord(255, 165, 0), 0); // Display "Normal" in orange color
            break; // Exit switch statement

        case 3: // If the difficulty level is 'Hard'
            printText("Hard", 85, 50, RGBToWord(255, 0, 0), 0); // Display "Hard" in red color
            break; // Exit switch statement

        case 4: // If the difficulty level is 'Nightmare'
            printText("Night", 85, 50, RGBToWord(128, 0, 128), 0); // Display "Night" in purple color
            break; // Exit switch statement
    }
}

// Function to display the introduction screen of the game
void intro(void) {
    int press = 0; // Variable to track if a button is pressed

    // Display the names of the creators with a delay between each
    printTextX2("Denis", 20, 30, RGBToWord(255, 255, 255), 0);
    delay(1000); // Delay for 1 second
    printTextX2("and", 40, 60, RGBToWord(255, 255, 255), 0);
    delay(1000); // Delay for 1 second
    printTextX2("Cillian", 20, 90, RGBToWord(255, 255, 255), 0);
    delay(1000); // Delay for 1 second
    printTextX2("Presents", 20, 120, RGBToWord(255, 255, 255), 0);
    delay(500); // Delay for 0.5 seconds

    // Clear the screen and prepare for the next part of the intro
    fillRectangle(0, 0, 128, 160, RGBToWord(0, 0, 0));
    delay(500); // Delay for 0.5 seconds

    // Display the game title "Key Quest" with a delay
    printTextX2("Key", 30, 20, RGBToWord(255, 255, 255), 0);
    delay(1000); // Delay for 1 second
    printTextX2("Quest", 30, 50, RGBToWord(255, 255, 255), 0);
    delay(1000); // Delay for 1 second

    // Display a directional indicator for the user to proceed from the intro
    printText("|", 110, 130, RGBToWord(255, 255, 255), 0);
    printText("V", 110, 140, RGBToWord(255, 255, 255), 0);

    // Loop to wait for player input to proceed from the intro
    while (press == 0) {
        if ((GPIOA->IDR & (1 << 11)) == 0) { // Check if 'down' button is pressed
            fillRectangle(0, 0, 128, 160, RGBToWord(0, 0, 0)); // Clear screen
            press = 1; // Update press variable to exit loop
        }
    }
}

// Function to display the main menu of the game
void main_menu(void) {
    int press = 0; // Variable to track if a button is pressed

    // Display the game title "Key Quest" on the screen
    printTextX2("Key", 30, 20, RGBToWord(255, 255, 255), 0);
    printTextX2("Quest", 30, 50, RGBToWord(255, 255, 255), 0);

    // Display a directional indicator for the user to start the game
    printText("|", 110, 130, RGBToWord(255, 255, 255), 0);
    printText("V", 110, 140, RGBToWord(255, 255, 255), 0);

    // Loop through the badges array to display the trophies earned
    for (int i = 0; i < BADGES_AMOUNT; i++) {
        if (badges[i] == 1) { // Easy skull trophy
            putImage(5, 130, 12, 16, easy_skull, 0, 0);
        }
        if (badges[i] == 2) { // Normal skull trophy
            putImage(25, 130, 12, 16, normal_skull, 0, 0);
        }
        if (badges[i] == 3) { // Hard skull trophy
            putImage(45, 130, 12, 16, hard_skull, 0, 0);
        }
        if (badges[i] == 4) { // Nightmare skull trophy
            putImage(65, 130, 12, 16, nightmare_skull, 0, 0);
        }
    }

    // Loop to wait for player input to start the game
    while (press == 0) {
        if ((GPIOA->IDR & (1 << 11)) == 0) { // Check if 'down' button is pressed
            fillRectangle(0, 0, 128, 160, RGBToWord(0, 0, 0)); // Clear screen
            press = 1; // Update press variable to exit loop
        }
    }
}

// Function to handle the end of the game
void gameend(int *start_game, int *current_difficulty_choice, int *difficulty) {
    // Flags to track unlocking of various achievements only once
    static int easy_skull_flag = 0;
    static int normal_skull_flag = 0;
    static int hard_skull_flag = 0;
    static int night_skull_flag = 0;
    static int nightmare_unlocked_flag = 0;
    int press = 0;

    // Messages to display upon unlocking special modes and trophies
    char nightmare_unlock[] = "Nightmare Mode Unlocked!";
    char skull_unlocked_easy[] = "Easy Skull Trophy Unlocked!";
    char skull_unlocked_normal[] = "Normal Skull Trophy Unlocked!";
    char skull_unlocked_hard[] = "Hard Skull Trophy Unlocked!";
    char skull_unlocked_nightmare[] = "Nightmare Skull Trophy Unlocked!";

    // Stop any ongoing music, clear the screen, and display victory message
    playNote(0);
    fillRectangle(0, 0, 128, 160, RGBToWord(0, 0, 0));
    printTextX2("You", 40, 40, RGBToWord(255, 255, 204), 0);
    printTextX2("Won!", 40, 60, RGBToWord(255, 255, 204), 0);
    printText("-->", 100, 140, RGBToWord(255, 255, 255), 0);

    // Check the difficulty level and unlock respective trophies if not already done
    if (*difficulty == 1 && easy_skull_flag == 0) {
        badges[0] = 1;
        serial_log(skull_unlocked_easy);
        easy_skull_flag = 1;
    } else if (*difficulty == 2 && normal_skull_flag == 0) {
        badges[1] = 2;
        serial_log(skull_unlocked_normal);
        normal_skull_flag = 1;
    } else if (*difficulty == 3 && hard_skull_flag == 0) {
        badges[2] = 3;
        serial_log(skull_unlocked_hard);
        hard_skull_flag = 1;
    } else if (*difficulty == 4 && night_skull_flag == 0) {
        badges[3] = 4;
        serial_log(skull_unlocked_nightmare);
        night_skull_flag = 1;
    }

    // Unlock Nightmare Mode if all other modes are completed
    if (badges[0] == 1 && badges[1] == 2 && badges[2] == 3 && nightmare_unlocked_flag == 0) {
        nightmare_enabled = 1;
        serial_log(nightmare_unlock);
        nightmare_unlocked_flag = 1;
    }

    // Loop to wait for player input to acknowledge game end
    while (press == 0) {
        if ((GPIOB->IDR & (1 << 4)) == 0) { // If 'down' button is pressed
            fillRectangle(0, 0, 128, 160, RGBToWord(0, 0, 0));
            press = 1;
            // Resetting various game state variables for a new game
            *start_game = 0;
            *difficulty = 0;
            *current_difficulty_choice = 0;
            current_level = 1;
            start_movement = 0;
            nightmare_flag = 0;
            music_flag = 0;
            max_time = 60000; // Change it from seconds to milliseconds
            timer = 60; // Reset timer back to 60 seconds
            milliseconds_timer = 0;
            seed = 0;
            if (badges[3] == 4) {
                nightmare_enabled = 1;
            }
        }   
    }
    fillRectangle(0, 0, 128, 160, RGBToWord(0, 0, 0));
    current_level = 1; // Resetting back to level 1 for a new game
}

// Function to play music based on an array of notes
void play_music(int music_notes[], int num_of_notes) {
    static int next_note = 0;  // Static variable to keep track of the current note index

    if (milliseconds >= 500) {
        // If 500 milliseconds have passed,
        playNote(0);              // stop the current note by playing a note with value 0
        next_note++;              // Move on to the next note in the array
        milliseconds = 0;         // Reset the milliseconds counter
    } else {
        // If 500 milliseconds have not passed,
        playNote(music_notes[next_note]);  // Play the current note from the music_notes array
    }

    if (next_note >= num_of_notes) {
        // If the end of the notes array is reached,
        next_note = 0;  // Start from the beginning of the array again
    }
}

// Function to handle the 'Nightmare' difficulty setting in the game
void Difficulty_Nightmare(int* difficulty, int choice, int *hearts_used) {
    // Loop runs as long as the difficulty isn't set to 4 (indicating Nightmare difficulty)
    // and the nightmare_flag is not set
    while (*difficulty != 4 && nightmare_flag == 0) {
        // Displaying the difficulty settings on the screen
        printTextX2("Difficulty", 10, 20, RGBToWord(255, 255, 255), 0);
        printTextX2("Nightmare", 15, 40, RGBToWord(128, 0, 128), 0);
        
        // Display a skull image as a symbol for the Nightmare difficulty
        putImage(58, 60, 12, 16, nightmare_skull, 0, 0);
        // Displaying features of Nightmare difficulty - Stronger enemies, 1 minute timer, etc.
        printText("Stronger Enemies", 15, 80, RGBToWord(255, 255, 255), 0);
        printText("1 Minute Timer", 25, 90, RGBToWord(255, 255, 255), 0);
        printText("1", 60, 105, RGBToWord(255, 255, 255), 0);
        putImage(70, 98, 12, 16, nightmare_heart, 0, 0);

        // Options to accept or reject the Nightmare difficulty
        printText("|", 115, 120, RGBToWord(255, 255, 255), 0);
        printText("V", 115, 130, RGBToWord(255, 255, 255), 0);
        printText("Yes", 105, 140, RGBToWord(255, 255, 255), 0);
        printText("^", 10, 125, RGBToWord(255, 255, 255), 0);
        printText("|", 10, 130, RGBToWord(255, 255, 255), 0);
        printText("No", 5, 140, RGBToWord(255, 255, 255), 0);

        // Checking for player input through buttons connected to GPIOA
        if ((GPIOA->IDR & (1 << 11)) == 0) { // If 'down' button is pressed
            // Clear screen and set difficulty to Nightmare
            fillRectangle(0, 0, 128, 160, RGBToWord(0, 0, 0));
            *difficulty = 4; // Set difficulty to Nightmare
            choice = 1; // Record player's choice
            *hearts_used = 1; // Set the number of hearts used
            nightmare_flag = 1; // Set the flag indicating Nightmare difficulty chosen
            break; // Exit the loop
        }
        if ((GPIOA->IDR & (1 << 8)) == 0) { // If 'up' button is pressed
            // Clear screen and exit the Nightmare difficulty option
            fillRectangle(0, 0, 128, 160, RGBToWord(0, 0, 0));
            nightmare_flag = 1; // Set the flag indicating exit from Nightmare difficulty option
            break; // Exit the loop
        }
    }
}

// Function for logging messages over serial communication
void serial_log(char log[]) {
    // Iterate over each character in the provided log message
    int i = 0;
    while(log[i] != '\0') { // '\0' marks the end of a string in C
        eputchar(log[i]);   // Send each character of the log message
        i++;                // Move to the next character
    }
    eputs("\r\n");         // Send a newline and carriage return to end the log entry
}


// Function to turn the red LED on
void RedOn(void) {
    GPIOB->ODR = GPIOB->ODR | (1 << 3); // Sets the 3rd bit of the Output Data Register (ODR) of GPIOB,
    // effectively turning on the red LED connected to this pin.
}

// Function to turn the red LED off
void RedOff(void) {
    GPIOB->ODR = GPIOB->ODR & ~(1 << 3); // Clears the 3rd bit of the ODR of GPIOB,
    // effectively turning off the red LED connected to this pin.
}

// Function to turn the green LED on
void GreenOn(void) {
    GPIOB->ODR = GPIOB->ODR | (1 << 0); // Sets the 0th bit of the ODR of GPIOB,
    // effectively turning on the green LED connected to this pin.
}

// Function to turn the green LED off
void GreenOff(void) {
    GPIOB->ODR = GPIOB->ODR & ~(1 << 0); // Clears the 0th bit of the ODR of GPIOB,
    // effectively turning off the green LED connected to this pin.
}