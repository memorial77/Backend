#pragma once

#include <iostream>

template <typename T>
class Node
{
public:
    Node(T data) : data_(data), left_(nullptr), right_(nullptr) {}

    T data_;
    Node<T> *left_;
    Node<T> *right_;
};

template <typename T>
class Tree
{
public:
    // 默认构造函数
    Tree() : root_(nullptr) {}

    // 插入节点
    bool insert(const T &val)
    {
        // 首次插入
        if (root_ == nullptr)
        {
            root_ = new Node<T>(val); // 如果树为空则新节点成为根节点
            return true;
        }

        Node<T> *cur = root_; // 使用cur来遍历树
        while (cur)
        {
            if (val < cur->data_) // 如果插入值小于当前节点值
            {
                if (cur->left_ == nullptr)
                {
                    cur->left_ = new Node<T>(val);
                    return true;
                }
                cur = cur->left_; // 否则向左子树继续查找
            }
            else if (val > cur->data_) // 如果插入值大于当前节点值
            {
                if (cur->right_ == nullptr)
                {
                    cur->right_ = new Node<T>(val);
                    return true;
                }
                cur = cur->right_; // 否则向右子树继续查找
            }
            else
            {
                return false; // 如果树中已存在相同值则不插入返回false
            }
        }
    }

    // 插入递归版本
    bool insert_recursive(const T &val)
    {
        if (root_ == nullptr)
        {
            root_ = new Node<T>(val);
            return true;
        }

        return insert_recursive_helper(root_, val);
    }

    // 删除节点
    void delete_node(const T &val)
    {
        Node<T> *cur = root_;
        Node<T> *parent = nullptr;

        // 第一步：查找要删除的节点及其父节点
        while (cur != nullptr && cur->data_ != val)
        {
            parent = cur;
            if (val < cur->data_)
                cur = cur->left_;
            else
                cur = cur->right_;
        }

        if (cur == nullptr)
            return; // 没有找到要删除的节点

        // 第二步：删除节点
        // 情况1：节点有两个子节点
        if (cur->left_ != nullptr && cur->right_ != nullptr)
        {
            Node<T> *successor = cur->right_;
            Node<T> *successor_parent = cur;
            // 查找右子树中最小节点
            while (successor->left_ != nullptr)
            {
                successor_parent = successor;
                successor = successor->left_;
            }

            cur->data_ = successor->data_;
            cur = successor;
            parent = successor_parent;
        }

        Node<T> *child = (cur->left_ != nullptr) ? cur->left_ : cur->right_;
        if (parent == nullptr)
            root_ = child;
        else if (parent->left_ = cur)
            parent->left_ = child;
        else
            parent->right_ = child;

        // 释放节点
        delete cur;
    }

    bool delete_recursive(const T &val)
    {
        delete_recursive_helper(root_, val);
    }

        // 查找非递归版本
    Node<T> *find(T val)
    {
        Node<T> *cur = root_;
        while (cur != nullptr)
        {
            if (val < cur->data_)
                cur = cur->left_;
            else if (val > cur->data_)
                cur = cur->right_;
            else
                return cur; // 找到返回即可
        }
        return nullptr; // 未找到返回nullptr
    }

    // 查找递归版本
    Node<T> *find_recursive(const T &val)
    {
        find_recursive(root_, val);
    }

    // 中序遍历
    void inorder()
    {
        inorder_helper(root_);
    }

private:
    Node<T> *root_;

    // 中序遍历辅助函数
    void inorder_helper(Node<T> *root)
    {
        if (root == nullptr)
            return;

        inorder_helper(root->left_);
        std::cout << root->data_ << " ";
        inorder_helper(root->right_);
    }

    // 递归查找辅助函数
    Node<T> *find_recursive_helper(Node<T> *root, const T &val)
    {
        if (root == nullptr)
            return nullptr;
        else if (val < root->data_)
            find_recursive_helper(root->left_, val);
        else if (val > root->data_)
            find_recursive_helper(root->right_, val);
        else
            return root;
    }

    // 递归插入辅助函数
    bool insert_recursive_helper(Node<T> *root, const T &val)
    {
        if (val < root->data_)
        {
            if (root->left_ == nullptr)
            {
                root->left_ = new Node<T>(val);
                return true;
            }
            return insert_recursive_helper(root->left_, val);
        }
        else if (val > root->data_)
        {
            if (root->right_ == nullptr)
            {
                root->right_ = new Node<T>(val);
                return true;
            }
            return insert_recursive_helper(root->right_, val);
        }
        else
            return false;
    }

    bool delete_recursiv_helper(Node<T> *&root, const T &val)
    {
        if (root == nullptr)
            return false;

        if (val < root->data_)
        {
            return delete_recursiv_helper(root->left_, val);
        }
        else if (val > root->data_)
        {
            return delete_recursiv_helper(root->right_, val);
        }
        else
        {
            // 找到要删除的节点
            if (root->left_ == nullptr)
            {
                Node<T> *temp = root;
                root = root->right_; // 右子节点提升为新的根节点
                delete temp;         // 删除当前节点
            }
            else if (root->right_ == nullptr)
            {
                Node<T> *temp = root;
                root = root->left_; // 左子节点提升为新的根节点
                delete temp;        // 删除当前节点
            }
            else
            {
                // 节点有两个子节点，找到右子树的最小节点
                Node<T> *&minNode = findMin(root->right_);
                root->data_ = minNode->data_;                                 // 将最小节点的值复制到当前节点
                return delete_recursive_helper(root->right_, minNode->data_); // 删除右子树中的最小节点
            }
            return true; // 成功删除节点
        }
    }

    // 辅助函数：找到以root为根的树中的最小节点
    Node<T> *&findMin(Node<T> *&root)
    {
        if (root->left_ == nullptr)
            return root;
        else
            return findMin(root->left_);
    }
};