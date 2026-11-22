//接下来的内容用于构建程序的前端可视界面，由ds辅助完成
#include "Controller.h"
#include "Game.h"
#include "Renderer.h"
#include "Input.h"
#include "Robot.h"
#include "Base.h"

#include <iostream>

using namespace std;

namespace {


int hpAt(const Game& game, Pos p) {
    const Robot* r = game.map().at(p);
    if (r != nullptr) {
        return r->hp();
    }
    if (p.r == game.redBase().pos().r && p.c == game.redBase().pos().c) {
        return game.redBase().hp();
    }
    if (p.r == game.blueBase().pos().r && p.c == game.blueBase().pos().c) {
        return game.blueBase().hp();
    }
    return 0;
}

// 把"这一手打的是谁"记下来：先查机器人，没有再查基地。
// 注意要在 Game 结算之前记——目标被打死会被移出 Map，之后就查不到了；
// 但 Robot 对象本身还在（Game 拥有它），所以存指针就能一直读到名字和 0 血。
void bindTarget(const Game& game, Pos p, ActionReport& rep) {
    rep.targetRobot = game.map().at(p);
    if (rep.targetRobot != nullptr) {
        return;
    }
    if (p.r == game.redBase().pos().r && p.c == game.redBase().pos().c) {
        rep.targetBase = &game.redBase();
    } else if (p.r == game.blueBase().pos().r && p.c == game.blueBase().pos().c) {
        rep.targetBase = &game.blueBase();
    }
}

}   // namespace

Controller::Controller(Game& game, Renderer& renderer, Input& input)
    : game_(game), renderer_(renderer), input_(input) {}

void Controller::run() {
    while (!game_.over()) {
        if (!playPhase()) {
            cout << "\n  （玩家退出，对局中断）\n";
            return;
        }
    }
    game_.logFinal();         // 新增：终局写记录
    renderer_.draw(game_);
    renderer_.drawResult(game_);
}

bool Controller::playPhase() {
    // 只遍历一遍 robots()：这既是"点名"，也顺手把"一台一次"钉死。
    for (Robot* r : game_.robots()) {
        if (r->team() != game_.currentTeam()) {
            continue;
        }
        if (!r->alive()) {
            continue;          // 本阶段中途阵亡的，跳过（不做遗物行动）
        }
        if (!actOnce(r)) {
            return false;      // 玩家退出
        }
        if (game_.over()) {
            return true;       // 基地没了，本阶段立刻收手，别再让人行动
        }
    }
    game_.endTurn();
    return true;
}

bool Controller::actOnce(Robot* r) {
    renderer_.draw(game_);

    while (true) {
        input_.prompt(*r);
        const Command cmd = Input::parse(input_.readLine());

        switch (cmd.kind) {
        case Command::Kind::Move: {
            // 行动前记下起点，成功后补终点，战报就能写"(1,2) → (4,3)"。
            ActionReport rep;
            rep.kind = ActionReport::Kind::Move;
            rep.actor = r;
            rep.from = game_.map().find(r);
            rep.to = cmd.target;
            if (game_.move(r, cmd.target)) {
                renderer_.reportAction(rep);
                return true;
            }
            break;
        }
        case Command::Kind::Attack: {
            // 先记目标身份和当前 HP，再让 Game 结算；回头读 HP 算差值。
            ActionReport rep;
            rep.kind = ActionReport::Kind::Attack;
            rep.actor = r;
            bindTarget(game_, cmd.target, rep);
            rep.hpBefore = hpAt(game_, cmd.target);
            if (game_.attack(r, cmd.target)) {
                rep.hpAfter = hpAt(game_, cmd.target);
                renderer_.reportAction(rep);
                return true;
            }
            break;
        }
        case Command::Kind::Repair: {
            ActionReport rep;
            rep.kind = ActionReport::Kind::Repair;
            rep.actor = r;
            bindTarget(game_, cmd.target, rep);
            rep.hpBefore = hpAt(game_, cmd.target);
            if (game_.repair(r, cmd.target)) {
                rep.hpAfter = hpAt(game_, cmd.target);
                renderer_.reportAction(rep);
                return true;
            }
            break;
        }
        case Command::Kind::Supply: {
            // 补给改的是"自己"：记下自己的 HP / 弹药，成功后再读一次算差值。
            ActionReport rep;
            rep.kind = ActionReport::Kind::Supply;
            rep.actor = r;
            rep.hpBefore = r->hp();
            rep.ammoBefore = r->ammo();
            if (game_.supply(r)) {
                rep.hpAfter = r->hp();
                rep.ammoAfter = r->ammo();
                renderer_.reportAction(rep);
                return true;
            }
            break;
        }
        case Command::Kind::Wait:
            return true;       // 待机总是合法，用了就算行动过
        case Command::Kind::Quit:
            return false;
        case Command::Kind::Help:
            cout << "  说明: move 行 列  移动（逐格判通行，最多 moveRange 格）\n"
                    "        attack 行 列  攻击（要弹药，可打敌机器人/敌方基地）\n"
                    "        repair 行 列  修理（只有工程兵，修射程内的友军）\n"
                    "        supply        补给（须站在补给区 + 上，回 HP 和弹药）\n"
                    "        wait 待机 / quit 退出 / help 本说明\n"
                    "        坐标先写行 r 再写列 c，左上角是 (0,0)。\n";
            continue;
        case Command::Kind::Invalid:
            cout << "  看不懂这条命令，输入 help 看用法。\n";
            continue;
        }

        // 能走到这里只有一种情况：命令认得出，但 Game 否决了。
        // 行动没有消耗，所以循环回去重新问这台机器人。
        cout << "  Game 否决：这一步不合法（阵营 / 射程 / 地形 / 目标不对），换一个。\n";
    }
}
