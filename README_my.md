如果要运行

```
0.01_hello_glfw
0.02_hellotriangle
0.02_hellotriangle2
0.03_hello_triangle_exercise1
0.04_hello_triangle_exercise2
0.05_hello_triangle_exercise3
0.06_shaders_uniform
0.07_shaders_interpolated
0.08_shaders_using_object
0.09_textures_combined
0.10_textures_exercise2
0.11_textures_exercise3
0.12_textures_exercise4
0.13_transformations
0.14_transformations_exercise2
0.15_coordinate_systems
0.16_coordinate_systems_with_depth
0.17_coordinate_systems_multiple_objects
0.18_camera_circle
0.19_camera_keyboard
0.20_camera_keyboard_dt
0.21_camera_mouse
0.22_camera_zoom
0.23_camera_with_class
```

需要将CMakeLists.txt里下面代码注释打开

```
#find_package(GLEW REQUIRED)
#message(STATUS "Found GLEW in ${GLEW_INCLUDE_DIR}")
#find_package(SOIL REQUIRED)
#message(STATUS "Found SOIL in ${SOIL_INCLUDE_DIR}")

#  SET(APPLE_LIBS ${APPLE_LIBS} ${GLEW_LIBRARY} ${SOIL_LIBRARY})
```

然后重新reload cmake project

这些项目是直接使用glew和soil库，没有使用glad。



另外，glew的安装，见

[glew]: !result(/Users/wangdong/github/opengl/LearnOpenGL/src/1.getting_started/readme.md)	"glew的mac安装"

soil的安装，见

[soil]: !result(/Users/wangdong/github/opengl/LearnOpenGL/src/1.getting_started/0.09_textures_combined/readme_install_soil.md)	"soil的mac安装"

