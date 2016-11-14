#include <stdlib.h>
#include <math.h>

#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <GLUT/glut.h>
#else
#ifdef _WIN32
  #include <windows.h>
#endif
#include <GL/gl.h>
#include <GL/glut.h>
#endif

double PX = 0.0;    // ボールの初期位置
double PY = 0.2;
double PZ = 0.0;
double TABLE_WIDTH = 3;   // テーブルの大きさ
double TABLE_DEPTH = 6;
double TABLE_HEIGHT = 0.5;  // エプロンの高さ
double BALL_RADIUS = 0.2;   // ボールの半径
double TIMESCALE = 0.01;    // フレームごとの経過時間
double SPEED = 30.0;    // ボールの初速度
double MU = 0.5;        // テーブルとボールの摩擦係数
double WEIGHT = 1.0;    // ボールの質量

int windowHeight;       // ウィンドウの高さ
int frame = 0;          // 現在のフレーム数
double vx0, vy0, vz0;   // ボールの初速度
double px0 = PX;        // ボールの初期位置
double py0 = PY;
double pz0 = PZ;

void drawGround();
void drawTable(double height);
void drawBall(void);
void display(void);
void resize(int w, int h);
void keyboard(unsigned char key, int x, int y);
void mouse(int button, int state, int x, int y);
void init(void);

void drawGround() {
    double ground_max_x = 300.0;
    double ground_max_z = 300.0;
    glColor3d(0.8, 0.8, 0.8);
    glBegin(GL_LINES);
    for (double lz = -ground_max_z; lz <= ground_max_z; lz += 1.0) {
        glVertex3d(-ground_max_x, 0, lz);
        glVertex3d(ground_max_x, 0, lz);
    }
    for (double lx = -ground_max_x; lx <= ground_max_x; lx += 1.0) {
        glVertex3d(lx, 0, ground_max_x);
        glVertex3d(lx, 0, -ground_max_x);
    }
    glEnd();
}

void drawTable(double height) {
    // テーブルの色
    static GLfloat tableColor[][4] = {
            {0.3, 0.6, 0.3, 1.0},
            {0.0, 0.3, 0.0, 1.0}
    };

    // エプロンの色
    static GLfloat apronColor[] = {0.4, 0.2, 0.1, 1.0};

    // エプロンの形状
    static GLdouble apron[][9] = {
            { 0.0, 0.0,  1.0, -TABLE_WIDTH, 0.0, -TABLE_DEPTH, -TABLE_WIDTH, TABLE_HEIGHT, -TABLE_DEPTH},
            {-1.0, 0.0,  0.0,  TABLE_WIDTH, 0.0, -TABLE_DEPTH,  TABLE_WIDTH, TABLE_HEIGHT, -TABLE_DEPTH},
            { 0.0, 0.0, -1.0,  TABLE_WIDTH, 0.0,  TABLE_DEPTH,  TABLE_WIDTH, TABLE_HEIGHT,  TABLE_DEPTH},
            { 1.0, 0.0,  0.0, -TABLE_WIDTH, 0.0,  TABLE_DEPTH, -TABLE_WIDTH, TABLE_HEIGHT,  TABLE_DEPTH},
            { 0.0, 0.0,  1.0, -TABLE_WIDTH, 0.0, -TABLE_DEPTH, -TABLE_WIDTH, TABLE_HEIGHT, -TABLE_DEPTH}
    };

    glBegin(GL_QUADS);

    // テーブルの描画
    glNormal3d(0.0, 1.0, 0.0);
    int i, j;
    for (j = (int) -TABLE_DEPTH; j < TABLE_DEPTH; ++j) {
        for (i = (int) -TABLE_WIDTH; i < TABLE_WIDTH; ++i) {
            glMaterialfv(GL_FRONT, GL_DIFFUSE, tableColor[(i + j) & 1]);
            glVertex3d((GLdouble) i, height,(GLdouble) j);
            glVertex3d((GLdouble) i, height,(GLdouble) (j + 1));
            glVertex3d((GLdouble) (i + 1), height,(GLdouble) (j + 1));
            glVertex3d((GLdouble) (i + 1), height,(GLdouble) j);
        }
    }

    // エプロンの描画
    glMaterialfv(GL_FRONT, GL_DIFFUSE, apronColor);
    for (i = 0; i < 4; ++i) {
        glNormal3dv(apron[i]);
        glVertex3dv(apron[i] + 3);
        glVertex3dv(apron[i + 1] + 3);
        glVertex3dv(apron[i + 1] + 6);
        glVertex3dv(apron[i] + 6);
    }

    glEnd();
}


void drawBall(void) {
    glutSolidSphere(BALL_RADIUS, 20, 20);  // 半径, Z軸まわりの分割数, Z軸に沿った分割数
    glEnd();
}

void display(void) {
    static GLfloat lightpos[] = {3.0, 4.0, 5.0, 1.0};   // 光源の位置
    static GLfloat white[] = {0.9, 0.9, 0.9, 1.0}; // ボールの色
    double t = TIMESCALE * frame;   // 現在時刻
    double v = exp(-MU * t / WEIGHT);   // ボールの速度比
    double p = WEIGHT * (1.0 - v) / MU; // ボールの相対位置
    // ボールの現在位置
    double px = vx0 * p + px0;
    double py = py0;
    double pz = vz0 * p + pz0;

    // TODO: ボールが壁の位置に来たら方向を変える
    // TODO: 速度が一定以下になったらアニメーションを止める

    ++frame;
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // 隠面消去処理
    glLoadIdentity();   // 変換行列の初期化
    glLightfv(GL_LIGHT0, GL_POSITION, lightpos);
    glTranslated(0.0, 0.0, -20.0);
    glRotated(30.0, 1.0, 0.0, 0.0);

    drawGround();
    drawTable(0.0);
    glPushMatrix();
    glTranslated(px, py, pz);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, white);
    drawBall();
    glPopMatrix();
    glFlush();
}

void resize(int w, int h) {
    glViewport(0, 0, w, windowHeight = h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(30.0, (double) w / (double) h, 1.0, 100.0);
    glMatrixMode(GL_MODELVIEW);
}

void keyboard(unsigned char key, int x, int y) {
    if (key == '\033' || key == 'q') {
        exit(0);
    }
}

void mouse(int button, int state, int x, int y) {
    switch (button) {
        case GLUT_LEFT_BUTTON:
            if (state == GLUT_DOWN) {
                // TODO: ボールを打ち出す方向と速度を決定してアニメーションさせる
                break;
            }
        case GLUT_MIDDLE_BUTTON:
            break;
        case GLUT_RIGHT_BUTTON:
            break;
        default:
            break;
    }
}

void init(void) {
    glClearColor(1.0, 1.0, 1.0, 1.0);
    glEnable(GL_DEPTH_TEST);    // デプス・バッファ
    //glEnable(GL_CULL_FACE);   // カリング
    //glCullFace(GL_BACK)
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
}

int main(int argc, char *argv[]) {
    glutInitWindowSize(800, 600);
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH);
    glutCreateWindow(argv[0]);
    glutDisplayFunc(display);
    glutReshapeFunc(resize);
    glutKeyboardFunc(keyboard);
    glutMouseFunc(mouse);
    init();
    glutMainLoop();
    return 0;
}