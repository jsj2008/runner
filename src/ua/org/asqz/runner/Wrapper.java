package ua.org.asqz.runner;

class Wrapper {
   public static native int init(String apkPath);
   public static native void resize(int width, int height);
   public static native void update();
   public static native void scroll(long dt, float dx1, float dy1, float dx2, float dy2);

   static {
      System.loadLibrary("wrapper");
   }
}

