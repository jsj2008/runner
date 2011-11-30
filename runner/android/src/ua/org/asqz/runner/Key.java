package ua.org.asqz.runner;

import android.view.KeyEvent;

enum Key {
   None,
   Back,
   Up,
   Down,
   Left,
   Right;

   public static int map(int keyCode) {
      return mapKeyCode(keyCode).ordinal();
   }

   protected static Key mapKeyCode(int keyCode) {
      switch (keyCode) {
      case KeyEvent.KEYCODE_DPAD_UP:
         return Key.Up;

      case KeyEvent.KEYCODE_DPAD_DOWN:
         return Key.Down;

      case KeyEvent.KEYCODE_DPAD_LEFT:
         return Key.Left;

      case KeyEvent.KEYCODE_DPAD_RIGHT:
         return Key.Left;

      case KeyEvent.KEYCODE_BACK:
         return Key.Back;

      default:
         return Key.None;
      }
   }
};

