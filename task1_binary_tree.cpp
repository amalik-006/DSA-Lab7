#include <iostream>
#include <string>
#include <stack>
#include <vector>
#include <stdexcept>
#include <cctype>
using namespace std;

// Each node stores one character and pointers to its children
struct Node {
    char data;
    Node* left;
    Node* right;

    explicit Node(char value) : data(value), left(nullptr), right(nullptr) {}
};

// Release every node to avoid memory leaks
void deleteTree(Node* root) {
    if (root == nullptr) {
        return;
    }

    deleteTree(root->left);
    deleteTree(root->right);
    delete root;
}

// Parses expressions such as a(b(d,e),c(,f(g,)))
class BinaryTreeParser {
private:
    string expression;
    size_t position;

    // Read one node and recursively read its children when parentheses follow
    Node* parseNode() {
        if (position >= expression.size() ||
            expression[position] == '(' ||
            expression[position] == ')' ||
            expression[position] == ',') {
            throw invalid_argument("Expected a node value");
        }

        char value = expression[position++];
        Node* node = new Node(value);

        try {
            if (position < expression.size() && expression[position] == '(') {
                ++position;

                // An empty left child is represented by a comma immediately after '('
                if (position < expression.size() && expression[position] != ',') {
                    node->left = parseNode();
                }

                if (position >= expression.size() || expression[position] != ',') {
                    throw invalid_argument("Expected a comma between children");
                }
                ++position;

                // An empty right child is represented by ')' immediately after ','
                if (position < expression.size() && expression[position] != ')') {
                    node->right = parseNode();
                }

                if (position >= expression.size() || expression[position] != ')') {
                    throw invalid_argument("Expected a closing parenthesis");
                }
                ++position;
            }
        } catch (...) {
            deleteTree(node);
            throw;
        }

        return node;
    }

public:
    explicit BinaryTreeParser(const string& input) : expression(input), position(0) {}

    Node* parse() {
        if (expression.empty()) {
            throw invalid_argument("The tree expression cannot be empty");
        }

        Node* root = parseNode();

        // A valid expression must be consumed completely
        if (position != expression.size()) {
            deleteTree(root);
            throw invalid_argument("Unexpected characters after the tree");
        }

        return root;
    }
};

// Task 1.2: Visit root, then left subtree, then right subtree
void preorder(Node* root) {
    if (root == nullptr) {
        return;
    }

    cout << root->data << ' ';
    preorder(root->left);
    preorder(root->right);
}

// Task 1.3: Inorder traversal without recursion, using a stack
void inorderIterative(Node* root) {
    stack<Node*> nodes;
    Node* current = root;

    while (current != nullptr || !nodes.empty()) {
        // Save ancestors while moving as far left as possible
        while (current != nullptr) {
            nodes.push(current);
            current = current->left;
        }

        current = nodes.top();
        nodes.pop();
        cout << current->data << ' ';

        // Visit the right subtree after visiting the node
        current = current->right;
    }
}

// Task 1.4: Iterative postorder using two stacks
void postorderIterative(Node* root) {
    if (root == nullptr) {
        return;
    }

    stack<Node*> first;
    stack<Node*> second;
    first.push(root);

    // The second stack reverses root-right-left into left-right-root
    while (!first.empty()) {
        Node* current = first.top();
        first.pop();
        second.push(current);

        if (current->left != nullptr) {
            first.push(current->left);
        }
        if (current->right != nullptr) {
            first.push(current->right);
        }
    }

    while (!second.empty()) {
        cout << second.top()->data << ' ';
        second.pop();
    }
}

// Task 1.5: Check the string grammar without creating any tree nodes
class TreeStringValidator {
private:
    const string& expression;
    size_t position = 0;

    void skipSpaces() {
        while (position < expression.size() &&
               isspace(static_cast<unsigned char>(expression[position]))) {
            ++position;
        }
    }

    // A node is one non-reserved character, optionally followed by (left,right)
    bool parseNode() {
        skipSpaces();

        if (position >= expression.size() ||
            expression[position] == '(' ||
            expression[position] == ')' ||
            expression[position] == ',') {
            return false;
        }

        ++position;
        skipSpaces();

        if (position < expression.size() && expression[position] == '(') {
            ++position;
            skipSpaces();

            // Left child may be empty
            if (position < expression.size() && expression[position] != ',') {
                if (!parseNode()) {
                    return false;
                }
            }

            skipSpaces();
            if (position >= expression.size() || expression[position] != ',') {
                return false;
            }
            ++position;
            skipSpaces();

            // Right child may be empty
            if (position < expression.size() && expression[position] != ')') {
                if (!parseNode()) {
                    return false;
                }
            }

            skipSpaces();
            if (position >= expression.size() || expression[position] != ')') {
                return false;
            }
            ++position;
        }

        return true;
    }

public:
    explicit TreeStringValidator(const string& input) : expression(input) {}

    bool isValid() {
        if (!parseNode()) {
            return false;
        }

        skipSpaces();
        return position == expression.size();
    }
};

bool isValidTreeString(const string& expression) {
    TreeStringValidator validator(expression);
    return validator.isValid();
}

// Task 1.6: Height is measured in nodes, so an empty tree has height 0
int height(Node* root) {
    if (root == nullptr) {
        return 0;
    }

    int leftHeight = height(root->left);
    int rightHeight = height(root->right);
    return 1 + (leftHeight > rightHeight ? leftHeight : rightHeight);
}

// Print each node's depth, where the root has depth 0
void printDepths(Node* root, int depth = 0) {
    if (root == nullptr) {
        return;
    }

    cout << "Node " << root->data << ": depth " << depth << '\n';
    printDepths(root->left, depth + 1);
    printDepths(root->right, depth + 1);
}

// Main Function

int main() {
    const string expression = "a(b(d,e),c(,f(g,)))";
    Node* root = nullptr;

    cout << "\nTASK 1: BINARY TREE REPRESENTATION AND OPERATIONS\n";
    cout << "-----------------------------------------------\n";

    // Task 1.1: Build the binary tree from the supplied expression
    try {
        BinaryTreeParser parser(expression);
        root = parser.parse();
        cout << "1. Tree construction: successful\n";
        cout << "   Expression: " << expression << "\n";
    } catch (const exception& error) {
        cerr << "Tree construction failed: " << error.what() << '\n';
        return 1;
    }

    // Task 1.2: Recursive preorder traversal
    cout << "\n2. Preorder (recursive): ";
    preorder(root);
    cout << '\n';

    // Task 1.3: Iterative inorder traversal
    cout << "\n3. Inorder (iterative, stack): ";
    inorderIterative(root);
    cout << '\n';

    // Task 1.4: Iterative postorder traversal
    cout << "\n4. Postorder (iterative, two stacks): ";
    postorderIterative(root);
    cout << '\n';

    // Task 1.5: Validate the expression independently of tree construction
    cout << "\n5. String validation: "
         << (isValidTreeString(expression) ? "valid" : "invalid") << '\n';

    // Task 1.6: Display the tree height and every node's depth
    cout << "\n6. Tree height: " << height(root) << '\n';
    cout << "   Node depths:\n";
    printDepths(root);

    // Free all allocated nodes before the program ends
    deleteTree(root);

    return 0;
}