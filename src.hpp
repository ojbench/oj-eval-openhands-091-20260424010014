
#ifndef SRC_HPP
#define SRC_HPP

#include <iostream>
#include <memory>
#include <utility>
#include <stdexcept>

template<class Key, class Compare = std::less<Key>>
class ESet {
private:
    enum Color { RED, BLACK };
    
    struct Node {
        Key key;
        Color color;
        Node* left;
        Node* right;
        Node* parent;
        
        Node(const Key& k) : key(k), color(RED), left(nullptr), right(nullptr), parent(nullptr) {}
        Node(Key&& k) : key(std::move(k)), color(RED), left(nullptr), right(nullptr), parent(nullptr) {}
    };
    
    Node* root;
    Node* NIL;
    size_t sz;
    Compare comp;
    
    void init() {
        NIL = new Node(Key());
        NIL->color = BLACK;
        NIL->left = NIL->right = NIL->parent = NIL;
        root = NIL;
        sz = 0;
    }
    
    void destroy(Node* node) {
        if (node != NIL) {
            destroy(node->left);
            destroy(node->right);
            delete node;
        }
    }
    
    Node* copyTree(Node* node, Node* parent, Node* nil) {
        if (node == nil) return NIL;
        Node* newNode = new Node(node->key);
        newNode->color = node->color;
        newNode->parent = parent;
        newNode->left = copyTree(node->left, newNode, nil);
        newNode->right = copyTree(node->right, newNode, nil);
        return newNode;
    }
    
    void leftRotate(Node* x) {
        Node* y = x->right;
        x->right = y->left;
        if (y->left != NIL) {
            y->left->parent = x;
        }
        y->parent = x->parent;
        if (x->parent == NIL) {
            root = y;
        } else if (x == x->parent->left) {
            x->parent->left = y;
        } else {
            x->parent->right = y;
        }
        y->left = x;
        x->parent = y;
    }
    
    void rightRotate(Node* y) {
        Node* x = y->left;
        y->left = x->right;
        if (x->right != NIL) {
            x->right->parent = y;
        }
        x->parent = y->parent;
        if (y->parent == NIL) {
            root = x;
        } else if (y == y->parent->right) {
            y->parent->right = x;
        } else {
            y->parent->left = x;
        }
        x->right = y;
        y->parent = x;
    }
    
    void insertFixup(Node* z) {
        while (z->parent->color == RED) {
            if (z->parent == z->parent->parent->left) {
                Node* y = z->parent->parent->right;
                if (y->color == RED) {
                    z->parent->color = BLACK;
                    y->color = BLACK;
                    z->parent->parent->color = RED;
                    z = z->parent->parent;
                } else {
                    if (z == z->parent->right) {
                        z = z->parent;
                        leftRotate(z);
                    }
                    z->parent->color = BLACK;
                    z->parent->parent->color = RED;
                    rightRotate(z->parent->parent);
                }
            } else {
                Node* y = z->parent->parent->left;
                if (y->color == RED) {
                    z->parent->color = BLACK;
                    y->color = BLACK;
                    z->parent->parent->color = RED;
                    z = z->parent->parent;
                } else {
                    if (z == z->parent->left) {
                        z = z->parent;
                        rightRotate(z);
                    }
                    z->parent->color = BLACK;
                    z->parent->parent->color = RED;
                    leftRotate(z->parent->parent);
                }
            }
        }
        root->color = BLACK;
    }
    
    void transplant(Node* u, Node* v) {
        if (u->parent == NIL) {
            root = v;
        } else if (u == u->parent->left) {
            u->parent->left = v;
        } else {
            u->parent->right = v;
        }
        v->parent = u->parent;
    }
    
    Node* minimum(Node* node) {
        while (node->left != NIL) {
            node = node->left;
        }
        return node;
    }
    
    void deleteFixup(Node* x) {
        while (x != root && x->color == BLACK) {
            if (x == x->parent->left) {
                Node* w = x->parent->right;
                if (w->color == RED) {
                    w->color = BLACK;
                    x->parent->color = RED;
                    leftRotate(x->parent);
                    w = x->parent->right;
                }
                if (w->left->color == BLACK && w->right->color == BLACK) {
                    w->color = RED;
                    x = x->parent;
                } else {
                    if (w->right->color == BLACK) {
                        w->left->color = BLACK;
                        w->color = RED;
                        rightRotate(w);
                        w = x->parent->right;
                    }
                    w->color = x->parent->color;
                    x->parent->color = BLACK;
                    w->right->color = BLACK;
                    leftRotate(x->parent);
                    x = root;
                }
            } else {
                Node* w = x->parent->left;
                if (w->color == RED) {
                    w->color = BLACK;
                    x->parent->color = RED;
                    rightRotate(x->parent);
                    w = x->parent->left;
                }
                if (w->right->color == BLACK && w->left->color == BLACK) {
                    w->color = RED;
                    x = x->parent;
                } else {
                    if (w->left->color == BLACK) {
                        w->right->color = BLACK;
                        w->color = RED;
                        leftRotate(w);
                        w = x->parent->left;
                    }
                    w->color = x->parent->color;
                    x->parent->color = BLACK;
                    w->left->color = BLACK;
                    rightRotate(x->parent);
                    x = root;
                }
            }
        }
        x->color = BLACK;
    }
    
    Node* findNode(const Key& key) const {
        Node* current = root;
        while (current != NIL) {
            if (comp(key, current->key)) {
                current = current->left;
            } else if (comp(current->key, key)) {
                current = current->right;
            } else {
                return current;
            }
        }
        return NIL;
    }
    
    Node* lowerBoundNode(const Key& key) const {
        Node* current = root;
        Node* result = NIL;
        while (current != NIL) {
            if (!comp(current->key, key)) {
                result = current;
                current = current->left;
            } else {
                current = current->right;
            }
        }
        return result;
    }
    
    Node* upperBoundNode(const Key& key) const {
        Node* current = root;
        Node* result = NIL;
        while (current != NIL) {
            if (comp(key, current->key)) {
                result = current;
                current = current->left;
            } else {
                current = current->right;
            }
        }
        return result;
    }
    
    size_t countRange(Node* node, const Key& l, const Key& r) const {
        if (node == NIL) return 0;
        if (comp(node->key, l)) {
            return countRange(node->right, l, r);
        } else if (comp(r, node->key)) {
            return countRange(node->left, l, r);
        } else {
            return 1 + countRange(node->left, l, r) + countRange(node->right, l, r);
        }
    }
    
public:
    class iterator {
    private:
        Node* node;
        Node* nil;
        const ESet* set;
        
    public:
        iterator() : node(nullptr), nil(nullptr), set(nullptr) {}
        iterator(Node* n, Node* nilNode, const ESet* s) : node(n), nil(nilNode), set(s) {}
        
        const Key& operator*() const {
            if (node == nil || node == nullptr) {
                throw std::out_of_range("Iterator dereference end()");
            }
            return node->key;
        }
        
        iterator& operator++() {
            if (node == nil || node == nullptr) return *this;
            
            Node* current = node;
            if (current->right != nil) {
                current = current->right;
                while (current->left != nil) {
                    current = current->left;
                }
            } else {
                Node* parent = current->parent;
                while (parent != nil && current == parent->right) {
                    current = parent;
                    parent = parent->parent;
                }
                current = parent;
            }
            if (current == nullptr) {
                current = nil;
            }
            node = current;
            return *this;
        }
        
        iterator operator++(int) {
            iterator tmp = *this;
            ++(*this);
            return tmp;
        }
        
        iterator& operator--() {
            if (node == nullptr || set == nullptr) {
                return *this;
            }
            
            if (node == nil) {
                if (set && set->root != nil) {
                    node = set->root;
                    while (node->right != nil) {
                        node = node->right;
                    }
                }
                return *this;
            }
            
            Node* current = node;
            if (current->left != nil) {
                current = current->left;
                while (current->right != nil) {
                    current = current->right;
                }
            } else {
                Node* parent = current->parent;
                while (parent != nil && current == parent->left) {
                    current = parent;
                    parent = parent->parent;
                }
                current = parent;
            }
            if (current == nullptr) {
                current = nil;
            }
            node = current;
            return *this;
        }
        
        iterator operator--(int) {
            iterator tmp = *this;
            --(*this);
            return tmp;
        }
        
        bool operator==(const iterator& other) const {
            return node == other.node;
        }
        
        bool operator!=(const iterator& other) const {
            return node != other.node;
        }
        
        bool isEnd() const {
            return node == nil || node == nullptr;
        }
        
        Node* getNode() const { return node; }
    };
    
    ESet() : comp(Compare()) {
        init();
    }
    
    ~ESet() {
        destroy(root);
        delete NIL;
    }
    
    ESet(const ESet& other) : comp(other.comp) {
        init();
        root = copyTree(other.root, NIL, other.NIL);
        sz = other.sz;
    }
    
    ESet& operator=(const ESet& other) {
        if (this != &other) {
            destroy(root);
            delete NIL;
            comp = other.comp;
            init();
            root = copyTree(other.root, NIL, other.NIL);
            sz = other.sz;
        }
        return *this;
    }
    
    ESet(ESet&& other) noexcept : root(other.root), NIL(other.NIL), sz(other.sz), comp(other.comp) {
        other.root = other.NIL = nullptr;
        other.sz = 0;
    }
    
    ESet& operator=(ESet&& other) noexcept {
        if (this != &other) {
            destroy(root);
            delete NIL;
            
            root = other.root;
            NIL = other.NIL;
            sz = other.sz;
            comp = other.comp;
            
            other.root = other.NIL = nullptr;
            other.sz = 0;
        }
        return *this;
    }
    
    template<class... Args>
    std::pair<iterator, bool> emplace(Args&&... args) {
        Key key(std::forward<Args>(args)...);
        Node* y = NIL;
        Node* x = root;
        
        while (x != NIL) {
            y = x;
            if (comp(key, x->key)) {
                x = x->left;
            } else if (comp(x->key, key)) {
                x = x->right;
            } else {
                return std::make_pair(iterator(x, NIL, this), false);
            }
        }
        
        Node* z = new Node(std::move(key));
        z->parent = y;
        
        if (y == NIL) {
            root = z;
        } else if (comp(z->key, y->key)) {
            y->left = z;
        } else {
            y->right = z;
        }
        
        z->left = NIL;
        z->right = NIL;
        z->color = RED;
        
        insertFixup(z);
        sz++;
        
        return std::make_pair(iterator(z, NIL, this), true);
    }
    
    size_t erase(const Key& key) {
        Node* z = findNode(key);
        if (z == NIL) {
            return 0;
        }
        
        Node* y = z;
        Node* x;
        Color yOriginalColor = y->color;
        
        if (z->left == NIL) {
            x = z->right;
            transplant(z, z->right);
        } else if (z->right == NIL) {
            x = z->left;
            transplant(z, z->left);
        } else {
            y = minimum(z->right);
            yOriginalColor = y->color;
            x = y->right;
            if (y->parent == z) {
                x->parent = y;
            } else {
                transplant(y, y->right);
                y->right = z->right;
                y->right->parent = y;
            }
            transplant(z, y);
            y->left = z->left;
            y->left->parent = y;
            y->color = z->color;
        }
        
        delete z;
        
        if (yOriginalColor == BLACK) {
            deleteFixup(x);
        }
        
        sz--;
        return 1;
    }
    
    iterator find(const Key& key) const {
        Node* node = findNode(key);
        return iterator(node, NIL, this);
    }
    
    size_t range(const Key& l, const Key& r) const {
        if (comp(r, l)) return 0;
        return countRange(root, l, r);
    }
    
    size_t size() const noexcept {
        return sz;
    }
    
    iterator lower_bound(const Key& key) const {
        Node* node = lowerBoundNode(key);
        return iterator(node, NIL, this);
    }
    
    iterator upper_bound(const Key& key) const {
        Node* node = upperBoundNode(key);
        return iterator(node, NIL, this);
    }
    
    iterator begin() const noexcept {
        if (root == NIL) {
            return iterator(NIL, NIL, this);
        }
        Node* node = root;
        while (node->left != NIL) {
            node = node->left;
        }
        return iterator(node, NIL, this);
    }
    
    iterator end() const noexcept {
        return iterator(NIL, NIL, this);
    }
};

#endif
