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

    explicit GLWidget(QWidget *parent = 0);
    ~GLWidget();

    void addXY(GLfloat x, GLfloat y);
    void addPulsesVrVl(int vr, int vl);


    QList<int> vrList;
    QList<int> vlList;

    void stoptimer();
    void starttimer();


signals:
    void xRotationChanged(const int angle) const;
    void yRotationChanged(const int angle) const;
    void zRotationChanged(const int angle) const;
    void xTranslationChanged(const float distance) const;
    void yTranslationChanged(const float distance) const;
    void zoomChanged(const float zoom);

public slots:
    void timerEvent(void);

    void setXRotation(int angle);
    void setYRotation(int angle);
    void setZRotation(int angle);
    void setXTranslation(const float distance);
    void setYTranslation(const float distance);
    void setZoomLevel(const float level);
    void setLeftMouseButtonMode(const GLWidget::LeftMouseButtonMode mode);
    void update();

    void on_addEncoderPulesVrVl(int32_t *encoders);

protected:
    void initializeGL();
    void paintGL();
    void resizeGL(int w, int h);

    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent*);
    void mouseMoveEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *event);

private:
    void draw(void);
    void updateCursor();
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


};

#endif // GLWIDGET_H
