// #include "BST.hpp"

// int main()
// {
//     Tree<int> bst;

//     // 插入操作
//     bst.insert(5);
//     bst.insert(3);
//     bst.insert(7);
//     bst.insert(2);
//     bst.insert(4);
//     bst.insert(6);
//     bst.insert(8);

//     std::cout << "中序遍历插入结果: ";
//     bst.inorder();
//     std::cout << "\n";

//     // 查找操作
//     Node<int> *found = bst.find(4);
//     if (found != nullptr)
//         std::cout << "找到了值为4的节点\n";
//     else
//         std::cout << "没有找到值为4的节点\n";

//     // 删除操作
//     bst.delete_node(3); // 尝试删除一个存在的节点
//     std::cout << "删除值为3的节点后的中序遍历: ";
//     bst.inorder();
//     std::cout << "\n";

//     // 尝试删除一个不存在的节点
//     bst.delete_node(10);
//     std::cout << "尝试删除不存在的节点后的中序遍历: ";
//     bst.inorder();
//     std::cout << "\n";

//     return 0;
// }

#include "BST.hpp"
#include <cassert>

void TestDeleteLeafNode() {
    Tree<int> tree;
    tree.insert(10);
    tree.insert(5);
    tree.insert(15);
    tree.insert(3); // Leaf node
    assert(tree.delete_recursive(3) == true);
    assert(tree.find(3) == nullptr);
}

void TestDeleteNodeWithOneChild() {
    Tree<int> tree;
    tree.insert(10);
    tree.insert(5);
    tree.insert(15);
    tree.insert(3);
    tree.insert(4); // Node with one child
    assert(tree.delete_recursive(3) == true);
    assert(tree.find(3) == nullptr);
    assert(tree.find(4) != nullptr); // Child should still exist
}

void TestDeleteNodeWithTwoChildren() {
    Tree<int> tree;
    tree.insert(10);
    tree.insert(5);
    tree.insert(15);
    tree.insert(3);
    tree.insert(7); // Node with two children
    tree.insert(6);
    tree.insert(8);
    assert(tree.delete_recursive(5) == true);
    assert(tree.find(5) == nullptr);
    assert(tree.find(6) != nullptr && tree.find(8) != nullptr); // Children should still exist
}

void TestDeleteRootNode() {
    Tree<int> tree;
    tree.insert(10); // Root node
    tree.insert(5);
    tree.insert(15);
    assert(tree.delete_recursive(10) == true);
    assert(tree.find(10) == nullptr);
    assert(tree.find(5) != nullptr && tree.find(15) != nullptr); // Children should still exist
}

void TestDeleteNonExistentNode() {
    Tree<int> tree;
    tree.insert(10);
    tree.insert(5);
    tree.insert(15);
    assert(tree.delete_recursive(20) == false); // Non-existent node
}

void TestDeleteFromEmptyTree() {
    Tree<int> tree;
    assert(tree.delete_recursive(10) == false); // Empty tree
}

int main() {
    TestDeleteLeafNode();
    TestDeleteNodeWithOneChild();
    TestDeleteNodeWithTwoChildren();
    TestDeleteRootNode();
    TestDeleteNonExistentNode();
    TestDeleteFromEmptyTree();
    std::cout << "All tests passed successfully." << std::endl;
    return 0;
}