#include "mainwindow.h"

#include <QDebug>
#include <QScreen>
#include <stdlib.h>
#include <QOpenGLContext>
#include <math.h>

#define COLOR_SEED 57636437
#define NEAR_PLANE 2
#define FAR_PLANE 500
#define PI 3.14159265359

MainWindow::MainWindow()
    : m_shaderProgram(0)
{
    qDebug() << "Constructor of MainWindow";
    // NOTE: OpenGL functions are not yet enabled when the constructor is called
}

MainWindow::~MainWindow()
{
    qDebug() << "Desctructor of MainWindow";

    // Free all your used resources here

    // Destroy buffers first, before destroying the VertexArrayObject

    m_funcs->glDeleteVertexArrays(1, &VAO);
    m_funcs->glDeleteBuffers(1, &vertices);
    m_funcs->glDeleteBuffers(1, &colors);
    m_funcs->glDeleteBuffers(1, &uvCoords);
    m_funcs->glDeleteTextures(1, &textureWood);


    // free the pointer of the shading program
    delete m_shaderProgram;

}

void MainWindow::readTexture(){

    // Load image
    char* fileName = ":/textures/wood.png";
    QImage texture;
    if (!texture.load(QString(fileName))){
        qDebug() << "IMAGE NOT LOADED";
    }

    // Generate texture in Glint
    m_funcs->glGenTextures(1, &textureWood);
    m_funcs->glBindTexture(GL_TEXTURE_2D, textureWood );
    // Set the texture and give boundaries
    m_funcs->glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture.width(), texture.height(), 0, GL_BGRA, GL_UNSIGNED_BYTE, texture.bits());
    m_funcs->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    m_funcs->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);


}

// Initialize all your OpenGL objects here
void MainWindow::initialize()
{
    // Initialize important variables and the MVP matrices
    mouseClick = QPoint(0,0);
    eye = QVector3D(0,0,-4);
    center = QVector3D(0,0,0) ;
    up = QVector3D(0,1,0);
    FoV = 60;
    model.setToIdentity();
    view.lookAt(eye, center, up);
    calculateProjection();

    qDebug() << "MainWindow::initialize()";
    QString glVersion;
    glVersion = reinterpret_cast<const char*>(glGetString(GL_VERSION));
    qDebug() << "Using OpenGL" << qPrintable(glVersion);

    // Initialize the shaders
    m_shaderProgram = new QOpenGLShaderProgram(this);
    // Use the ":" to load from the resources files (i.e. from the resources.qrc)
    m_shaderProgram->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/vertex.glsl");
    m_shaderProgram->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/fragment.glsl");
    m_shaderProgram->link();


    // Shaders are initialized
    // You can retrieve the locations of the uniforms from here


    // Initialize your objects and buffers
    QVector<QVector3D> objectVectors;
    QVector<QVector3D> generatedColors;
    QVector<QVector2D> txtcoord;

    OBJModel cube = OBJModel(":/models/cube.obj");

    objectVectors = cube.vertices;
    vectorsNumber = objectVectors.length();
    txtcoord = cube.texcoords;

    // Calculate the random colours using the color seed
    for(int i = 0; i < vectorsNumber/3; i++){
        float colorArray[3] = {0,0,0};
        for(unsigned int j = 0; j < 3; j++){
            srand(COLOR_SEED*(j+1)*(i+1));
            colorArray[j] = ((double)rand()/RAND_MAX);
        }
        generatedColors.append(QVector3D(colorArray[0], colorArray[1], colorArray[2]));
        generatedColors.append(QVector3D(colorArray[0], colorArray[1], colorArray[2]));
        generatedColors.append(QVector3D(colorArray[0], colorArray[1], colorArray[2]));
    }

    // generate VAO and bind it
    m_funcs->glGenVertexArrays(1, &VAO);
    m_funcs->glBindVertexArray(VAO);

    // generate color and vertice buffers
    m_funcs->glGenBuffers(1, &vertices);
    m_funcs->glGenBuffers(1, &colors);
    m_funcs->glGenBuffers(1, &uvCoords);

    // bind vertice buffer and fill it with the vertices
    m_funcs->glBindBuffer(GL_ARRAY_BUFFER, vertices);
    m_funcs->glBufferData(GL_ARRAY_BUFFER, sizeof(QVector3D) * objectVectors.length(),objectVectors.data(), GL_STATIC_DRAW);

    m_funcs->glEnableVertexAttribArray(0);
    m_funcs->glVertexAttribPointer(0,3, GL_FLOAT, GL_FALSE, 0,0);

    // Bind color buffer, fill it with the generated colors
    m_funcs->glBindBuffer(GL_ARRAY_BUFFER, colors);
    m_funcs->glBufferData(GL_ARRAY_BUFFER, sizeof(QVector3D) * generatedColors.length(), generatedColors.data(), GL_STATIC_DRAW);

    m_funcs->glEnableVertexAttribArray(1);
    m_funcs->glVertexAttribPointer(1,3, GL_FLOAT, GL_FALSE, 0,0);

    // read the texture
    readTexture();

    // Bind the uv coordinates to the shader
    m_funcs->glBindBuffer(GL_ARRAY_BUFFER, uvCoords);
    m_funcs->glBufferData(GL_ARRAY_BUFFER, sizeof(QVector2D) * txtcoord.length(), txtcoord.data(), GL_STATIC_DRAW);

    m_funcs->glEnableVertexAttribArray(3);
    m_funcs->glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 0, 0);

    // Create your Vertex Array Object (VAO) and Vertex Buffer Objects (VBO) here.



    // Set OpenGL to use Filled triangles (can be used to render points, or lines)
    m_funcs->glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );

    // Enable Z-buffering
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    qDebug() << "Depth Buffer size:" <<  this->format().depthBufferSize() << "bits";

    // Function for culling, removing faces which don't face the camera
    //glEnable(GL_CULL_FACE);
    //glCullFace(GL_BACK);

    // Set the clear color to be black (color used for resetting the screen)
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
}

void MainWindow::renderSphere(QVector3D pos, QVector3D color, QVector4D material, QVector3D lightpos)
{
    // OpenGL assignment 1, part 2: create a function to render the sphere
    // Use OBJModel(":/models/spehere.obj") for the model

    // you must remove these Q_UNUSED when you implement this function
    Q_UNUSED(pos)
    Q_UNUSED(color)
    Q_UNUSED(material)
    Q_UNUSED(lightpos)
}

/**
 * Renders a similar scene used for the raytracer:
 * 5 colored spheres with a single light
 */
void MainWindow::renderRaytracerScene()
{
    QVector3D lightpos = QVector3D(-200,600,1500);

    // Blue sphere
    renderSphere(QVector3D(90,320,100),QVector3D(0,0,1),QVector4D(0.2f,0.7f,0.5f,64),lightpos);

    // Green sphere
    renderSphere(QVector3D(210,270,300),QVector3D(0,1,0),QVector4D(0.2f,0.3f,0.5f,8),lightpos);

    // Red sphere
    renderSphere(QVector3D(290,170,150),QVector3D(1,0,0),QVector4D(0.2f,0.7f,0.8f,32),lightpos);

    // Yellow sphere
    renderSphere(QVector3D(140,220,400),QVector3D(1,0.8f,0),QVector4D(0.2f,0.8f,0.0f,1),lightpos);

    // Orange sphere
    renderSphere(QVector3D(110,130,200),QVector3D(1,0.5f,0),QVector4D(0.2f,0.8f,0.5f,32),lightpos);
}

// The render function, called when an update is requested
void MainWindow::render()
{
    // glViewport is used for specifying the resolution to render
    // Uses the window size as the resolution
    const qreal retinaScale = devicePixelRatio();
    glViewport(0, 0, width() * retinaScale, height() * retinaScale);

    // Clear the screen at the start of the rendering.
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);


    // Bind the shaderprogram to use it
    m_shaderProgram->bind();
    m_shaderProgram->setUniformValue("model", model);
    m_shaderProgram->setUniformValue("view", view);
    m_shaderProgram->setUniformValue("projection", projection);
    m_funcs->glBindVertexArray(VAO);
    m_funcs->glBindTexture(GL_TEXTURE_2D, textureWood);
    // Draw cube
    m_funcs->glDrawArrays(GL_TRIANGLES, 0, vectorsNumber);

    // relases the current shaderprogram (to bind an use another shaderprogram for example)
    m_shaderProgram->release();

}

// Calculates the projection matrix using FoV, NEAR_PLANE and FAR_PLANE
void MainWindow::calculateProjection(){
    float top, bottom, right, left;
    top = NEAR_PLANE*(tanf(PI/180 * FoV/2));
    bottom = -top;
    right = top * (height()/width());
    left = -right;
    projection = QMatrix4x4((2*NEAR_PLANE)/(right-left), 0, (right+left)/(right-left), 0, 0, (2*NEAR_PLANE)/(top-bottom), (top+bottom)/(top-bottom),0,0,0,-((FAR_PLANE+NEAR_PLANE)/(FAR_PLANE-NEAR_PLANE)), -((2*FAR_PLANE*NEAR_PLANE)/(FAR_PLANE-NEAR_PLANE)), 0,0,-1,0);
}

// Below are functions which are triggered by input events:

// Triggered by pressing a key
void MainWindow::keyPressEvent(QKeyEvent *ev)
{
    switch(ev->key()) {
    case 'A': qDebug() << "A pressed"; break;
    default:
        // ev->key() is an integer. For alpha numeric characters keys it equivalent with the char value ('A' == 65, '1' == 49)
        // Alternatively, you could use Qt Key enums, see http://doc.qt.io/qt-5/qt.html#Key-enum
        qDebug() << ev->key() << "pressed";
        break;
    }

    // Used to update the screen
    renderLater();
}

// Triggered by releasing a key
void MainWindow::keyReleaseEvent(QKeyEvent *ev)
{
    switch(ev->key()) {
    case 'A': qDebug() << "A released"; break;
    default:
        qDebug() << ev->key() << "released";
        break;
    }

    renderLater();
}

// Triggered by clicking two subsequent times on any mouse button.
void MainWindow::mouseDoubleClickEvent(QMouseEvent *ev)
{
    qDebug() << "Mouse double clicked:" << ev->button();

    renderLater();
}

// Triggered when moving the mouse inside the window (even when no mouse button is clicked!)
void MainWindow::mouseMoveEvent(QMouseEvent *ev)
{
    // Check if not default position
    if(!(mouseClick.x() == 0 && mouseClick.y() ==0)){
        qDebug() << ev->button();
        //Calculate delta position
        QPoint deltaPos = mouseClick-ev->pos();
        // rotate
        model.rotate(-deltaPos.x(), model.inverted()*QVector3D(0,1,0));
        model.rotate(deltaPos.y(), model.inverted()*QVector3D(1,0,0));
        // set mouseclick to position
        mouseClick = ev->pos();

        renderLater();
    }

}

// Triggered when pressing any mouse button
void MainWindow::mousePressEvent(QMouseEvent *ev)
{
    qDebug() << "Mouse button pressed:" << ev->button();

    mouseClick =  ev->pos();

    renderLater();
}

// Triggered when releasing any mouse button
void MainWindow::mouseReleaseEvent(QMouseEvent *ev)
{
    qDebug() << "Mouse button released" << ev->button();

    mouseClick = QPoint(0,0);

    renderLater();
}

// Triggered when clicking scrolling with the scroll wheel on the mouse
void MainWindow::wheelEvent(QWheelEvent * ev)
{
    // Implement something
    qDebug() << "Mouse wheel:" << ev->delta();
    FoV -= ev->delta()/100;
    calculateProjection();

    renderLater();
}
