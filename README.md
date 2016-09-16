This is the source code for the "hello world" OpenGL application described in
chapter 5 of Joe Groff's OpenGL tutorial: 

(no chapter available yet...)
http://duriansoftware.com/joe/

This is also helpful:

https://en.wikibooks.org/wiki/OpenGL_Programming


## To install on OSX


### GLUT

On OSX, `GLUT` framework is already available,
located at `/System/Library/Frameworks/GLUT.framework`.

Note that many of the functions are now deprecated by Apple,
and these functions can cause compile warnings, and also
not show up in TAGS file.

Still, `man` pages are available for most of the functions
(i.e. `man glutInit`)

### GLEW

`GLEW` needs to be installed. So do `brew install glew`.
This installs the header files in `/usr/local/include/GL/`

To add a TAGS file for it:
```
cd /usr/local/include/GL/;
sudo ctags -e -R
```


### Building

```
make clean; make; ./flag
```

### Running
```
make; ./flag
```

Click mouse and drag to change camera position, press "r" to reload the shaders.

i.e. `Makefile` is a symlink to `Makefile.MacOSX`



## Note that GL matrices are column-major
i.e. inverted to how they are normally written in maths papers.




## Angle of View

https://en.wikipedia.org/wiki/Angle_of_view

Alpha in [0, pi].

Bigger angle (wider field of view) - further away items appear smaller (ZOOM OUT).

Smaller angle (narrower field of view) - further away items appear less different to nearer items (ZOOM IN).



## Viewing Frustrum

https://en.wikipedia.org/wiki/Viewing_frustum
