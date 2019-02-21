1.从soil网站下载zip

http://www.lonesock.net/soil.html

2.解压；cd Simple OpenGL Image Library/projects/makefile

3.vi makefile

add `-m64` in `CXXFLAGS`  -> `CXXFLAGS = -O2 -s -Wall -m64`

(http://danlec.com/st4k#questions/32599470)

(https://stackoverflow.com/questions/32599470/issues-linking-soil-through-g-in-yosemite)

4.mkdir obj

5.sudo make

6.sudo make install

