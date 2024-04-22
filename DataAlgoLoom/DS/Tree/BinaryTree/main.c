#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

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

// 单值二叉树 v1
bool is_unival_tree_v1(Node *root)
{
    if (root == NULL)
        return true;

    if (root->left_ && root->left_->data_ != root->data_)
        return false;

    if (root->right_ && root->right_->data_ != root->data_)
        return false;

    return is_unival_tree_v1(root->left_) && is_unival_tree_v1(root->right_);
}

// 以前序遍历的方式比较树中每个节点的值是否与给定值相等
bool preorder_compare(Node *root, int val)
{
    if (root == NULL)
        return true;

    if (root->data_ != val)
        return false;

    return preorder_compare(root->left_, val) &&
           preorder_compare(root->right_, val);
}

// 单值二叉树 v2
bool is_unival_tree_v2(Node *root)
{
    if (root == NULL)
        return true;

    // 使用preorder_compare函数检查树中所有节点的值是否与根节点的值相等
    return preorder_compare(root, root->data_);
}

// 相同的树
bool is_same_tree(Node *root1, Node *root2)
{
    // 如果两个树都为空，则它们相同
    if (root1 == NULL && root2 == NULL)
        return true;

    // 如果其中一个树为空而另一个不为空，则它们不相同
    if (root1 == NULL || root2 == NULL)
        return false;

    // 如果两个树的当前节点值不相同，则它们不相同
    if (root1->data_ != root2->data_)
        return false;

    // 递归比较两颗树的左子树和右子树是否相同
    return is_same_tree(root1->left_, root2->left_) &&
           is_same_tree(root1->right_, root2->right_);
}

// 辅助函数来检查两个树是否为镜像对称
bool isMirror(Node *left, Node *right)
{
    if (left == NULL && right == NULL)
        return true;
    if (left == NULL || right == NULL || left->data_ != right->data_)
        return false;
    return isMirror(left->left_, right->right_) && isMirror(left->right_, right->left_);
}

// 轴对称二叉树
bool is_symmetric_tree(Node *root)
{
    if (root == NULL)
        return true;
    return isMirror(root->left_, root->right_);
}

// 另一棵树的子树
bool is_subtree(Node *root, Node *subRoot)
{
    if (root == NULL)
        return false;
    if (is_same_tree(root, subRoot))
        return true;
    return is_subtree(root->left_, subRoot) || is_subtree(root->right_, subRoot);
}

// 销毁二叉树
void destroy_tree(Node *root)
{
    if (root == NULL)
        return;

    destroy_tree(root->left_);
    destroy_tree(root->right_);
    free(root);
}

// 二叉树层序遍历
void level_order(Node *root)
{
    if (root == NULL) // 如果根节点为空，则直接返回
        return;

    // 在堆上动态分配队列空间
    Node **queue = (Node **)malloc(1000 * sizeof(Node *));
    if (queue == NULL) // 检查内存分配是否成功
        return;

    int front = 0, rear = 0; // 初始化队列的前端和后端指针
    queue[rear++] = root;    // 将根节点加入队列

    // 当队列不为空时循环
    while (front < rear)
    {
        Node *node = queue[front++]; // 从队列中取出一个节点
        printf("%d ", node->data_);  // 打印节点数据

        // 如果左子节点存在，将其加入队列
        if (node->left_)
            queue[rear++] = node->left_;
        // 如果右子节点存在，将其加入队列
        if (node->right_)
            queue[rear++] = node->right_;
    }

    free(queue); // 使用完毕后释放队列占用的内存
}

// 测试层序遍历
void level_order_test()
{
    Node *root = buildTree();
    level_order(root);
}

// 是否为完全二叉树
bool is_complete_tree(Node *root)
{
    if (root == NULL)
        return true;

    Node **queue = (Node **)malloc(1000 * sizeof(Node *));
    if (queue == NULL)
        return false;

    int front = 0, rear = 0;
    // 初始化队列并将根节点加入队列
    queue[rear++] = root;

    // 使用层次遍历来检查树的完整性
    while (front < rear)
    {
        Node *node = queue[front++]; // 从队列中取出一个节点

        if (node == NULL) // 如果节点为空，则终止循环
            break;

        // 将当前节点的左右子节点加入队列，即使它们是NULL
        queue[rear++] = node->left_;
        queue[rear++] = node->right_;
    }

    // 跳过所有的NULL节点，这些节点代表了树的最后一层的空位置
    while (front < rear && queue[front] == NULL)
        front++;

    // 检查在遇到第一个NULL节点之后是否还有非NULL节点，如果有，则不是完全二叉树
    while (front < rear)
    {
        if (queue[front++] != NULL) // 如果找到非NULL节点，则树不是完全二叉树
        {
            free(queue);  // 释放队列内存
            return false; // 返回false，表示这不是一个完全二叉树
        }
    }

    // 如果所有检查都通过，则释放队列内存并返回true，表示这是一个完全二叉树
    free(queue);
    return true;
}

// 是否为满二叉树
bool is_full_tree(Node *root)
{
    if (root == NULL)
        return true;

    if (root->left_ == NULL && root->right_ == NULL)
        return true;

    if (root->left_ && root->right_)
        return is_full_tree(root->left_) && is_full_tree(root->right_);

    return false;
}

int main()
{
    Node *root = buildTree();

    printf("tree_size: %zu\n", tree_size(root));
    printf("leaf_size: %zu\n", leaf_size(root));
    printf("level_size: %zu\n", level_size(root, 3));
    printf("tree_find: %p\n", tree_find(root, 5));
    printf("tree_depth: %zu\n", tree_depth(root));

    level_order_test();

    return 0;
}