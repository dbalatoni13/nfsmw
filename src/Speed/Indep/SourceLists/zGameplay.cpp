#include "Speed/Indep/Src/Gameplay/GRuntimeInstance.cpp"
#include "Speed/Indep/Src/Gameplay/GTrigger.cpp"
#include "Speed/Indep/Src/Gameplay/GMarker.cpp"
#include "Speed/Indep/Src/Gameplay/GActivity.cpp"
#include "Speed/Indep/Src/Gameplay/GRaceStatus.cpp"
#include "Speed/Indep/Src/Gameplay/GRaceDatabase.cpp"
#include "Speed/Indep/Src/Gameplay/GCharacter.cpp"
#include "Speed/Indep/Src/Gameplay/GState.cpp"
#include "Speed/Indep/Src/Gameplay/GHandler.cpp"
#include "Speed/Indep/Src/Gameplay/GManager.cpp"
#include "Speed/Indep/Src/Gameplay/GTimer.cpp"
#include "Speed/Indep/Src/Gameplay/GVault.cpp"
#include "Speed/Indep/Src/Gameplay/GObjectBlock.cpp"
#include "Speed/Indep/Src/Gameplay/GInfractionManager.cpp"
#include "Speed/Indep/Src/Gameplay/GMilestone.cpp"
#include "Speed/Indep/Src/Gameplay/GSpeedTrap.cpp"
#include "Speed/Indep/Src/Gameplay/GIcon.cpp"

namespace _STL {
template <class _Dummy> _Rb_tree_node_base* _STLP_CALL
_Rb_global<_Dummy>::_Rebalance_for_erase(_Rb_tree_node_base* __z,
                                         _Rb_tree_node_base*& __root,
                                         _Rb_tree_node_base*& __leftmost,
                                         _Rb_tree_node_base*& __rightmost)
{
  _Rb_tree_node_base* __y = __z;
  _Rb_tree_node_base* __x = 0;
  _Rb_tree_node_base* __x_parent = 0;
  if (__y->_M_left == 0)
    __x = __y->_M_right;
  else
    if (__y->_M_right == 0)
      __x = __y->_M_left;
    else {
      __y = __y->_M_right;
      while (__y->_M_left != 0)
        __y = __y->_M_left;
      __x = __y->_M_right;
    }
  if (__y != __z) {
    __z->_M_left->_M_parent = __y;
    __y->_M_left = __z->_M_left;
    if (__y != __z->_M_right) {
      __x_parent = __y->_M_parent;
      if (__x) __x->_M_parent = __y->_M_parent;
      __y->_M_parent->_M_left = __x;
      __y->_M_right = __z->_M_right;
      __z->_M_right->_M_parent = __y;
    }
    else
      __x_parent = __y;
    if (__root == __z)
      __root = __y;
    else if (__z->_M_parent->_M_left == __z)
      __z->_M_parent->_M_left = __y;
    else
      __z->_M_parent->_M_right = __y;
    __y->_M_parent = __z->_M_parent;
    _STLP_STD::swap(__y->_M_color, __z->_M_color);
    __y = __z;
  }
  else {
    __x_parent = __y->_M_parent;
    if (__x) __x->_M_parent = __y->_M_parent;
    if (__root == __z)
      __root = __x;
    else
      if (__z->_M_parent->_M_left == __z)
        __z->_M_parent->_M_left = __x;
      else
        __z->_M_parent->_M_right = __x;
    if (__leftmost == __z)
      if (__z->_M_right == 0)
        __leftmost = __z->_M_parent;
      else
        __leftmost = _Rb_tree_node_base::_S_minimum(__x);
    if (__rightmost == __z)
      if (__z->_M_left == 0)
        __rightmost = __z->_M_parent;
      else
        __rightmost = _Rb_tree_node_base::_S_maximum(__x);
  }
  if (__y->_M_color != _S_rb_tree_red) {
    while (__x != __root && (__x == 0 || __x->_M_color == _S_rb_tree_black))
      if (__x == __x_parent->_M_left) {
        _Rb_tree_node_base* __w = __x_parent->_M_right;
        if (__w->_M_color == _S_rb_tree_red) {
          __w->_M_color = _S_rb_tree_black;
          __x_parent->_M_color = _S_rb_tree_red;
          _Rotate_left(__x_parent, __root);
          __w = __x_parent->_M_right;
        }
        if ((__w->_M_left == 0 ||
             __w->_M_left->_M_color == _S_rb_tree_black) && (__w->_M_right == 0 ||
             __w->_M_right->_M_color == _S_rb_tree_black)) {
          __w->_M_color = _S_rb_tree_red;
          __x = __x_parent;
          __x_parent = __x_parent->_M_parent;
        } else {
          if (__w->_M_right == 0 ||
              __w->_M_right->_M_color == _S_rb_tree_black) {
            if (__w->_M_left) __w->_M_left->_M_color = _S_rb_tree_black;
            __w->_M_color = _S_rb_tree_red;
            _Rotate_right(__w, __root);
            __w = __x_parent->_M_right;
          }
          __w->_M_color = __x_parent->_M_color;
          __x_parent->_M_color = _S_rb_tree_black;
          if (__w->_M_right) __w->_M_right->_M_color = _S_rb_tree_black;
          _Rotate_left(__x_parent, __root);
          break;
        }
      } else {
        _Rb_tree_node_base* __w = __x_parent->_M_left;
        if (__w->_M_color == _S_rb_tree_red) {
          __w->_M_color = _S_rb_tree_black;
          __x_parent->_M_color = _S_rb_tree_red;
          _Rotate_right(__x_parent, __root);
          __w = __x_parent->_M_left;
        }
        if ((__w->_M_right == 0 ||
             __w->_M_right->_M_color == _S_rb_tree_black) && (__w->_M_left == 0 ||
             __w->_M_left->_M_color == _S_rb_tree_black)) {
          __w->_M_color = _S_rb_tree_red;
          __x = __x_parent;
          __x_parent = __x_parent->_M_parent;
        } else {
          if (__w->_M_left == 0 ||
              __w->_M_left->_M_color == _S_rb_tree_black) {
            if (__w->_M_right) __w->_M_right->_M_color = _S_rb_tree_black;
            __w->_M_color = _S_rb_tree_red;
            _Rotate_left(__w, __root);
            __w = __x_parent->_M_left;
          }
          __w->_M_color = __x_parent->_M_color;
          __x_parent->_M_color = _S_rb_tree_black;
          if (__w->_M_left) __w->_M_left->_M_color = _S_rb_tree_black;
          _Rotate_right(__x_parent, __root);
          break;
        }
      }
    if (__x) __x->_M_color = _S_rb_tree_black;
  }
  return __y;
}

template _Rb_tree_node_base* _Rb_global<bool>::_Rebalance_for_erase(_Rb_tree_node_base*,
                                                                    _Rb_tree_node_base*&,
                                                                    _Rb_tree_node_base*&,
                                                                    _Rb_tree_node_base*&);
}
