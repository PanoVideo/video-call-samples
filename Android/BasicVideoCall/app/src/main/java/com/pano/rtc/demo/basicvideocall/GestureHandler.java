package com.pano.rtc.demo.basicvideocall;

import android.content.Context;
import android.graphics.PointF;
import android.view.MotionEvent;
import android.view.ScaleGestureDetector;
import android.view.VelocityTracker;


public class GestureHandler {
    static final float MAX_CLICK_DISTANCE = 10.0f;
    static final float MAX_DBLCLICK_DISTANCE = 25.0f;
    static final int MAX_CLICK_INTERVAL = 300;
    static final int MAX_DBLCLICK_INTERVAL = 500;

    private PointF downPoint_ = null;
    private PointF lastPoint_ = null; // for calc relative distance when move
    private float maxDistance_ = 0.0f;
    private int pointerId_ = -1;

    private PointF clickPoint_ = null; // for detecting double click
    private long clickTime_ = 0;
    private ActionType action_ = ActionType.Null;
    private int pointerCount_ = 0;
    private int maxPointers_ = 0;

    private VelocityTracker velocityTracker_;

    private ScaleGestureDetector scaleDetector_;

    private Callback cb_;

    public static enum ActionType { Drag, Scroll, Move, Click, RightClick, DoubleClick, End, Null }

    public GestureHandler(Context ctx, Callback cb) {
        cb_ = cb;

        scaleDetector_ = new ScaleGestureDetector(ctx, new ScaleGestureDetector.SimpleOnScaleGestureListener() {
            @Override
            public boolean onScale(ScaleGestureDetector detector) {
                GestureHandler.this.onScale(detector.getScaleFactor(),
                        detector.getFocusX(), detector.getFocusY());
                return true;
            }
        });
    }

    public Boolean handleEvent(MotionEvent event) {
        scaleDetector_.onTouchEvent(event);
        if(scaleDetector_.isInProgress()) {
            return true;
        }
        return handleTouch(event);
    }

    private boolean handleTouch(MotionEvent event) {
        int maskedAction = event.getActionMasked();
        switch (maskedAction) {
            case MotionEvent.ACTION_DOWN: {
                int pointerIndex = event.getActionIndex();
                pointerId_ = event.getPointerId(pointerIndex);
                downPoint_ = new PointF();
                downPoint_.x = event.getX(pointerIndex);
                downPoint_.y = event.getY(pointerIndex);
                lastPoint_ = new PointF(downPoint_.x, downPoint_.y);
                maxDistance_ = 0.0f;
                pointerCount_ = 1;
                maxPointers_ = 1;
                action_ = ActionType.Null;

                if(velocityTracker_ == null) {
                    velocityTracker_ = VelocityTracker.obtain();
                } else {
                    velocityTracker_.clear();
                }
                velocityTracker_.addMovement(event);
                cb_.onBegin(downPoint_.x, downPoint_.y);
                break;
            }
            case MotionEvent.ACTION_POINTER_DOWN: {
                // We have a new pointer. Lets add it to the list of pointers
                ++pointerCount_;
                ++maxPointers_;
                break;
            }
            case MotionEvent.ACTION_MOVE: { // a pointer was moved
                //Log.i(NodeManager.TAG, "ACTION_MOVE, count=" + event.getPointerCount());
                if(lastPoint_ == null || downPoint_ == null) {
                    break;
                }
                int pointerIndex = event.findPointerIndex(pointerId_);
                velocityTracker_.addMovement(event);
                float x = event.getX(pointerIndex);
                float y = event.getY(pointerIndex);
                float dx = x - downPoint_.x;
                float dy = y - downPoint_.y;
                ActionType action = action_;
                if (action_ == ActionType.Null) {
                    // detect action type
                    float d = (float) Math.sqrt(dx * dx + dy * dy);
                    if (d > maxDistance_) {
                        maxDistance_ = d;
                    }
                    if (maxDistance_ >= MAX_CLICK_DISTANCE) {
                        if (pointerCount_ == 2) {
                            action_ = ActionType.Scroll;
                        } else if (pointerCount_ == 3) {
                            action_ = ActionType.Drag;
                        } else {
                            action_ = ActionType.Move;
                        }
                        action = action_;
                    } else {
                        action = ActionType.Move;
                    }
                }
                velocityTracker_.computeCurrentVelocity(1000);
                float vx = velocityTracker_.getXVelocity(pointerId_);
                float vy = velocityTracker_.getYVelocity(pointerId_);
                dx = x - lastPoint_.x;
                dy = y - lastPoint_.y;
                lastPoint_.x = x;
                lastPoint_.y = y;
                if (action == ActionType.Drag) {
                    cb_.onDrag(dx, dy, vx, vy);
                } else if (action == ActionType.Scroll) {
                    cb_.onScroll(dx, dy, vx, vy);
                } else {
                    cb_.onMove(dx, dy, vx, vy);
                }
                break;
            }
            case MotionEvent.ACTION_UP: {
                int pointerIndex = event.getActionIndex();
                float x = event.getX(pointerIndex);
                float y = event.getY(pointerIndex);
                //if(pointerId == pointerId_) {
                ActionType action = ActionType.End;
                long downTime = event.getDownTime();
                long upTime = event.getEventTime();
                //Log.i(NodeManager.TAG, "ACTION_UP, maxDistance_=" + maxDistance_ + ", interval=" + (upTime - downTime));
                if (maxDistance_ < MAX_CLICK_DISTANCE && upTime - downTime < MAX_CLICK_INTERVAL) {
                    action = ActionType.Click; // click
                    //Log.i(NodeManager.TAG, "click, interval=" + (downTime - clickTime_));
                    if(maxPointers_ == 2) {
                        //Log.i(NodeManager.TAG, "right click");
                        action = ActionType.RightClick;
                    } else if (clickPoint_ != null && downTime - clickTime_ < MAX_DBLCLICK_INTERVAL) {
                        float dx = downPoint_.x - clickPoint_.x;
                        float dy = downPoint_.y - clickPoint_.y;
                        float d = (float) Math.sqrt(dx * dx + dy * dy);
                        //Log.i(NodeManager.TAG, "click, distance=" + d);
                        if (d < MAX_DBLCLICK_DISTANCE) {
                            //Log.i(NodeManager.TAG, "double click");
                            action = ActionType.DoubleClick;
                        }
                    }
                    if (action == ActionType.Click) {
                        clickPoint_ = new PointF(x, y);
                        clickTime_ = upTime;
                    } else {
                        clickPoint_ = null;
                        clickTime_ = 0;
                    }
                } else {
                    clickPoint_ = null;
                    clickTime_ = 0;
                }
                if (action == ActionType.RightClick) {
                    cb_.onRightClicked(x, y);
                } else if (action == ActionType.DoubleClick) {
                    cb_.onDoubleClicked(x, y);
                } else if (action == ActionType.Click) {
                    cb_.onClicked(x, y);
                } else {
                    cb_.onEnd();
                }
                clearMouseAction();
                //}
                break;
            }
            case MotionEvent.ACTION_POINTER_UP: {
                --pointerCount_;
                int pointerIndex = event.getActionIndex();
                int pointerId = event.getPointerId(pointerIndex);
                if(pointerId == pointerId_) {
                    int newPointerIndex = pointerIndex == 0 ? 1 : 0;
                    lastPoint_ = new PointF(event.getX(newPointerIndex),
                            event.getY(newPointerIndex));
                    pointerId_ = event.getPointerId(newPointerIndex);
                }
                break;
            }
            case MotionEvent.ACTION_CANCEL: {
                cb_.onEnd();
                break;
            }
        }
        return true;
    }

    private void onScale(float scaleFactor, float focusX, float focusY) {
        cb_.onScale(scaleFactor, focusX, focusY);
    }

    private void clearMouseAction() {
        downPoint_ = null;
        lastPoint_ = null;
        pointerId_ = -1;
        maxDistance_ = 0.0f;
        action_ = ActionType.Null;
        pointerCount_ = 0;
        maxPointers_ = 0;
        if (velocityTracker_ != null) {
            velocityTracker_.recycle();
            velocityTracker_ = null;
        }
    }

    public interface Callback {
        public void onBegin(float x, float y);
        public void onClicked(float x, float y);
        public void onRightClicked(float x, float y);
        public void onDoubleClicked(float x, float y);
        public void onMove(float dx, float dy, float vx, float vy);
        public void onDrag(float dx, float dy, float vx, float vy);
        public void onScroll(float dx, float dy, float vx, float vy);
        public void onScale(float factor, float focusX, float focusY);
        public void onEnd();
    }
}
