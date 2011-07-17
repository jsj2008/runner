package ua.org.asqz.runner;

import android.content.Context;
import android.content.pm.ApplicationInfo;
import android.content.pm.PackageManager.NameNotFoundException;
import android.content.pm.PackageManager;
import android.opengl.GLSurfaceView;
import android.view.MotionEvent;
import android.view.SurfaceHolder;
import android.util.Log;
import javax.microedition.khronos.opengles.GL10;
import javax.microedition.khronos.egl.EGL10;
import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.egl.EGLContext;
import javax.microedition.khronos.egl.EGLDisplay;

class RunnerSurfaceView extends GLSurfaceView {
   private static final String TAG = "RunnerSurfaceView";

   private static final float M_EPSILON = 0.005f;

   public interface Handler {
      public void handleError(String error);
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
         long dt = event.getEventTime() - mPrevEventTime;
         if (dt > 0) {
            float dx1 = event.getX(0) - mPrevPositions[0];
            float dy1 = event.getY(0) - mPrevPositions[1];
            float dx2 = 0.0f;
            float dy2 = 0.0f;

            if (event.getPointerCount() > 1) {
               dx2 = event.getX(1) - mPrevPositions[2];
               dy2 = event.getY(1) - mPrevPositions[3];
            }

            if (dx1 > M_EPSILON || dy1 > M_EPSILON || dx2 > M_EPSILON || dy2 > M_EPSILON) {
               Wrapper.scroll(dt, dx1, dy1, dx2, dy2);
               Log.i(TAG, "Pointer moved on [" + dx1 + "," + dy1 + "] [" + dx2 + "," + dy2 +  "] in " + dt + "ms");
            }
         }
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

      mPrevPositions[0] = event.getX(0);
      mPrevPositions[1] = event.getY(0);
      if (event.getPointerCount() > 1) {
         mPrevPositions[2] = event.getX(1);
         mPrevPositions[3] = event.getY(1);
      }
      else {
         mPrevPositions[2] = 0.0f;
         mPrevPositions[3] = 0.0f;
      }
      mPrevEventTime = event.getEventTime();
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

   @Override
   public void surfaceDestroyed(SurfaceHolder holder) {
      Wrapper.shutdown();
      super.surfaceDestroyed(holder);
   }

   private GLSurfaceView.Renderer mRenderer = new GLSurfaceView.Renderer() {
      public void onSurfaceCreated(GL10 gl, EGLConfig config) {
         PackageManager pm = getContext().getPackageManager();
         try {
            ApplicationInfo info = pm.getApplicationInfo(getContext().getPackageName(), 0);
            mInitialized = (Wrapper.init(info.sourceDir) == 0);
            if (!mInitialized) {
               mHandler.handleError("Unable to initialize");
            }
         }
         catch (NameNotFoundException ex) {
            ex.printStackTrace();
            mHandler.handleError(ex.getMessage());
         }
      }

      public void onSurfaceChanged(GL10 gl, int width, int height) {
         if (mInitialized) {
            Wrapper.resize(width, height);
         }
      }

      public void onDrawFrame(GL10 gl) {
         if (mInitialized) {
            Wrapper.update();
         }
      }

      private boolean mInitialized = false;
   };

   private Handler mHandler;
   private float[] mPrevPositions = new float[4];
   private long mPrevEventTime = 0;
}

