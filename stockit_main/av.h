void av_update(){
  for(int i = 0; i < NO_OF_RGB; i++){
    led[i].update();
  }
}

void av_minor_error(int zone){
  led[zone].set_flash(MINOR_ERROR_COLOUR, MINOR_ERROR_DURATION, MINOR_ERROR_NO_FLASHES);
}

void av_major_error(){
  for(int i = 0; i < NO_OF_RGB; i++){
    led[i].set_flash(MAJOR_ERROR_COLOUR, MAJOR_ERROR_DURATION, MAJOR_ERROR_NO_FLASHES);
  }
}

void av_item_placed(int zone){
  led[zone].set_fade_up(ITEM_REMOVED_COLOUR, ITEM_PLACED_DURATION);
}

void av_item_removed(int zone){
  led[zone].set_fade_down(ITEM_REMOVED_COLOUR, ITEM_REMOVED_DURATION);
}

void av_locate_item(int zone){
  led[zone].set_flash(LOCATE_ITEM_COLOUR, LOCATE_ITEM_DURATION, LOCATE_ITEM_NO_FLASHES);
}

void av_sleep(){
  for(int i = 0; i < NO_OF_RGB; i++){
    led[i].sleep();
  }
}

