package ua.org.asqz.runner;

import android.app.Activity;
import android.os.Bundle;

public class MainActivity extends Activity {
   @Override
   public void onCreate(Bundle savedInstanceState) {
      super.onCreate(savedInstanceState);
      mView = new RunnerSurfaceView(getApplicationContext());
      setContentView(mView);
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

   private RunnerSurfaceView mView = null;
}

