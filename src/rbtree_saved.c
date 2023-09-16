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

void delete_rbtree(rbtree *t) {
  // TODO: reclaim the tree nodes's memory
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
  lc->right = node;
  lc->parent = node->parent;

  // 루트에 도달했으면.

  if (node->parent == t->nil) {
    t->root = lc;
    t->root->color = black;
  }

  else {
    // node가 부모의 오른쪽자식일 경우
    if (node->parent->key <= node->key) {
      node->parent->right = lc;
    }

    // lc가 부모의 왼쪽 자식일 경우
    else if (node->parent->key > node->key) {
      node->parent->left = lc;
    }
  }

  node->parent = lc;
}

void left_rotate(rbtree* t, node_t* node) {
  node_t* rc = node->right;

  node->right = rc->left;
  rc->left = node;
  rc->parent = node->parent;

  if (node->parent == t->nil) {
    t->root = rc;
    t->root->color = black;
  }
  else {
    // node가 부모의 오른쪽 자식일 경우
    if (node->parent->key <= node->key) {
      node->parent->right = rc;
    }

    else if (node->parent->key > node->key) {
      node->parent->left = rc;
    }
  }

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
void delete_fix(rbtree* t, node_t* node) {
  node_t* temp = node;

  // case4 - 형제(블랙), 오른쪽 조카:빨강, 왼쪽조카: 아무거나, 부모: 아무거나

  while (temp->parent != t->nil) {
    // 삭제될 노드가 부모의 왼쪽에 있을 때
    if (node->parent->key > node->key) {
      // 형제가 블랙일 때
      if (node->parent->right->color == black) {
        // 오른쪽 조카색이 레드일 때(case4)
        if (node->parent->right->right->color == red) {
          node->parent->right->color = node->parent->color;
          node->parent->right->right->color = black;
          node->parent->color = black;
          right_rotate(t, node->parent);
        }

        // 오른쪽 조카색이 블랙일 때
        else if (node->parent->right->right->color == black) {
          // 왼쪽 조카색이 레드면(case3)
          if (node->parent->right->left->color == red) {
            node->parent->right->color = red;
            node->parent->right->left->color = black;
            right_rotate(t, node->parent->right);
            // 이후 while 다시 가서 case4 수행
          }

          // 왼쪽 조카색이 블랙이면(case2)
          else if (node->parent->right->left->color == black) {
            node->parent->right->color = red;
            
            // 부모가 레드면
            if (node->parent->color == red) {
              node->parent->color = black;
              break;
            }

            // 블랙이면 db
            temp = node->parent;
          }

        }
      }
      // 형제가 레드일 때(case1)
      else {
        node->parent->color = red;
        node->parent->right->color = black;
        // 이후 다시 case2,3,4 확인
      }
    }

    // 삭제될 노드가 부모의 오른쪽에 있을 때
    else if (node->parent->key <= node->key) {

      // 형제가 블랙일 때
      if (node->parent->left->color == black) {
        // 왼쪽 조카색이 레드일 때(case4)
        if (node->parent->left->left->color == red) {

        }

        // 왼쪽 조카색이 블랙일 때
        else {
          // 오른쪽 조카색이 레드면(case3)
          if (node->parent->left->right->color == red) {

          }

          // 오른쪽 조카색도 블랙이면(case2)
          else {

          }
        }
      }
      
      // 형제가 레드면(case1)
      else {

      }
    }
  // case3 - 형제, 오른조카 = 블랙, 왼조카 = 레드, 부모= 아무거나
  // case2 - 형제, 양 조카 = 블랙, 부모=아무거나
  // case1 - 형제 = 레드, 부모, 양 조카 = 블랙
  }
}

int rbtree_erase(rbtree *t, node_t *p) {
  // TODO: implement erase

  node_t* cur = t->root;

  // 비어있으면 0 반환
  if (cur == t->nil) {
    return 0;
  }

  // 노드 있으면 찾는다. p의 key값을 기준으로 찾는다.
  while (cur != t->nil) {
    // 해당 포인터가 가리키는 노드의 키값이,
    // 현재 노드의 키값보다 작으면,
    if (cur->key > p->key) {
      // cur을 cur의 왼쪽 자식으로 교체
      cur = cur->left;
    }

    // 더 큰 경우,
    else if (cur->key < p->key) {
      cur = cur->right;
    }

    // 같은 경우
    // key값은 중복될 수 있기 때문에, 같은 포인터일 때까지 왼쪽 자식을 탐색한다.
    else {
      while (cur->key == p->key) {
        // 찾았다. 삭제로직 수행 후 1 리턴
        if (cur == p) {
          /* 삭제로직*/
          delete_fix(t, cur);
          free(cur);
          // 자식 개수 확인
          // 0개인 경우
          // if (cur->left == t->nil && cur->right == t->nil) {
          //   // 빨간색이면 그냥 지우고, 
          //   if (cur->color == red) {
          //     if (cur->parent->key <= cur->key) {
          //       cur->right = t->nil;
          //     }
          //     else {
          //       cur->left = t->nil;
          //     }
          //     free(cur);
          //   }

          //   // 검은색이면 더블리블랙 생성(해당 노드를 삭제한 후, 닐노드에 extra black 붙이기)
          //   else {
          //     // 자식 0개인데 루트노드 삭제할 경우
          //     if (cur->parent == t->nil) {
          //       t->root = t->nil;
          //       free(cur);
          //     }
          //     // 그 외의 경우 삭제될 노드 cur을 넣어서 정렬
          //     else {
          //       delete_fix(t, cur);
          //       // cur노드를 삭제
          //       free(cur);
          //     }
          //   }
          // }

          // // 왼쪽 자식 1개만 있으면
          // else if (cur->left != t->nil && cur->right == t->nil) {  

          //   }
          // }


          // // 오른쪽 자식 있으면
          // else if (cur->right != t->nil) {
          //   // 오른쪽 자식들 중 가장 작은걸 찾아야 함
          //   node_t* succ = cur->right;

          //   while (succ->left != t->nil) {
          //     succ = succ->left;
          //   }

          //   // succ 찾았다.
          //   // cur에 succ의 키값 넣고 succ을 삭제해야 함

          //   // 키값 넣기
          //   cur->key = succ->key;

          //   delete_fix(t, succ);
          // }

          return 1;
        }

        // 못찾았으면 다음 오른쪽자식 보러가기. 
        cur = cur->right;
      }

      // while 빠져 나왔으면 못찾은거다. 0 리턴
      return 0;
    }
  }

  // while을 빠져나온거면 못찾은거다. 0 리턴.
  return 0;
}

int cnts = 0;
void inorder(const rbtree* t, node_t* root, key_t* arr, const size_t n) {
  if (cnts == n) return;
  if (root != t->nil) {
    inorder(t, root->left, arr, n);
    if (cnts == n) return;

    *arr = root->key;
    arr++;
    cnts++;
    if (cnts == n) return;
    inorder(t, root->right, arr, n);
    if (cnts == n) return;
  }
}

// rbtree를 키 순서대로 array에 n개 넣어줌.
// 중위순회 BST 기본
int rbtree_to_array(const rbtree *t, key_t *arr, const size_t n) {
  // TODO: implement to_array

  // 중위순회 하면서 key를 arr에 넣어주면 됨.
  inorder(t, t->root, arr, n);
  
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
    node_t* p = rbtree_insert(t, 10);
    int res = rbtree_erase(t, p);
    if (t->root == t->nil) {
      printf("HI");
    }
    // inorder_print(t, t->root);
}