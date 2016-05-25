#include "openglwindow.h"
#include "mainwindow.h"

#include <QDebug>

#include <QCoreApplication>

#include <QOpenGLContext>
#include <QOpenGLPaintDevice>
#include <QPainter>

// IMPORTANT!
// You do not need to edit this file in the first couple of weeks!
// Please use mainwindow.cpp for your rendering!

OpenGLWindow::OpenGLWindow(QWindow *parent)
    : QWindow(parent)
    , m_update_pending(false)
    , m_context(0)
{
    setSurfaceType(QWindow::OpenGLSurface);
}

OpenGLWindow::~OpenGLWindow()
{
    delete m_context;
}

void OpenGLWindow::initialize()
{
    qDebug() << "OpenGLWindow::initialize";
}

void OpenGLWindow::render()
{
    // TODO: write implementation in subclass
}

void OpenGLWindow::renderLater()
{
    if (!m_update_pending) {
        m_update_pending = true;
        QCoreApplication::postEvent(this, new QEvent(QEvent::UpdateRequest));
    }
}

bool OpenGLWindow::event(QEvent *event)
{
    switch (event->type()) {
    case QEvent::UpdateRequest:
        m_update_pending = false;
        renderNow();
        return true;
    default:
        return QWindow::event(event);
    }
}

void OpenGLWindow::exposeEvent(QExposeEvent *event)
{
    Q_UNUSED(event);

    if (isExposed())
        renderNow();
}

void OpenGLWindow::renderNow()
{
    if (!isExposed())
        return;

    bool needsInitialize = false;

    // Create an OpenGL context if there is not any
    if (!m_context) {
        m_context = new QOpenGLContext(this);
        m_context->setFormat(requestedFormat());
        m_context->create();

        needsInitialize = true;
    }

    m_context->makeCurrent(this);

    m_funcs = m_context->versionFunctions<QOpenGLFunctions_3_3_Core>();

    if (needsInitialize) {
        initializeOpenGLFunctions();
        initialize();

    }

    render();

    m_context->swapBuffers(this);
}

