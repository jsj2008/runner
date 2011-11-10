package ua.org.asqz.runner;

import android.app.Activity;
import android.app.AlertDialog;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.util.Log;

public class MainActivity extends Activity {
   private static final String TAG = "MainActivity";
   private static final int MESSAGE_ERROR = 1;
   private static final int MESSAGE_QUIT = 2;

   @Override
   public void onCreate(Bundle savedInstanceState) {
      super.onCreate(savedInstanceState);
      mView = new RunnerSurfaceView(this);
      setContentView(mView);

      mView.setHandler(new RunnerSurfaceView.Handler() {
         public void handleError(String error) {
            Log.i(TAG, "ERROR: " + error);
            mHandler.sendMessage(mHandler.obtainMessage(MESSAGE_ERROR, error));
         }
      });
   }

   @Override
   public void onResume() {
      super.onResume();
      mView.onResume();
   }

   @Override
   public void onPause() {
      super.onPause();
      mView.onPause();
   }

   @Override
   public boolean onSearchRequested() {
      return false;
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
}

