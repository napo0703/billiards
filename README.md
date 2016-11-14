# billiards

[OpenGL](https://www.opengl.org)でビリヤード作る

## 実行

- 何らかのCコンパイラと[CMake](https://cmake.org)と[GLUT](https://www.opengl.org/resources/libraries/glut/)が必要です
- [CLion](https://www.jetbrains.com/clion/)が使えるならCLionで開くととても楽です

### Mac/Linux

```
% cmake .
% make
% ./billiards
```

### Windows

1. GUIのCMakeアプリケーションでVisual Studioプロジェクト（.sln）を作る
2. 生成されたSLNファイルをVisual Studioで開いてビルドする