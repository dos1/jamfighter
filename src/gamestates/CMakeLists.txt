register_gamestate("loading")
register_gamestate("dosowisko")
register_gamestate("select")
register_gamestate("fight")

option(WITH_RUST "build gamestates written in Rust" false)

if(WITH_RUST)
  add_subdirectory("rust")
endif(WITH_RUST)
