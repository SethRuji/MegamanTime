#include <pebble.h>
  
static Window *s_main_window;
static TextLayer *s_time_layer;
static GFont s_time_font;

static BitmapLayer *s_background_layer;
static GBitmap *s_background_bitmap;

#define NUM_LEVELS 6
static const int background_image_ids[NUM_LEVELS] = {
																						RESOURCE_ID_IMAGE_BACKGROUND_ELECMAN,
																						RESOURCE_ID_IMAGE_BACKGROUND_GUTSMAN, 
																						RESOURCE_ID_IMAGE_BACKGROUND_FIREMAN, 
																						RESOURCE_ID_IMAGE_BACKGROUND_ICEMAN,
																						RESOURCE_ID_IMAGE_BACKGROUND_CUTMAN, 
																						RESOURCE_ID_IMAGE_BACKGROUND_BOMBMAN
                                            };


static BitmapLayer *s_megaman_layer;
static GBitmap *s_megaman_bitmap;
static const int megaman_image_ids[7] = {																					
																						RESOURCE_ID_IMAGE_MEGAMAN_FIRE,
																						RESOURCE_ID_IMAGE_MEGAMAN_ICE,
																						RESOURCE_ID_IMAGE_MEGAMAN_ELEC,
																						RESOURCE_ID_IMAGE_MEGAMAN_BOMB,
																						RESOURCE_ID_IMAGE_MEGAMAN_CUT,
																						RESOURCE_ID_IMAGE_MEGAMAN_SUPER_ARM,
																						RESOURCE_ID_IMAGE_MEGAMAN
																						};

static BitmapLayer *s_pebble_battery_layer;
static GBitmap *s_pebble_battery_bitmap;
static Layer *s_pebble_battery_cover_layer;

static BitmapLayer *s_phone_battery_layer;
static GBitmap *s_phone_battery_bitmap;
static Layer *s_phone_battery_cover_layer;
static int phone_battery_level = -1;
static int config_show_phone_battery;

static BitmapLayer *s_bt_layer;
static GBitmap *s_bt_bitmaps[2];

static TextLayer *s_date_layer;
static GFont s_date_font;

static int s_time_min;

#define MSG_POWER_UP 2
#define PHONE_BATTERY_ENABLED 3
#define MSG_BATTERY_REQUEST 4
#define MSG_BATTERY_ANSWER 5
#define BOSS_LEVEL 6	
	
#define BATTERY_LEVEL_MAX_PIXELS 65

void battery_handler(BatteryChargeState charge_state)
{
	//tells it to refresh the battery cover when state has changed.
  layer_mark_dirty(s_pebble_battery_cover_layer);
}

static void update_pebble_battery(Layer *layer, GContext *ctx) {
  BatteryChargeState charge_state = battery_state_service_peek();
	
  // Draw a black filled rectangle with sharp corners
  graphics_context_set_fill_color(ctx, GColorBlack);
	float percentage = ((100.0-charge_state.charge_percent)/10.0)* 6.4;
	APP_LOG(APP_LOG_LEVEL_DEBUG, "percentage %d", (int)percentage);
	graphics_fill_rect(ctx, GRect(0, 0, 11, percentage), 0, GCornerNone);
}


static void update_phone_battery(Layer *layer, GContext *ctx) {
	// Draw a black filled rectangle with sharp corners
  graphics_context_set_fill_color(ctx, GColorBlack);
	float percentage=BATTERY_LEVEL_MAX_PIXELS;
	if(config_show_phone_battery==1){
		if(phone_battery_level>=0){
			percentage = (10.0-phone_battery_level)* 6.5;	
		}
	}else{
		//use Minutes remaining in hour as Boss HP
		percentage = ((float)s_time_min/60.0)* 58;
		APP_LOG(APP_LOG_LEVEL_DEBUG, "time min: %d percentage:%d", (int)s_time_min, (int)percentage);
	}
	layer_mark_dirty(s_phone_battery_cover_layer);

	graphics_fill_rect(ctx, GRect(0, 0, 11, percentage), 0, GCornerNone);
}

static void request_phone_battery()
{
	//Send app message to phone to request the battery level
  if (config_show_phone_battery==1)
  {
    DictionaryIterator *iter;
    app_message_outbox_begin(&iter);
    if (config_show_phone_battery==1)
    {
      Tuplet tupleRequest = TupletInteger(MSG_BATTERY_REQUEST, 0);
      dict_write_tuplet(iter, &tupleRequest);
    }
    app_message_outbox_send();
  }
	layer_mark_dirty(s_phone_battery_cover_layer);
}

static void bt_handler(bool connected) {
  // Show current connection state
  if (connected) {
    bitmap_layer_set_bitmap(s_bt_layer, s_bt_bitmaps[0]);
		if(config_show_phone_battery==1){
			request_phone_battery();
		}
  } else {
    bitmap_layer_set_bitmap(s_bt_layer, s_bt_bitmaps[1]);
		phone_battery_level=-1;
  }
	layer_mark_dirty(s_phone_battery_cover_layer);
}

//Generic method to dynamically load bitmaps. The GBitmap and bmp layers should already be created.
//Then pass in a resource ID and Grect(as the location)
static void set_container_image(GBitmap **bmp_image, BitmapLayer *bmp_layer, const int resource_id, GRect rect) {
    GBitmap *old_image = *bmp_image;
    *bmp_image = gbitmap_create_with_resource(resource_id);
    bitmap_layer_set_bitmap(bmp_layer, *bmp_image);
    layer_set_frame(bitmap_layer_get_layer(bmp_layer), rect);
    gbitmap_destroy(old_image);
}

static void main_window_load(Window *window) {
	Layer *window_layer = window_get_root_layer(window);
	 GRect window_bounds = layer_get_bounds(window_layer);
	
	// Create Background Layer
	s_background_layer = bitmap_layer_create(GRect(0, 0, 144, 168));
	bitmap_layer_set_bitmap(s_background_layer, gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BACKGROUND_GUTSMAN));
	layer_add_child(window_layer, bitmap_layer_get_layer(s_background_layer));
	
	// Create Megaman Layer
	s_megaman_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_MEGAMAN);
	s_megaman_layer = bitmap_layer_create(GRect(-13, 51,  window_bounds.size.w,window_bounds.size.h));
	bitmap_layer_set_bitmap(s_megaman_layer, s_megaman_bitmap);
	bitmap_layer_set_compositing_mode(s_megaman_layer, GCompOpSet);
	layer_add_child(window_layer, bitmap_layer_get_layer(s_megaman_layer));
	
	// Create TIME TextLayer
	s_time_layer = text_layer_create(GRect(1, 14, window_bounds.size.w,window_bounds.size.h));
	text_layer_set_background_color(s_time_layer, GColorClear);
	text_layer_set_text_color(s_time_layer, GColorWhite);

  //Set custom font
	s_time_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_MEGAMAN_22));
	text_layer_set_font(s_time_layer, s_time_font);
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);

  // Add TIME layer as a child layer to the Window's root layer
  layer_add_child(window_layer, text_layer_get_layer(s_time_layer));
	
	// Create DATE TextLayer
	s_date_layer = text_layer_create(GRect(-8, 43, window_bounds.size.w,20));
	s_date_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_MEGAMAN_8));
	text_layer_set_background_color(s_date_layer, GColorClear);
	text_layer_set_text_alignment(s_date_layer, GTextAlignmentCenter);
	text_layer_set_font(s_date_layer, s_date_font);
  text_layer_set_text_color(s_date_layer, GColorWhite);
	layer_add_child(window_layer, text_layer_get_layer(s_date_layer));
	
	// Create PEBBLE BATTERY Layer
	s_pebble_battery_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_PEBBLE_BATTERY);
	s_pebble_battery_layer = bitmap_layer_create(GRect(4, 0, 11, 72));
	bitmap_layer_set_bitmap(s_pebble_battery_layer, s_pebble_battery_bitmap);
	layer_add_child(window_layer, bitmap_layer_get_layer(s_pebble_battery_layer));
	
	// Create PEBBLE BATTERY COVER Layer and set the update_proc
  s_pebble_battery_cover_layer = layer_create(GRect(5, 0, 11, BATTERY_LEVEL_MAX_PIXELS));
	layer_add_child(window_layer, s_pebble_battery_cover_layer);
  layer_set_update_proc(s_pebble_battery_cover_layer, update_pebble_battery);
	
	// Create PHONE BATTERY Layer
	s_phone_battery_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_PHONE_BATTERY);
	s_phone_battery_layer = bitmap_layer_create(GRect(129, 0, 11, 72));
	bitmap_layer_set_bitmap(s_phone_battery_layer, s_phone_battery_bitmap);
	layer_add_child(window_layer, bitmap_layer_get_layer(s_phone_battery_layer));
	
	// Create PHONE BATTERY COVER Layer and set the update_proc
  s_phone_battery_cover_layer = layer_create(GRect(129, 0, 11, BATTERY_LEVEL_MAX_PIXELS));
	layer_add_child(window_layer, s_phone_battery_cover_layer);
  layer_set_update_proc(s_phone_battery_cover_layer, update_phone_battery);
	
	// Create BLUETOOTH TextLayer
	s_bt_bitmaps[0] = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BLUETOOTH_CONNECTED);
	s_bt_bitmaps[1] = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BLUETOOTH_DISCONNECTED);
	s_bt_layer = bitmap_layer_create(GRect(101, 38, 16, 15));
	bitmap_layer_set_bitmap(s_bt_layer, s_bt_bitmaps[0]);
	bitmap_layer_set_compositing_mode(s_bt_layer, GCompOpSet);
	layer_add_child(window_layer, bitmap_layer_get_layer(s_bt_layer));
  bt_handler(bluetooth_connection_service_peek());
}

static void main_window_unload(Window *window) {
	// Destroy Background Layer
	bitmap_layer_destroy(s_background_layer);
	gbitmap_destroy(s_background_bitmap);
	
	// Destroy Megaman Layer
	bitmap_layer_destroy(s_megaman_layer);
	gbitmap_destroy(s_megaman_bitmap);
	
  // Destroy Time layer
  text_layer_destroy(s_time_layer);
	
	// Unload GFont
	fonts_unload_custom_font(s_time_font);
	
	// Destroy pebble battery Layer
  bitmap_layer_destroy(s_pebble_battery_layer);
	gbitmap_destroy(s_pebble_battery_bitmap);
	layer_destroy(s_pebble_battery_cover_layer);
	
	
	// Destroy phone battery Layer
  bitmap_layer_destroy(s_phone_battery_layer);
	gbitmap_destroy(s_phone_battery_bitmap);
	layer_destroy(s_phone_battery_cover_layer);
		
	// Destroy bluetooth Layer
  bitmap_layer_destroy(s_bt_layer);
	gbitmap_destroy(s_bt_bitmaps[0]);
	gbitmap_destroy(s_bt_bitmaps[1]);
	
	// Destroy DATE Layer
  text_layer_destroy(s_date_layer);
}

static void update_date(struct tm *time){
		static char date_text[32];
		strftime(date_text, sizeof(date_text), "%a,%b %e", time);	
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
	
	if(tick_time->tm_min ==0){
		//Every hour randomly choose a power up and a boss level
		set_container_image(&s_background_bitmap, s_background_layer, background_image_ids[rand() % NUM_LEVELS], GRect(0, 0, 144, 168));
		set_container_image(&s_megaman_bitmap, s_megaman_layer, megaman_image_ids[rand() % 7], GRect(-13, 51, 144, 168));
	}
	
	if(config_show_phone_battery==0){
	  s_time_min = tick_time->tm_min;
		layer_mark_dirty(s_phone_battery_cover_layer);
	}
	
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
		case MSG_POWER_UP:
			APP_LOG(APP_LOG_LEVEL_INFO, "Case2. Got weapon %d with val: %d", key, value);
			set_container_image(&s_megaman_bitmap, s_megaman_layer, megaman_image_ids[value], GRect(-13, 51, 144, 168));
			break;
		case PHONE_BATTERY_ENABLED:
			APP_LOG(APP_LOG_LEVEL_INFO, "show Phone battery? Got settings key %d with val: %d", key, value);
			config_show_phone_battery = value;
			layer_mark_dirty(s_phone_battery_cover_layer);
			request_phone_battery();
			break;
		case MSG_BATTERY_REQUEST:
			APP_LOG(APP_LOG_LEVEL_INFO, "request battery level. Got key %d with val: %d", key, value);
			layer_mark_dirty(s_phone_battery_cover_layer);
			break;
		case MSG_BATTERY_ANSWER:
			APP_LOG(APP_LOG_LEVEL_INFO, "Phone battery level. Got key %d with val: %d", key, value);
			phone_battery_level = value;
    	layer_mark_dirty(s_phone_battery_cover_layer);
			break;
		case BOSS_LEVEL:
			APP_LOG(APP_LOG_LEVEL_INFO, "Case6. Got boss %d with val: %d", key, value);
			set_container_image(&s_background_bitmap, s_background_layer, background_image_ids[value], GRect(0, 0, 144, 168));
			break;
  }
}

void inbox(DictionaryIterator *iter, void *context){
	//For all received tuples from the phone, send to process_tuple
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
	
	//initially set the time
	update_time();
	
	// Register with TickTimerService, handles actually getting time
	tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
	
	// Subscribe to the Battery State Service
  battery_state_service_subscribe(battery_handler);
	
	// Subscribe to Bluetooth updates
  bluetooth_connection_service_subscribe(bt_handler);
	
	//setup app message handlers
	app_message_register_inbox_received(inbox);
	app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());
	
	request_phone_battery();
}

static void deinit() {
  window_destroy(s_main_window);
	bluetooth_connection_service_unsubscribe();
  battery_state_service_unsubscribe();
  app_message_deregister_callbacks();
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
