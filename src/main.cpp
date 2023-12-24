#include <iostream> 
#define SDL_MAIN_HANDLED    
#include <SDL2/SDL.h> 

//Colors 
namespace Colors{
    const SDL_Color BLACK = {0,0,0, SDL_ALPHA_OPAQUE};
    const SDL_Color GREEN = {0,255,0, SDL_ALPHA_OPAQUE};
}
//Graphics 
namespace Graphics{
    struct Screen{
        const int WIDTH = 512; 
        const int HEIGHT = 284;
        const int center_x = WIDTH/2; 
        const int center_y = HEIGHT/2; 
    };
};

//Game Objects 
enum SquareState{IDLE, MOVING_UP, MOVING_DOWN, MOVING_LEFT, MOVING_RIGHT}; 

struct Square{
    int WIDTH;
    int HEIGHT; 
    int x;
    int y; 
    SquareState state;  
    int step; 
};

//Application 
struct App{
    const int DEFAULT_STEP = 2; 
    SDL_Window* window; 
    SDL_Renderer* renderer;
    Graphics::Screen screen;  
    Square square{32,32, screen.center_x, screen.center_y, IDLE, DEFAULT_STEP}; 
}app;

//SDL Routines
bool InitSDL(){
    if(SDL_Init(SDL_INIT_EVERYTHING)<0){
        std::cout<<"SDL failed with error"<<SDL_GetError() << std::endl; 
        return false; 
    }
    return true;
}
//Graphis routines
static void ClearScreen(SDL_Renderer* renderer){
    SDL_SetRenderDrawColor(renderer, Colors::BLACK.r, Colors::BLACK.g, Colors::BLACK.b, Colors::BLACK.a); 
    SDL_RenderClear(renderer); 
}

void DrawSquare(int x, int y){
    SDL_Rect rect; 
    rect.x = x - (app.square.WIDTH/2); 
    rect.y = y - (app.square.HEIGHT/2);
    rect.w = app.square.WIDTH;
    rect.h = app.square.HEIGHT; 

    //define green on renderer
    SDL_SetRenderDrawColor(app.renderer, Colors::GREEN.r, Colors::GREEN.g, Colors::GREEN.b, Colors::GREEN.a);
    //fill rectangle with color
    SDL_RenderFillRect(app.renderer, &rect); 

}   

//Application routines
void ShutdownApplication(){
    if(app.window!=nullptr){
        SDL_DestroyWindow(app.window);
        app.window = nullptr; 
    }

    if(app.renderer != nullptr){
        SDL_DestroyRenderer(app.renderer);
            app.renderer = nullptr; 
    }

    SDL_Quit(); 
}

bool InitApplication(){
    if(InitSDL()==false){
        ShutdownApplication();
        return false;
    }
    app.window = SDL_CreateWindow(
        "SDL Window", 
        SDL_WINDOWPOS_CENTERED, 
        SDL_WINDOWPOS_CENTERED,
        app.screen.WIDTH,
        app.screen.HEIGHT, 
        SDL_WINDOW_OPENGL
    );
    if(app.window==nullptr){
        std::cout<< "Unable to create window. Error: " << SDL_GetError() << std::endl; 
        SDL_Quit();
        return false;  
    }

    app.renderer = SDL_CreateRenderer(app.window, -1, SDL_RENDERER_PRESENTVSYNC); 

    return true; 
}

void setState (SquareState new_state){
    app.square.state = new_state; 
}

void IncreaseStep(){
    app.square.step = app.square.step * app.DEFAULT_STEP;
}

void DecreaseStep(){
    int new_step = app.square.step / app.DEFAULT_STEP;
    if(new_step>app.DEFAULT_STEP){
        app.square.step = new_step; 
    }else{
        app.square.step = app.DEFAULT_STEP; 
    }
}

int main (int argc, char* argv[]){

    if(InitApplication()==false){
        ShutdownApplication(); 
        return EXIT_FAILURE;
    }

    //Draw Loop
    SDL_Event event; 
    bool running = true; 
    while(running){
        ClearScreen(app.renderer);
        //Check square state 
        switch(app.square.state){
            case IDLE:{
                break; 
            }
            case MOVING_UP:{
                int new_y = app.square.y - app.square.step; 
                if(new_y>app.square.HEIGHT/2){
                    app.square.y = new_y; 
                }else{
                    app.square.y = app.square.HEIGHT/2;
                    setState(MOVING_DOWN);
                }
                break; 
            }
            case MOVING_DOWN:{
                int new_y = app.square.y + app.square.step; 
                if(new_y<app.screen.HEIGHT - app.square.HEIGHT/2){
                    app.square.y = new_y; 
                }else{
                    app.square.y = app.screen.HEIGHT - app.square.HEIGHT/2;
                    setState(MOVING_UP);
                }
                break; 
            }
            case MOVING_LEFT:{
                int new_top_left_x = app.square.x - app.square.step - app.square.WIDTH/2; 
                if(new_top_left_x>app.square.step){
                    app.square.x = app.square.x - app.square.step; 
                }else{
                    app.square.x = app.square.WIDTH/2; 
                    setState(MOVING_RIGHT);
                }
                break; 
            }
            case MOVING_RIGHT:{
                int new_top_right_x = app.square.x + app.square.step + app.square.WIDTH/2; 
                if(new_top_right_x<=app.screen.WIDTH){
                    app.square.x = app.square.x + app.square.step; 
                }else{
                    app.square.x = app.screen.WIDTH - app.square.WIDTH/2; 
                    setState(MOVING_LEFT);
                } 
                break; 
            }
            default: 
                break; 
        }

        //Check for user input
        if(SDL_PollEvent(&event)){
            switch(event.type){
                case SDL_KEYDOWN:{
                    running = event.key.keysym.scancode != SDL_SCANCODE_ESCAPE; 
                    if(event.key.keysym.scancode == SDL_SCANCODE_UP){
                        setState(MOVING_UP); 
                    }
                    if(event.key.keysym.scancode == SDL_SCANCODE_LEFT){
                        setState(MOVING_LEFT); 
                    }
                    if(event.key.keysym.scancode == SDL_SCANCODE_DOWN){
                        setState(MOVING_DOWN); 
                    }
                    if(event.key.keysym.scancode == SDL_SCANCODE_RIGHT){
                        setState(MOVING_RIGHT); 
                    }
                    if(event.key.keysym.scancode == SDL_SCANCODE_I){
                        IncreaseStep(); 
                    }
                    if(event.key.keysym.scancode == SDL_SCANCODE_D){
                        DecreaseStep();  
                    }
                    if(event.key.keysym.scancode == SDL_SCANCODE_S){
                        setState(IDLE);   
                    }
                    break; 
                }
                /*
                case SDL_KEYUP:{
                    setState(IDLE);
                    break; 
                }
                */
                case SDL_QUIT:{
                    running = false;
                    break; 
                }
                default: 
                    break; 
            }
        }
        //draw new content 
        DrawSquare(app.square.x,app.square.y);
        //Update Screen with content
        SDL_RenderPresent(app.renderer);
    }

    ShutdownApplication(); 
    return EXIT_SUCCESS;
}