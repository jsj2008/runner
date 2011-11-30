package ua.org.asqz.runner;

import android.content.res.AssetManager;

class Wrapper {
   public static native int init(AssetManager manager);
   public static native int restore();
   public static native int update();
   public static native void shutdown();
   public static native void resize(int width, int height);
   public static native void activated();
   public static native void deactivated();
   public static native void pointer_down(int pointerId, float x, float y);
   public static native void pointer_up(int pointerId, float x, float y);
   public static native void pointer_move(int pointerId, float x, float y);
   public static native void key_down(int key);
   public static native void key_up(int key);

   static {
      System.loadLibrary("wrapper");
   }
}

