#include "Game.h"
#include "Renderer.h"
#include "Input.h"
#include "Controller.h"

#include <iostream>

using namespace std;

// ============================================================
//  main.cpp：组装层——把模型（Game）和视图（Renderer / Input）接起来。
//
//  main 只做四件事：
//   1) 建 Game：规则、场地、机器人全在它里面；
//   2) 建 Renderer / Input：一个负责画，一个负责读；
//   3) 建 Controller：把命令翻译成 Game 的调用，并管"一台一次"；
//   4) 把控制权交给 controller.run()，一直跑到终局或玩家退出。
//
//  这里故意不写规则、也不画棋盘：main 越薄，说明分层越干净。
//  将来换 EasyX，只要换一个 Renderer 的实现、改这里的构造，Game 一行不动。
// ============================================================

int main() {
    Game game;
    Renderer renderer;
    Input input;
    Controller controller(game, renderer, input);

    cout << "\n  RoboMaster 回合制对战 · 控制台版\n"
            "  红方（上方，基地 R） 对 蓝方（下方，基地 B）\n"
            "  目标：摧毁对方基地。每台机器人每回合只能行动一次。\n"
            "  命令：move/attack/repair 行 列 | wait | help | quit\n";

    controller.run();

    // 双击启动时，main 一返回控制台窗口就关了，结算画面会一闪而过。
    // 这里等一次回车再退出；如果输入来自管道 / 文件（已到 EOF），
    // 这一下会立刻返回，不影响脚本化测试。
    cout << "\n  按回车键退出…";
    cout.flush();
    cin.get();
    return 0;
}
