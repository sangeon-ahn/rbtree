#include "rbtree.h"
#include <stdio.h>
#include <stdlib.h>
color_t black = RBTREE_BLACK;
color_t red = RBTREE_RED;

rbtree *new_rbtree(void) {
  rbtree *p = (rbtree *)calloc(1, sizeof(rbtree));
  // TODO: initialize struct if needed
  p->nil = (node_t*)malloc(sizeof(node_t));
  p->nil->color = black;
  p->root = p->nil;
  return p;
}

void delete_all_node(rbtree* t, node_t* root) {
  if (root != t->nil) {
    delete_all_node(t, root->left);
    delete_all_node(t, root->right);
    free(root);
  }
}

void delete_rbtree(rbtree *t) {
  // TODO: reclaim the tree nodes's memory
  delete_all_node(t, t->root);
  free(t);
}


node_t* create_node(rbtree* tree, const key_t key) {
  node_t* new_node = (node_t*)malloc(sizeof(node_t));
  new_node->key = key;
  new_node->color = red;
  new_node->left = tree->nil;
  new_node->right = tree->nil;
  new_node->parent = tree->nil;

  return new_node;
}

// 노드를 기준으로 오른쪽 회전
void right_rotate(rbtree* t, node_t* node) {
  node_t* lc = node->left;

  node->left = lc->right; 

  if (lc->right != t->nil) {
    lc->right->parent = node;
  }

  lc->parent = node->parent;

  // 루트에 도달했으면.

  if (node->parent == t->nil) {
    t->root = lc;
    t->root->color = black;
  }

  else {
    // node가 부모의 오른쪽자식일 경우
    if (node->parent->right == node) {
      node->parent->right = lc;
    }

    // lc가 부모의 왼쪽 자식일 경우
    else {
      node->parent->left = lc;
    }
  }

  lc->right = node;
  node->parent = lc;
}

void left_rotate(rbtree* t, node_t* node) {
  node_t* rc = node->right;

  node->right = rc->left;
  
  if (rc->left != t->nil) {
    rc->left->parent = node;
  }

  rc->parent = node->parent;

  if (node->parent == t->nil) {
    t->root = rc;
    t->root->color = black;
  }
  else {
    // node가 부모의 오른쪽 자식일 경우
    if (node->parent->right == node) {
      node->parent->right = rc;
    }

    else {
      node->parent->left = rc;
    }
  }

  rc->left = node;
  node->parent = rc;
}

void restructure(rbtree* t, node_t* node) {
  node_t* temp = node;

  // node의 부모가 red면 계속 재구조화
  while (temp->parent->color == red) {
    // 부모가 nil노드면 root다.
    // 블랙으로 바꾸고 리턴
    // if (temp->parent == t->nil) {
    //   t->root = temp;
    //   t->root->color = black;
    //   break;
    // }

    node_t* granpa = temp->parent->parent;

    // 엉클도 빨간색이면 조상이랑 교체
    if (granpa->left->color == red && granpa->right->color == red) {
      granpa->color = red;
      granpa->left->color = black;
      granpa->right->color = black;
      temp = granpa;
      continue;
    }

    // /인 경우,
    if (temp->parent->key > temp->key && granpa->key > temp->parent->key) {
      right_rotate(t, granpa);
      granpa->color = red;
      temp->parent->color = black;
      temp = temp->parent;
      break;
    }
    // \인 경우,
    else if (temp->parent->key <= temp->key && granpa->key <= temp->parent->key) {
      left_rotate(t, granpa);
      granpa->color = red;
      temp->parent->color = black;
      temp = temp->parent;
      break;
    }

    // <모양이면,
    else if (temp->parent->key < granpa->key && temp->key >= temp->parent->key) {
      left_rotate(t, temp->parent);
      granpa->color = red;
      temp->color = black;
      right_rotate(t, temp->parent);
      temp = temp;
      break;
    }

    // >모양이면,
    else if (temp->parent->key >= granpa->key && temp->key < temp->parent->key) {
      right_rotate(t, temp->parent);
      granpa->color = red;
      temp->color = black;
      left_rotate(t, temp->parent);
      temp = temp;
      break;
    }
  }

  t->root->color = black;
}

node_t *rbtree_insert(rbtree *t, const key_t key) {
  // TODO: implement insert

  // tree의 root노드가 NULL이면 새 노드가 root노드가 된다.
  if (t->root == t->nil) {
    t->root = create_node(t, key);
    t->root->color = black;

    return t->root;
  }

  // 트리에 루트노드가 있으면 새 노드를 넣을 위치 찾기
  
  node_t* cur = t->root;
  node_t* cur_parent = cur->parent;
  int pos = 0; // 1이면 오른쪽 자식, 0이면 왼쪽 자식
  
  while (cur != t->nil) {
    // 크거나 같거나 작나로 판단.
    // cur노드보다 크면
    if (cur->key <= key) {
      cur_parent = cur;
      pos = 1;
      cur = cur->right;
    }

    // cur노드보다 작거나
    else if (cur->key > key) {
      cur_parent = cur;
      pos = 0;
      cur = cur->left;
    }

    // cur노드와 같으면 중단.
    else {
      break;
    }
  }

  // 새노드 생성
  node_t* new_node = create_node(t, key);

    // 우선 부모 설정해주고
    new_node->parent = cur_parent;

    // 왼쪽인지 오른쪽인지 판단
    // 왼쪽자식이면 부모의 왼쪽에 넣기
    if (pos == 0) {
      cur_parent->left = new_node;  
    }

    // 오른쪽 자식이면 오른쪽에 넣기
    else if (pos == 1) {
      cur_parent->right = new_node;
    }

  // 부모노드가 빨간색일 때만 확인해준다.
  if (new_node->parent->color == red) {
    // // 이제 재조정시간, new_node 포인터만 넣어주면 될듯
    restructure(t, new_node);
  }

  return new_node;
}

// 해당 key값을 가지는 노드 발견하면 노드 포인터 반환. 없으면 NULL
// BST 기본
node_t *rbtree_find(const rbtree *t, const key_t key) {
  // TODO: implement find

  // 현재 노드를 담는 포인터변수 정의 
  node_t* cur_node = t->root;
  
  // cur_node가 NULL이 아닐 때까지 반복
  while (cur_node != t->nil) {
    // cur노드보다 작으면 왼쪽 자식으로 포인터 변경
    if (cur_node->key > key) {
      cur_node = cur_node->left;
    }

    // cur노드보다 크면 오른쪽 자식으로 포인터 변경
    else if (cur_node->key <= key) {
      cur_node = cur_node->right;
    }

    // cur노드의 key와 같으면 반환
    else {
      return cur_node;
    }
  }

  // while에서 return 안됐으면 해당 키값을 가지는 노드가 없다는 뜻. 따라서 NULL 반환.
  return NULL;
}

// 최소값을 가지는 노드의 포인터 반환.
// BST 기본
node_t *rbtree_min(const rbtree *t) {
  // TODO: implement find
  
  // 노드 없으면 NULL 반환
  if (t->root == t->nil) {
    return NULL;
  }

  // 노드 있으면 맨 왼쪽 노드 반환.
  node_t* cur = t->root;
  while (cur->left != t->nil) {
    cur = cur->left;
  }

  return cur;
}

// 최대값을 가지는 노드의 포인터 반환.
// BST 기본
node_t *rbtree_max(const rbtree *t) {
  // TODO: implement find

  if (t->root == t->nil) {
    return NULL;
  }

  // 노드 있으면 맨 오른쪽 노드 반환.
  node_t* cur = t->root;
  while (cur->right != t->nil) {
    cur = cur->right;
  }

  return cur;
}

// 노드의 포인터 p를 입력받아서 메모리 해제하는 함수.
// BST 기본

// doubly black을 처리하는 함수
// 케이스별로 나눠서 푼다.

// node는 extra black이 붙어 있는 노드다.


void rb_transplant(rbtree* t, node_t* from_node, node_t* to_node) {
  // 대체될 노드가 루트면
  if (from_node->parent == t->nil) {
    // 루트노드를 to_node로 바꾼다.
    t->root = to_node;
  }

  // from_node가 부모의 왼쪽 자식일 때,
  else if (from_node->parent->left == from_node) {
    from_node->parent->left = to_node;
  }

  // 오른쪽 자식일 때,
  else {
    from_node->parent->right = to_node;
  }

  // 부모노드 설정.
  to_node->parent = from_node->parent;
}

// 여기서 fix_node는 extra black이 붙은 노드가 된다.
// 내가 코드 구현할 때 어려워했던 부분이다. extra black이 nil 노드에 붙을 경우, nil의 부모를 연결지어줘도 되는지 몰랐다.
// 그리고 fixup의 인자에 넣을 노드로, (타겟, 석세서, extra black이 붙을 노드) 이 셋중에 뭘 넣어야 할지 어려웠다.
// 답은 extrablack이 붙을 노드였고, 상식적으로 맞는데 쉽게 결정하지 못했던 이유는 NIL에 extra black이 붙을 경우 부모를 찾을 수 없다는 생각 때문이었다.
void rb_delete_fixup(rbtree* t, node_t* fix_node) {
  // 위반 체크할 노드가 루트면 그냥 블랙으로 바꾸면 되고, 빨간색이면 red and black이 됐다는 소리다.
  // flx_node.color == black이면 현재 doubly-black이라는 소리
  // 루트도 아니고 빨간색도 아니면 while 수행
  while (fix_node != t->root && fix_node->color == black) {
    // case1, 2, 3, 4 순으로 체크해 나가야 함.

    // fix_node(extra black 붙은 블랙노드)가 부모의 왼쪽 자식일 때,
    if (fix_node == fix_node->parent->left) {
      node_t* brother = fix_node->parent->right;

      // 형제가 빨간색일 때는 케이스1이다. 이거 먼저 고려한다.
      if (brother->color == red) {
        brother->color = black;
        fix_node->parent->color = red;
        left_rotate(t, fix_node->parent);

        // fix_node의 형제가 바뀌었다.
        brother = fix_node->parent->right; 
      }

      // case2: 형제의 두 자식이 모두 블랙
      if (brother->left->color == black && brother->right->color == black) {
        // 나와 형제의 검은색 하나를 지우고 부모에게 주는건데 코드로 구현하면 이렇게 하면 된다.
        // 나에서 extra black 하나 빼도 블랙이고, brother는 레드가 된다.
        // 이후 다시 확인해야 할 노드는 나의 부모가 된다.
        // 그냥 이렇게만 해줘도 되는 이유는, 만약 부모가 레드였다면 red and black이라서 부모를 블랙으로 바꿔주면 끝이고, 따라서 while문을 빠져나간 후, fix_node = black으로 해주고 끝.
        // 부모가 블랙이면 다시 case1부터 수행. 
        brother->color = red;
        fix_node = fix_node->parent;
      }

      // case2가 아니면 case3, case4중 하나이다.
      else {
        // case3: 형제는 검은색, 왼쪽 자식은 빨간색, 오른쪽 자식은 검은색.
        // case3일 때 아래 로직을 수행하면 case4 모양이 된다.
        // case4인 경우 무조건 5조건을 만족하게 만들 수 있다.
        // 결국 case1,2,3은 case4를 만들어주는 역할을 한다.
        if (brother->color == black && brother->left->color == red && brother->right->color == black) {
          brother->left->color = black;
          brother->color = red;
          right_rotate(t, brother);

          // 형제노드 바뀌었으므로 다시 설정
          brother = fix_node->parent->right;
        }
        // case4: 수행
        // 현재 parent 기준 왼편에 있는 extra black를 처리해줘야 함.
        // 처리한다는 것은 extra black이 없어도 블랙 개수를 균형이뤄야 한다는 소리
        // 그래서 \ 부분을 한칸씩 밑으로 땡겨준 후, 부모를 블랙으로 해주면 기존의 부모-형제 색은 한칸 밑에 저장하고 부모를 검은색으로 해준 셈이다. 이후 왼쪽 회전을 하면 검은색을 하나 왼쪽에 더해주는 셈이 되면서 블랙 균형이 이루어진다.
        brother->color = fix_node->parent->color;
        fix_node->parent->color = black;
        brother->right->color = black;
        left_rotate(t, fix_node->parent);

        // case4가 끝나면 무조건 5조건 만족하므로 while 빠져나가도록 함.
        fix_node = t->root;
      }
    }
    else {
      node_t* brother = fix_node->parent->left;
      
      // case1
      if (brother->color == red) {
        brother->color = black;
        brother->parent->color = red;
        right_rotate(t, brother->parent);
        brother = fix_node->parent->left;
      }

      // case2
      if (brother->color == black && brother->left->color == black && brother->right->color == black) {
        brother->color = red;
        fix_node = fix_node->parent;
      }

      // case3
      else {
        if (brother->left->color == black) {
          brother->right->color = black;
          brother->color = red;
          left_rotate(t, brother);
          brother = fix_node->parent->left;
        }

        brother->color = fix_node->parent->color;
        fix_node->parent->color = black;
        brother->left->color = black;
        right_rotate(t, fix_node->parent);
        fix_node = t->root;
      }
    }
  }
  fix_node->color = black;
}

// p를 지운다.
int rbtree_erase(rbtree *t, node_t *p) {
  // TODO: implement erase
  if (t->root == t->nil) {
    return 0;
  }

  // p의 색 저장
  color_t deleted_color = p->color; 
  node_t* fix_node;
  
  // 삭제해야 할 p의 왼쪽 노드가 없으면 그냥 p의 오른쪽 노드를 이어붙이면 된다.
  if (p->left == t->nil) {
    fix_node = p->right;
    rb_transplant(t, p, p->right);
  }

  // p의 오른쪽 노드가 없으면 마찬가지로 왼쪽 노드를 연결하면 된다.
  else if (p->right == t->nil) {
    fix_node = p->left;
    rb_transplant(t, p, p->left);
  }

  // 양 자식 둘 다 있을 때,
  else {
    // succ을 찾는다. succ은 target 대신 삭제되어야 할 노드다.
    node_t* succ = p->right;

    while (succ->left != t->nil) {
      succ = succ->left;
    }

    // 석의 색을 저장한다.
    deleted_color = succ->color;

    // 석의 오른쪽 서브트리 저장
    fix_node = succ->right;

    // succ 찾으러 한번 이상 왼쪽으로 갔을 때,
    if (succ != p->right) {
      // 석 없애고 그자리에 석의 오른쪽 자식 서브트리를 삽입
      // 석은 타겟자리로 올라간다.
      rb_transplant(t, succ, succ->right);

      // 석의 오른쪽 자식에 target노드의 오른쪽 자식을 넣는다.
      succ->right = p->right;
      
      // target노드의 오른쪽 자식의 부모를 석으로 한다.
      succ->right->parent = succ;
    }

    // 그냥 타겟의 오른쪽 자식이 석일 때
    else {
      // 석의 오른쪽 자식의 부모를 타겟에서 석으로 바꾼다.
      // 초기값 자체와 다를게 없는데 해주는 이유는, 석의 오른쪽 자식이 NIL인 경우에 NIL의 부모를 석으로 해주기 위해서다.
      // 이는 delete_fixup할 때 필요하다.
      fix_node->parent = succ;
    }

    // 타겟노드의 부모가 타겟대신 석을 자식으로 갖게 하고,
    // 타겟노드의 자식이 타겟대신 석을 부모로 갖게한다.
    // 이렇게 해서 타겟을 없애고 석을 넣는 과정 수행
    rb_transplant(t, p, succ);

    // 타겟의 왼쪽노드를 석의 왼쪽에 붙이고,
    succ->left = p->left;
    succ->left->parent = succ;

    // 색도 타겟의 색으로 바꿔준다.
    succ->color = p->color;
  }

  // 빨간색을 지우면 문제가 되지 않는다(블랙 카운트에 영향 없고, 블랙은 연달아 있어도 되기 때문)
  // 하지만 블랙을 지우면 문제가 된다(빨강이 연달아 올 수 있고, 블랙카운트가 달라져서)
  if (deleted_color == black) {
    rb_delete_fixup(t, fix_node);
  }
  return 1;
}

void inorder(const rbtree* t, node_t* root, key_t* arr, const size_t n, int* cnts) {
  if (*cnts == n) {
    return;
  }

  if (root != t->nil) {
    inorder(t, root->left, arr, n, cnts);
    if (*cnts == n) {
      return;
    }
    arr[*cnts] = root->key;
    *cnts += 1;
    inorder(t, root->right, arr, n, cnts);
  }
}

// rbtree를 키 순서대로 array에 n개 넣어줌.
// 중위순회 BST 기본
int rbtree_to_array(const rbtree *t, key_t *arr, const size_t n) {
  // TODO: implement to_array

  int cnts = 0;
  // 중위순회 하면서 key를 arr에 넣어주면 됨.
  inorder(t, t->root, arr, n, &cnts);
  
  return 0;
}


void inorder_print(rbtree* t, node_t* root) {
  if (root != t->nil) {
    inorder_print(t, root->left);
    printf("%d %d\n", root->key, root->color);
    inorder_print(t, root->right);
  }
}

int main(void) {
    rbtree *t = new_rbtree();
    //10, 5, 8, 34, 67, 23, 156, 24, 2, 12, 24, 36, 990, 25};
    rbtree_insert(t, 10);
    rbtree_insert(t, 5);
    rbtree_insert(t, 8);
    rbtree_insert(t, 34);
    rbtree_insert(t, 67);
    rbtree_insert(t, 23);
    rbtree_insert(t, 156);
    rbtree_insert(t, 24);
    rbtree_insert(t, 2);
    rbtree_insert(t, 12);
    rbtree_insert(t, 24);
    rbtree_insert(t, 36);
    rbtree_insert(t, 990);
    rbtree_insert(t, 25);
    inorder_print(t, t->root);
    delete_all_node(t, t->root);
}