// written by tmids manually
// www.tmidsisthebest.org
// prewritten code for sdl programs so that i wouldnt need to rewrite the same thing over and over again
#include <SDL2/SDL.h>
#include <SDL2/SDL_video.h>
#include <inttypes.h>

#define WINDOW_TITLE "TicTacToe by tmids"

struct Grid {
  size_t w; // default 3
  size_t h; // default 3
  uint8_t grid[];
};

struct Program {
  SDL_Window *window;
  //SDL_Renderer *renderer;
  uint8_t state; // 0 - endgame screen, 1 - cross turn, 2 - circle turn
  size_t squareSize;
  size_t winLineSize; // how many crosses/circles in a row you need in order to win. Default 3
  struct Grid *grid;
};

void SDL_Shutdown(struct Program *program, int exit_code);
int SDL_MyInit(struct Program *program);
void updateFrame(struct Program *program);
void drawCircle(SDL_Surface *surface, size_t centerx, size_t centery, size_t r, Uint32 color, size_t inner_r);
void drawCross(SDL_Surface *surface, size_t centerx, size_t centery, size_t thickness, size_t size);
void event_handler(struct Program *program);
size_t pow2(size_t x) {return x*x;}
size_t getSquareNumber(size_t x, size_t y, size_t width) {return y*width+x;}
size_t min(size_t x, size_t y) {return x<y?x:y;}
size_t max(size_t x, size_t y) {return x<y?x:y;}
uint8_t checkForWin(struct Grid *grid, size_t x, size_t y, size_t winLineSize);

int SDL_MyInit(struct Program *program) {
  if (SDL_Init(SDL_INIT_EVERYTHING)) {

    printf("didnt initialise sdl. %s", SDL_GetError());
    return 1;
  };

    program->window =
      SDL_CreateWindow(WINDOW_TITLE, SDL_WINDOWPOS_CENTERED,
                       SDL_WINDOWPOS_CENTERED, program->grid->w*program->squareSize, program->grid->h*program->squareSize, 0);
  if (!program->window) {
    printf("didnt create window. %s", SDL_GetError());
    return 1;
  }
  /*
  program->renderer = SDL_CreateRenderer(program->window, -1, 0);
  if (!program->renderer) {
    printf("didnt create renderer. %s", SDL_GetError());
    return 1;
    }*/

  return 0;
}

void resetField(struct Grid *grid) {
  for (size_t y; y<grid->h; y++) {
    for (size_t x; x<grid->w; x++) {
      grid->grid[getSquareNumber(x, y, grid->w)] = 0;
    }
  }
}

uint8_t checkForWin(struct Grid *grid, size_t x, size_t y, size_t winLineSize) {
  uint8_t square = grid->grid[getSquareNumber(x, y, grid->w)];
  size_t line = 0;
  // check in the row
  for (size_t i = 0; i<=grid->h; i++) {
    if (square==grid->grid[getSquareNumber(x, i, grid->w)]) {line +=1;}
    else {line=0;}
    if (line>=winLineSize) {return 1;}
  }
  line=0;
  // check in the column
 for (size_t i = 0; i<=grid->w; i++) {
    if (square==grid->grid[getSquareNumber(i, y, grid->w)]) {line += 1;}
    else {line=0;}
    if (line>=winLineSize) {return 1;}
  }
  line=0;
  //check on the diagonal stripes
  for (size_t i = max(x, y)-min(x, y); i<=grid->h && i<=grid->w; i++) {
    if (square==grid->grid[getSquareNumber(x-i, y-i, grid->w)]) {line += 1;}
    else {line=0;}
    if (line>=winLineSize) {return 1;}
  }
  line=0;
  for (size_t i = max(x, y)-min(x, y); i<=grid->h && i<=grid->w; i++) {
    if (square==grid->grid[getSquareNumber(x+i, y-i, grid->w)]) {line += 1;}
    else {line=0;}
    if (line>=winLineSize) {return 1;}
  }
  line=0;
  for (size_t i = 0; i<grid->w*grid->h; i++) {
    if (grid->grid[i]==0) {return 0;}
  }
  return 2;
}

void event_handler(struct Program *program) {
  size_t WINDOW_WIDTH = program->squareSize*program->grid->w;
  size_t WINDOW_HEIGHT = program->squareSize*program->grid->h;
  SDL_Event event;
  while (SDL_PollEvent(&event)) {
    switch (event.type) {
    case SDL_QUIT: {
      SDL_Shutdown(program, EXIT_SUCCESS);
      break;
    }
    case SDL_WINDOWEVENT: {
      switch (event.window.event) {
      case SDL_WINDOWEVENT_FOCUS_GAINED: case SDL_WINDOWEVENT_MOVED:
	updateFrame(program);
	break; 
      }
      break;
    }
    case SDL_MOUSEBUTTONDOWN: {
      if (program->state==0) {break;}
      int32_t mousex, mousey;
      SDL_GetMouseState(&mousex, &mousey);
      size_t x = floor((float)mousex/program->squareSize);
      size_t y = floor((float)mousey/program->squareSize);
      uint8_t *squareState = &program->grid->grid[getSquareNumber(x, y, program->grid->w)];
      if (*squareState==0) {
	*squareState = program->state;
	updateFrame(program);
	program->state = 3-program->state;
	uint8_t checkforwin = checkForWin(program->grid, x, y, program->winLineSize);
	if (checkforwin==2) {
	  program->state = 0;
	  printf("tie!\n");
	  updateFrame(program);
	} else if (checkforwin) {
	  program->state = 0;
	  printf("player %d has won the game!\n", *squareState);
	  updateFrame(program);
	}
      }
      break;
    }
    }
  }
}

void SDL_Shutdown(struct Program *program, int exit_status) {
  //  SDL_DestroyRenderer(program->renderer);
  SDL_DestroyWindow(program->window);
  SDL_Quit();
  exit(exit_status);
}

void drawCross(SDL_Surface *surface, size_t centerx, size_t centery, size_t thickness, size_t size) {
  Uint32 red = SDL_MapRGB(surface->format, 255, 0, 0);
  if (size<=0) {return;}
  double coef = (float)1/size;
  for (double length = 0; length<=1; length+=coef) {
    size_t destx = centerx-(float)size/2+(size*length);
    size_t desty = centery-(float)size/2+(size*length);
    SDL_FillRect(surface, &(SDL_Rect){.x=destx-thickness/2,.y=desty-thickness/2,.w=thickness,.h=thickness,}, red);
  }
  for (double length = 0; length<=1; length+=coef) {
    size_t destx = centerx+(float)size/2-(size*length);
    size_t desty = centery-(float)size/2+(size*length);
    SDL_FillRect(surface, &(SDL_Rect){.x=destx-thickness/2,.y=desty-thickness/2,.w=thickness,.h=thickness,}, red);
  }
}

void drawCircle(SDL_Surface *surface, size_t centerx, size_t centery, size_t r, Uint32 color, size_t inner_r) {
  if (inner_r>r) {return;}
  for (size_t y = centery-r; y<centery+r; y++) {
    for (size_t x = centerx-r; x<centerx+r; x++) {
      if (pow2(centerx-x)+pow2(centery-y)<pow2(r) && pow2(centerx-x)+pow2(centery-y)>pow2(inner_r)) { 
	SDL_FillRect(surface, &(SDL_Rect){.x=x,.y=y,.w=1,.h=1,}, color);
      }
    }
  }
}

void updateFrame(struct Program *program) {
  size_t WINDOW_WIDTH = program->grid->w*program->squareSize;
  size_t WINDOW_HEIGHT = program->grid->h*program->squareSize;
  SDL_Surface *windowSurface = SDL_GetWindowSurface(program->window);

  Uint32 bgColor;
  if (program->state==0) {bgColor = SDL_MapRGB(windowSurface->format, 255, 255, 120);}
  else {bgColor = SDL_MapRGB(windowSurface->format, 110, 190, 255);}
  
  Uint32 lineColor;
  if (program->state==0) {lineColor = SDL_MapRGB(windowSurface->format, 255*2/3, 255*2/3, 120*2/3);}
  else {lineColor = SDL_MapRGB(windowSurface->format, 110*2/3, 190*2/3, 255*2/3);}
  
  Uint32 red = SDL_MapRGB(windowSurface->format, 255, 0, 0);
  Uint32 blue = SDL_MapRGB(windowSurface->format, 0, 0, 255);
  
  size_t lineThickness = ceil((float)program->squareSize/10);

  struct Grid *grid = program->grid;
  
  SDL_FillRect(windowSurface, NULL, bgColor);

  //draw the grid
  SDL_FillRect(windowSurface, NULL, bgColor);
  for (size_t y=0; y<grid->h; y++) {
    for (size_t x=0; x<grid->w; x++) {
      if ((x+y%2)%2==0) {
	SDL_FillRect(windowSurface, &(SDL_Rect){.x=program->squareSize*x,.y=program->squareSize*y,.w=program->squareSize,.h=program->squareSize,}, lineColor);
      }
    }
  }

  for (size_t y = 0; y<grid->h; y++) {
    for (size_t x = 0; x<grid->w; x++) {
      size_t squarex = program->squareSize*x;
      size_t squarey = program->squareSize*y;
      size_t centerx = squarex+program->squareSize/2;
      size_t centery = squarey+program->squareSize/2;
      size_t radius = ceil((float)program->squareSize/2);
      size_t inner_radius = (radius-lineThickness);
      switch (grid->grid[getSquareNumber(x, y, grid->w)]) {
      case 0: break;
      case 1:
	drawCross(windowSurface, centerx, centery, lineThickness, program->squareSize);
	break;
      case 2:
	drawCircle(windowSurface, centerx, centery, radius, blue, inner_radius);
        break;
      }
    }
  }
  SDL_UpdateWindowSurface(program->window);
}

int main(int argC, char *argV[]) {
  int32_t gridWidth;
  int32_t gridHeight;
  int32_t squareSize;
  int32_t winLineSize;
  
  if (argC<=1) {gridWidth = 3;}
  else if ((gridWidth=strtol(argV[1], NULL, 10))==0 || gridWidth<1) {
    printf("Invalid grid width! Defaulting to 3\n");
    gridWidth = 3;
  }
    
  if (argC<=2) {gridHeight = 3;}
  else if ((gridHeight=strtol(argV[2], NULL, 10))==0 || gridHeight<1) {
    printf("Invalid grid height! Defaulting to 3\n");
    gridHeight = 3;
  }

  if (argC<=3) {squareSize = 100;}
  else if ((squareSize=strtol(argV[3], NULL, 10))==0 || squareSize<1) {
    printf("Invalid square size! Defaulting to 100\n");
    squareSize = 100;
  }

  if (argC<=4) {winLineSize = 3;}
  else if ((winLineSize=strtol(argV[4], NULL, 10))==0 || winLineSize<2) {
    printf("Invalid number of shapes in a row to win! Defaulting to 3\n");
    winLineSize = 3;
  }
 
  struct Grid *grid = malloc(sizeof(struct Grid) + (gridWidth*gridHeight) * sizeof(size_t));
  
  grid->w = gridWidth;
  grid->h = gridHeight;
  
  struct Program program = {
    .window = NULL,
    //    .renderer = NULL,
    .grid = grid,
    .squareSize = squareSize,
    .state = 1,
    .winLineSize = winLineSize,
  };

  if (SDL_MyInit(&program)) {
    SDL_Shutdown(&program, EXIT_FAILURE);
  }
  
  updateFrame(&program);
  while (1) {
    event_handler(&program);
  }
  
  return 0;
}

