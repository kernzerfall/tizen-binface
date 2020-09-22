#include "binface.h"

typedef struct appdata {
	// Tizen stuff
	Evas_Object *win;
	Evas_Object *conform;
	Evas_Object *label;
	// Hours: Number goes up to 24, so we need up to 5 binary digits to display it (1111b=15d and 11111b=31d)
	Evas_Object *hours[5];
	// Minutes: Number goes up to 60, so we need up to 6 binary digits to display it (11111b=31d and 111111b=63d)
	Evas_Object *minutes[6];
	// Seconds: Number goes up to 60, so we need up to 6 binary digits to display it (11111b=31d and 111111b=63d)
	Evas_Object *seconds[6];
} appdata_s;

// Turn on i-th hour indicator
static void hrIndicatorOn(appdata_s *ad, int i){
	evas_object_color_set(ad->hours[i], COLOR_ON);
}
// Turn on i-th minute indicator
static void minIndicatorOn(appdata_s *ad, int i){
	evas_object_color_set(ad->minutes[i], COLOR_ON);
}
// Turn on i-th second indicator
static void secIndicatorOn(appdata_s *ad, int i){
	evas_object_color_set(ad->seconds[i], COLOR_ON);
}
// Turn off i-th hour indicator
static void hrIndicatorOff(appdata_s *ad, int i){
	evas_object_color_set(ad->hours[i], COLOR_OFF);
}
// Turn off i-th minute indicator
static void minIndicatorOff(appdata_s *ad, int i){
	evas_object_color_set(ad->minutes[i], COLOR_OFF);
}
// Turn off i-th second indicator
static void secIndicatorOff(appdata_s *ad, int i){
	evas_object_color_set(ad->seconds[i], COLOR_OFF);
}

static void update_watch(appdata_s *ad, watch_time_h watch_time, int ambient){
	int hour24, minute, second;

	if (watch_time == NULL)
		return;

	// Get time from watch
	watch_time_get_hour24(watch_time, &hour24);
	watch_time_get_minute(watch_time, &minute);
	watch_time_get_second(watch_time, &second);

	// Update layout segments accordingly
	int n=hour24,c,k;
	for(c=4;c>=0;--c){
		k=n>>c;
		if (k&1) hrIndicatorOn(ad, 4-c);
		else     hrIndicatorOff(ad, 4-c);
	}
	n=minute;
	for(c=5;c>=0;--c){
		k=n>>c;
		if (k&1) minIndicatorOn(ad, 5-c);
		else	 minIndicatorOff(ad, 5-c);
	}

	// If !ambient, update/show seconds
	if (!ambient) {
		n=second;
		for(c=5;c>=0;--c){
			k=n>>c;
			if (k&1) secIndicatorOn(ad, 5-c);
			else	 secIndicatorOff(ad, 5-c);
		}
		// If ambient, black out seconds
	}else for(c=0;c<6;++c) secIndicatorOff(ad,c);
}

static void create_base_gui(appdata_s *ad, int width, int height) {
	int ret;
	watch_time_h watch_time = NULL;

	/* Window */
	ret = watch_app_get_elm_win(&ad->win);
	if (ret != APP_ERROR_NONE) {
		dlog_print(DLOG_ERROR, LOG_TAG, "failed to get window. err = %d", ret);
		return;
	}

	evas_object_resize(ad->win, width, height);

	/* Conformant */
	ad->conform = elm_conformant_add(ad->win);
	evas_object_size_hint_weight_set(ad->conform, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	elm_win_resize_object_add(ad->win, ad->conform);
	evas_object_show(ad->conform);

	/*hbox*/
	float unit=48;

	for(int i=0;i<5;++i){
		// Create hour box i
		ad->hours[i] = evas_object_rectangle_add(ad->conform);
		
		// Resize to unit*unit
		evas_object_resize(ad->hours[i], unit, unit);
		
		// Move to predefined position
		evas_object_move(ad->hours[i], (float)width/2.0+((float)i-5.0/2.0)*(float)unit,height/2.0-(float)unit/2.0);
		
		// Uncomment for debugging: make boxes easily distinguishable. Comment out update_watch for this to work.
		//evas_object_color_set(ad->hours[i],255-(50*i),255-(30*i),255-(10*i),255);

		// Make it visible
		evas_object_show(ad->hours[i]);
	}
	for(int i=0;i<6;++i){
		// Create minute box i
		ad->minutes[i] = evas_object_rectangle_add(ad->conform);
		
		// Resize to unit*unit
		evas_object_resize(ad->minutes[i],unit,unit);

		// Move to predefined position
		evas_object_move(ad->minutes[i],(float)width/2.0+((float)i-3.0)*(float)unit,height/3.0-(float)unit/2.0);

		// Uncomment for debugging: make boxes easily distinguishable. Comment out update_watch for this to work.
		//evas_object_color_set(ad->minutes[i],255-(50*i),255-(30*i),255-(10*i),255);

		// Make it visible
		evas_object_show(ad->minutes[i]);
	}
	for(int i=0;i<6;++i){
		// Create second box i
		ad->seconds[i] = evas_object_rectangle_add(ad->conform);

		// Resize to unit*unit
		evas_object_resize(ad->seconds[i],48,48);

		// Move to predefined position
		evas_object_move(ad->seconds[i],(float)width/2.0+((float)i-3.0)*(float)unit,2.0*(float)height/3.0-(float)unit/2.0);

		// Uncomment for debugging: make boxes easily distinguishable. Comment out update_watch for this to work.
		evas_object_color_set(ad->seconds[i],255-(50*i),255-(30*i),255-(10*i),255);

		// Make it visible
		evas_object_show(ad->seconds[i]);
	}

	ret = watch_time_get_current_time(&watch_time);
	if (ret != APP_ERROR_NONE)
		dlog_print(DLOG_ERROR, LOG_TAG, "failed to get current time. err = %d", ret);

	// Call update_watch to eliminate a glitch 
	update_watch(ad, watch_time, 0);
	watch_time_delete(watch_time);

	/* Show window after base gui is set up */
	evas_object_show(ad->win);
}

static bool app_create(int width, int height, void *data){
	/* Hook to take necessary actions before main event loop starts
		Initialize UI resources and application's data
		If this function returns true, the main loop of application starts
		If this function returns false, the application is terminated */
	appdata_s *ad = data;

	create_base_gui(ad, width, height);

	return true;
}

static void app_control(app_control_h app_control, void *data){
	/* Handle the launch request. */
}

static void app_pause(void *data){
	/* Take necessary actions when application becomes invisible. */
}

static void app_resume(void *data){
	/* Take necessary actions when application becomes visible. */
}

static void app_terminate(void *data){
	/* Release all resources. */
}

static void app_time_tick(watch_time_h watch_time, void *data){
	/* Called at each second while your app is visible. Update watch UI. */
	appdata_s *ad = data;
	update_watch(ad, watch_time, 0);
}

static void app_ambient_tick(watch_time_h watch_time, void *data){
	/* Called at each minute while the device is in ambient mode. Update watch UI. */
	appdata_s *ad = data;
	update_watch(ad, watch_time, 1);
}

static void app_ambient_changed(bool ambient_mode, void *data){
	/* Update your watch UI to conform to the ambient mode */
}

static void watch_app_lang_changed(app_event_info_h event_info, void *user_data){
	/*APP_EVENT_LANGUAGE_CHANGED*/
	char *locale = NULL;
	app_event_get_language(event_info, &locale);
	elm_language_set(locale);
	free(locale);
	return;
}

static void watch_app_region_changed(app_event_info_h event_info, void *user_data){
	/*APP_EVENT_REGION_FORMAT_CHANGED*/
}

int main(int argc, char *argv[]){
	appdata_s ad = {0,};
	int ret = 0;

	watch_app_lifecycle_callback_s event_callback = {0,};
	app_event_handler_h handlers[5] = {NULL, };

	event_callback.create = app_create;
	event_callback.terminate = app_terminate;
	event_callback.pause = app_pause;
	event_callback.resume = app_resume;
	event_callback.app_control = app_control;
	event_callback.time_tick = app_time_tick;
	event_callback.ambient_tick = app_ambient_tick;
	event_callback.ambient_changed = app_ambient_changed;

	watch_app_add_event_handler(&handlers[APP_EVENT_LANGUAGE_CHANGED],
		APP_EVENT_LANGUAGE_CHANGED, watch_app_lang_changed, &ad);
	watch_app_add_event_handler(&handlers[APP_EVENT_REGION_FORMAT_CHANGED],
		APP_EVENT_REGION_FORMAT_CHANGED, watch_app_region_changed, &ad);

	ret = watch_app_main(argc, argv, &event_callback, &ad);
	if (ret != APP_ERROR_NONE) {
		dlog_print(DLOG_ERROR, LOG_TAG, "watch_app_main() is failed. err = %d", ret);
	}

	return ret;
}

