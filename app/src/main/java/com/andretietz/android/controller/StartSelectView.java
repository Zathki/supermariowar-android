package com.andretietz.android.controller;

import android.annotation.TargetApi;
import android.content.Context;
import android.content.res.TypedArray;
import android.graphics.drawable.Drawable;
import android.os.Build;
import android.util.AttributeSet;

import ca.thekidd.supermariowar.R;


/**
 * This view can be used for action inputs. it has 4 Button, as it is usual
 * for common game controllers such as PS,XBox and similar ones
 */
public class StartSelectView extends InputView {

	private static final int BUTTON_COUNT = 2;
	private Drawable[][] drawables = new Drawable[BUTTON_COUNT][2];
	private int[][] resources = new int[BUTTON_COUNT][2];

	public StartSelectView(Context context) {
		super(context);
	}

	public StartSelectView(Context context, AttributeSet attrs) {
		super(context, attrs);
		init(context, attrs);
	}

	public StartSelectView(Context context, AttributeSet attrs, int defStyleAttr) {
		super(context, attrs, defStyleAttr);
		init(context, attrs);
	}

	@TargetApi(Build.VERSION_CODES.LOLLIPOP) public StartSelectView(Context context, AttributeSet attrs, int defStyleAttr, int defStyleRes) {
		super(context, attrs, defStyleAttr, defStyleRes);
		init(context, attrs);
	}

	protected void init(Context context, AttributeSet attrs) {
		// read the attributes from xml
		TypedArray a = context.getTheme().obtainStyledAttributes(attrs, R.styleable.ActionView, 0, R.style.default_startselectview);
		super.init(context, attrs, R.style.default_startselectview);
		try {
			resources[0][0] = a.getResourceId(R.styleable.StartSelectView_startselect1_enabled, R.drawable.start_select_usual);
			resources[0][1] = a.getResourceId(R.styleable.StartSelectView_startselect1_pressed, R.drawable.start_select_pressed);
			resources[1][0] = a.getResourceId(R.styleable.StartSelectView_startselect2_enabled, R.drawable.start_select_usual);
			resources[1][1] = a.getResourceId(R.styleable.StartSelectView_startselect2_pressed, R.drawable.start_select_pressed);
		} finally {
			a.recycle();
		}
	}

	@Override protected Drawable getStateDrawable(int buttonIndex, ButtonState state) {
		if (null == drawables[buttonIndex][state.ordinal()]) {
			drawables[buttonIndex][state.ordinal()] = getContext().getDrawable(resources[buttonIndex][state.ordinal()]);
		}
		return drawables[buttonIndex][state.ordinal()];
	}

	@Override protected int getButtonCount() {
		return BUTTON_COUNT;
	}
}
