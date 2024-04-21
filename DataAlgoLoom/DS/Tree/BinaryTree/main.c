#include <stdio.h>
#include <stdlib.h>

// 二叉树节点
typedef int val_type;
typedef struct Node
{
    val_type data_;      // 节点数据
    struct Node *left_;  // 左孩子节点
    struct Node *right_; // 右孩子节点
} Node;

// 创建新节点
Node *createNode(val_type data)
{
    struct Node *newNode = (Node *)malloc(sizeof(Node));
    newNode->data_ = data;
    newNode->left_ = NULL;
    newNode->right_ = NULL;
    return newNode;
}

// 构建二叉树
Node *buildTree()
{
    Node *root = createNode(1);
    root->left_ = createNode(2);
    root->right_ = createNode(3);
    // root->left_->left_ = createNode(4);
    root->left_->right_ = createNode(5);
    // root->right_->left_ = createNode(6);
    root->right_->right_ = createNode(7);
    // root->left_->left_->left_ = createNode(8);
    // root->left_->left_->right_ = createNode(9);
    // root->left_->right_->left_ = createNode(10);
    // root->left_->right_->right_ = createNode(11);
    // root->right_->left_->left_ = createNode(12);
    // root->right_->left_->right_ = createNode(13);
    // root->right_->right_->left_ = createNode(14);
    // root->right_->right_->right_ = createNode(15);
    return root;
}

// 前序遍历
void preorderTraversal(Node *root)
{
    if (root == NULL)
    {
        printf("NULL ");
        return;
    }

    printf("%d ", root->data_);
    preorderTraversal(root->left_);
    preorderTraversal(root->right_);
}

// 中序遍历
void inorderTraversal(Node *root)
{
    if (root == NULL)
    {
        printf("NULL ");
        return;
    }

    preorderTraversal(root->left_);
    printf("%d ", root->data_);
    preorderTraversal(root->right_);
}

// 后序遍历
void postorderTraversal(Node *root)
{
    if (root == NULL)
    {
        printf("NULL ");
        return;
    }

    preorderTraversal(root->left_);
    preorderTraversal(root->right_);
    printf("%d ", root->data_);
}

// 节点个数
size_t tree_size(Node *root)
{
    if (root == NULL)
        return 0;

    // 树节点总个数为 左树节点个数 + 右树节点个数 + 根节点
    return tree_size(root->left_) + tree_size(root->right_) + 1;
}

// 叶子节点个数
size_t leaf_size(Node *root)
{
    if (root == NULL)
        return 0;

    if (root->left_ == NULL && root->right_ == NULL)
        return 1;

    return leaf_size(root->left_) + leaf_size(root->right_);
}

// 第 k 层节点数
size_t level_size(Node *root, size_t k)
{
    if (root == NULL)
        return 0;

    if (k == 1)
        return 1;

    return level_size(root->left_, k - 1) + level_size(root->right_, k - 1);
}

// 寻找值为 val 的节点
Node *tree_find(Node *root, val_type val)
{
    if (root == NULL)
        return NULL;

    if (root->data_ == val)
        return root;

    Node *res_left = tree_find(root->left_, val);
    if (res_left)
        return res_left;

    Node *res_right = tree_find(root->right_, val);
    if (res_right)
        return res_right;

    return NULL;
}

// 树的深度
size_t tree_depth(Node *root)
{
    if (root == NULL)
        return 0;

    size_t depth_left = tree_depth(root->left_);
    size_t depth_right = tree_depth(root->right_);
    return depth_left > depth_right ? depth_left + 1 : depth_right + 1;
}

// 测试遍历函数
void order_test()
{
    Node *root = buildTree();

    printf("preorderTraversal: \t");
    preorderTraversal(root);
    printf("\n");

    printf("inorderTraversal: \t");
    inorderTraversal(root);
    printf("\n");

    printf("postorderTraversal: \t");
    postorderTraversal(root);
    printf("\n");
}

int main()
{
    return 0;
}