
#pragma once

class Game;
class Renderer;
class Input;
class Robot;

// ============================================================
//  Controller：视图层的"手"——把玩家输入翻译成 Game 的调用。
//
//  它只管三件事，一件都不越界：
//   1) 按回合流程点名：当前阵营每台活着的机器人依次问一次；
//   2) 把 Command 变成 game_.move / attack / repair 的调用；
//   3) 把 Game 的否决（返回 false）翻译成人话，让玩家重选。
//
//  模型层故意没做的那条规则——"每台机器人每回合只能行动一次"——
//  在这里补上。做法很土但很稳：一个阶段只把 robots() 遍历一遍，
//  遍历到谁就问谁、问完就走，想"多动"连入口都没有。
//  这样"行动额度"是结构上保证的，不靠一个容易漏掉的计数器。
//
//  注意：合法性判断一行都不许写在这里。move/attack/repair 返回 false
//  就是 Game 说不行，Controller 只负责提示，不负责猜原因。
// ============================================================
class Controller {
public:
    Controller(Game& game, Renderer& renderer, Input& input);

    void run();        // 一直跑到终局，或者玩家 quit / 输入结束

private:
    bool playPhase();          // 当前阵营每台机器人各行动一次；false = 玩家退出
    bool actOnce(Robot* r);    // 让一台机器人行动一次；false = 玩家退出

    Game&     game_;
    Renderer& renderer_;
    Input&    input_;
};
