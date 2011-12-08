package ua.org.asqz.runner;

import android.app.Activity;
import android.app.AlertDialog;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.util.Log;
import android.view.KeyEvent;

public class MainActivity extends Activity {
   private static final String TAG = "MainActivity";
   private static final int MESSAGE_ERROR = 1;
   private static final int MESSAGE_QUIT = 2;

   @Override
   public void onCreate(Bundle savedInstanceState) {
      super.onCreate(savedInstanceState);

      if (Wrapper.init(getAssets()) != 0) {
         mHandler.sendMessage(mHandler.obtainMessage(MESSAGE_ERROR, "Unable to initialize"));
         return;
      }

      mView = new RunnerSurfaceView(this);
      setContentView(mView);

      mView.setHandler(new RunnerSurfaceView.Handler() {
         public void handleError(String error) {
            Log.i(TAG, "ERROR: " + error);
            mHandler.sendMessage(mHandler.obtainMessage(MESSAGE_ERROR, error));
         }
         public void handleExit() {
            Log.i(TAG, "EXIT");
            mHandler.sendMessage(mHandler.obtainMessage(MESSAGE_QUIT));
         }
      });
   }

   @Override
   public void onDestroy() {
      super.onDestroy();
      Wrapper.shutdown();
   }

   @Override
   public boolean onKeyDown(int keyCode, KeyEvent event) {
      Wrapper.key_down(Key.map(keyCode));
      return super.onKeyDown(keyCode, event);
   }

   @Override
   public boolean onKeyUp(int keyCode, KeyEvent event) {
      Wrapper.key_up(Key.map(keyCode));
      return super.onKeyUp(keyCode, event);
   }

   @Override
   public void onResume() {
      super.onResume();
      mView.onResume();

      if (!mActivated) {
         if (mHasFocus) {
            Wrapper.activated();
            mActivated = true;
         }
         else {
            mActivationPending = true;
         }
      }
   }

   @Override
   public void onPause() {
      super.onPause();
      mView.onPause();
      if (mActivated) {
         Wrapper.deactivated();
         mActivated = false;
      }
   }

   @Override
   public void onBackPressed() {
   }

   @Override
   public boolean onSearchRequested() {
      return false;
   }

   @Override
   public void onWindowFocusChanged(boolean hasFocus) {
      mHasFocus = hasFocus;

      if (mHasFocus && mActivationPending) {
         Wrapper.activated();
         mActivated = true;
         mActivationPending = false;
      }
   }

   private Handler mHandler = new Handler() {
      public void handleMessage(Message msg) {
         switch (msg.what) {
         case MESSAGE_ERROR:
            AlertDialog dlg = (new AlertDialog.Builder(MainActivity.this)).create();
            dlg.setTitle("Error");
            dlg.setMessage((String)msg.obj);
            dlg.setButton(AlertDialog.BUTTON_NEGATIVE, "Close", mHandler.obtainMessage(MESSAGE_QUIT));
            dlg.setCancelMessage(mHandler.obtainMessage(MESSAGE_QUIT));
            dlg.show();
            break;
         case MESSAGE_QUIT:
            MainActivity.this.finish();
            break;
         }
      }
   };

   private RunnerSurfaceView mView = null;
   private boolean mHasFocus = false;
   private boolean mActivated = false;
   private boolean mActivationPending = false;
}

