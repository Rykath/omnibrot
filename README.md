# How-To: Test deepixel
* Requirements
  * cmake
  * python3 with pypng
* Compile the `deepixel` library with the `mandel` test-program.
  * pull the repository form git and `cd` into it.
  * `mkdir ./build` then `cd build` and `cmake ../deepixel`
  * `make deepixel-mandelbrot`, you should now have a `deepixel-mandelbrot` binary
  * setup the results directory: `cd ..`, `mkdir results`
  * and finally you can execute the script, e.g.: `python3 ./deepixel-test.py`
* You can change all parameters inside the python-script, however if you want to change calculation accuracy you have to
do that in `deepxiel.hdd` and recompile.
* **DO NOT** push your changes back upstream! Do not clutter the repository with build and results...
  * you might want to create these directories somewhere else instead... 
  (you then have to adjust the paths in the python-script)