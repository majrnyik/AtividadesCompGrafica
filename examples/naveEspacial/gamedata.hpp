#ifndef GAMEDATA_HPP_
#define GAMEDATA_HPP_

#include <bitset>

enum class Input { Right, Left, Down, Up };
enum class State { Playing, GameOver, Restart };

struct GameData {
  State m_state{State::Playing};
  std::bitset<4> m_input;  // [up, down, left, right]
};

#endif