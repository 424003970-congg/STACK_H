#include <iostream>
#include <string>
#include <vector>
#include <cctype>
#include <cmath>

using namespace std;

template <typename T>
class Stack {
    struct Node {
        T val;
        Node* next;
        Node(const T& v, Node* n) : val(v), next(n) {}
    };
    Node* head = nullptr;
    int count = 0;

public:
    Stack() = default;
    ~Stack() { T dummy; while (pop(dummy)); }
    Stack(const Stack&) = delete;
    Stack& operator=(const Stack&) = delete;

    void push(const T& v) { head = new Node(v, head); count++; }

    bool pop(T& v) {
        if (!head) return false;
        Node* tmp = head;
        v = head->val;
        head = head->next;
        delete tmp;
        count--;
        return true;
    }

    bool peek(T& v) const {
        if (!head) return false;
        v = head->val;
        return true;
    }

    bool isEmpty() const { return !head; }
    int size() const { return count; }

    void print() const {
        vector<T> items;
        for (Node* cur = head; cur; cur = cur->next) items.push_back(cur->val);
        cout << "[ ";
        for (int i = (int)items.size() - 1; i >= 0; i--) cout << items[i] << " ";
        cout << "]";
    }
};

bool isOpen(char c) { return c == '(' || c == '[' || c == '{'; }
bool isClose(char c) { return c == ')' || c == ']' || c == '}'; }
bool isOp(char c) { return c == '+' || c == '-' || c == '*' || c == '/'; }
int prec(char c) { return (c == '*' || c == '/') ? 2 : (c == '+' || c == '-') ? 1 : 0; }
bool match(char a, char b) {
    return (a == '(' && b == ')') || (a == '[' && b == ']') || (a == '{' && b == '}');
}

bool checkBrackets(const string& s, string& err) {
    Stack<char> st;
    for (char c : s) {
        if (isOpen(c)) st.push(c);
        else if (isClose(c)) {
            char t;
            if (!st.pop(t)) { err = "Unmatched closing bracket"; return false; }
            if (!match(t, c)) { err = "Mismatched brackets"; return false; }
        }
    }
    if (!st.isEmpty()) { err = "Unclosed opening bracket"; return false; }
    return true;
}

bool toPostfix(const string& s, vector<string>& out, string& err) {
    out.clear();
    Stack<char> ops;
    bool needOperand = true;

    for (size_t i = 0; i < s.size();) {
        char c = s[i];
        if (isspace((unsigned char)c)) { i++; continue; }

        if (isdigit((unsigned char)c)) {
            if (!needOperand) { err = "Missing operator"; return false; }
            string num;
            while (i < s.size() && isdigit((unsigned char)s[i])) num += s[i++];
            if (i < s.size() && s[i] == '.') { err = "Decimals unsupported"; return false; }
            out.push_back(num);
            needOperand = false;
        } else if (isOpen(c)) {
            if (!needOperand) { err = "Implicit multiplication"; return false; }
            ops.push('(');
            needOperand = true;
            i++;
        } else if (isClose(c)) {
            if (needOperand) { err = "Empty group or missing operand"; return false; }
            char t;
            while (ops.pop(t) && t != '(') out.push_back(string(1, t));
            needOperand = false;
            i++;
        } else if (isOp(c)) {
            if (needOperand) { err = "Invalid operator or unary sign"; return false; }
            char t;
            while (ops.peek(t) && t != '(' && prec(t) >= prec(c)) {
                ops.pop(t);
                out.push_back(string(1, t));
            }
            ops.push(c);
            needOperand = true;
            i++;
        } else {
            err = "Unsupported character";
            return false;
        }
    }

    if (out.empty() && ops.isEmpty()) { err = "Empty expression"; return false; }
    if (needOperand) { err = "Trailing operator"; return false; }

    char t;
    while (ops.pop(t)) out.push_back(string(1, t));
    return true;
}

bool evalPostfix(const vector<string>& pf, double& res, string& err) {
    Stack<double> st;
    for (const string& tok : pf) {
        if (tok.size() == 1 && isOp(tok[0])) {
            double r, l;
            if (!st.pop(r) || !st.pop(l)) { err = "Missing operands"; return false; }
            char op = tok[0];
            if (op == '/' && r == 0) { err = "Division by zero"; return false; }
            double v = (op == '+') ? l + r : (op == '-') ? l - r : (op == '*') ? l * r : l / r;
            if (!isfinite(v)) { err = "Non-finite result"; return false; }
            st.push(v);
        } else {
            try { st.push(stod(tok)); }
            catch (...) { err = "Invalid number"; return false; }
        }
    }
    if (st.size() != 1) { err = "Malformed expression"; return false; }
    st.pop(res);
    return true;
}

void demo() {
    Stack<int> s;
    int v;
    cout << "pop empty: " << s.pop(v) << " | size: " << s.size() << "\n";
    s.push(10); s.push(20); s.peek(v);
    cout << "push 10, 20 -> peek: " << v << " | size: " << s.size() << " | "; s.print(); cout << "\n";
    s.push(30);
    cout << "push 30 -> size: " << s.size() << " | "; s.print(); cout << "\n";
    for (int i = 0; i < 3; i++) { s.pop(v); cout << "pop: " << v << " | size: " << s.size() << "\n"; }
    cout << "peek empty: " << s.peek(v) << " | size: " << s.size() << "\n";
}

int main() {
    while (true) {
        cout << "\n1. Check brackets\n2. Convert and evaluate\n3. Stack demo\n0. Exit\nChoice: ";
        string ch;
        if (!getline(cin, ch) || ch == "0") break;

        if (ch == "1") {
            cout << "Expression: ";
            string expr, err;
            getline(cin, expr);
            cout << (checkBrackets(expr, err) ? "Balanced\n" : "Error: " + err + "\n");
        } else if (ch == "2") {
            cout << "Expression: ";
            string expr, err;
            getline(cin, expr);
            cout << "Input: " << expr << "\n";
            if (!checkBrackets(expr, err)) { cout << "Bracket status: Invalid (" << err << ")\n"; continue; }
            cout << "Bracket status: Balanced\n";

            vector<string> pf;
            if (!toPostfix(expr, pf, err)) { cout << "Error: " << err << "\n"; continue; }

            cout << "Postfix: ";
            for (size_t i = 0; i < pf.size(); i++) cout << pf[i] << (i + 1 < pf.size() ? " " : "\n");

            double res;
            if (!evalPostfix(pf, res, err)) { cout << "Error: " << err << "\n"; continue; }
            cout << "Result: " << res << "\n";
        } else if (ch == "3") {
            demo();
        } else {
            cout << "Invalid choice\n";
        }
    }
    return 0;
}
