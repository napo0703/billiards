#include <stdlib.h>
#include <math.h>
#include <printf.h>

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

const double PX = 0.0;    // ボールの初期位置
const double PY = 0.2;
const double PZ = 0.0;
const double TABLE_WIDTH = 3.2;   // テーブルの大きさ
const double TABLE_DEPTH = 5.8;
const double APRON_HEIGHT = 0.4;  // エプロンの高さ
const double APRON_WIDTH = 0.3;
const double BALL_RADIUS = 0.2;   // ボールの半径
const double TIMESCALE = 0.01;    // フレームごとの経過時間
const double SPEED = 30.0;    // ボールの初速度
const double MU = 3.0;        // テーブルとボールの摩擦係数
const double WEIGHT = 5.0;    // ボールの質量
const double CR = 0.8;        // エプロンの反発係数

int windowHeight;       // ウィンドウの高さ
int frame = 0;          // 現在のフレーム数
double vx0, vy0, vz0;   // ボールの初速度
double px0 = PX;        // ボールの初期位置
double py0 = PY;
double pz0 = PZ;
double tpx = px0;
double tpz = pz0;

void drawGround(void);
void drawTable(double height);
void drawBall(void);
void display(void);
void resize(int w, int h);
void idle(void);
void keyboard(unsigned char key, int x, int y);
void mouse(int button, int state, int x, int y);
double calcCurrentPosition(double v0, double p, double size);
void init(void);

void drawBox(double x, double y, double z, GLfloat color[]) {
    // 直方体の定義
    GLdouble vertex[][3] = {
            { 0, 0, 0 },
            { x, 0, 0 },
            { x, y, 0 },
            { 0, y, 0 },
            { 0, 0, z },
            { x, 0, z },
            { x, y, z },
            { 0, y, z }
    };

    const static int face[][4] = {
            { 0, 1, 2, 3 },
            { 1, 5, 6, 2 },
            { 5, 4, 7, 6 },
            { 4, 0, 3, 7 },
            { 4, 5, 1, 0 },
            { 3, 2, 6, 7 }
    };

    const static GLdouble normal[][3] = {
            { 0.0, 0.0,-1.0 },
            { 1.0, 0.0, 0.0 },
            { 0.0, 0.0, 1.0 },
            {-1.0, 0.0, 0.0 },
            { 0.0,-1.0, 0.0 },
            { 0.0, 1.0, 0.0 }
    };

    glMaterialfv(GL_FRONT, GL_DIFFUSE, color);
    glBegin(GL_QUADS);
    int i, j;
    for (j = 0; j < 6; j += 1) {
        glNormal3dv(normal[j]);
        for (i = 0; i < 4; i += 1) {
            glVertex3dv(vertex[face[j][i]]);
        }
    }
    glEnd();
}

void drawGround(void) {
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

void drawTable() {
    GLfloat tableColor[] = {0.0, 0.35, 0.14, 1.0};
    GLfloat apronColor[] = {0.4, 0.2, 0.1, 1.0};

    // テーブル
    glPushMatrix();
    glTranslated(-TABLE_WIDTH, -0.11, -TABLE_DEPTH);
    drawBox(TABLE_WIDTH * 2, 0.1, TABLE_DEPTH *2, tableColor);
    glPopMatrix();

    // エプロン
    glPushMatrix();
    glTranslated(-(TABLE_WIDTH + APRON_WIDTH), -0.1, -(TABLE_DEPTH + APRON_HEIGHT));
    drawBox((TABLE_WIDTH * 2) + (APRON_WIDTH * 2), APRON_HEIGHT, APRON_HEIGHT, apronColor);
    glPopMatrix();
    glPushMatrix();
    glTranslated(-(TABLE_WIDTH + APRON_WIDTH), -0.1, TABLE_DEPTH);
    drawBox((TABLE_WIDTH * 2) + (APRON_WIDTH * 2), APRON_HEIGHT, APRON_HEIGHT, apronColor);
    glPopMatrix();
    glPushMatrix();
    glTranslated(-(TABLE_WIDTH + APRON_WIDTH), -0.1, -TABLE_DEPTH);
    drawBox(APRON_WIDTH, APRON_HEIGHT, TABLE_DEPTH * 2, apronColor);
    glPopMatrix();
    glPushMatrix();
    glTranslated(TABLE_WIDTH, -0.1, -TABLE_DEPTH);
    drawBox(APRON_WIDTH, APRON_HEIGHT, TABLE_DEPTH * 2, apronColor);
    glPopMatrix();

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
    double v = exp(-MU * t / WEIGHT);   // 初速に対しての現在速度の割合
    double sp = sqrt((vx0 * vx0) + (vz0 * vz0)) * v;  // ボールの速さ
    double p = WEIGHT * (1.0 - v) / MU; // ボールの相対位置
    // ボールが真っ直ぐ進み続けた場合の現在位置
    double px = (vx0 * p) + px0;
    double pz = (vz0 * p) + pz0;

    // TODO: エプロンにぶつかったらスピードを落とす

    // テーブル上での位置
    tpx = calcCurrentPosition(vx0, px, TABLE_WIDTH - BALL_RADIUS);
    tpz = calcCurrentPosition(vz0, pz, TABLE_DEPTH - BALL_RADIUS);

    // 速度が一定以下になったらアニメーションを止める
    if (sp < 0.3) {
        glutIdleFunc(0);
    }

    frame += 1;
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // 隠面消去処理
    glLoadIdentity();   // 変換行列の初期化
    glLightfv(GL_LIGHT0, GL_POSITION, lightpos);
    glTranslated(0.0, 0.0, -15.0);
    glRotated(20.0, 0.0, 0.0, 0.0);

    drawGround();
    drawTable();
    glPushMatrix();
    glTranslated(tpx, py0, tpz);
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

void idle(void) {
    glutPostRedisplay();
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
//                GLdouble model[16], proj[16];
//                GLint view[4];
//                GLfloat z;
//                GLdouble ox, oy, oz;
//                frame = 0;
//                glGetDoublev(GL_MODELVIEW_MATRIX, model);
//                glGetDoublev(GL_PROJECTION_MATRIX, proj);
//                glGetIntegerv(GL_VIEWPORT, view);
//                glReadPixels(x, windowHeight - y, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &z);
//                gluUnProject(x, windowHeight - y, z, model, proj, view, &ox, &oy, &oz);

                // TODO: ボールを打ち出す方向と速度を決定してアニメーションさせる
            } else {
                // ボールの初速度
                px0 = tpx;
                pz0 = tpz;
                frame = 0;
                vx0 = -10.0;
                vz0 = -30.0;
                glutIdleFunc(idle);
            }
            break;
        case GLUT_MIDDLE_BUTTON:
            break;
        case GLUT_RIGHT_BUTTON:
            glutIdleFunc(0);
            //glutPostRedisplay();
            break;
        default:
            break;
    }
}

void init(void) {
    drawTable();
    glClearColor(1.0, 1.0, 1.0, 1.0);
    glEnable(GL_DEPTH_TEST);    // デプス・バッファ
    //glEnable(GL_CULL_FACE);   // カリング
    //glCullFace(GL_BACK)
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
}

// ボールが真っ直ぐ進み続けた場合の現在位置からテーブル上での位置を計算する関数
double calcCurrentPosition(double v0, double p, double size) {
    int nx = v0 >= 0 ? (int) (p / size) : (int) (-p / size);
    if (v0 >= 0) {
        nx = (int) (p / size);
        if ((int) ((p + size) / (size * 2)) & 1) {
            if (nx & 1) {
                return size - (p - (size * (int) (p / size)));
            } else {
                return -(p - (size * (int) (p / size)));
            }
        } else {
            if (nx & 1) {
                return -size + (p - (size * (int) (p / size)));
            } else {
                return p - (size * (int) (p / size));
            }
        }
    } else {
        if ((int) ((-p + size) / (size * 2)) & 1) {
            if (nx & 1) {
                return -size - (p + (size * (int) (-p / size)));
            } else {
                return -(p + (size * (int) (-p / size)));
            }
        } else {
            if (nx & 1) {
                return size + (p + (size * (int) (-p / size)));
            } else {
                return p + (size * (int) (-p / size));
            }
        }
    }
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