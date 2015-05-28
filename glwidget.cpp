#include "glwidget.h"

#include <math.h>
#include <QDebug>

/* User defined constants */
#define WHEEL_DIAMETER 0.032f
#define PULSES_PER_REVOLUTION 14400.0f
#define AXLE_LENGTH 0.083f

/* Fixed constants */
#define PI 3.14159

/*********************/
/* define structures */
/*********************/

struct position
{
    float x;        /* meter */
    float y;        /* meter */
    float theta;    /* radian (counterclockwise from x-axis) */
};

/********************/
/* global variables */
/********************/

struct position current_position;
//Qlist<int> listpoints;

#define SCALE 1

GLWidget::GLWidget(QWidget *parent)
    : QGLWidget(parent),
      leftMouseButtonMode(INACTIVE),
      xRot(0), yRot(0), zRot(0),
      xPos(0), yPos(0), zPos(0),
      xTrans(0), yTrans(0), zTrans(0),
      zoomFactor(1.0), zoomInc(0), defaultZoomFactor(1.0)
{
//    zoomfactor = 10;

    xRot = 0.0f;
    yRot = 0.0f;
    zRot = 0.0f;


    defaultZoomFactor = 1;
    zoomInc = defaultZoomFactor/1000;

    yAxisReversed = false;

    current_position.x = 0.0;
    current_position.y = 0.0;
    current_position.theta = 0.0;

    timer = new QTimer(this);

    newPoint = false;

    left_count = 0;
    right_count = 0;
    dist_left = 0;
    dist_right = 0;
    left_ticks = 0;
    right_ticks = 0;
    expr1 = 0;
    cos_current = 0;
    sin_current = 0;
    right_minus_left = 0;
    MUL_COUNT = 0;

    MUL_COUNT  = (PI * WHEEL_DIAMETER) / PULSES_PER_REVOLUTION;

    connect(timer, SIGNAL(timeout()), this, SLOT(timerEvent()));

//    timer->start(500);
}

GLWidget::~GLWidget()
{

}


void GLWidget::addXY(GLfloat x, GLfloat y)
{

    this->mX = x;
    this->mY = y;

    listPointsx.append(x);
    listPointsy.append(y);

//    zoomfactor = qMax(qMax(x,y),zoomfactor);

//    resizeGL(this->width(), this->height());
//    updateGL();

}


void GLWidget::addPulsesVrVl(int vr, int vl)
{

    left_count = vl;
    right_count = vr;

    left_ticks = left_count;
    right_ticks = right_count;
    left_count = 0;
    right_count = 0;

    dist_left = (float)left_ticks * MUL_COUNT;
    dist_right = (float)right_ticks * MUL_COUNT;

    cos_current = cos(current_position.theta);
    sin_current = sin(current_position.theta);

    if (abs(left_ticks - right_ticks) == 0)
    {
        /* Moving in a straight line */
        current_position.x += dist_left * cos_current;
        current_position.y += dist_left * sin_current;
    }
    else
    {
        /* Moving in an arc */
        expr1 = (AXLE_LENGTH * (dist_right + dist_left)) / (2.0 * (dist_right - dist_left));

        right_minus_left = dist_right - dist_left;

        current_position.x += expr1 * (sin(right_minus_left /  AXLE_LENGTH + current_position.theta) - sin_current);

        current_position.y -= expr1 * (cos(right_minus_left / AXLE_LENGTH + current_position.theta) - cos_current);

        /* Calculate new orientation */
        current_position.theta += right_minus_left / AXLE_LENGTH;

        /* Keep in the range -PI to +PI */
        while(current_position.theta > PI)
            current_position.theta -= (2.0*PI);
        while(current_position.theta < -PI)
            current_position.theta += (2.0*PI);
    }

    listPointsx.append(current_position.x*SCALE);
    listPointsy.append(current_position.y*SCALE);


//    qDebug() << "x = " << current_position.x << "; y = " << current_position.y;
}

void GLWidget::convertVrVl(int vr, int vl)
{

    left_count = vl;
    right_count = vr;



    left_ticks = left_count;
    right_ticks = right_count;
    left_count = 0;
    right_count = 0;

    dist_left = (float)left_ticks * MUL_COUNT;
    dist_right = (float)right_ticks * MUL_COUNT;

    cos_current = cos(current_position.theta);
    sin_current = sin(current_position.theta);

    if (abs(left_ticks - right_ticks) <= 30)
    {
        /* Moving in a straight line */
        current_position.x += dist_left * cos_current;
        current_position.y += dist_left * sin_current;
    }
    else
    {
        /* Moving in an arc */
        expr1 = AXLE_LENGTH * (dist_right + dist_left)
                / 2.0 / (dist_right - dist_left);

        right_minus_left = dist_right - dist_left;

        current_position.x += expr1 * (sin(right_minus_left /
                                           AXLE_LENGTH + current_position.theta) - sin_current);

        current_position.y -= expr1 * (cos(right_minus_left /
                                           AXLE_LENGTH + current_position.theta) - cos_current);

        /* Calculate new orientation */
        current_position.theta += right_minus_left / AXLE_LENGTH;

        /* Keep in the range -PI to +PI */
        while(current_position.theta > PI)
            current_position.theta -= (2.0*PI);
        while(current_position.theta < -PI)
            current_position.theta += (2.0*PI);
    }

    listPointsx.append(current_position.x*SCALE);
    listPointsy.append(current_position.y*SCALE);


//    qDebug() << "x = " << current_position.x << "; y = " << current_position.y;
}

void GLWidget::update()
{
    updateGL();
}

void GLWidget::stoptimer()
{
    timer->stop();
}

void GLWidget::starttimer()
{
    timer->start();
}

void GLWidget::timerEvent()
{
//    xRot += 90;
    updateGL();
}

void GLWidget::setXRotation(int angle)
{
    normalizeAngle(&angle);
    if(angle != xRot)
    {
        xRot = angle;
        emit xRotationChanged(angle);
        updateGL();

    }
}

void GLWidget::setYRotation(int angle)
{
    normalizeAngle(&angle);
    if(angle != yRot)
    {
        yRot = angle;
        emit yRotationChanged(angle);
        updateGL();
//        update();
    }
}

void GLWidget::setZRotation(int angle)
{
    normalizeAngle(&angle);
    if(angle != zRot)
    {
        zRot = angle;
        emit zRotationChanged(angle);
        updateGL();
//        update();
    }
}

void GLWidget::setXTranslation(const float distance)
{
    if(distance != xTrans)
    {
        xTrans = distance;
        emit xTranslationChanged(distance);
        updateGL();
//        update();
    }
}

void GLWidget::setYTranslation(const float distance)
{
    if(distance != yTrans)
    {
        yTrans = distance;
        emit yTranslationChanged(distance);
        updateGL();
//        update();
    }
}

void GLWidget::setZoomLevel(const float level)
{
    if(level != zoomFactor)
    {
        zoomFactor = level;
        if(level <= 0)
        {
            zoomFactor = 0.001;
        }
        zoomInc = zoomFactor/1000;
        emit zoomChanged(level);
        updateGL();
//        update();
    }
}

void GLWidget::setLeftMouseButtonMode(const GLWidget::LeftMouseButtonMode mode)
{
    leftMouseButtonMode = mode;
    updateCursor();
}

void GLWidget::initializeGL()
{
    qglClearColor(Qt::black);

    glEnable(GL_DEPTH_TEST);

}

void GLWidget::paintGL()
{

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();


    // Adjust clipping box
    if(width <= height)
    {
        glOrtho(-zoomFactor, zoomFactor, -zoomFactor*height/width,
                zoomFactor*height/width, -zoomFactor*5000.0f, zoomFactor*5000.0f);
    }
    else
    {
        glOrtho(-zoomFactor*width/height, zoomFactor*width/height,
                -zoomFactor, zoomFactor, -zoomFactor*5000.0f, zoomFactor*5000.0f);
    }
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glTranslated(-xTrans, -yTrans, -zTrans);
    glRotated(xRot / 16.0, 1.0, 0.0, 0.0);
    glRotated(yRot / 16.0, 0.0, 1.0, 0.0);
    glRotated(zRot / 16.0, 0.0, 0.0, 1.0);


    glTranslated(-xPos, -yPos, -zPos);



    draw();

//    glFlush();
}

void GLWidget::resizeGL(int w, int h)
{
    this->width = w;
    this->height = h;

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

void GLWidget::mousePressEvent(QMouseEvent *event)
{
    Q_UNUSED(event);

    lastPos = event->pos();
    QCursor cursor = this->cursor();
    if(leftMouseButtonMode == ROTATE)
    {
        cursor.setShape(Qt::SizeAllCursor);
    }
    if(leftMouseButtonMode == PANNING)
    {
        cursor.setShape(Qt::SizeAllCursor);
    }
    if(event->buttons() & Qt::RightButton)
    {
    }
    else if(event->buttons() & Qt::MidButton)
    {
        cursor.setShape(Qt::SizeAllCursor);
    }
    QWidget::setCursor(cursor);

}

void GLWidget::mouseReleaseEvent(QMouseEvent *)
{
    updateCursor();
}

void GLWidget::mouseMoveEvent(QMouseEvent *event)
{
    Q_UNUSED(event);

    int dx = event->x() - lastPos.x();
    int dy = event->y() - lastPos.y();

    //int face = this->faceAtPosition(event->pos());

//    qDebug() << "Face selected -> " << face;

    // Reverse Y-Axis if needed
    if(yAxisReversed)
    {
        dy = -dy;
    }

    if  (
        (event->buttons() & Qt::LeftButton && leftMouseButtonMode == PANNING) ||
        event->buttons() & Qt::MidButton
        )
    {
        if(width <= height)
        {
            setXTranslation(xTrans - dx*2*zoomFactor/width);
            setYTranslation(yTrans + dy*2*zoomFactor/width);
        }
        else
        {
            setXTranslation(xTrans - dx*2*zoomFactor/height);
            setYTranslation(yTrans + dy*2*zoomFactor/height);
        }
    }
    else if (
            (event->buttons() & Qt::LeftButton && leftMouseButtonMode == ROTATE) ||
            event->buttons() & Qt::RightButton
            )
    {
        setXRotation(xRot + 8 * dy);
        setZRotation(zRot - 8 * dx);
    }
    lastPos = event->pos();
}

void GLWidget::wheelEvent(QWheelEvent *event)
{
    int delta = event->delta();

    setZoomLevel(zoomFactor - delta*zoomInc);
}

void GLWidget::draw()
{



    qglColor(Qt::red);

    glPointSize(2.0);
    glBegin(GL_POINTS);
    for(int i = 0; i < listPointsx.count(); i++)
    {
        glVertex3f(listPointsx[i], listPointsy[i], 0);
    }
    glEnd();

//    qglColor(Qt::red);
//    glBegin(GL_QUADS);
//        glNormal3f(0,0,-1);
//        glVertex3f(-1,-1,0);
//        glVertex3f(-1,1,0);
//        glVertex3f(1,1,0);
//        glVertex3f(1,-1,0);
//    glEnd();

//    qglColor(Qt::green);
//    glBegin(GL_TRIANGLES);
//        glNormal3f(0,-1,0.707);
//        glVertex3f(-1,-1,0);
//        glVertex3f(1,-1,0);
//        glVertex3f(0,0,1.2);
//    glEnd();

//    qglColor(Qt::blue);
//    glBegin(GL_TRIANGLES);
//        glNormal3f(1,0, 0.707);
//        glVertex3f(1,-1,0);
//        glVertex3f(1,1,0);
//        glVertex3f(0,0,1.2);
//    glEnd();

//    qglColor(Qt::white);
//    glBegin(GL_TRIANGLES);
//        glNormal3f(0,1,0.707);
//        glVertex3f(1,1,0);
//        glVertex3f(-1,1,0);
//        glVertex3f(0,0,1.2);
//    glEnd();

//    qglColor(Qt::yellow);
//    glBegin(GL_TRIANGLES);
//        glNormal3f(-1,0,0.707);
//        glVertex3f(-1,1,0);
//        glVertex3f(-1,-1,0);
//        glVertex3f(0,0,1.2);
    //    glEnd();
}

void GLWidget::updateCursor()
{
    QCursor cursor = this->cursor();
    cursor.setShape(Qt::ArrowCursor);
    if(leftMouseButtonMode == ROTATE)
    {
        cursor.setShape(Qt::SizeAllCursor);
    }
    else if(leftMouseButtonMode == PANNING)
    {
        cursor.setShape(Qt::SizeAllCursor);
    }
    QWidget::setCursor(cursor);
}

void GLWidget::normalizeAngle(int *angle)
{
    while(*angle < 0)
    {
        *angle += 360 * 16;
    }
    while(*angle > 360 * 16)
    {
        *angle -= 360 * 16;
    }
}




