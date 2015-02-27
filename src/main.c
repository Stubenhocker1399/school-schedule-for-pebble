#include <pebble.h>
#include <time.h>
static BitmapLayer *backg;
static GBitmap *backgb;
static BitmapLayer *overlay_img;
static GBitmap *overlay_bitmap;
Window *m_window;
AppTimer  *overlay_timer_handler;
TextLayer *bar_layer;
static int hourbuf;
static int minbuf;
static int minarea;
static int barlength;
static Layer *s_canvas_layer;
static char time_buf[10];
static TextLayer *time_layer;
static char dayl_buf[10];
static TextLayer *dayl_layer;
static char dayr_buf[10];
static TextLayer *dayr_layer;
static int weeknbr;
static int daynbr;
static char weeknbrchar[5];//="00";
static char daynbrchar[5];//="0";
static char *sinfo_name="Herr Mustermann"; //Yes, I'm german. :P
static char *sinfo_abc="ABC";
static char *sinfo_123="123";
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*

Here comes the configuable part.
You have a list for the names, the class & the room number.
The first 5 places are from monday to friday in even weeks, the 5 places afters are from monday to friday in uneven weeks.

You'll also need to give the starting time and the end time of your school day.
You can do that below the huge data blocks.
You can do it! ;)

static char *info_name[10][4]={  
 {"MON EVEN 1", "MON EVEN 2", "MON EVEN 3", "MON EVEN 4"} ,    initializers for row indexed by 0 MON even
 {"DIE EVEN 1", "DIE EVEN 2", "DIE EVEN 3", "DIE EVEN 4"} ,    initializers for row indexed by 1 DIE even
 {"MIT EVEN 1", "MIT EVEN 2", "MIT EVEN 3", "MIT EVEN 4"} ,    initializers for row indexed by 3 DON even
 {"DON EVEN 1", "DON EVEN 2", "DON EVEN 3", "DON EVEN 4"} ,    initializers for row indexed by 2 MIT even
 {"FRE EVEN 1", "FRE EVEN 2", "FRE EVEN 3", "FRE EVEN 4"} ,    initializers for row indexed by 4 FRE even
 {"MON UNEVEN 1", "MON UNEVEN 2", "MON UNEVEN 3", "MON UNEVEN 4"} ,   initializers for row indexed by 5 MON uneven
 {"DIE UNEVEN 1", "DIE UNEVEN 2", "DIE UNEVEN 3", "DIE UNEVEN 4"} ,    initializers for row indexed by 6 DIE uneven
 {"MIT UNEVEN 1", "MIT UNEVEN 2", "MIT UNEVEN 3", "MIT UNEVEN 4"} ,    initializers for row indexed by 7 MIT uneven
 {"DON UNEVEN 1", "DON UNEVEN 2", "DON UNEVEN 3", "DON UNEVEN 4"} ,    initializers for row indexed by 8 DON uneven
 {"FRE UNEVEN 1", "FRE UNEVEN 2", "FRE UNEVEN 3", "FRE UNEVEN 4"}      initializers for row indexed by 9 FRE uneven
};
*/
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static char *info_name[10][4]={  
 {"Mr. Name", "Mrs. Name", "Mr. AnotherN", "Mrs 2Name"} ,  /*  initializers for row indexed by 0 MON even*/
 {"Classes &", "Names on", "your", "pebble!"} ,  /*  initializers for row indexed by 1 DIE even*/
 {"You need", "to download", "the code", "for settings"} ,  /*  initializers for row indexed by 3 DON even*/
 {"I'm sorry...", "Maybe I can", "change that","in future"} ,  /*  initializers for row indexed by 2 MIT even*/
 {"Have a", "nice day!", " ", " "} ,  /*  initializers for row indexed by 4 FRE even*/
   
 {" ","Sample Teacher", "Try to keep the lenght down, as too long is too long. :P",    ""} ,  /*  initializers for row indexed by 5 MON uneven*/
 {"Thank you", "for checking", "this out!", "<3"} ,  /*  initializers for row indexed by 6 DIE uneven*/
 {" ", "Filling in",  "random stuff","is difficult"} ,  /*  initializers for row indexed by 7 MIT uneven*/
 {"Remember,", "these are", "supp. to be","names."} ,  /*  initializers for row indexed by 8 DON uneven*/
 {"Friday!", "Everybody", "loves", "friday!"}    /*  initializers for row indexed by 9 FRE uneven*/
};
static char *info_abc[10][4]={  
 {"AEI","OU","AEI","OU"} ,  /*  initializers for row indexed by 0 MON even*/
 {"SCH","OOL","CLAS","SES"} ,  /*  initializers for row indexed by 1 DIE even*/
 {"IW","TC", "I","RD"} ,  /*  initializers for row indexed by 3 DON even*/
 {"RA","ND","TE","XT"} ,  /*  initializers for row indexed by 2 MIT even*/
 {"THE","CAKE","IS A ","LIE"} ,  /*  initializers for row indexed by 4 FRE even*/
   
 {"","BEEP", "BOOP", "SON"} ,  /*  initializers for row indexed by 5 MON uneven*/
 {"Beep",  "boop", "beep","boop"} ,  /*  initializers for row indexed by 6 DIE uneven*/
 {" ",  "Just",  "more","fill"} ,  /*  initializers for row indexed by 7 MIT uneven*/
 {"DO",  "RE",  "MI", "FA"} ,  /*  initializers for row indexed by 8 DON uneven*/
 {"SO","LA","TI",  "DO"}    /*  initializers for row indexed by 9 FRE uneven*/
};
static char *info_123[10][4]={  
 {"Room",   "1337", "4815", "1642"} ,  /*  initializers for row indexed by 0 MON even*/
 {"1234", "4321", "1234",   "4321"} ,  /*  initializers for row indexed by 1 DIE even*/
 {"0118", "999", "881", "999"} ,  /*  initializers for row indexed by 3 DON even*/
 {"010", "000", "11", "010"} ,  /*  initializers for row indexed by 2 MIT even*/
 {"010", "00", "010",   "100"} ,  /*  initializers for row indexed by 4 FRE even*/
   
 {"10","010", "010", "01"} ,  /*  initializers for row indexed by 5 MON uneven*/
 {"010", "100", "11", "000"} ,  /*  initializers for row indexed by 6 DIE uneven*/
 {" ",   "010", "10", " "} ,  /*  initializers for row indexed by 7 MIT uneven*/
 {"3.14", "159", "265", "359"} ,  /*  initializers for row indexed by 8 DON uneven*/
 {"42", "42", "42", "42"}    /*  initializers for row indexed by 9 FRE uneven*/
};

//Start hour:
#define SHOUR 8
//Start minute:
#define SMINUTE 0
//End hour:  
#define EHOUR 15
//End minute:
#define EMINUTE 0
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void tick_handler_min(struct tm *tick_time, TimeUnits units_changed){
  strftime(time_buf, sizeof(time_buf), "%k:%M", tick_time);
  
  text_layer_set_text(time_layer, time_buf);  
  
  ////////   BAR Calculation   ///////////
  //write buffers
  hourbuf = (tick_time->tm_hour);
  minbuf = (tick_time->tm_min);
  //calculate barlength
  //start time 8:00
  //end time 15:00
  //tot min = end - start = 15 - 8 = 7h = 7*60 min
  //curr time = (hourbuf*60)+(minbuf)
  //perc curr / tot 
  minarea=(EHOUR-SHOUR)*60+(EMINUTE-SMINUTE);
  
  barlength=((((hourbuf-8)*60)+minbuf)*152/minarea);
 // barlength = (secbuf*152/60)+1; //
  //draw bar
  layer_remove_from_parent(text_layer_get_layer(bar_layer));
  text_layer_destroy(bar_layer);
  bar_layer = text_layer_create(GRect(69,16,  6,barlength));
  text_layer_set_background_color(bar_layer,GColorBlack);
  layer_add_child(window_get_root_layer(m_window), text_layer_get_layer(bar_layer));
}
static void tick_handler_day(struct tm *tick_time, TimeUnits units_changed){
  strftime(dayl_buf, sizeof(dayl_buf), "%a/%V", tick_time);
  strftime(dayr_buf, sizeof(dayr_buf), "%e.%m", tick_time);
  text_layer_set_text(dayl_layer, dayl_buf);
  text_layer_set_text(dayr_layer, dayr_buf);
}
static void general_tick_handler(struct tm *tick_time, TimeUnits units_changed){
  time_t now = time(NULL);
    tick_handler_min(localtime(&now), MINUTE_UNIT);
    tick_handler_day(localtime(&now), DAY_UNIT);
}

static void canvas_update_proc(Layer *this_layer, GContext *ctx) {
  GFont main_font = fonts_get_system_font(FONT_KEY_GOTHIC_14);
  graphics_context_set_text_color(ctx, GColorBlack);
  int i;
  for(i=0;i<8;i=i+1)
  {
   //Calc offset
    int x,y;
    if(i<4)
      {
      x=0;
      y=i*38;
    }
    else
      {
      x=76;
      y=(i-4)*38;
    }
    
    ///////////////////////////////////
    time_t now = time(NULL);
    strftime(daynbrchar,  sizeof(daynbrchar),  "%u",localtime(&now));
    strftime(weeknbrchar, sizeof(weeknbrchar), "%V",localtime(&now));
    //////////////////////////////////
    weeknbr=atoi(weeknbrchar);
    daynbr =atoi(daynbrchar);
    APP_LOG(APP_LOG_LEVEL_DEBUG, "WEEKNBR %s", weeknbrchar);
    APP_LOG(APP_LOG_LEVEL_DEBUG, "DAYNBR %s", daynbrchar);
    int daynbroff=0;
    int hur;
    if (i<4){
      hur=i;
       switch(daynbr){
      case 1:daynbroff=0;break;//Mon
      case 2:daynbroff=1;break;//Die
      case 3:daynbroff=2;break;//Mit
      case 4:daynbroff=3;break;//Don
      case 5:daynbroff=4;break;//Fri
      case 6:daynbroff=4;break;//Sam
      case 7:daynbroff=4;break;//Son
      }
    }
    else{
      hur=i-4;
      switch(daynbr){
      case 1:daynbroff=1;break;//Mon
      case 2:daynbroff=2;break;//Die
      case 3:daynbroff=3;break;//Mit
      case 4:daynbroff=4;break;//Don
      case 5:daynbroff=0;weeknbr=weeknbr+1;break;//Fri next week
      case 6:daynbroff=0;weeknbr=weeknbr+1;break;//Sam next week
      case 7:daynbroff=0;weeknbr=weeknbr+1;break;//Son next week
    }  
  }
  

    if (weeknbr%2==0){
      sinfo_name=info_name[0+daynbroff][hur];
      sinfo_123=info_123[0+daynbroff][hur];
      sinfo_abc=info_abc[0+daynbroff][hur];
    }
    else{ 
      sinfo_name=info_name[5+daynbroff][hur];
      sinfo_123=info_123[5+daynbroff][hur];
      sinfo_abc=info_abc[5+daynbroff][hur];
    }
  //Draw filled textboxes
  graphics_draw_text(ctx,sinfo_name,main_font,GRect(x,y+16+19,68,19), GTextOverflowModeFill, GTextAlignmentCenter, NULL);
  graphics_draw_text(ctx,sinfo_123,main_font,GRect(x+34,y+16,34,19),GTextOverflowModeFill , GTextAlignmentRight , NULL);
  graphics_draw_text(ctx,sinfo_abc,main_font,GRect(x,y+16,34,19), GTextOverflowModeFill , GTextAlignmentLeft , NULL);
  }
}

void  overlay_timer_end(void *data){
  layer_set_hidden((Layer *)overlay_img, true);
}

static void tap_handler(AccelAxisType axis, int32_t direction) {
 APP_LOG(APP_LOG_LEVEL_DEBUG,"uhh~ so shaky! :D");
 layer_set_hidden((Layer *)overlay_img, false); 
  if(overlay_timer_handler!=NULL)
    app_timer_cancel(overlay_timer_handler);
  overlay_timer_handler = app_timer_register(5000, overlay_timer_end, 0);

}

static void m_window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect window_bounds = layer_get_bounds(window_layer);
  // Create Layer
  s_canvas_layer = layer_create(GRect(0, 0, window_bounds.size.w, window_bounds.size.h));
  layer_add_child(window_layer, s_canvas_layer);
    layer_add_child(window_get_root_layer(m_window), bitmap_layer_get_layer(overlay_img));
  // Set the update_proc
  layer_set_update_proc(s_canvas_layer, canvas_update_proc);
}
static void m_window_unload(Window *window) {
  // Destroy Layer
  layer_destroy(s_canvas_layer);
}

void handle_init(void) {
  m_window = window_create();
   window_set_window_handlers(m_window, (WindowHandlers) {
    .load = m_window_load,
    .unload = m_window_unload,
  });
  time_layer = text_layer_create(GRect(0,-1,144,17));
  dayl_layer = text_layer_create(GRect(0,-1,50,17));
  text_layer_set_text_color(dayl_layer, GColorWhite);
  text_layer_set_text_alignment(dayl_layer, GTextAlignmentLeft);
  dayr_layer = text_layer_create(GRect(94,-1,50,17));
  text_layer_set_text_color(dayr_layer, GColorWhite);
  text_layer_set_text_alignment(dayr_layer, GTextAlignmentRight);
  text_layer_set_text_alignment(time_layer, GTextAlignmentCenter);
  text_layer_set_text_color(time_layer, GColorWhite);
  text_layer_set_background_color(time_layer, GColorBlack);
  text_layer_set_background_color(dayl_layer, GColorBlack);
  text_layer_set_background_color(dayr_layer, GColorBlack);
  GFont time_font = fonts_get_system_font(FONT_KEY_GOTHIC_14);
  text_layer_set_font(time_layer, time_font);
  text_layer_set_font(dayl_layer, time_font);
  text_layer_set_font(dayr_layer, time_font);
  overlay_img = bitmap_layer_create(GRect(76,16,68,152));
  overlay_bitmap = gbitmap_create_with_resource(RESOURCE_ID_OVERLAY_IMG);
  bitmap_layer_set_bitmap(overlay_img, overlay_bitmap);
  if(overlay_bitmap==NULL)
    APP_LOG(APP_LOG_LEVEL_DEBUG,"BITMAP == NULL!!!");
  bitmap_layer_set_background_color(overlay_img, GColorWhite);
  bitmap_layer_set_compositing_mode(overlay_img,GCompOpAssign);
  bitmap_layer_set_background_color(overlay_img, GColorBlack);
  backg = bitmap_layer_create(GRect(0,16,144,152));
  backgb = gbitmap_create_with_resource(RESOURCE_ID_IMG_BACKG);
  bitmap_layer_set_bitmap(backg,backgb);
  layer_add_child(window_get_root_layer(m_window),bitmap_layer_get_layer(backg));
  layer_add_child(window_get_root_layer(m_window),text_layer_get_layer(time_layer));
  layer_add_child(window_get_root_layer(m_window),text_layer_get_layer(dayl_layer));
  layer_add_child(window_get_root_layer(m_window),text_layer_get_layer(dayr_layer));
  layer_set_hidden((Layer *)overlay_img, true);
  time_t now = time(NULL);
  general_tick_handler(localtime(&now),MINUTE_UNIT);
  accel_tap_service_subscribe(&tap_handler);
  window_stack_push(m_window, true);
  tick_timer_service_subscribe(MINUTE_UNIT, general_tick_handler);  
}

void handle_deinit(void) {
  text_layer_destroy(bar_layer);
  window_destroy(m_window);
  bitmap_layer_destroy(backg);
  gbitmap_destroy(backgb);
  app_timer_cancel(overlay_timer_handler);
  bitmap_layer_destroy(overlay_img);
}

int main(void) {
  handle_init();
  app_event_loop();
  handle_deinit();
}
