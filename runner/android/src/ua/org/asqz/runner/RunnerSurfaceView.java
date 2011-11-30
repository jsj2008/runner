package ua.org.asqz.runner;

import android.content.Context;
import android.content.res.AssetManager;
import android.opengl.GLSurfaceView;
import android.view.MotionEvent;
import android.view.SurfaceHolder;
import android.util.Log;
import javax.microedition.khronos.opengles.GL10;
import javax.microedition.khronos.egl.EGL10;
import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.egl.EGLContext;
import javax.microedition.khronos.egl.EGLDisplay;
import java.lang.Math;

class RunnerSurfaceView extends GLSurfaceView {
   private static final String TAG = "RunnerSurfaceView";

   public interface Handler {
      public void handleError(String error);
      public void handleExit();
   }

   public RunnerSurfaceView(Context ctx) {
      super(ctx);

      setEGLContextFactory(mContextFactory);
      setRenderer(mRenderer);
      setRenderMode(GLSurfaceView.RENDERMODE_CONTINUOUSLY);
   }

   public void setHandler(Handler handler) {
      mHandler = handler;
   }

   public boolean onTouchEvent(MotionEvent event) {
      int action = event.getAction();
      int pointerIndex = (action & MotionEvent.ACTION_POINTER_ID_MASK) >> MotionEvent.ACTION_POINTER_ID_SHIFT;
      int actionCode = action & MotionEvent.ACTION_MASK;

      if (actionCode == MotionEvent.ACTION_MOVE) {
         float x = event.getX(pointerIndex) / getWidth();
         float y = event.getY(pointerIndex) / getHeight();
         Wrapper.pointer_move(event.getPointerId(pointerIndex), x, y);
      }
      else if (actionCode == MotionEvent.ACTION_DOWN || actionCode == MotionEvent.ACTION_POINTER_DOWN) {
         float x = event.getX(pointerIndex) / getWidth();
         float y = event.getY(pointerIndex) / getHeight();
         Wrapper.pointer_down(event.getPointerId(pointerIndex), x, y);
      }
      else if (actionCode == MotionEvent.ACTION_UP || actionCode == MotionEvent.ACTION_POINTER_UP) {
         float x = event.getX(pointerIndex) / getWidth();
         float y = event.getY(pointerIndex) / getHeight();
         Wrapper.pointer_up(event.getPointerId(pointerIndex), x, y);
      }

      return true;
   }

   private GLSurfaceView.EGLContextFactory mContextFactory = new GLSurfaceView.EGLContextFactory() {
      final static int EGL_CONTEXT_CLIENT_VERSION = 0x3098;

      public EGLContext createContext(EGL10 egl, EGLDisplay display, EGLConfig eglConfig) {
         int[] attrib_list = { EGL_CONTEXT_CLIENT_VERSION, 2, EGL10.EGL_NONE };
         EGLContext context = egl.eglCreateContext(display, eglConfig, EGL10.EGL_NO_CONTEXT, attrib_list);
         return context;
      }

      public void destroyContext(EGL10 egl, EGLDisplay display, EGLContext context) {
         egl.eglDestroyContext(display, context);
      }
   };

   private GLSurfaceView.Renderer mRenderer = new GLSurfaceView.Renderer() {
      public void onSurfaceCreated(GL10 gl, EGLConfig config) {
         mInitialized = (Wrapper.restore() == 0);
         if (!mInitialized) {
            mHandler.handleError("Unable to restore");
         }
      }

      public void onSurfaceChanged(GL10 gl, int width, int height) {
         if (mInitialized) {
            Wrapper.resize(width, height);
         }
      }

      public void onDrawFrame(GL10 gl) {
         if (mInitialized) {
            if (Wrapper.update() != 0) {
               mInitialized = false;
               mHandler.handleExit();
            }
         }
      }

      private boolean mInitialized = false;
   };

   private Handler mHandler;
}

