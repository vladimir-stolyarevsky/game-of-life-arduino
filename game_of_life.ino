#include <SPI.h>
#include <Adafruit_GFX.h>
#include <TFT_ILI9163C.h>

// All wiring required, only 3 defines for hardware SPI on 328P
#define __DC 9
#define __CS 10
// MOSI --> (SDA) --> D11
#define __RST 12
// SCLK --> (SCK) --> D13

// Color definitions
#define BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0  
#define WHITE   0xFFFF

TFT_ILI9163C tft = TFT_ILI9163C(__CS, __DC, __RST);

const uint8_t gridSize = 32;
const uint32_t UNSIGNED_ONE = 1;

uint32_t* grid = new uint32_t[gridSize]{};
uint32_t* newGrid = new uint32_t[gridSize]{};

uint8_t  offsetSize = 4;



void setup() {
  tft.begin();
 /* for(int i = 0; i < gridSize; i++){
    grid[i] = random();
  }
  */ 
  randomSeed(analogRead(0));
  auto x = random(0,gridSize-1);
  grid[x] = analogRead(0);
    
}

void loop(){
  //tft.clearScreen(); 
  makeGolTurn();
      for (uint8_t i = 0; i < gridSize; i++) { 
        uint32_t container = newGrid[i];
        uint32_t prevContainer = grid[i];
        for (uint8_t j = 0; j < gridSize; j++) {
          uint32_t val = container & UNSIGNED_ONE << j;
          if(val == (prevContainer & UNSIGNED_ONE << j)) {
            continue; 
          }         
          
          int16_t color;
          if (val) {
            color = GREEN;
          }
          else {
            color = BLACK;
          }
          tft.fillRect(i*offsetSize, j*offsetSize, offsetSize,offsetSize, color);
        }
    }
  
    delete[] grid;
    grid = newGrid;
    newGrid = new uint32_t[gridSize]{}; 
}
  
uint8_t getSiftBack(uint8_t x, uint8_t size) {
  return x > 0 ? x - 1 : size - 1;
}
uint8_t getSiftForth(uint8_t x, uint8_t size) {
  return x < size - 1 ? x + 1 : 0;
}
 

void makeGolTurn()
{ 
    for (uint8_t i = 0; i < gridSize; i++) { 
      for (uint8_t j = 0; j < gridSize; j++) {

        bool ne = grid[getSiftForth(i, gridSize)] & UNSIGNED_ONE << getSiftBack(j, gridSize);
        bool n = grid[i] & UNSIGNED_ONE << getSiftBack(j, gridSize);
        bool nw = grid[getSiftBack(i, gridSize)] & UNSIGNED_ONE << getSiftBack(j, gridSize);

        bool e = grid[getSiftForth(i, gridSize)] & UNSIGNED_ONE << j;
        bool w = grid[getSiftBack(i, gridSize)] & UNSIGNED_ONE << j;

        bool se = grid[getSiftForth(i, gridSize)] & UNSIGNED_ONE << getSiftForth(j, gridSize);
        bool s = grid[i] & UNSIGNED_ONE << getSiftForth(j, gridSize);
        bool sw = grid[getSiftBack(i, gridSize)] & UNSIGNED_ONE << getSiftForth(j, gridSize);

        auto neighboursCnt = ne + n + nw + e + w + se + s + sw;

        //default state = 0;
        /*
        * Any live cell with fewer than two live neighbors dies, as if by underpopulation.
        * Any live cell with two or three live neighbors lives on to the next generation.
        * Any live cell with more than three live neighbors dies, as if by overpopulation.
        * Any dead cell with exactly three live neighbors becomes a live cell, as if by reproduction.
        */
        if ((neighboursCnt == 2 && UNSIGNED_ONE << j > 0) || neighboursCnt == 3)
          newGrid[i] |= UNSIGNED_ONE << j;
      }
    } 
}
