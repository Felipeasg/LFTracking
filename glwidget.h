#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QObject>
#include <QWidget>
#include <QGLWidget>
#include <QTimer>

class GLWidget : public QGLWidget
{
    Q_OBJECT
public:
    explicit GLWidget(QWidget *parent = 0);
    ~GLWidget();

signals:

public slots:
    void timerEvent(void);
protected:
    void initializeGL();
    void paintGL();
    void resizeGL(int w, int h);

private:
    void draw(void);

    GLfloat zoomfactor;

    GLint xRot, yRot, zRot;

    QTimer* timer;
};

#endif // GLWIDGET_H
