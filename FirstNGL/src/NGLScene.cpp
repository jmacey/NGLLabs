#include <QMouseEvent>
#include <QGuiApplication>

#include "NGLScene.h"
#include <ngl/ShaderLib.h>
#include <ngl/NGLStream.h>
#include <ngl/NGLInit.h>
#include <ngl/Util.h>
#include <ngl/VAOPrimitives.h>
#include <iostream>

NGLScene::NGLScene()
{
  // re-size the widget to that of the parent (in this case the GLFrame passed in on construction)
  setTitle("First NGL");
}


NGLScene::~NGLScene()
{
  std::cout<<"Shutting down NGL, removing VAO's and Shaders\n";
}


void NGLScene::loadMatrixToShader(ngl::Transformation& _tx)
{
    auto shader = ngl::ShaderLib::instance();
    shader->setUniform("MVP", m_project*m_view*_tx.getMatrix());

}
void NGLScene::resizeGL(int _w , int _h)
{
  m_win.width  = static_cast<int>( _w * devicePixelRatio() );
  m_win.height = static_cast<int>( _h * devicePixelRatio() );
  m_project = ngl::perspective(45.0f, static_cast<float>(_w) / _h, 0.5f, 5.0f);

}


void NGLScene::initializeGL()
{
  // we need to initialise the NGL lib which will load all of the OpenGL functions, this must
  // be done once we have a valid GL context but before we call any GL commands. If we dont do
  // this everything will crash
  ngl::NGLInit::instance();
  glClearColor(0.6f, 0.6f, 0.6f, 1.0f);			   // Grey Background
  // enable depth testing for drawing
  glEnable(GL_DEPTH_TEST);
  // enable multisampling for smoother drawing
  glEnable(GL_MULTISAMPLE);

  auto shader = ngl::ShaderLib::instance();
  shader->loadShader("ColourShader", "shaders/VertexShader.glsl",
      "shaders/FragmentShader.glsl");
  ngl::Vec3 eye(2.0f, 2.0f, 2.0f);
  m_view = ngl::lookAt(eye, { 0.0f,0.0f,0.0f }, ngl::Vec3::up());
  std::cout << m_view << '\n';
}



void NGLScene::paintGL()
{
  // clear the screen and depth buffer
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glViewport(0,0,m_win.width,m_win.height);
  auto shader = ngl::ShaderLib::instance();
  shader->use("ColourShader");
  shader->setUniform("colour", 1.0f, 0.0f, 0.0f );
  ngl::Transformation tx;
  loadMatrixToShader(tx);     
  ngl::VAOPrimitives::instance()->draw(ngl::teapot);

  shader->setUniform("colour", 0.0f, 1.0f, 0.0f);
  tx.reset();
  tx.setScale(0.1f, 0.1f, 0.1f);
  tx.setPosition(-1.0f, 0.0f, 0.0f);
  tx.setRotation(45, 12, 56);
  loadMatrixToShader(tx);
  ngl::VAOPrimitives::instance()->draw("bunny");


}

//----------------------------------------------------------------------------------------------------------------------

void NGLScene::keyPressEvent(QKeyEvent *_event)
{
  // this method is called every time the main window recives a key event.
  // we then switch on the key value and set the camera in the GLWindow
  switch (_event->key())
  {
  // escape key to quite
  case Qt::Key_Escape : QGuiApplication::exit(EXIT_SUCCESS); break;
  case Qt::Key_Space :
      m_win.spinXFace=0;
      m_win.spinYFace=0;
      m_modelPos.set(ngl::Vec3::zero());

      break;
  case Qt::Key_W: glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); break;
  case Qt::Key_S: glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); break;

  default : break;
  }
  // finally update the GLWindow and re-draw

    update();
}
