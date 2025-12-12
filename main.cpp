#include <iostream>
#include <raylib.h>
#include <deque>
#include <raymath.h>

using namespace std;

Color green = {173, 204, 96, 255};
Color darkGreen = {43, 51, 24, 255};

int cellSize = 30;
int cellCont = 25;
int OffSet = 75;

double lastUpdateTime = 0;

enum class GameState {
    Menu,
    Playing,
    GameOver
};

bool ElementInDeque(Vector2 element, deque<Vector2> deque)
{
    for(unsigned int i = 0; i < deque.size(); ++i)
    {
        if(Vector2Equals(deque[i], element))
        {
            return true;
        }
    }
    return false;
}

bool eventTriggerd(double interval)
{
    double currentTime =GetTime();
    if(currentTime - lastUpdateTime >= interval)
    {
        lastUpdateTime = currentTime;
        return true;
    }
    return false;
}

class Snake
{
    public:
        deque<Vector2> body = {Vector2{6,9},Vector2{5,9},Vector2{4,9}};
        Vector2 direction = {1, 0};
        bool addSegment = false;

        void Draw()
        {
            for(unsigned int i = 0; i < body.size(); ++i)
            {
                float x = body[i].x;
                float y = body[i].y;
                Rectangle segment = Rectangle{OffSet+x*cellSize,OffSet+y*cellSize, (float)cellSize, (float)cellSize};
                DrawRectangleRounded(segment, 0.5, 6,darkGreen);
            }
        }

        void Update()
        {
            body.push_front(Vector2Add(body[0], direction));
            if(addSegment == true)
            {
                addSegment = false;
            }
            else{
                body.pop_back();
            }
            
        }

        void Reset()
        {
            body = {Vector2{6,9}, Vector2{5,9}, Vector2{4,9}};
            direction = {1, 0};
        }
};

class Food
{

   public:
    Vector2 position; 
    Texture2D texture;
    
    Food(deque<Vector2> snakebody)
    {
        Image image = LoadImage("apple.png");
        ImageResize(&image, cellSize, cellSize);
        texture = LoadTextureFromImage(image);
        UnloadImage(image);
        position = GenerateRandomPosition(snakebody);
    }
    // destructer responsible for releasing 
    ~Food()
    {
        UnloadTexture(texture);
    }

    void Draw()
    {
       DrawTexture(texture, OffSet + position.x * cellSize, OffSet + position.y * cellSize, WHITE);
    }

    Vector2 GenerateRandomCells()
    {
        float x = GetRandomValue(0, cellCont - 1);
        float y = GetRandomValue(0, cellCont - 1);
        return Vector2{x,y};
    }

    Vector2 GenerateRandomPosition(deque<Vector2> snakeBody)
    {
        Vector2 position = GenerateRandomCells();
        while(ElementInDeque(position, snakeBody))
        {
            position = GenerateRandomCells();
        }
        return position;
    }

};

class Game
{
    public: 
        Snake snake = Snake();
        Food food = Food(snake.body);
        bool running = true;
        int score = 0;
        Sound eatSound;
        Sound wallSound;

        Game()
        {
            InitAudioDevice();
            eatSound = LoadSound("Sounds_eat.mp3");
            wallSound = LoadSound("Sound_wall.mp3");
        }

        ~Game()
        {
            UnloadSound(eatSound);
            UnloadSound(wallSound);
            CloseAudioDevice();
        }

        void Draw()
        {
            food.Draw();
            snake.Draw();
        }

        void Update()
        {
            if(running)
            {
                 snake.Update();
                 CheckCollisionWithFood();
                 CheckColllision();
                 CheckWithTail();
            }       
     }

        void CheckCollisionWithFood()
        {
            if(Vector2Equals(snake.body[0], food.position))
            {
                food.position = food.GenerateRandomPosition(snake.body);
                snake.addSegment = true;
                score++;
                PlaySound(eatSound);
            }
        }
        void CheckColllision()
        {
            if(snake.body[0].x == cellCont || snake.body[0].x == -1)
            {
                GameOver();
            }
            if(snake.body[0].y == cellCont  || snake.body[0].y == -1)
            {
                GameOver();
            }
        }
        void GameOver()
        {
            snake.Reset();
            food.position = food.GenerateRandomPosition(snake.body);
            running = false;
            score = 0;
            PlaySound(wallSound);
        }

        void CheckWithTail()
        {
            deque<Vector2> headless = snake.body;
            headless.pop_front();
            if(ElementInDeque(snake.body[0], headless))
            {
                GameOver();
            }
        }

};
int main () {
    cout << "Game is created" <<endl;
    InitWindow(2 * OffSet + cellCont * cellSize, 2 * OffSet + cellCont * cellSize, "Nokia Snake");
    SetTargetFPS(60);

    Game game = Game();

    while(WindowShouldClose() == false)
    {
        BeginDrawing();
        if(eventTriggerd(0.2))
        {
            game.Update();
        }
        if(IsKeyPressed(KEY_UP) && game.snake.direction.y != 1)
        {
            game.snake.direction = {0, -1};
            game.running = true;
        }
        if(IsKeyPressed(KEY_DOWN) && game.snake.direction.y != -1)
        {
            game.snake.direction = {0, 1};
            game.running = true;
        }
          if(IsKeyPressed(KEY_LEFT)&& game.snake.direction.x != 1)
        {
            game.snake.direction = {-1, 0};
            game.running = true;
        }
          if(IsKeyPressed(KEY_RIGHT)&& game.snake.direction.x != -1)
        {
            game.snake.direction = {1, 0};
            game.running = true;
        }

        // Drawing 
        ClearBackground(green);
        DrawRectangleLinesEx(Rectangle{(float)OffSet - 5, (float)OffSet - 5, (float)cellSize*cellCont+10, (float)cellSize*cellCont+10}, 5, darkGreen);
        DrawText("Sanke Game", OffSet - 5, 20, 40, darkGreen);
        DrawText(TextFormat("%i", game.score), OffSet -5, OffSet + cellCont * cellSize + 10, 40, darkGreen);
        game.Draw();

        EndDrawing();

    }


    CloseWindow();
    return 0;
   
}