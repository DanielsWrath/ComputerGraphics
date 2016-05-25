#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "openglwindow.h"

#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QKeyEvent>
#include <QMouseEvent>

// Math includes
#include <QVector2D>
#include <QVector3D>
#include <QVector4D>
#include <QMatrix3x3>
#include <QMatrix4x4>

#include "objmodel.h"

/* MainWindow is the window used to display the application
 * Do all your changes to the code in this class (and the mainwindow.cpp file)
 */
class MainWindow : public OpenGLWindow
{
public:
    MainWindow();
    ~MainWindow();
    void initialize() Q_DECL_OVERRIDE;
    void render() Q_DECL_OVERRIDE;

protected:
    // Functions for keyboard input events
    void keyPressEvent(QKeyEvent * ev);
    void keyReleaseEvent(QKeyEvent * ev);

    // Function for mouse input events
    void mouseDoubleClickEvent(QMouseEvent * ev);
    void mouseMoveEvent(QMouseEvent * ev);
    void mousePressEvent(QMouseEvent * ev);
    void mouseReleaseEvent(QMouseEvent * ev);
    void wheelEvent(QWheelEvent * ev);
    void timerEvent(QTimerEvent *event);

private:
    enum textureNames{
        SUN_TEX,
        MERCURY_TEX,
        VENUS_TEX,
        EARTH_TEX,
        MARS_TEX,
        JUPITER_TEX,
        SATURN_TEX,
        URANUS_TEX,
        NEPTUNE_TEX,
        PLUTO_TEX,
        MOON_TEX,
        TEX_COUNT

    };

    typedef enum shaderNames{
        SHADER_DEFAULT,
        SHADER_BLUR,
        SHADER_FRAMEBUFFER,
        SHADER_GOOCH,
        SHADER_COUNT
    }shaderName;

    enum VAONames{
        VAO_SPHERE,
        VAO_CUBE,
        VAO_SPOT,
        VAO_SUZANNE,
        VAO_FERTILITY,
        VAO_COUNT
    };

    void renderSphere(QVector3D pos, QVector3D color, QVector4D material, QVector3D lightpos, int tex, float scale, float speed);
    void renderObj(QVector3D pos, QVector3D color, QVector4D material, QVector3D lightpos, float scale,int type);
    void renderRaytracerScene();
    void renderScene();
    void calculateProjection();
    void readTexture(char* fileName, int i);
    void genVAO(OBJModel obj, GLuint type);

    // Declare your VAO and VBO variables here
    int *vectorsNumber;
    GLuint *VAO, VAOQuad, *vertices, verticesQuad, *normals, *uvCoord, frameBuffer, frameBufferTexture,frameBufferDepth;
    GLuint textures[TEX_COUNT];
    GLenum *frameBufferDraw;
    QOpenGLVertexArrayObject QVAO;
    QOpenGLBuffer *QCoordinates, *QColors;
    QMatrix4x4 model, view, projection;
    QMatrix3x3 normalMatrix;
    float FoV, rotation;
    QPoint mouseClick;
    QVector3D eye, center, up;
    QTimer *timer;
    int time;
    double moveDistance;

    // Members for the shader and uniform variables
    QOpenGLShaderProgram **m_shaderProgram;


};

#endif // MAINWINDOW_H
