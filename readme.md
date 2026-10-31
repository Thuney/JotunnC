We rewriting Jotunn to use C modules bby

Dependencies:

- GLFW ([Link](https://github.com/glfw/glfw))
- GLEW ([Link](https://github.com/nigels-com/glew))
- stb_image ([Link](https://github.com/nothings/stb))
- freetype2 ([Link](https://github.com/aseprite/freetype2))

One of these days I will write some real documentation, but for now, from the root of the repo:

```
cmake -S . -B .build -DCMAKE_BUILD_TYPE=Debug -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
cmake --build .build --parallel
```

The example applications in `ExampleApplications` output to `./ExampleApplications/.build/<App>/<App>`, and VS Code has a task to build and run each one :)
