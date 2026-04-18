cmake_minimum_required(VERSION 4.0 FATAL_ERROR)

project(
  moonchild
  LANGUAGES C CXX
)

find_package(SDL3 REQUIRED CONFIG REQUIRED COMPONENTS SDL3)
find_package(ZLIB REQUIRED)

add_executable(moonchild)

target_compile_features(moonchild PRIVATE c_std_99)
target_link_libraries(moonchild PRIVATE
  SDL3::SDL3
  ZLIB::ZLIB
)

target_include_directories(moonchild PRIVATE
  Classes/framewrk
  Classes/moonchild
)
target_sources(moonchild PRIVATE
  main_sdl.cpp

  # Classes/framewrk/audio.mm
  Classes/framewrk/Blitbuf.cpp
  Classes/framewrk/Fastfile.cpp
  # Classes/framewrk/movie.mm
  Classes/framewrk/Pcxff.cpp
  Classes/framewrk/Sprite.cpp
  Classes/framewrk/timer.cpp
  Classes/framewrk/unzip.c
  Classes/framewrk/video.cpp
  # Classes/framewrk/windows.cpp

  Classes/moonchild/Anim.cpp
  Classes/moonchild/asset.cpp
  Classes/moonchild/Asteroid.cpp
  Classes/moonchild/basic.cpp
  Classes/moonchild/Bat.cpp
  Classes/moonchild/Bbot.cpp
  Classes/moonchild/Bee.cpp
  Classes/moonchild/bolt.cpp
  Classes/moonchild/bonus.cpp
  Classes/moonchild/boss.cpp
  Classes/moonchild/bouncey.cpp
  Classes/moonchild/bullet.cpp
  Classes/moonchild/Bump.cpp
  Classes/moonchild/cannon.cpp
  Classes/moonchild/Chain.cpp
  Classes/moonchild/Claw.cpp
  Classes/moonchild/cloud.cpp
  Classes/moonchild/diamond.cpp
  Classes/moonchild/Doldoler.cpp
  Classes/moonchild/editor.cpp
  Classes/moonchild/Element.cpp
  Classes/moonchild/elevat.cpp
  Classes/moonchild/Glim.cpp
  Classes/moonchild/globals.cpp
  Classes/moonchild/goodday.cpp
  Classes/moonchild/gravlift.cpp
  Classes/moonchild/gumbal.cpp
  Classes/moonchild/hoi.cpp
  Classes/moonchild/Ironauto.cpp
  Classes/moonchild/ironring.cpp
  Classes/moonchild/levinits.cpp
  Classes/moonchild/Lift.cpp
  Classes/moonchild/mc.cpp
  Classes/moonchild/metbal.cpp
  Classes/moonchild/metcan.cpp
  Classes/moonchild/mine.cpp
  Classes/moonchild/Mixer.cpp
  Classes/moonchild/mouth.cpp
  Classes/moonchild/mushroom.cpp
  Classes/moonchild/objects.cpp
  Classes/moonchild/Paal.cpp
  Classes/moonchild/plant.cpp
  Classes/moonchild/Plof.cpp
  Classes/moonchild/PREFS.CPP
  Classes/moonchild/Ptoei.cpp
  Classes/moonchild/Punt.cpp
  Classes/moonchild/robyn.cpp
  Classes/moonchild/rock.cpp
  Classes/moonchild/rocket.cpp
  Classes/moonchild/score.cpp
  Classes/moonchild/scroll.cpp
  Classes/moonchild/seamine.cpp
  Classes/moonchild/smlheart.cpp
  Classes/moonchild/snake.cpp
  Classes/moonchild/Sneak.cpp
  Classes/moonchild/sokoban.cpp
  Classes/moonchild/sound.cpp
  Classes/moonchild/spies.cpp
  Classes/moonchild/Spike.cpp
  Classes/moonchild/Stukhout.cpp
  Classes/moonchild/Switsj.cpp
  Classes/moonchild/test.cpp
  Classes/moonchild/trigger.cpp
  Classes/moonchild/Vgdll.cpp
  Classes/moonchild/warp.cpp
  Classes/moonchild/Weight.cpp
  Classes/moonchild/wheel.cpp
)
