package org.opencv.samples.tutorial3;

import org.opencv.core.Core;
import org.opencv.core.Mat;
import org.opencv.core.Rect;
import org.opencv.imgproc.Imgproc;

import android.content.Context;
import android.graphics.Bitmap;
import android.util.Log;
import android.view.MotionEvent;
import android.view.View;
import android.view.View.OnTouchListener;

class Sample3View extends SampleViewBase implements OnTouchListener{
    private static final String TAG = "OCVSample::View";

    private int                 mFrameSize;
    private Bitmap              mBitmap;
    private int[]               mRGBA;
    private int                mDoInitialize;

    public Sample3View(Context context) {
        super(context);
        setOnTouchListener(this);
        Log.i(TAG, "Instantiated new " + this.getClass());
    }

    @Override
    protected void onPreviewStarted(int previewWidth, int previewHeight) {
        Log.i(TAG, "called onPreviewStarted("+previewWidth+", "+previewHeight+")");

        mFrameSize = previewWidth * previewHeight;
        mRGBA = new int[mFrameSize];
        mBitmap = Bitmap.createBitmap(previewWidth, previewHeight, Bitmap.Config.ARGB_8888);
    }

    @Override
    protected void onPreviewStopped() {
        if(mBitmap != null) {
            mBitmap.recycle();
            mBitmap = null;
        }
        mRGBA = null;
    }


    public boolean onTouch(View v, MotionEvent event) {
    	mDoInitialize = 1;

        return false; // don't need subsequent touch events
    }
    
    @Override
    protected Bitmap processFrame(byte[] data) {
        int[] rgba = mRGBA;

        int do_initialize_before = mDoInitialize;
        FindFeatures(mDoInitialize, Sample3Native.doRansac, getFrameWidth(), getFrameHeight(), Sample3Native.menuItem, data, rgba);
		if ((mDoInitialize > 0) && (do_initialize_before > 0))
			mDoInitialize = 0;

		Sample3Native.menuItem = Sample3Native.MENU_NONE;

        Bitmap bmp = mBitmap;
        bmp.setPixels(rgba, 0/* offset */, getFrameWidth() /* stride */, 0, 0, getFrameWidth(), getFrameHeight());
        return bmp;
    }

    public native void FindFeatures(int doInitialize, boolean do_ransac, int width, int height, int mode, byte yuv[], int[] rgba);
}
