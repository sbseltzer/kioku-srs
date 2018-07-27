#include "luautils.h"
#include <stdio.h>

static const char* SETTINGS_FILE = "settings.lua" ;

void print_settings( lua_State* L ) {
  const char* width = 0 ;
  int height = -1 ;
  int fullscreen = -1 ;
  int image_count = -1 ;
  int i = 0 ;

  /* Read resolution settings */
  width = lua_stringexpr( L, "settings.resolution.width", 0 );
  lua_intexpr( L, "settings.resolution.height", &height ) ;
  fullscreen = lua_boolexpr( L, "settings.resolution.fullscreen" );

  /* Print resolution settings */
  printf( "settings.resolution.width = %s\n", width );
  printf( "settings.resolution.height = %d\n", height );
  printf( "settings.resolution.fullscreen = %s\n",fullscreen ? "true":"false" );

  /* Read the number of images */
  lua_intexpr( L, "#settings.images", &image_count ) ;

  /* Print the image file paths. */
  for ( i=0; i<image_count; ++i ) {
    const char* image ;
    char expr[64] = "" ;
    sprintf( expr, "settings.images[%d]", i+1 );
    image = lua_stringexpr( L, expr, 0 );
    printf( "settings.images[%d] = %s\n", i+1, image );
  }
}

int main() {
  lua_State *L = luaL_newstate();
  luaL_openlibs( L );

  if ( luaL_dofile( L, SETTINGS_FILE ) == 1 ) {
    printf( "Error loading %s\n", SETTINGS_FILE );
    return 1 ;
  }

  print_settings( L );

  lua_close(L);
  return 0;
}
