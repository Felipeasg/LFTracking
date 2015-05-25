#include "glwidget.h"

GLWidget::GLWidget(QWidget *parent) : QGLWidget(parent)
{
    zoomfactor = 5;

    xRot = 0.0f;
    yRot = 0.0f;
    zRot = 0.0f;

    timer = new QTimer(this);

    connect(timer, SIGNAL(timeout()), this, SLOT(timerEvent()));

    timer->start(1500);
}

GLWidget::~GLWidget()
{

}

void GLWidget::timerEvent()
{
    xRot += 90;
    updateGL();
}

void GLWidget::initializeGL()
{
    qglClearColor(Qt::black);

    glEnable(GL_DEPTH_TEST);

}

void GLWidget::paintGL()
{

    glMatrixMode(GL_MODELVIEW);;
    glLoadIdentity();

    // Limpa a janela de visualização com a cor de fundo especificada
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


    glRotated(xRot , 1.0, 0.0, 0.0);
    glRotated(yRot , 0.0, 1.0, 0.0);
    glRotated(zRot , 0.0, 0.0, 1.0);

    // Especifica que a cor corrente é vermelha
    //         R     G     B
    glColor3f(0.3f, 0.6f, 0.45f);


    draw();

    glFlush();
}

void GLWidget::resizeGL(int w, int h)
{
    // Evita a divisao por zero
    if(h == 0) h = 1;

    // Especifica as dimensões da Viewport
    glViewport(0, 0, w, h);

    // Inicializa o sistema de coordenadas
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();


    if(w < h)
    {
        glOrtho(-zoomfactor, zoomfactor, -zoomfactor*(h/w),
                zoomfactor*h/w, -zoomfactor*5000.0f, zoomfactor*5000);
    }
    else
    {
        glOrtho(-zoomfactor*w/h, zoomfactor*w/h,
                -zoomfactor, zoomfactor, -zoomfactor*5000.0f, zoomfactor*5000.0f);
    }
}

void GLWidget::draw()
{
    qglColor(Qt::red);
    glBegin(GL_QUADS);
        glNormal3f(0,0,-1);
        glVertex3f(-1,-1,0);
        glVertex3f(-1,1,0);
        glVertex3f(1,1,0);
        glVertex3f(1,-1,0);
    glEnd();

    qglColor(Qt::green);
    glBegin(GL_TRIANGLES);
        glNormal3f(0,-1,0.707);
        glVertex3f(-1,-1,0);
        glVertex3f(1,-1,0);
        glVertex3f(0,0,1.2);
    glEnd();

    qglColor(Qt::blue);
    glBegin(GL_TRIANGLES);
        glNormal3f(1,0, 0.707);
        glVertex3f(1,-1,0);
        glVertex3f(1,1,0);
        glVertex3f(0,0,1.2);
    glEnd();

    qglColor(Qt::white);
    glBegin(GL_TRIANGLES);
        glNormal3f(0,1,0.707);
        glVertex3f(1,1,0);
        glVertex3f(-1,1,0);
        glVertex3f(0,0,1.2);
    glEnd();

    qglColor(Qt::yellow);
    glBegin(GL_TRIANGLES);
        glNormal3f(-1,0,0.707);
        glVertex3f(-1,1,0);
        glVertex3f(-1,-1,0);
        glVertex3f(0,0,1.2);
    glEnd();
}

