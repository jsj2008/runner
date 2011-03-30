package ua.org.asqz.runner;

import android.content.Context;
import android.content.pm.ApplicationInfo;
import android.content.pm.PackageManager;
import android.content.pm.PackageManager.NameNotFoundException;
import android.opengl.GLSurfaceView;
import javax.microedition.khronos.egl.EGL10;
import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.egl.EGLContext;
import javax.microedition.khronos.egl.EGLDisplay;
import javax.microedition.khronos.opengles.GL10;

class RunnerRenderer implements GLSurfaceView.Renderer {
   public RunnerRenderer(Context ctx) {
      PackageManager pm = ctx.getPackageManager();
      try {
         ApplicationInfo info = pm.getApplicationInfo(ctx.getPackageName(), 0);
         mApkPath = info.sourceDir;
      }
      catch (NameNotFoundException ex) {
         ex.printStackTrace();
      }
   }

   public void onSurfaceCreated(GL10 gl, EGLConfig config) {
      Wrapper.init(mApkPath);
   }

   public void onSurfaceChanged(GL10 gl, int width, int height) {
      Wrapper.resize(width, height);
   }

   public void onDrawFrame(GL10 gl) {
      Wrapper.update();
   }

   private String mApkPath;
}

