#include "mainwindow.h"

#include <QDebug>
#include <QScreen>
#include <stdlib.h>
#include <QOpenGLContext>
#include <QTimer>
#include <math.h>


#define COLOR_SEED 576337
#define NEAR_PLANE 2
#define FAR_PLANE 1200
#define PI 3.14159265359
#define NOTSOLAR // sets to rendering solar system
#define NOTROTATE // sets rotate (not working well)
#define NOTBLUR // sets blur
#define shaderUsed SHADER_GOOCH // choose the shader (SHADER_DEFAULT or SHADER_GOOCH)
#define NOTRAYTRACER // Render raytracer scene
// NOTE: If textures have to be used, change the vec4 tex in the shaders

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
    for(int i = 0; i < VAO_COUNT; i++){
        m_funcs->glDeleteBuffers(1, &vertices[i]);
        m_funcs->glDeleteBuffers(1, &normals[i]);
        m_funcs->glDeleteBuffers(1, &uvCoord[i]);
        m_funcs->glDeleteVertexArrays(1, &VAO[i]);
    }
    m_funcs->glDeleteVertexArrays(1, &VAOQuad);
    m_funcs->glDeleteBuffers(1, &verticesQuad);
    for (unsigned i = 0; i < TEX_COUNT; i ++){
        m_funcs->glDeleteTextures(1,&textures[i]);
    }
    m_funcs->glDeleteFramebuffers(1,&frameBuffer);
    // free the pointer of the shading program
    delete[] m_shaderProgram;
    delete[] vectorsNumber;
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

void MainWindow::genVAO(OBJModel obj, GLuint type){
    vectorsNumber[type] = obj.vertices.length();

    // generate VAO and bind it
    m_funcs->glGenVertexArrays(1, &VAO[type]);
    m_funcs->glBindVertexArray(VAO[type]);

    // generate color and vertice buffers
    m_funcs->glGenBuffers(1, &vertices[type]);
    m_funcs->glGenBuffers(1, &normals[type]);
    m_funcs->glGenBuffers(1, &uvCoord[type]);

    // bind vertice buffer and fill it with the vertices
    m_funcs->glBindBuffer(GL_ARRAY_BUFFER, vertices[type]);
    m_funcs->glBufferData(GL_ARRAY_BUFFER, sizeof(QVector3D) * obj.vertices.length(),obj.vertices.data(), GL_STATIC_DRAW);

    m_funcs->glEnableVertexAttribArray(0);
    m_funcs->glVertexAttribPointer(0,3, GL_FLOAT, GL_FALSE, 0,0);

    // Bind color buffer, fill it with the generated colors
    m_funcs->glBindBuffer(GL_ARRAY_BUFFER, normals[type]);
    m_funcs->glBufferData(GL_ARRAY_BUFFER, sizeof(QVector3D) * obj.normals.length(), obj.normals.data(), GL_STATIC_DRAW);

    m_funcs->glEnableVertexAttribArray(2);
    m_funcs->glVertexAttribPointer(2,3, GL_FLOAT, GL_FALSE, 0,0);

    // Create your Vertex Array Object (VAO) and Vertex Buffer Objects (VBO) here.
    m_funcs->glBindBuffer(GL_ARRAY_BUFFER, uvCoord[type]);
    m_funcs->glBufferData(GL_ARRAY_BUFFER, sizeof(QVector2D) * obj.texcoords.length(), obj.texcoords.data(), GL_STATIC_DRAW);

    m_funcs->glEnableVertexAttribArray(3);
    m_funcs->glVertexAttribPointer(3,2, GL_FLOAT, GL_FALSE, 0,0);
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

    // Initialize Framebuffer
    m_funcs->glGenFramebuffers(1, &frameBuffer);

    frameBufferDraw = new GLenum[1];
    frameBufferDraw[0] = GL_COLOR_ATTACHMENT0;

    // generate texture
    m_funcs->glGenTextures(1, &frameBufferTexture);
    m_funcs->glBindTexture(GL_TEXTURE_2D, frameBufferTexture);
    m_funcs->glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    m_funcs->glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    m_funcs->glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    m_funcs->glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    m_funcs->glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width(), height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

    // Generate depth texture
    m_funcs->glGenTextures(1, &frameBufferDepth);
    m_funcs->glBindTexture(GL_TEXTURE_2D, frameBufferDepth);
    m_funcs->glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    m_funcs->glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    m_funcs->glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    m_funcs->glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    m_funcs->glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, width(), height(), 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);

    m_funcs->glBindFramebuffer(GL_DRAW_FRAMEBUFFER, frameBuffer);
    m_funcs->glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, frameBufferTexture, 0);
    m_funcs->glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, frameBufferDepth, 0);
    m_funcs->glDrawBuffers(1, frameBufferDraw);


    // Initialize the shaders
    m_shaderProgram = new QOpenGLShaderProgram*[SHADER_COUNT];

    // Blur shader
    m_shaderProgram[SHADER_BLUR] = new QOpenGLShaderProgram(this);
    m_shaderProgram[SHADER_BLUR]->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/blur_vertex.glsl");
    m_shaderProgram[SHADER_BLUR]->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/blur_fragment.glsl");
    m_shaderProgram[SHADER_BLUR]->link();

    // Phong shader
    m_shaderProgram[SHADER_DEFAULT] = new QOpenGLShaderProgram(this);
    m_shaderProgram[SHADER_DEFAULT]->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/vertex.glsl");
    m_shaderProgram[SHADER_DEFAULT]->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/fragment.glsl");
    m_shaderProgram[SHADER_DEFAULT]->link();

    // Normal shader for a clear framebuffer
    m_shaderProgram[SHADER_FRAMEBUFFER] = new QOpenGLShaderProgram(this);
    m_shaderProgram[SHADER_FRAMEBUFFER]->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/framebuffer_vertex.glsl");
    m_shaderProgram[SHADER_FRAMEBUFFER]->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/framebuffer_fragment.glsl");
    m_shaderProgram[SHADER_FRAMEBUFFER]->link();

    // Gooch shader
    m_shaderProgram[SHADER_GOOCH] = new QOpenGLShaderProgram(this);
    m_shaderProgram[SHADER_GOOCH]->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/gooch_vertex.glsl");
    m_shaderProgram[SHADER_GOOCH]->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/gooch_fragment.glsl");
    m_shaderProgram[SHADER_GOOCH]->link();

    // Shaders are initialized
    // You can retrieve the locations of the uniforms from here

    // Generate Quad VAO
    m_funcs->glGenVertexArrays(1, &VAOQuad);
    m_funcs->glBindVertexArray(VAOQuad);

    m_funcs->glGenBuffers(1, &verticesQuad);
    m_funcs->glBindBuffer(GL_ARRAY_BUFFER, verticesQuad);

    // Quad
    QVector<QVector3D> sq;
    sq.append(QVector3D(-1.0f,-1.0f,0.0f));
    sq.append(QVector3D(1.0f,1.0f,0.0f));
    sq.append(QVector3D(-1.0f,1.0f,0.0f));

    sq.append(QVector3D(-1.0f,-1.0f,0.0f));
    sq.append(QVector3D(1.0f,-1.0f,0.0f));
    sq.append(QVector3D(1.0f,1.0f,0.0f));

    // Buffer the data to VAO
    m_funcs->glBufferData(GL_ARRAY_BUFFER, sizeof(QVector3D) * sq.length(),sq.data(),GL_STATIC_DRAW);

    m_funcs->glEnableVertexAttribArray(0);
    m_funcs->glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,0,0);

    m_funcs->glBindVertexArray(0);

    QString objLoc[TEX_COUNT] = {":/models/sphere.obj", ":/models/cube.obj",":/Meshes/Spot.obj" , ":/Meshes/Suzanne.obj", ":/Meshes/Fertility.obj"  };
    VAO = new GLuint[VAO_COUNT];
    vertices = new GLuint[VAO_COUNT];
    uvCoord = new GLuint[VAO_COUNT];
    normals = new GLuint[VAO_COUNT];
    vectorsNumber = new int[VAO_COUNT];

#ifdef RAYTRACER
    genVAO(OBJModel(objLoc[VAO_SPHERE]), VAO_SPHERE);
#else

    for (int i = 0; i<VAO_COUNT; i++){
        genVAO(OBJModel(objLoc[i]), i);
    }
#endif
    char* texLoc[TEX_COUNT] = {":/textures/sun.png",":/textures/Small/mercury.png" , ":/textures/Small/venus.png" ,":/textures/earth.png" ,":/textures/Small/mars.png" ,":/textures/Small/jupiter.png" ,":/textures/Small/saturn.png" ,":/textures/Small/uranus.png" ,":/textures/Small/neptune.png" , ":/textures/Small/pluto.png" , ":/textures/Small/moon.png" };
    for (unsigned i = 0; i < TEX_COUNT; i++) {
        readTexture( texLoc[i] ,i);
    }
    m_funcs->glBindTexture(GL_TEXTURE_2D, 0);
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
    if(shaderUsed == SHADER_GOOCH){
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    }else{
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    }
#ifdef ROTATE
    time = startTimer(1000/60);
#endif
}

void MainWindow::renderSphere(QVector3D pos, QVector3D color, QVector4D material, QVector3D lightpos, int tex, float scale, float speed)
{
    normalMatrix = (view*model).normalMatrix();
    QMatrix3x3 NMLine = (view*model).normalMatrix().transposed();
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
    m_shaderProgram[shaderUsed]->setUniformValue("model", currentModel);
    m_shaderProgram[shaderUsed]->setUniformValue("MaterialColor", color);
    m_shaderProgram[shaderUsed]->setUniformValue("intensities", material);
    m_shaderProgram[shaderUsed]->setUniformValue("lightPos", lightpos);
    m_shaderProgram[shaderUsed]->setUniformValue("normalMatrix",normalMatrix);
    // extra uniform for gooch lines
    if(shaderUsed == SHADER_GOOCH) m_shaderProgram[shaderUsed]->setUniformValue("normalMatrixLine",NMLine);
    m_shaderProgram[shaderUsed]->setUniformValue("viewPos", eye);

    // Draw current sphere
    if(tex >= 0){
        m_funcs->glBindTexture(GL_TEXTURE_2D, textures[tex]);
    }
    m_funcs->glDrawArrays(GL_TRIANGLES, 0, vectorsNumber[VAO_SPHERE]);


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
    renderSphere(QVector3D(90,320,100),QVector3D(0,0,1),QVector4D(0.2f,0.7f,0.5f,64),lightpos, EARTH_TEX,1 ,0);

    // Green sphere
    renderSphere(QVector3D(210,270,300),QVector3D(0,1,0),QVector4D(0.2f,0.3f,0.5f,8),lightpos, SUN_TEX, 1, 0);

    // Red sphere
    renderSphere(QVector3D(290,170,150),QVector3D(1,0,0),QVector4D(0.2f,0.7f,0.8f,32),lightpos, MERCURY_TEX, 1, 0);

    // Yellow sphere
    renderSphere(QVector3D(140,220,400),QVector3D(1,0.8f,0),QVector4D(0.2f,0.8f,0.0f,1),lightpos, MOON_TEX, 1, 0);

    // Orange sphere
    renderSphere(QVector3D(110,130,200),QVector3D(1,0.5f,0),QVector4D(0.2f,0.8f,0.5f,32),lightpos, PLUTO_TEX, 1, 0);
#else
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

void MainWindow::renderObj(QVector3D pos, QVector3D color, QVector4D material, QVector3D lightpos, float scale,int type){
    m_funcs->glBindVertexArray(VAO[type]);

    normalMatrix = (view*model).normalMatrix();
    QMatrix3x3 NMLine = (view*model).normalMatrix().transposed();
    QMatrix4x4 currentModel = model;
    currentModel.translate(pos.x(), pos.y(), pos.z());
    currentModel.rotate(-90, QVector3D(1,0,0));
    currentModel.scale(scale);
    m_shaderProgram[shaderUsed]->setUniformValue("model", currentModel);
    m_shaderProgram[shaderUsed]->setUniformValue("MaterialColor", color);
    m_shaderProgram[shaderUsed]->setUniformValue("intensities", material);
    m_shaderProgram[shaderUsed]->setUniformValue("lightPos", lightpos);
    m_shaderProgram[shaderUsed]->setUniformValue("normalMatrix",normalMatrix);
    // extra uniform for gooch lines
    if(shaderUsed == SHADER_GOOCH) m_shaderProgram[shaderUsed]->setUniformValue("normalMatrixLine",NMLine);
    m_shaderProgram[shaderUsed]->setUniformValue("viewPos", eye);

    m_funcs->glDrawArrays(GL_TRIANGLES, 0, vectorsNumber[type]);
}

void MainWindow::renderScene()
{
    QVector3D lightpos = QVector3D(-200,-600,1500);


    renderObj(QVector3D(200,200,200),QVector3D(1,0,1),QVector4D(0.2f,0.7f,0.8f,32),lightpos, 100, VAO_FERTILITY);
    renderObj(QVector3D(320,170,150),QVector3D(1,0,0),QVector4D(0.2f,0.5f,0.33f,2),lightpos, 100, VAO_SUZANNE );
    renderObj(QVector3D(110,130,200),QVector3D(0,1,0),QVector4D(0.2f,0.8f,0.5f,32),lightpos, 100, VAO_SPOT );



}

// The render function, called when an update is requested
void MainWindow::render()
{

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, frameBuffer);
    // glViewport is used for specifying the resolution to render
    // Uses the window size as the resolution
    const qreal retinaScale = devicePixelRatio();
    glViewport(0, 0, width() * retinaScale, height() * retinaScale);

    // Clear the screen at the start of the rendering.
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);


    // Bind the shaderprogram to use it
    m_shaderProgram[shaderUsed]->bind();
    //m_shaderProgram->setUniformValue("model", model);
    m_shaderProgram[shaderUsed]->setUniformValue("view", view);
    m_shaderProgram[shaderUsed]->setUniformValue("projection", projection);
    m_shaderProgram[shaderUsed]->setUniformValue("sampler", 0);
#ifdef RAYTRACER
    m_funcs->glBindVertexArray(VAO[VAO_SPHERE]);

    renderRaytracerScene();
#else
    m_funcs->glBindVertexArray(VAO[VAO_FERTILITY]);
    renderScene();
#endif
    // relases the current shaderprogram (to bind an use another shaderprogram for example)
    m_shaderProgram[shaderUsed]->release();
#ifdef BLUR
    // Use blur shader to generate a blurred image
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    // bind shader
    m_shaderProgram[SHADER_BLUR]->bind();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // set uniforms
    m_shaderProgram[SHADER_BLUR]->setUniformValue("vertical", true);
    m_shaderProgram[SHADER_BLUR]->setUniformValue("sampler", 0);
    m_shaderProgram[SHADER_BLUR]->setUniformValue("depthSampler", 1);

    // bind VAO
    m_funcs->glBindVertexArray(VAOQuad);

    // Activate textures
    m_funcs->glActiveTexture(GL_TEXTURE0);
    m_funcs->glBindTexture(GL_TEXTURE_2D, frameBufferTexture);

    m_funcs->glActiveTexture(GL_TEXTURE1);
    m_funcs->glBindTexture(GL_TEXTURE_2D, frameBufferDepth);

    // draw to screen
    m_funcs->glDrawArrays(GL_TRIANGLES, 0, 6);
    m_funcs->glBindVertexArray(0);
    m_funcs->glActiveTexture(GL_TEXTURE0);
    m_shaderProgram[SHADER_BLUR]->release();
#else
    // Default shader, nothing is done extra
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    m_shaderProgram[SHADER_FRAMEBUFFER]->bind();
    m_shaderProgram[SHADER_FRAMEBUFFER]->setUniformValue("position",0);
    m_shaderProgram[SHADER_FRAMEBUFFER]->setUniformValue("Texcoord",QVector2D(width(),height()));

    //m_funcs->glDisable(GL_DEPTH_TEST);

    m_funcs->glActiveTexture(GL_TEXTURE0);
    m_funcs->glBindTexture(GL_TEXTURE_2D, frameBufferTexture);

    m_funcs->glBindVertexArray(VAOQuad);
    m_funcs->glDrawArrays(GL_TRIANGLES, 0, 6);
    m_funcs->glBindVertexArray(0);
    m_shaderProgram[SHADER_FRAMEBUFFER]->release();
#endif

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
       // qDebug() << ev->key() << "released";
        break;
    }

    renderLater();
}

// Triggered by clicking two subsequent times on any mouse button.
void MainWindow::mouseDoubleClickEvent(QMouseEvent *ev)
{
    //qDebug() << "Mouse double clicked:" << ev->button();

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
        //model.rotate(deltaPos.y(), model.inverted()* QVector3D(1,0,0));
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
   // qDebug() << "Mouse button pressed:" << ev->button();

    mouseClick =  ev->pos();

    renderLater();
}

// Triggered when releasing any mouse button
void MainWindow::mouseReleaseEvent(QMouseEvent *ev)
{
   // qDebug() << "Mouse button released" << ev->button();
    // Set default
    mouseClick = QPoint(0,0);

    renderLater();
}

// Triggered when clicking scrolling with the scroll wheel on the mouse
void MainWindow::wheelEvent(QWheelEvent * ev)
{
   // qDebug() << "Mouse wheel:" << ev->delta();
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

