package ua.org.asqz.runner;

class Wrapper {
   public static native void init(String apkPath);
   public static native void resize(int width, int height);
   public static native void update();
   public static native void scroll(long delta_time, float delta_x, float delta_y);

   static {
      System.loadLibrary("runner");
   }
}

