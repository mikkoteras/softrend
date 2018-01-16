A very brief introduction:

To compile:
  Visual Studio:
    Load the .sln and build. You'll need the SDL2 NuGet -- install it yourself if
    VS fails to do this for you.
  Linux:
    Install SDL2 libs and run make. You may have to adjust the library dir in
    the Makefiles to match your environment. This works on Ubuntu:

    sudo apt install g++ make libsdl2-dev libsdl2-ttf-dev
    cd /wherever/softrend/src
    make
    ../bin/softrend -o /path/to/object.obj

Command line args:

-w <window-width>
-h <window-height>
-r <renderer-threads>
-o <path-to-obj-file>
    automatic freecam scene
-d demo
    demonstration scene
-v
    be more verbose

You must specify one of -o, -d.

Mouse and keyboard controls:

Any mode:

  Ctrl + q: exit
  Ctrl + c: toggle coordinate system display
  Ctrl + i: toggle info text overlay
  Ctrl + n: toggle normal vector overlay
  Ctrl + p: toggle Phong reflection vector overlay
  Ctrl + w: toggle wireframe overlay
  Ctrl + s: rotate shading model (try with the info overlay)

When invoked with -o:

  Ctrl + l: toggle lights (between 100% ambient and 20% ambient + 100% directional)
  Ctrl + r: reset scene
  w,a,s,d: move viewer position around the object
  q,e: tilt viewer position
  f,r: move viewer closer to and further from the object
  Left mouse button + drag: move viewer position around the object
  Mouse scroll wheel: move viewer closer to and further from the object

When invoked with -s demo:

  pageup, pagedown: switch scenes back and forward
  (to run this mode you'll need some object files that aren't available to you for now, sorry)
