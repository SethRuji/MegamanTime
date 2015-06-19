#include <pebble.h>
  
static Window *s_main_window;
static TextLayer *s_time_layer;
static GFont s_time_font;
static BitmapLayer *s_background_layer;
static GBitmap *s_background_bitmaps[2];
static TextLayer *s_peb_battery_layer;
static TextLayer *s_bt_layer;
static TextLayer *s_date_layer;


static void battery_handler(BatteryChargeState new_state) {
  // Write to buffer and display
  static char s_battery_buffer[32];
	if(new_state.is_charging){
		snprintf(s_battery_buffer, sizeof(s_battery_buffer), "charging");
	}else{
		snprintf(s_battery_buffer, sizeof(s_battery_buffer), "%d/100", new_state.charge_percent);
	}
  text_layer_set_text(s_peb_battery_layer, s_battery_buffer);
}

static void bt_handler(bool connected) {
  // Show current connection state
  if (connected) {
    text_layer_set_text(s_bt_layer, "O");
  } else {
    text_layer_set_text(s_bt_layer, "X");
  }
}

static void main_window_load(Window *window) {
	Layer *window_layer = window_get_root_layer(window);
	 GRect window_bounds = layer_get_bounds(window_layer);
	
	// Create GBitmap, then set to created BitmapLayer
	s_background_bitmaps[0] = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_MEGAMAN_JUMP);
	s_background_bitmaps[1] = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BACKGROUND_GUTSMAN);
	s_background_layer = bitmap_layer_create(GRect(0, 0, 144, 168));
	bitmap_layer_set_background_color(s_background_layer, GColorRed);
	bitmap_layer_set_bitmap(s_background_layer, s_background_bitmaps[1]);
	layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(s_background_layer));

	// Create TIME TextLayer
	s_time_layer = text_layer_create(GRect(0, 10, window_bounds.size.w,window_bounds.size.h));
	text_layer_set_background_color(s_time_layer, GColorClear);
	text_layer_set_text_color(s_time_layer, GColorWhite);

  //Set custom font
	s_time_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_MEGAMAN_22));
	text_layer_set_font(s_time_layer, s_time_font);
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);

  // Add TIME layer as a child layer to the Window's root layer
  layer_add_child(window_layer, text_layer_get_layer(s_time_layer));
	
	// Create DATE TextLayer
	s_date_layer = text_layer_create(GRect(0, 30, window_bounds.size.w,20));
	text_layer_set_background_color(s_date_layer, GColorClear);
	text_layer_set_text_alignment(s_date_layer, GTextAlignmentCenter);
	// Add date_layer as a child layer to the Window's root layer
  layer_add_child(window_layer, text_layer_get_layer(s_date_layer));
	text_layer_set_text_color(s_date_layer, GColorWhite);

	
	// Create pebble BATTERY TextLayer
  s_peb_battery_layer = text_layer_create(GRect(30, 0, window_bounds.size.w,window_bounds.size.h));
	text_layer_set_background_color(s_peb_battery_layer, GColorClear);
  text_layer_set_text_alignment(s_peb_battery_layer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(s_peb_battery_layer));
	battery_handler(battery_state_service_peek());  
	
	// Create BLUETOOTH TextLayer
  s_bt_layer = text_layer_create(GRect(-50, 0, window_bounds.size.w, window_bounds.size.h));
  text_layer_set_text_alignment(s_bt_layer, GTextAlignmentCenter);
	text_layer_set_background_color(s_bt_layer, GColorClear);
  layer_add_child(window_layer, text_layer_get_layer(s_bt_layer));
  bt_handler(bluetooth_connection_service_peek());
}

static void main_window_unload(Window *window) {
	// Destroy GBitmaps
	gbitmap_destroy(s_background_bitmaps[0]);
	gbitmap_destroy(s_background_bitmaps[1]);

	// Destroy BitmapLayer
	bitmap_layer_destroy(s_background_layer);
	
  // Destroy TextLayer
  text_layer_destroy(s_time_layer);
	
	// Unload GFont
	fonts_unload_custom_font(s_time_font);
	
	// Destroy pebble battery TextLayer
  text_layer_destroy(s_peb_battery_layer);
	
	// Destroy bluetooth TextLayer
  text_layer_destroy(s_bt_layer);
	
	// Destroy date TextLayer
  text_layer_destroy(s_date_layer);
}

static void update_date(struct tm *time){
		static char date_text[32];
		strftime(date_text, sizeof(date_text), "%a,%B %e", time);	
		//strftime(date_text, sizeof(date_text), "%A\n%B %e", time);
		text_layer_set_text(s_date_layer, date_text);
}
static void update_time() {
  // Get a tm structure
  time_t temp = time(NULL); 
  struct tm *tick_time = localtime(&temp);
	
	update_date(tick_time);
  // Create a long-lived buffer
  static char buffer[] = "00:00";

  // Write the current hours and minutes into the buffer
  if(clock_is_24h_style() == true) {
    // Use 24 hour format
    strftime(buffer, sizeof("00:00"), "%H:%M", tick_time);
  } else {
    // Use 12 hour format
    strftime(buffer, sizeof("00:00"), "%I:%M", tick_time);
  }
	
	//if(tick_time->tm_min % 2 ==0){
		//bitmap_layer_set_bitmap(s_background_layer, s_background_bitmaps[0]);
	//}else{
		//bitmap_layer_set_bitmap(s_background_layer, s_background_bitmaps[1]);
	//}

  // Display this time on the TextLayer
  text_layer_set_text(s_time_layer, buffer);
	
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
	update_time();
}
  

void process_tuple(Tuple *t){
  int key = t->key;
  int value = t->value->int32;
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Got key %d, value %d", key, value);
  switch(key){
    case 0:
      APP_LOG(APP_LOG_LEVEL_INFO, "Got 'hello' message!");
      break;
    case 1:
		 APP_LOG(APP_LOG_LEVEL_INFO, "Got 'testMessage' message!");
      break;
		case 2:
			APP_LOG(APP_LOG_LEVEL_INFO, "Case2. Got settings key %d with val: %d", key, value);
			bitmap_layer_set_bitmap(s_background_layer, s_background_bitmaps[value]);
			break;
		case 3:
			APP_LOG(APP_LOG_LEVEL_INFO, "Case3. Got settings key %d with val: %d", key, value);
			break;
  }
}
 void inbox(DictionaryIterator *iter, void *context){
  Tuple *t = dict_read_first(iter);
  if(t){
    process_tuple(t);
  }
  while(t != NULL){
    t = dict_read_next(iter);
    if(t){
      process_tuple(t);
    }
  }
}



static void init() {
  // Create main Window element and assign to pointer
  s_main_window = window_create();

  // Set handlers to manage the elements inside the Window
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });

  // Show the Window on the watch, with animated=true
  window_stack_push(s_main_window, true);
	
	update_time();
	
	// Register with TickTimerService
	tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
	
	// Subscribe to the Battery State Service
  battery_state_service_subscribe(battery_handler);
	
	// Subscribe to Bluetooth updates
  bluetooth_connection_service_subscribe(bt_handler);
	
	//setup app message handlers
	app_message_register_inbox_received(inbox);
	app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());
}

static void deinit() {
  // Destroy Window
   window_destroy(s_main_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
