#include <iostream>
#include <GL/glut.h>
#include <cmath>
#include <stdio.h>
#include <windows.h>   
#include <string>
#include <chrono>
#include <thread>
#include <random>


// a - влево
// d - вправо
// q - выход
// w - замеддение или отмотка
// e - ускорение или перемотка
// s - пауза
// 1-6 - выбор цвета шара

// игра является бесконечной
// возникли сложности с отрисовкой и программированием несколько блоков,
// поэтому каждый новый блок появляется после уничтожения старого




int i = 0;
int r = 0.0f;
int g = 0.0f;
int b = 0.0f;
int lives = 3;
std::string text_score;
int kx = 500, ky = 240, kscore = 0;
float speedk = 0.0f;
const int WINDOWWIDTH = 640;
const int WINDOWHEIGHT = 480;
const int PADDLEWIDTH = 80;
const int PADDLEHEIGHT = 16;
const int BALL_RADIUS = 8;
const int WALL_ROWS = 15;
const int WALL_COLS = 24;
const int WALL_HEIGHT = 15;
const int WALL_WIDTH = 60;
int paddleX = WINDOWWIDTH / 2 - PADDLEWIDTH / 2;
int paddleY = WINDOWHEIGHT - 32;
float ballX = WINDOWWIDTH / 2;
float ballY = WINDOWHEIGHT / 2;
float ballSpeed = 0.09f;
float ballDX = 0.0f;
float ballDY = 0.0f;
bool walls[WALL_ROWS][WALL_COLS] = { true };


 
namespace breakout {
    void drawText(const char* text, int length, int x, int y) {
        glMatrixMode(GL_PROJECTION);
        double* matrix = new double[16];
        glGetDoublev(GL_PROJECTION_MATRIX, matrix);
        glLoadIdentity();
        glOrtho(0, 800, 0, 600, -5, 5);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glPushMatrix();
        glLoadIdentity();
        glRasterPos2i(x, y);
        for (int i = 0; i < length; i++) {
            glutBitmapCharacter(GLUT_BITMAP_9_BY_15, (int)text[i]);
        }
        glPopMatrix();
        glMatrixMode(GL_PROJECTION);
        glLoadMatrixd(matrix);
        glMatrixMode(GL_MODELVIEW);
        delete[] matrix;
    }
    void drawRect(int x, int y, int width, int height) {
        glBegin(GL_QUADS);
        glVertex2i(x, y);
        glVertex2i(x + width, y);
        glVertex2i(x + width, y + height);
        glVertex2i(x, y + height);
        glEnd();
    }

    void drawCircle(float x, float y, float radius) {
        glBegin(GL_TRIANGLE_FAN);
        glVertex2f(x, y);
        for (int i = 0; i <= 360; i++) {
            float angle = 2.0f * 3.1415926f * i / 360.0f;
            float dx = radius * cosf(angle);
            float dy = radius * sinf(angle);
            glVertex2f(x + dx, y + dy);
        }
        glEnd();
    }

    void drawWalls() {
        for (int i = 0; i < WALL_ROWS; i++) {
            for (int j = 0; j < WALL_COLS; j++) {
                if (walls[i][j]) {
                    int x = j * WALL_WIDTH;
                    int y = i * WALL_HEIGHT;
                    drawRect(x + kx, y + ky, WALL_WIDTH, WALL_HEIGHT);
                }
            }
        }
    }



    void draw() {
        glClear(GL_COLOR_BUFFER_BIT);

        glColor3f(0.0f, 0.0f, 0.0f);
        drawRect(paddleX, paddleY, PADDLEWIDTH, PADDLEHEIGHT);
        drawText("Score: ", 7, 670, 560);
        drawText(std::to_string(kscore).c_str(), 7, 750, 560);
        drawText("Lives: ", 7, 10, 560);
        drawText(std::to_string(lives).c_str(), 7, 90, 560);



        glColor3f(r, g, b);
        drawCircle(ballX, ballY, BALL_RADIUS);

        glColor3f(0.0f, 0.0f, 1.0f);
        drawWalls();


        glutSwapBuffers();
    }



    void update() {
        if (i != 0) {
            ballX += ballSpeed * ballDX;
            ballY += ballSpeed * ballDY;



            if (ballX - BALL_RADIUS < 0 || ballX + BALL_RADIUS > WINDOWWIDTH) {
                ballDX = -ballDX;
            }

            if (ballY - BALL_RADIUS < 0) {
                ballDY = -ballDY;
            }


            if (ballY + BALL_RADIUS > WINDOWHEIGHT) {
                lives--; // Уменьшаем количество жизней

                if (lives > 0) {
                    // Респавн мяча
                    ballX = WINDOWWIDTH / 2;
                    ballY = WINDOWHEIGHT / 2;
                    float angle = 2.0f * 3.1415926f * (rand() % 360) / 360.0f;
                    ballDX = cosf(angle);
                    ballDY = sinf(angle);
                    i = 0;
                }
                else {
                    // Игра окончена, выполните необходимые действия (например, вывод сообщения о конце игры)
                    std::cout << "Game over!";
                    std::chrono::milliseconds duration(2000); // 2 seconds
                    std::this_thread::sleep_for(duration);
                    exit(0);
                }
            }
 

            if (ballY + BALL_RADIUS >= paddleY && ballX > paddleX && ballX < paddleX + PADDLEWIDTH) {
                ballDY = -ballDY;
            }

            int wallHitRow = -1;
            int wallHitCol = -1;

            for (int i = 0; i < WALL_ROWS; i++) {
                for (int j = 0; j < WALL_COLS; j++) {
                    if (walls[i][j]) {
                        int x = j * WALL_WIDTH;
                        int y = i * WALL_HEIGHT;
                        if (ballX > x + kx && ballX < x + kx + WALL_WIDTH && ballY > y + ky && ballY < y + ky + WALL_HEIGHT) {
                            wallHitRow = i;
                            wallHitCol = j;
                            break;
                        }
                    }
                }
                if (wallHitRow != -1) {
                    break;
                }
            }

            if (wallHitRow != -1) {
                wallHitRow = -1;
                wallHitCol = -1;
                ballDY = -ballDY;

                std::random_device rd;
                std::mt19937 gen(rd());
                std::uniform_int_distribution<> disx(50, 550);
                std::uniform_int_distribution<> disy(350, 450);
                kx = disx(gen);
                ky = disy(gen);
                kscore++;
            }


            glutPostRedisplay();
        }

    }

    void keyboard(unsigned char key, int x, int y) {
        if (key == 'a') {
            if (paddleX > 0) {
                paddleX -= 20;
            }
        }
        if (key == 'd') {
            if (paddleX < WINDOWWIDTH - PADDLEWIDTH) {
                paddleX += 20;
            }
        }
        if (key == 'q') {
            exit(0);
        }
        if (key == 'w') {
            ballSpeed -= 0.01f;
        }
        if (key == 'e') {
            ballSpeed += 0.01f;
        }
        if (key == 's') {
            speedk = ballSpeed;
            ballSpeed = 0;
        }
        if (key == '1') {
            r = 1.0f;
            g = 0.0f;
            b = 0.0f;
        }
        if (key == '2') {
            r = 0.0f;
            g = 1.0f;
            b = 0.0f;
        }
        if (key == '3') {
            r = 0.0f;
            g = 0.0f;
            b = 1.0f;
        }
        if (key == '4') {
            r = 1.0f;
            g = 1.0f;
            b = 0.0f;
        }
        if (key == '5') {
            r = 0.0f;
            g = 1.0f;
            b = 1.0f;
        }
        if (key == '6') {
            r = 1.0f;
            g = 0.0f;
            b = 1.0f;
        }
        if (key) {
            i++;
        }
    }
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(WINDOWWIDTH, WINDOWHEIGHT);
    glutCreateWindow("Breakout - challenge");

    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, WINDOWWIDTH, WINDOWHEIGHT, 0, -1, 1);
    glMatrixMode(GL_MODELVIEW);

    float angle = 2.0f * 3.1415926f * (rand() % 360) / 360.0f;
    ballDX = cosf(angle);
    ballDY = sinf(angle);

    glutDisplayFunc(breakout::draw);
    glutIdleFunc(breakout::update);
    glutKeyboardFunc(breakout::keyboard);

    glutMainLoop();

    return 0;
}