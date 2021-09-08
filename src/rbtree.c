#include "rbtree.h"

#include <stdlib.h>
#include<stdio.h>

void print_tree(node_t* t){
  if (t){
    // inorder 순회
    print_tree(t->left);
    printf("%d(%d)", t->key, t->color);
    print_tree(t->right);
  }
}

rbtree *new_rbtree(void) {
  rbtree *p = (rbtree *)calloc(sizeof(rbtree), 1);
  return p;
}

// helper function for delete_rbtree
void helper_delete_rbtree(node_t* p){
  if (p == NULL)
    return;
  helper_delete_rbtree(p->left);
  helper_delete_rbtree(p->right);
  free(p);
}

void delete_rbtree(rbtree *t) {
  // TODO: reclaim the tree nodes's memory
  helper_delete_rbtree(t->root);
  free(t);
}

// helper function for insertion_fixup
void left_rotate(rbtree *t, node_t* z){
  /*
        z                                child  
         \                            /        \
          child            ->        z          child.R
        /      \                      \
      child.L   child.R                child.L

  해줘야 할 거 list
  1. z의 오른쪽에 child 의 왼쪽 연결
  2. child의 왼쪽의 부모를 z로 설정
  3. z 부모가 root인지 확인
  4. z부모의 왼쪽이나 오른쪽에 child연결
  5. child의 부모를 z의 부모로 설정
  6. z의 부모를 child로 설정
  7. child의 왼쪽을 z로 설정
  */
  node_t* child = z->right;

  z->right = child->left;
  if (child->left)
    child->left->parent = z;
  
  if (!z->parent)
    t->root = child;
  else if (z == z->parent->left)
    z->parent->left = child;
  else if (z == z->parent->right)
    z->parent->right = child;

  child->parent = z->parent;
  z->parent = child;
  child->left = z;
}

// helper function for insertion_fixup
void right_rotate(rbtree *t, node_t* z){
  /*     z         child  
       /      ->       \
      child             z
  */
  node_t* child = z->left;
  z->left = child->right;
  if (child->right)
    child->right->parent = z;
  
  if (!z->parent)
    t->root = child;
  else if (z == z->parent->left)
    z->parent->left = child;
  else if (z == z->parent->right)
    z->parent->right = child;
  
  child->parent = z->parent;
  z->parent = child;
  child->right = z;
} 

// helper function for rbtree_insert
void insertion_fixup(rbtree* t, node_t* z){

  // 부모 색이 검은색이 될때까지 도는거임 -- Double Red를 피하기 위해
  while (z!= t->root && z->parent->color == RBTREE_RED){
    node_t* parents = z->parent;
    node_t* grandparents = parents->parent;

    //parent가 왼쪽 자식이면
    if (parents == grandparents->left){
      node_t* uncle = grandparents->right;

      //case1: uncle이 없거나 uncle이 검은색일 때 restructuring
      if (!uncle || uncle->color == RBTREE_BLACK){
        // 2-1. 내가 부모의 오른쪽 자식일 때
        if(z == parents->right){
          z = parents;
          left_rotate(t, z);
          // left rotate 후에 z는 또 다시 child node가 됨
        }
        // case3: (2-2). 내가 부모의 왼쪽 자식일 때 
        z->parent->color = RBTREE_BLACK;
        grandparents->color = RBTREE_RED;
        right_rotate(t, grandparents);
        // right_rotate 하면 parent가 위로 올라가고 parent.R == grandparents 됨
      }

      //case2: parent, uncle 둘다 빨간색일 때  recoloring
      else if (uncle->color == RBTREE_RED){
        parents->color = uncle->color = RBTREE_BLACK;
        // grandparents가 root면 무조건 black칠, 아니면 red칠 하고 double red를 피하기 위해 다시 while loop로
        grandparents->color = (grandparents == t->root) ? RBTREE_BLACK : RBTREE_RED ;
        z = grandparents;
      }
    }

    else if (parents == grandparents->right){
      node_t* uncle = grandparents->left;

      //case1: uncle이 검은색일 때
      if (!uncle || uncle->color == RBTREE_BLACK){
        // 부모의 왼쪽 자식일 때
        if(z == parents->left){
          z= z->parent;
          right_rotate(t,z);
        }
        // 부모의 오른쪽 자식일 때
        z->parent->color = RBTREE_BLACK;
        grandparents->color = RBTREE_RED;
        left_rotate(t, grandparents);
      }
      //case2: parent, uncle둘다 빨간색일 때
      else if (uncle->color == RBTREE_RED){
        parents->color = uncle->color = RBTREE_BLACK;
        grandparents->color = (grandparents == t->root) ? RBTREE_BLACK : RBTREE_RED ;
        z = grandparents;
      }
    }
  }
}

node_t *rbtree_insert(rbtree *t, const key_t key) {
  // TODO: implement insert
  node_t* newnode = (node_t*)calloc(1, sizeof(node_t));
  newnode->color = RBTREE_RED;
  newnode->key = key;
  newnode->left = NULL;
  newnode->right = NULL;
  newnode->parent = NULL;

  if (t->root == NULL){
    // root의 색은 언제나 black
    newnode->color = RBTREE_BLACK;
    t->root = newnode;
    return t->root;
  }
  node_t* parent = NULL;
  node_t* current = t->root;
  while (current){
    parent = current;
    if (current->key > key)
      current = current ->left;
    else if (current->key < key)
      current = current ->right;
    // key값이 이미 있을 때  
    else
      return t->root;
  }
  // 새로운 값의 부모 update
  newnode->parent = parent;
  // 부모 값의 left || right update
  if (parent->key > key)
    parent->left = newnode;
  else
    parent->right = newnode;

  // 값 추가하고 색상 fix
  insertion_fixup(t, newnode);

  return t->root;
}

node_t *rbtree_find(const rbtree *t, const key_t key) {
 // TODO: implement find
  node_t* ptr = t->root;
  while(ptr){
    if (ptr->key > key)
      ptr = ptr->left;
    else if (ptr->key < key)
      ptr = ptr->right;
    // key 값을 찾으면
    else
      return ptr;
  }
  // key 값을 못 찾으면
  return NULL;
}

node_t *rbtree_min(const rbtree *t) {
  // TODO: implement find
  node_t* ptr = t->root;
  while(ptr->left){
      ptr = ptr->left;
  }
  return ptr;
}

node_t *rbtree_max(const rbtree *t) {
  // TODO: implement find
  node_t* ptr = t->root;
  while(ptr->right){
      ptr = ptr->right;
  }
  return ptr;
}

// helper function for rbtree_erase
void swap_node(rbtree* t, node_t* out, node_t* in){
    if (out->parent == NULL)
        t->root = in;
    else if (out == out->parent->left)
        out->parent->left = in;
    else if (out == out->parent->right)
        out->parent->right = in;
    if (in != NULL)
        in->parent = out->parent;
}

// helper function for rbtree_erase
void delete_fixup(rbtree* t, node_t* x){
    while (x!= t->root && x->color == RBTREE_BLACK){

        // x가 왼쪽 자식일 때
        if (x == x->parent->left){
            node_t* w = x->parent->right;

            // case 1 - sibling 이 red일 때
            if (w->color == RBTREE_RED){
                w->color = RBTREE_BLACK;
                x->parent->color = RBTREE_RED;
                left_rotate(t, x->parent);
                w = x->parent->right;
            }
            // case 2 - sibling, sibling children 모두 black 일 때
            if (w->left->color == RBTREE_BLACK && w->right->color == RBTREE_BLACK){
                w->color = RBTREE_RED;
                x = x->parent;
            }
            else{
                // case 3 - sibling, sibling right 이 black, left가 red 일 때 
                if (w->right->color == RBTREE_BLACK){
                    w->left->color = RBTREE_BLACK;
                    w->color = RBTREE_RED;
                    right_rotate(t,w);  //w->right 가 red되고 case 4로 넘어감
                    w = x->parent->right;
                }
                // case 4 - sibling 이 black, sibling right이 red 일때
                if (w->right->color == RBTREE_RED){
                    w->color = x->parent->color;
                    x->parent->color = RBTREE_BLACK;
                    w->right->color = RBTREE_BLACK;
                    left_rotate(t, x->parent);
                    // while 문을 끝냄과 동시에 root 검은색으로 칠해주기 위해
                    x = t->root;
                }
            }
        }

        // x가 오른쪽 자식일 때
        else{
            node_t * w = x->parent->left;

            // case 1 - sibling이 red일 때
            if (w->color == RBTREE_RED){
                w->color = RBTREE_BLACK;
                x->parent->color = RBTREE_RED;
                right_rotate(t, x->parent);
                w = x->parent->left;
            }
            // case 2- sibling, sibling children 모두 black일 때
            if (w->color ==RBTREE_BLACK &&  w->left->color == RBTREE_BLACK && w->right->color == RBTREE_BLACK){
                w->color = RBTREE_RED;
                x = x->parent;
            }
            else{
                // case 3 - sibling left가 black, right이 red 일 때
                if (w->left->color == RBTREE_BLACK){
                    w->right->color = RBTREE_BLACK;
                    w->color = RBTREE_RED;
                    left_rotate(t, w);
                    w = x->parent->left;
                }
                // case 4 - sibling left가 red일 때
                if (w->left->color == RBTREE_RED){
                    w->color = x->parent->color;
                    x->parent->color = RBTREE_BLACK;
                    w->left->color = RBTREE_BLACK;
                    right_rotate(t, x->parent);
                    x = t->root;
                }
            }
        }
    }

    x->color = RBTREE_BLACK;
}
int rbtree_erase(rbtree *t, node_t *target) {
  // TODO: implement erase
  node_t *y = target;
  node_t *x = NULL;
  color_t original_color = target->color;

  // leaf node일 때
  if (!target->left && !target->right){
      if (target->parent == NULL)
        t->root = NULL;
      else{
          if (target == target->parent->right)
            target->parent->right = NULL;
          else
            target->parent->left = NULL;
      }
  }
  // 오른쪽 자식밖에 없을 때
  else if (!target->left){
      x = target->right;
      swap_node(t, target, x);
  }
  // 왼쪽 자식밖에 없을 때
  else if (!target->right){
    x = target->left;
    swap_node(t, target, x);
  }
  // 자식이 두명 있을 때
  else{
      // 오른쪽 자식의 left left로 가서 최소값 찾기
      y = target->right;
      while(y->left){y = y->left;}
      // 자식이 두명 있을 때는 black이 올라간 걸 수도 있으니 확인 위해 original_color 바꾸기
      original_color = y->color;
      x = y->right;
      if (x!= NULL && y->parent == target)
        x->parent = target;
      else{ 
          // 원래 y자리에 y->right넣는거임
          swap_node(t, y, y->right);
          y->right = target->right;
          if (y->right)
            y->right->parent = y;
      }
      //원래 target 자리에 y넣는거임
      swap_node(t, target, y);
      y->left = target->left;
      y->left->parent = y;
      y->color = target->color;
  }  
  if (x!= NULL && original_color == RBTREE_BLACK)
    delete_fixup(t, x);

  free(target);
  return 0;
}

int rbtree_to_array(const rbtree *t, key_t *arr, const size_t n) {
  // TODO: implement to_array

  return 0;
}