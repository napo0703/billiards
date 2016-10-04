#include <OpenGL/OpenGL.h>
#include <GLUT/GLUT.h>

int WindowPositionX = 200;
int WindowPositionY = 200;
int WindowWidth = 512;
int WindowHeight = 512;
char WindowTitle[] = "billiards";
GLdouble vertex[][3] = {
    { 0.0, 0.0, 0.0 },
    { 2.0, 0.0, 0.0 },
    { 2.0, 2.0, 0.0 },
    { 0.0, 2.0, 0.0 },
    { 0.0, 0.0, 30.0 },
    { 2.0, 0.0, 30.0 },
    { 2.0, 2.0, 30.0 },
    { 0.0, 2.0, 30.0 }
};
int face[][4] = {
    { 0, 1, 2, 3 },
    { 1, 5, 6, 2 },
    { 5, 4, 7, 6 },
    { 4, 0, 3, 7 },
    { 4, 5, 1, 0 },
    { 3, 2, 6, 7 }
};

void init(void);
void idle(void);
void display(void);
void ground(void);
void table(double a, double b, double c, double x, double y, double z);

int main(int argc, char *argv[]){
    glutInit(&argc, argv);
    glutInitWindowPosition(WindowPositionX, WindowPositionY);
    glutInitWindowSize(WindowWidth, WindowHeight);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
    glutCreateWindow(WindowTitle);
    glutIdleFunc(idle);
    glutDisplayFunc(display);
    init();
    glutMainLoop();
    return 0;
}

void init(void){
    glClearColor(1.0, 1.0, 1.0, 0.0);
    glEnable(GL_DEPTH_TEST);
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(30.0, (double)WindowWidth/(double)WindowHeight, 0.1, 1000.0);
    gluLookAt(
              0.0, -150.0, 100.0, // 視点の位置
              0.0, 0.0, 10.0,    // 視界の中心位置の参照点座標
              0.0, 0.0, 2.0);    // 視界の上方向のベクトル
}

void idle(){
    glutPostRedisplay();
}

void display(void) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glViewport(0, 0, WindowWidth, WindowHeight);
    table(50.0, 100.0, 1.0, 0.0, 0.0, 20.0);
    glEnd();
    glPopMatrix();
    
    ground();
    glutSwapBuffers();
}

//地面
void ground(void) {
    double ground_max_x = 300.0;
    double ground_max_y = 300.0;
    glColor3d(0.8, 0.8, 0.8);
    glBegin(GL_LINES);
    for(double ly = -ground_max_y; ly <= ground_max_y; ly+=10.0){
        glVertex3d(-ground_max_x, ly, 0);
        glVertex3d(ground_max_x, ly, 0);
    }
    for(double lx = -ground_max_x; lx <= ground_max_x; lx+=10.0){
        glVertex3d(lx, ground_max_y, 0);
        glVertex3d(lx, -ground_max_y, 0);
    }
    glEnd();
}

//テーブル
void table(double a, double b, double c, double x, double y, double z){
    GLdouble vertex[][3] = {
        { -a/2.0, -b/2.0, -c/2.0 },
        {  a/2.0, -b/2.0, -c/2.0 },
        {  a/2.0,  b/2.0, -c/2.0 },
        { -a/2.0,  b/2.0, -c/2.0 },
        { -a/2.0, -b/2.0,  c/2.0 },
        {  a/2.0, -b/2.0,  c/2.0 },
        {  a/2.0,  b/2.0,  c/2.0 },
        { -a/2.0,  b/2.0,  c/2.0 }
    };
    //面の定義
    int face[][4] = {
        { 3, 2, 1, 0 },
        { 1, 2, 6, 5 },
        { 4, 5, 6, 7 },
        { 0, 4, 7, 3 },
        { 0, 1, 5, 4 },
        { 2, 3, 7, 6 }
    };
    //面の法線ベクトル
    GLdouble normal[][3] = {
        { 0.0, 0.0, -1.0 },
        { 1.0, 0.0, 0.0 },
        { 0.0, 0.0, 1.0 },
        {-1.0, 0.0, 0.0 },
        { 0.0,-1.0, 0.0 },
        { 0.0, 1.0, 0.0 }
    };
    glPushMatrix();
    glTranslated( x, y, z);
    glBegin(GL_QUADS);
    for (int j = 0; j < 6; ++j) {
        glNormal3dv(normal[j]);
        for (int i = 0; i < 4; ++i) {
            glVertex3dv(vertex[face[j][i]]);
        }
    }
    glEnd();
    glPopMatrix();
}
