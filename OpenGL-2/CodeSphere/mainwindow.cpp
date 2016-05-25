#include "mainwindow.h"

#include <QDebug>
#include <QScreen>
#include <stdlib.h>
#include <QOpenGLContext>
#include <QTimer>
#include <math.h>


#define COLOR_SEED 576337
#define NEAR_PLANE 2
#define FAR_PLANE 2000
#define PI 3.14159265359
#define SOLAR
#define NOTROTATE

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
    m_funcs->glDeleteBuffers(1, &normals);
    m_funcs->glDeleteBuffers(1, &uvCoord);
    for (unsigned i = 0; i < TEX_COUNT; i ++){
        m_funcs->glDeleteTextures(1,&textures[i]);
    }
    // free the pointer of the shading program
    delete m_shaderProgram;

}

void MainWindow::readTexture(char* fileName, int i){
    QImage texture;
    if (!texture.load(QString(fileName))){
        qDebug() << "IMAGE NOT LOADED" << fileName;
    }

    // Generate texture in Glint
    m_funcs->glGenTextures(1, &textures[i]);
    m_funcs->glBindTexture(GL_TEXTURE_2D, textures[i] );
    // Set the texture and give boundaries
    m_funcs->glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture.width(), texture.height(), 0, GL_BGRA, GL_UNSIGNED_BYTE, texture.bits());
    m_funcs->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    m_funcs->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);


}

// Initialize all your OpenGL objects here
void MainWindow::initialize()
{
    mouseClick = QPoint(0,0);
    eye = QVector3D(200,200,1200);
    center = QVector3D(200,200,200) ;
    up = QVector3D(0,1,0);
    FoV = 50;
    moveDistance = 1;
    model.setToIdentity();
    view.lookAt(eye, center, up);
    calculateProjection();
    //projection.perspective(FoV,height()/width(),NEAR_PLANE, FAR_PLANE);
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
    QVector<QVector2D> textureCoords;

    OBJModel sphere = OBJModel(":/models/sphere.obj");
    textureCoords = sphere.texcoords;

    objectVectors = sphere.vertices;
    vectorsNumber = objectVectors.length();

    // generate VAO and bind it
    m_funcs->glGenVertexArrays(1, &VAO);
    m_funcs->glBindVertexArray(VAO);

    // generate color and vertice buffers
    m_funcs->glGenBuffers(1, &vertices);
    m_funcs->glGenBuffers(1, &normals);
    m_funcs->glGenBuffers(1, &uvCoord);

    // bind vertice buffer and fill it with the vertices
    m_funcs->glBindBuffer(GL_ARRAY_BUFFER, vertices);
    m_funcs->glBufferData(GL_ARRAY_BUFFER, sizeof(QVector3D) * objectVectors.length(),objectVectors.data(), GL_STATIC_DRAW);

    m_funcs->glEnableVertexAttribArray(0);
    m_funcs->glVertexAttribPointer(0,3, GL_FLOAT, GL_FALSE, 0,0);

    // Bind color buffer, fill it with the generated colors
    m_funcs->glBindBuffer(GL_ARRAY_BUFFER, normals);
    m_funcs->glBufferData(GL_ARRAY_BUFFER, sizeof(QVector3D) * sphere.normals.length(), sphere.normals.data(), GL_STATIC_DRAW);

    m_funcs->glEnableVertexAttribArray(2);
    m_funcs->glVertexAttribPointer(2,3, GL_FLOAT, GL_FALSE, 0,0);

    // Create your Vertex Array Object (VAO) and Vertex Buffer Objects (VBO) here.
    m_funcs->glBindBuffer(GL_ARRAY_BUFFER, uvCoord);
    m_funcs->glBufferData(GL_ARRAY_BUFFER, sizeof(QVector2D) * textureCoords.length(), textureCoords.data(), GL_STATIC_DRAW);

    m_funcs->glEnableVertexAttribArray(3);
    m_funcs->glVertexAttribPointer(3,2, GL_FLOAT, GL_FALSE, 0,0);

    char* texLoc[TEX_COUNT] = {":/textures/sun.png",":/textures/Small/mercury.png" , ":/textures/Small/venus.png" ,":/textures/earth.png" ,":/textures/Small/mars.png" ,":/textures/Small/jupiter.png" ,":/textures/Small/saturn.png" ,":/textures/Small/uranus.png" ,":/textures/Small/neptune.png" , ":/textures/Small/pluto.png" , ":/textures/Small/moon.png" };
    for (unsigned i = 0; i < TEX_COUNT; i++) {
        readTexture( texLoc[i] ,i);
    }

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

    //timer = new QTimer(this);
    //connect(timer, SIGNAL(timeout()), this, SLOT(timerEvent()));
    //timer->start(10);
    time = startTimer(1000/60);
}

void MainWindow::renderSphere(QVector3D pos, QVector3D color, QVector4D material, QVector3D lightpos, int tex, float scale, float speed)
{
    // OpenGL assignment 1, part 2: create a function to render the sphere
    // Use OBJModel(":/models/spehere.obj") for the model

    // you must remove these Q_UNUSED when you implement this function



    normalMatrix = (view*model).normalMatrix();
    QMatrix4x4 currentModel = model;

    //model.translate(pos.x(), pos.y() , pos.z());
    //currentModel.translate(pos.x()  , lightpos.y() + sinf(moveDistance/100 * speed), lightpos.z() + sinf(moveDistance/100 * speed) * radius);
    currentModel.translate(pos.x(), pos.y(), pos.z());
    currentModel.scale(scale);
#ifdef ROTATE
    currentModel.translate((lightpos+pos));
    //currentModel.rotate(moveDistance* speed, 0, 0, 1);
    currentModel.rotate(moveDistance* speed, 0, 1,1);
    currentModel.translate(-(lightpos+pos));
#endif
    // Give uniforms to shader
    m_shaderProgram->setUniformValue("model", currentModel);
    m_shaderProgram->setUniformValue("MaterialColor", color);
    m_shaderProgram->setUniformValue("intensities", material);
    m_shaderProgram->setUniformValue("lightPos", lightpos);
    m_shaderProgram->setUniformValue("normalMatrix",normalMatrix);
    m_shaderProgram->setUniformValue("viewPos", eye);

    // Draw current sphere
    if(tex >= 0){
        m_funcs->glBindTexture(GL_TEXTURE_2D, textures[tex]);
    }
    m_funcs->glDrawArrays(GL_TRIANGLES, 0, vectorsNumber);


}

/**
 * Renders a similar scene used for the raytracer:
 * 5 colored spheres with a single light
 */
void MainWindow::renderRaytracerScene()
{
#ifndef SOLAR
    QVector3D lightpos = QVector3D(-200,600,1500);


    // Blue sphere
    renderSphere(QVector3D(90,320,100),QVector3D(0,0,1),QVector4D(0.2f,0.7f,0.5f,64),lightpos, EARTH_TEX,1 );

    // Green sphere
    renderSphere(QVector3D(210,270,300),QVector3D(0,1,0),QVector4D(0.2f,0.3f,0.5f,8),lightpos, SUN_TEX, 1);

    // Red sphere
    renderSphere(QVector3D(290,170,150),QVector3D(1,0,0),QVector4D(0.2f,0.7f,0.8f,32),lightpos, MERCURY_TEX, 1);

    // Yellow sphere
    renderSphere(QVector3D(140,220,400),QVector3D(1,0.8f,0),QVector4D(0.2f,0.8f,0.0f,1),lightpos, MOON_TEX, 1);

    // Orange sphere
    renderSphere(QVector3D(110,130,200),QVector3D(1,0.5f,0),QVector4D(0.2f,0.8f,0.5f,32),lightpos, PLUTO_TEX, 1);
#endif
#ifdef SOLAR
    QVector3D lightpos = QVector3D(200,200,200);
    renderSphere(QVector3D(220,200,200),QVector3D(0,0,0),QVector4D(1,0,0,0),lightpos, SUN_TEX, 1, 0);
    renderSphere(QVector3D(250,200,200),QVector3D(0,0,0),QVector4D(0.2f,0.5f,0,0),lightpos, MERCURY_TEX, 0.1f, 10);
    renderSphere(QVector3D(300,200,200),QVector3D(0,0,0),QVector4D(0.2f,0.5f,0,0),lightpos, VENUS_TEX, 0.15f, 10);
    renderSphere(QVector3D(350,200,200),QVector3D(0,0,0),QVector4D(0.2f,0.5f,0.3f,30),lightpos, EARTH_TEX, 0.16f, 9);
    renderSphere(QVector3D(400,200,200),QVector3D(0,0,0),QVector4D(0.2f,0.5f,0,0),lightpos, MARS_TEX, 0.1f, 8);
    renderSphere(QVector3D(450,200,200),QVector3D(0,0,0),QVector4D(0.2f,0.5f,0,0),lightpos, JUPITER_TEX, 0.5f, 6);
    renderSphere(QVector3D(500,200,200),QVector3D(0,0,0),QVector4D(0.2f,0.5f,0,0),lightpos, SATURN_TEX, 0.4f, 6);
    renderSphere(QVector3D(550,200,200),QVector3D(0,0,0),QVector4D(0.2f,0.5f,0,0),lightpos, URANUS_TEX, 0.25f, 5);
    renderSphere(QVector3D(600,200,200),QVector3D(0,0,0),QVector4D(0.2f,0.5f,0,0),lightpos, NEPTUNE_TEX, 0.2f, 5);
    renderSphere(QVector3D(650,200,200),QVector3D(0,0,0),QVector4D(0.2f,0.5f,0,0),lightpos, PLUTO_TEX, 0.05f, 3);
#endif
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
    //m_shaderProgram->setUniformValue("model", model);
    m_shaderProgram->setUniformValue("view", view);
    m_shaderProgram->setUniformValue("projection", projection);
    m_funcs->glBindVertexArray(VAO);

    renderRaytracerScene();

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

void MainWindow::timerEvent(QTimerEvent *event){
    moveDistance += 0.1;// (2*PI)/1000;
    renderLater();
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
        model.rotate(deltaPos.x(), model.inverted()*QVector3D(0,1,0));
        model.rotate(deltaPos.y(), model.inverted()* QVector3D(1,0,0));
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
    // Set default
    mouseClick = QPoint(0,0);

    renderLater();
}

// Triggered when clicking scrolling with the scroll wheel on the mouse
void MainWindow::wheelEvent(QWheelEvent * ev)
{
    qDebug() << "Mouse wheel:" << ev->delta();
    float newFoV = FoV - ev->delta()/100;

    // Limit FoV
    if (newFoV < 15) {
        FoV = 15;
        return;
    }else if(newFoV > 75){
        FoV = 75;
        return;
    }
    // Set new FoV and recalculate projection Matrix
    FoV = newFoV;

    calculateProjection();

    renderLater();
}

