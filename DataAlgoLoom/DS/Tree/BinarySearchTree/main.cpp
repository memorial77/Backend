#include "BST.hpp"

int main()
{
    Tree<int> bst;

    // 插入操作
    bst.insert(5);
    bst.insert(3);
    bst.insert(7);
    bst.insert(2);
    bst.insert(4);
    bst.insert(6);
    bst.insert(8);

    std::cout << "中序遍历插入结果: ";
    bst.inorder();
    std::cout << "\n";

    // 查找操作
    Node<int> *found = bst.find(4);
    if (found != nullptr)
        std::cout << "找到了值为4的节点\n";
    else
        std::cout << "没有找到值为4的节点\n";

    // 删除操作
    bst.delete_node(3); // 尝试删除一个存在的节点
    std::cout << "删除值为3的节点后的中序遍历: ";
    bst.inorder();
    std::cout << "\n";

    // 尝试删除一个不存在的节点
    bst.delete_node(10);
    std::cout << "尝试删除不存在的节点后的中序遍历: ";
    bst.inorder();
    std::cout << "\n";

    return 0;
}