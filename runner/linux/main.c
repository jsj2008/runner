#include <GL/glew.h>
#include <GL/glut.h>
#include <runner.h>
#include <common.h>
#include "crash_handler.h"
#include <keys.h>
#include <config.h>

static int _width = 800;
static int _height = 600;

void display()
{
   if (update() != 0)
   {
      glutLeaveMainLoop();
   }
   glutSwapBuffers();
}

void reshape(int width, int height)
{
   _width = width;
   _height = height;
   resize(width, height);
}

static int _pressed_button = 0;

void mouse(int button, int state, int x, int y)
{
   ++button;
   switch (state)
   {
      case GLUT_DOWN:
         pointer_down(button, (float)x/(float)_width, (float)y/(float)_height);
         _pressed_button = button;
         break;

      case GLUT_UP:
         pointer_up(button, (float)x/(float)_width, (float)y/(float)_height);
         _pressed_button = 0;
         break;
   }
}

void motion(int x, int y)
{
   pointer_move(_pressed_button, (float)x/(float)_width, (float)y/(float)_height);
}

void idle()
{
   glutPostRedisplay();
}

void window_status(int state)
{
   LOGI("Window status: %d", state);
}

void entry(int state)
{
   LOGI("Entry: %d", state);
}

int map_key(unsigned char key)
{
   switch (key)
   {
      case 27:
         return KEY_BACK;

      default:
         return KEY_NONE;
   }
}

int map_special_key(int key)
{
   switch (key)
   {
      case GLUT_KEY_LEFT:
         return KEY_LEFT;

      case GLUT_KEY_RIGHT:
         return KEY_RIGHT;

      case GLUT_KEY_UP:
         return KEY_UP;

      case GLUT_KEY_DOWN:
         return KEY_DOWN;

      default:
         return KEY_NONE;
   }
}

void keyboard_down(unsigned char key, int x, int y)
{
   key_down(map_key(key));
}

void keyboard_up(unsigned char key, int x, int y)
{
   key_up(map_key(key));
}

void special_key_down(int key, int x, int y)
{
   key_down(map_special_key(key));
}

void special_key_up(int key, int x, int y)
{
   key_up(map_special_key(key));
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
   glutIgnoreKeyRepeat(1);

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
      glutWindowStatusFunc(window_status);
      glutEntryFunc(entry);
      glutKeyboardFunc(keyboard_down);
      glutKeyboardUpFunc(keyboard_up);
      glutSpecialFunc(special_key_down);
      glutSpecialUpFunc(special_key_up);
      restore();
      //glutFullScreen();
      glutMainLoop();
   }
   shutdown();
   return 0;
}

