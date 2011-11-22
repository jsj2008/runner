#include <GL/glew.h>
#include <GL/glut.h>
#include <runner.h>
#include <common.h>
#include "crash_handler.h"
#include <config.h>

static int _width = 800;
static int _height = 600;

void display()
{
   update();
   glutSwapBuffers();
}

void reshape(int width, int height)
{
   _width = width;
   _height = height;
   resize(width, height);
}

void mouse(int button, int state, int x, int y)
{
   switch (state)
   {
      case GLUT_DOWN:
         pointer_down(0, (float)x/(float)_width, (float)y/(float)_height);
         break;

      case GLUT_UP:
         pointer_up(0, (float)x/(float)_width, (float)y/(float)_height);
         break;
   }
}

void motion(int x, int y)
{
   pointer_move(0, (float)x/(float)_width, (float)y/(float)_height);
}

void idle()
{
   glutPostRedisplay();
}

int main(int argc, char** argv)
{
   crash_handler_init();

   LOGI("Runner v%d.%02d", VERSION_MAJOR, VERSION_MINOR);

   glutInit(&argc, argv);
   glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
   glutInitWindowSize(_width, _height);
   glutInitWindowPosition(0, 0);
   glutCreateWindow("runner");

   GLenum res = glewInit();
   if (res != GLEW_OK)
   {
      LOGE("Glew error: %s", glewGetErrorString(res));
      return -1;
   }

   if (!GLEW_VERSION_2_1)
   {
      LOGE("OpenGL 2.1 is not supported");
      return -1;
   }

   if (init(ASSETS_ROOT) == 0)
   {
      glutDisplayFunc(display);
      glutReshapeFunc(reshape);
      glutMouseFunc(mouse);
      glutMotionFunc(motion);
      glutPassiveMotionFunc(motion);
      glutIdleFunc(idle);
      glutMainLoop();
   }
   shutdown();
   return 0;
}

