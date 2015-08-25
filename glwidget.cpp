#include "glwidget.h"

#include <math.h>
#include <QDebug>


GLWidget::GLWidget(QWidget *parent)
    : QGLWidget(parent),

      xRot(0), yRot(0), zRot(0),
      xPos(0), yPos(0), zPos(0),
      xTrans(0), yTrans(0), zTrans(0),
      zoomFactor(1.0), zoomInc(0), defaultZoomFactor(1.0)

{

    leftMouseButtonMode = INACTIVE;

    wheelDiameter = 0.032;
    pulsesPerRevolution = 14400.0;
    axleLength = 0.083;

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

    MUL_COUNT  = (M_PI * wheelDiameter) / pulsesPerRevolution;

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
        expr1 = (axleLength * (dist_right + dist_left)) / (2.0 * (dist_right - dist_left));

        right_minus_left = dist_right - dist_left;

        current_position.x += expr1 * (sin(right_minus_left /  axleLength + current_position.theta) - sin_current);

        current_position.y -= expr1 * (cos(right_minus_left / axleLength + current_position.theta) - cos_current);

        /* Calculate new orientation */
        current_position.theta += right_minus_left / axleLength;

        /* Keep in the range -PI to +PI */
        while(current_position.theta > M_PI)
            current_position.theta -= (2.0*M_PI);
        while(current_position.theta < -M_PI)
            current_position.theta += (2.0*M_PI);
    }

    listPointsx.append(current_position.x);
    listPointsy.append(current_position.y);


    //    qDebug() << "x = " << current_position.x << "; y = " << current_position.y;
}

void GLWidget::addPulsesSrSl(int srPulses, int slPulses)
{

    float sr = (float)srPulses * MUL_COUNT;
    float sl = (float)slPulses * MUL_COUNT;

    float s = (sr + sl)/2;

    current_position.theta = (sr - sl)/axleLength;

    current_position.x = s*cos(current_position.theta);
    current_position.y = s*sin(current_position.theta);

    listPointsx.append(current_position.x);
    listPointsy.append(current_position.y);

}

void GLWidget::resetDeadReckoning()
{
    wheelDiameter = 0.032;
    pulsesPerRevolution = 14400.0;
    axleLength = 0.083;

    current_position.x = 0.0;
    current_position.y = 0.0;
    current_position.theta = 0.0;

    listPointsx.clear();
    listPointsy.clear();

    updateGL();
}

QSize GLWidget::sizeHint() const
{
    return QSize(800,600);
}


void GLWidget::clearEncoderLists()
{
    encoderLList.clear();
    encoderRList.clear();
}

int GLWidget::encoderListCount()
{
    int result = 0;
    if(encoderLList.count() > 0)
    {
        result = encoderLList.count();
    }


    return result;
}

void GLWidget::convertVrVl(int vr, int vl)
{

    if(abs(vr - vl) < 10)
    {
        vr = (vr + vl)/2;
        vl = vr;
    }

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

    if (left_ticks == right_ticks)
    {

        /* Moving in a straight line */
        current_position.x += dist_left * cos_current;
        current_position.y += dist_left * sin_current;
    }
    else
    {
        /* Moving in an arc */
        expr1 = axleLength * (dist_right + dist_left)
                / 2.0 / (dist_right - dist_left);

        right_minus_left = dist_right - dist_left;

        current_position.x += expr1 * (sin(right_minus_left /
                                           axleLength + current_position.theta) - sin_current);

        current_position.y -= expr1 * (cos(right_minus_left /
                                           axleLength + current_position.theta) - cos_current);

        /* Calculate new orientation */
        current_position.theta += right_minus_left / axleLength;

        /* Keep in the range -PI to +PI */
        while(current_position.theta > M_PI)
            current_position.theta -= (2.0*M_PI);
        while(current_position.theta < -M_PI)
            current_position.theta += (2.0*M_PI);
    }

    listPointsx.append(current_position.x);
    listPointsy.append(current_position.y);


//    qDebug() << "x = " << current_position.x << "; y = " << current_position.y;
}
QList<GLfloat> GLWidget::getEncoderLList() const
{
    return encoderLList;
}

void GLWidget::setEncoderLList(const QList<GLfloat> &value)
{
    encoderLList = value;
}

QList<GLfloat> GLWidget::getEncoderRList() const
{
    return encoderRList;
}

void GLWidget::setEncoderRList(const QList<GLfloat> &value)
{
    encoderRList = value;
}


void GLWidget::setDeadReckoningMethod(const DeadReckoningMethod &value)
{
    deadReckoningMethod = value;
}




void GLWidget::update()
{
    updateGL();
}

void GLWidget::addEncoderPulesVrVl(float* encoders)
{
    qDebug() << "D-R";
    if(deadReckoningMethod == INTEGRAL)
    {
        for(int i = 0; i < 200; i = i + 2)
        {
            this->encoderRList.append(encoders[i+1]);
            this->encoderLList.append(encoders[i]);
            this->addPulsesVrVl(encoders[i+1], encoders[i]);
            //        qDebug() << encoders[i+1] << " " << encoders[1];
        }
    }
    if(deadReckoningMethod == SIMPLE)
    {
        for(int i = 0; i < 200; i = i + 2)
        {
            this->encoderRList.append(encoders[i+1]);
            this->encoderLList.append(encoders[i]);
            this->addPulsesSrSl(encoders[i+1], encoders[i]);
        }
    }

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
    }
}

void GLWidget::setXTranslation(const float distance)
{
    if(distance != xTrans)
    {
        xTrans = distance;
        emit xTranslationChanged(distance);
        updateGL();
    }
}

void GLWidget::setYTranslation(const float distance)
{
    if(distance != yTrans)
    {
        yTrans = distance;
        emit yTranslationChanged(distance);
        updateGL();
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

    // Adjust clipping box (zoom)
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

float GLWidget::getAxleLength() const
{
    return axleLength;
}

void GLWidget::setAxleLength(float value)
{
    axleLength = value;
}

float GLWidget::getPulsesPerRevolution() const
{
    return pulsesPerRevolution;
}

void GLWidget::setPulsesPerRevolution(float value)
{
    pulsesPerRevolution = value;
}

float GLWidget::getWheelDiameter() const
{
    return wheelDiameter;
}

void GLWidget::setWheelDiameter(float value)
{
    wheelDiameter = value;
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

}

void GLWidget::updateCursor(void)
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




