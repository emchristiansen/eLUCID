package org.opencv.samples.tutorial3;

import org.opencv.android.BaseLoaderCallback;
import org.opencv.android.LoaderCallbackInterface;
import org.opencv.android.OpenCVLoader;

import android.app.Activity;
import android.app.AlertDialog;
import android.content.DialogInterface;
import android.os.Bundle;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.view.Window;
import android.view.WindowManager;

public class Sample3Native extends Activity {
    private static final String TAG = "OCVSample::Activity";

    public static final int MENU_NONE = 0;
    public static final int MENU_PLUS = 1;
    public static final int MEAU_MINUS = 2;
    public static final int MENU_RANSAC = 3;
    public static final int MENU_RANK = 4;
    public static final int MENU_BINARY = 5;
    public static final int MENU_ORB = 6;
    public static final int MENU_BRISK = 7;

    private MenuItem mItemPreviewPlus;
    private MenuItem mItemPreviewMinus;
    private MenuItem mItemPreviewRansac;
    private MenuItem mItemPreviewRank;
    private MenuItem mItemPreviewBinary;
    private MenuItem mItemPreviewOrb;
    private MenuItem mItemPreviewBrisk;

    public static int menuItem = MENU_NONE;

    private Sample3View mView;

    public static boolean doRansac = true;

    private BaseLoaderCallback mOpenCVCallBack = new BaseLoaderCallback(this) {
        @Override
        public void onManagerConnected(int status) {
            switch (status) {
            case LoaderCallbackInterface.SUCCESS: {
                Log.i(TAG, "OpenCV loaded successfully");

                // Load native library after(!) OpenCV initialization
                System.loadLibrary("native_sample");

                // Create and set View
                mView = new Sample3View(mAppContext);
                setContentView(mView);
                // Check native OpenCV camera
                if (!mView.openCamera()) {
                    AlertDialog ad = new AlertDialog.Builder(mAppContext)
                            .create();
                    ad.setCancelable(false); // This blocks the 'BACK' button
                    ad.setMessage("Fatal error: can't open camera!");
                    ad.setButton(AlertDialog.BUTTON_POSITIVE, "OK",
                            new DialogInterface.OnClickListener() {
                                public void onClick(DialogInterface dialog,
                                        int which) {
                                    dialog.dismiss();
                                    finish();
                                }
                            });
                    ad.show();
                }
            }
                break;
            /** OpenCV loader cannot start Google Play **/
            case LoaderCallbackInterface.MARKET_ERROR: {
                Log.d(TAG, "Google Play service is not accessible!");
                AlertDialog MarketErrorMessage = new AlertDialog.Builder(
                        mAppContext).create();
                MarketErrorMessage.setTitle("OpenCV Manager");
                MarketErrorMessage
                        .setMessage("Google Play service is not accessible!\nTry to install the 'OpenCV Manager' and the appropriate 'OpenCV binary pack' APKs from OpenCV SDK manually via 'adb install' command.");
                MarketErrorMessage.setCancelable(false); // This blocks the
                                                         // 'BACK' button
                MarketErrorMessage.setButton(AlertDialog.BUTTON_POSITIVE, "OK",
                        new DialogInterface.OnClickListener() {
                            public void onClick(DialogInterface dialog,
                                    int which) {
                                mAppContext.finish();
                            }
                        });
                MarketErrorMessage.show();
            }
                break;
            default: {
                super.onManagerConnected(status);
            }
                break;
            }
        }
    };

    public Sample3Native() {
        Log.i(TAG, "Instantiated new " + this.getClass());
    }

    @Override
    protected void onPause() {
        Log.i(TAG, "called onPause");
        if (null != mView)
            mView.releaseCamera();
        super.onPause();
    }

    @Override
    protected void onResume() {
        Log.i(TAG, "called onResume");
        super.onResume();

        Log.i(TAG, "Trying to load OpenCV library");
        // if (!OpenCVLoader.initAsync(OpenCVLoader.OPENCV_VERSION_2_4_2, this,
        // mOpenCVCallBack)) {
        // Log.e(TAG, "Cannot connect to OpenCV Manager");
        // }
        // OpenCVLoader.initAsync(OpenCVLoader.OPENCV_VERSION_2_4_3, this,
        // mLoaderCallback);
        if (!OpenCVLoader.initDebug()) {
            // TODO: Handle initialization error
        } else {
            // manual call init code since we don't use Manager when call
            // OpenCVLoader.initDebug()
            mOpenCVCallBack.onManagerConnected(LoaderCallbackInterface.SUCCESS);
        }
    }

    /** Called when the activity is first created. */
    @Override
    public void onCreate(Bundle savedInstanceState) {
        Log.i(TAG, "called onCreate");
        super.onCreate(savedInstanceState);
        requestWindowFeature(Window.FEATURE_NO_TITLE);
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        Log.i(TAG, "called onCreateOptionsMenu");
//        mItemPreviewPlus = menu.add("+");
//        mItemPreviewMinus = menu.add("-");
//        mItemPreviewRansac = menu.add("Ransac");
//
//        if (doRansac)
//            mItemPreviewRansac.setTitle("No Rsac");
//        else
//            mItemPreviewRansac.setTitle("Rsac");

        mItemPreviewRank = menu.add("Rank");
        mItemPreviewBinary = menu.add("Binary");
//        mItemPreviewOrb = menu.add("ORB");
//        mItemPreviewBrisk = menu.add("BRISK");

        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        Log.i(TAG, "called onOptionsItemSelected; selected item: " + item);
        if (item == mItemPreviewPlus)
            menuItem = MENU_PLUS;
        else if (item == mItemPreviewMinus)
            menuItem = MEAU_MINUS;
        else if (item == mItemPreviewRansac) {
            menuItem = MENU_RANSAC;
            doRansac = !doRansac;
            if (doRansac)
                mItemPreviewRansac.setTitle("No Rsac");
            else
                mItemPreviewRansac.setTitle("Rsac");
        } else if (item == mItemPreviewRank) {
            menuItem = MENU_RANK;
        } else if (item == mItemPreviewBinary) {
            menuItem = MENU_BINARY;
        } else if (item == mItemPreviewOrb) {
            menuItem = MENU_ORB;
        } else if (item == mItemPreviewBrisk) {
            menuItem = MENU_BRISK;
        } else
            menuItem = MENU_NONE;
        return true;
    }
}
