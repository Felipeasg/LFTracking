#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QObject>
#include <QWidget>
#include <QGLWidget>
#include <QTimer>
#include <QMouseEvent>

class GLWidget : public QGLWidget
{
    Q_OBJECT
public:

    enum LeftMouseButtonMode
    {
        INACTIVE,
        ROTATE,
        PANNING
    };

    enum DeadReckoningMethod
    {
        INTEGRAL,
        SIMPLE
    };

    explicit GLWidget(QWidget *parent = 0);
    ~GLWidget();

    void addXY(GLfloat x, GLfloat y);
    void addPulsesVrVl(int vr, int vl);
    void addPulsesSrSl(int srPulses, int slPulses);
    void resetDeadReckoning();
    QSize sizeHint() const;



    float getWheelDiameter() const;
    void setWheelDiameter(float value);

    float getPulsesPerRevolution() const;
    void setPulsesPerRevolution(float value);

    float getAxleLength() const;
    void setAxleLength(float value);

    void setDeadReckoningMethod(const DeadReckoningMethod &value);

signals:
    void xRotationChanged(const int angle) const;
    void yRotationChanged(const int angle) const;
    void zRotationChanged(const int angle) const;
    void xTranslationChanged(const float distance) const;
    void yTranslationChanged(const float distance) const;
    void zoomChanged(const float zoom);

public slots:

    void setXRotation(int angle);
    void setYRotation(int angle);
    void setZRotation(int angle);
    void setXTranslation(const float distance);
    void setYTranslation(const float distance);
    void setZoomLevel(const float level);
    void setLeftMouseButtonMode(const GLWidget::LeftMouseButtonMode mode);
    void update();

    void addEncoderPulesVrVl(int *encoders);

protected:
    //opengl events
    void initializeGL();
    void paintGL();
    void resizeGL(int w, int h);

    //mouse events
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent*);
    void mouseMoveEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *event);

private:

    //Struct to dead reckoning
    struct position
    {
        float x;        /* meter */
        float y;        /* meter */
        float theta;    /* radian (counterclockwise from x-axis) */
    };

    struct position current_position;

    float wheelDiameter;
    float pulsesPerRevolution;
    float axleLength;

    void draw(void);
    void updateCursor(void);
    void normalizeAngle(int *angle);
    void convertVrVl(int vr, int vl);

    GLfloat zoomfactor;

    int xRot, yRot, zRot;
    int xPos, yPos, zPos;
    float xTrans, yTrans, zTrans;
    QPoint lastPos;

    float zoomFactor;
    float zoomInc;
    float defaultZoomFactor;

    bool yAxisReversed;

    LeftMouseButtonMode leftMouseButtonMode;

    int width, height;

    QTimer* timer;

    bool newPoint;

    GLfloat mX, mY;

    QList<GLfloat> listPointsx;
    QList<GLfloat> listPointsy;

    int left_count;
    int right_count;
    float dist_left;
    float dist_right;
    int left_ticks;
    int right_ticks;
    float expr1;
    float cos_current;
    float sin_current;
    float right_minus_left;
    float MUL_COUNT;

    DeadReckoningMethod deadReckoningMethod;


};

#endif // GLWIDGET_H
