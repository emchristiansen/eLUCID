package org.opencv.samples.tutorial3;

import java.io.IOException;
import java.util.List;
import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.ImageFormat;
import android.graphics.SurfaceTexture;
import android.hardware.Camera;
import android.hardware.Camera.PreviewCallback;
import android.os.Build;
import android.util.Log;
import android.view.SurfaceHolder;
import android.view.SurfaceView;

public abstract class SampleViewBase extends SurfaceView implements SurfaceHolder.Callback, Runnable {
    private static final String TAG = "OCVSample::BaseView";

    private Camera              mCamera;
    private SurfaceHolder       mHolder;
    private int                 mFrameWidth;
    private int                 mFrameHeight;
    private byte[]              mFrame;
    private volatile boolean    mThreadRun;
    private byte[]              mBuffer;
    private SurfaceTexture      mSf;


    public SampleViewBase(Context context) {
        super(context);
        mHolder = getHolder();
        mHolder.addCallback(this);
        Log.i(TAG, "Instantiated new " + this.getClass());
    }

    public int getFrameWidth() {
        return mFrameWidth;
    }

    public int getFrameHeight() {
        return mFrameHeight;
    }

    public void setPreview() throws IOException {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.HONEYCOMB) {
            mSf = new SurfaceTexture(10);
            mCamera.setPreviewTexture( mSf );
        }
        else
            mCamera.setPreviewDisplay(null);
    }

    public boolean openCamera() {
        Log.i(TAG, "Opening Camera");
        mCamera = null;

        try {
            mCamera = Camera.open();
        }
        catch (Exception e){
            Log.e(TAG, "Camera is not available (in use or does not exist): " + e.getLocalizedMessage());
        }

        if(mCamera == null && Build.VERSION.SDK_INT >= Build.VERSION_CODES.GINGERBREAD) {
            for (int camIdx = 0; camIdx < Camera.getNumberOfCameras(); ++camIdx) {
                try {
                    mCamera = Camera.open(camIdx);
                }
                catch (RuntimeException e) {
                    Log.e(TAG, "Camera #" + camIdx + "failed to open: " + e.getLocalizedMessage());
                }
            }
        }

        if(mCamera == null) {
            Log.e(TAG, "Can't open any camera");
            return false;
        }

        mCamera.setPreviewCallbackWithBuffer(new PreviewCallback() {
            public void onPreviewFrame(byte[] data, Camera camera) {
                synchronized (SampleViewBase.this) {
                    System.arraycopy(data, 0, mFrame, 0, data.length);
                    SampleViewBase.this.notify();
                }
                camera.addCallbackBuffer(mBuffer);
            }
        });

        return true;
    }

    public void releaseCamera() {
        Log.i(TAG, "Releasing Camera");
        mThreadRun = false;
        synchronized (this) {
            if (mCamera != null) {
                mCamera.stopPreview();
                mCamera.setPreviewCallback(null);
                mCamera.release();
                mCamera = null;
            }
        }
        onPreviewStopped();
    }

    public synchronized void setupCamera(int width, int height) {
        if (mCamera != null) {
            Log.i(TAG, "Setup Camera - " + width + "x" + height);
            Camera.Parameters params = mCamera.getParameters();
            List<Camera.Size> sizes = params.getSupportedPreviewSizes();
            mFrameWidth = width;
            mFrameHeight = height;

            // selecting optimal camera preview size
            {
                int  minDiff = Integer.MAX_VALUE;
                for (Camera.Size size : sizes) {
                    if (Math.abs(size.height - height) < minDiff) {
                        mFrameWidth = size.width;
                        mFrameHeight = size.height;
                        minDiff = Math.abs(size.height - height);
                    }
                }
            }

            params.setPreviewSize(getFrameWidth(), getFrameHeight());

            List<String> FocusModes = params.getSupportedFocusModes();
            if (FocusModes.contains(Camera.Parameters.FOCUS_MODE_CONTINUOUS_VIDEO))
            {
                params.setFocusMode(Camera.Parameters.FOCUS_MODE_CONTINUOUS_VIDEO);
            }

            mCamera.setParameters(params);

            /* Now allocate the buffer */
            params = mCamera.getParameters();
            int size = params.getPreviewSize().width * params.getPreviewSize().height;
            size  = size * ImageFormat.getBitsPerPixel(params.getPreviewFormat()) / 8;
            mBuffer = new byte[size];
            /* The buffer where the current frame will be copied */
            mFrame = new byte [size];
            mCamera.addCallbackBuffer(mBuffer);

            /* Notify that the preview is about to be started and deliver preview size */
            onPreviewStarted(params.getPreviewSize().width, params.getPreviewSize().height);

            try {
                setPreview();
            } catch (IOException e) {
                Log.e(TAG, "mCamera.setPreviewDisplay/setPreviewTexture fails: " + e);
            }

            /* Now we can start a preview */
            mCamera.startPreview();
        }
    }

    public void surfaceChanged(SurfaceHolder _holder, int format, int width, int height) {
        Log.i(TAG, "called surfaceChanged");
        // stop preview before making changes
        try {
            mCamera.stopPreview();
        } catch (Exception e){
          // ignore: tried to stop a non-existent preview
        }

        // start preview with new settings
        setupCamera(width, height);
    }

    public void surfaceCreated(SurfaceHolder holder) {
        Log.i(TAG, "called surfaceCreated");
        (new Thread(this)).start();
    }

    public void surfaceDestroyed(SurfaceHolder holder) {
        Log.i(TAG, "called surfaceDestroyed");
    }

    /* The bitmap returned by this method shall be owned by the child and released in onPreviewStopped() */
    protected abstract Bitmap processFrame(byte[] data);

    /**
     * This method is called when the preview process is being started. It is called before the first frame delivered and processFrame is called
     * It is called with the width and height parameters of the preview process. It can be used to prepare the data needed during the frame processing.
     * @param previewWidth - the width of the preview frames that will be delivered via processFrame
     * @param previewHeight - the height of the preview frames that will be delivered via processFrame
     */
    protected abstract void onPreviewStarted(int previewWidtd, int previewHeight);

    /**
     * This method is called when preview is stopped. When this method is called the preview stopped and all the processing of frames already completed.
     * If the Bitmap object returned via processFrame is cached - it is a good time to recycle it.
     * Any other resources used during the preview can be released.
     */
    protected abstract void onPreviewStopped();

    public void run() {
        mThreadRun = true;
        Log.i(TAG, "Started processing thread");
        while (mThreadRun) {
            Bitmap bmp = null;

            synchronized (this) {
                try {
                    this.wait();
                    if (!mThreadRun)
                        break;
                    bmp = processFrame(mFrame);
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
            }

            if (bmp != null) {
                Canvas canvas = mHolder.lockCanvas();
                if (canvas != null) {
                    canvas.drawBitmap(bmp, (canvas.getWidth() - getFrameWidth()) / 2, (canvas.getHeight() - getFrameHeight()) / 2, null);
                    mHolder.unlockCanvasAndPost(canvas);
                }
            }
        }
        Log.i(TAG, "Finished processing thread");
    }
}