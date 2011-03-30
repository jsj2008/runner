package ua.org.asqz.runner;

import android.content.Context;
import android.opengl.GLSurfaceView;
import android.view.MotionEvent;
import android.util.Log;
import javax.microedition.khronos.egl.EGL10;
import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.egl.EGLContext;
import javax.microedition.khronos.egl.EGLDisplay;

class RunnerSurfaceView extends GLSurfaceView {
   public RunnerSurfaceView(Context ctx) {
      super(ctx);

      setEGLContextFactory(new ContextFactory());
      setRenderer(new RunnerRenderer(ctx));
      setRenderMode(GLSurfaceView.RENDERMODE_CONTINUOUSLY);
   }

   public boolean onTouchEvent(MotionEvent event) {
      int action = event.getAction();
      int nhistory = event.getHistorySize();
      if (action == MotionEvent.ACTION_MOVE) {
         int prevEvent = nhistory - 1;
         long delta_time = event.getEventTime() - event.getHistoricalEventTime(prevEvent);
         if (delta_time > 0) {
            float delta_x = event.getX() - event.getHistoricalX(prevEvent);
            float delta_y = event.getY() - event.getHistoricalY(prevEvent);
            Wrapper.scroll(delta_time, delta_x, delta_y);
            Log.i("RUNNER", "Pointer moved on [" + delta_x + "," + delta_y + " in " + delta_time + "ms");
         }
      }
      return true;
   }

   private static class ContextFactory implements GLSurfaceView.EGLContextFactory {
      final static int EGL_CONTEXT_CLIENT_VERSION = 0x3098;

      public EGLContext createContext(EGL10 egl, EGLDisplay display, EGLConfig eglConfig) {
         int[] attrib_list = { EGL_CONTEXT_CLIENT_VERSION, 2, EGL10.EGL_NONE };
         EGLContext context = egl.eglCreateContext(display, eglConfig, EGL10.EGL_NO_CONTEXT, attrib_list);
         return context;
      }

      public void destroyContext(EGL10 egl, EGLDisplay display, EGLContext context) {
         egl.eglDestroyContext(display, context);
      }
   }
}

