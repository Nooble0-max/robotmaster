本离线运行的回合制机器人对抗程序的核心逻辑与架构由本人完成，开发过程中使用 AI 辅助调试与代码审查，所有代码均经本人理解与验证。经本人检验，确认具有以下功能和元素：
1. 战队与机器人。
2. 战场与行动。
3. 交互与裁决。
4. 结束与记录。
## 编译运行
cl.exe /nologo /EHsc /utf-8 main.cpp robot.cpp Base.cpp Map.cpp Game.cpp \
       Logger.cpp Renderer.cpp Input.cpp Controller.cpp /Fe:main.exe
main.exe

## 操作
move/attack/repair 行 列 | supply | wait | help | quit

## 规则
7×7 棋盘，障碍 5 处、补给区 2 处；基地 200 HP；每回合上限 60 半回合；
补给 +20 HP / +5 弹药；详细规则见 robomaster/design.md
**注意**
对战过程中的行动命令是“命令” + 坐标，且横纵坐标需要空格隔开
