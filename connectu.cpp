/*
 * PROJECT: CONNECT-U (Starter Code)
 * Course: ECE367L Data Structures & Algorithms
 * Author: Kenneth Hironaka
 * 
 * SAFETY UPDATE:
 * The saveData() function call in main() is currently COMMENTED OUT.
 * This prevents you from accidentally wiping your 'posts.csv' file
 * if your Lab 1 implementation is incomplete.
 * * ONLY uncomment saveData() after you have verified Lab 1 works!
 */

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <ctime>
#include <vector>
#include <set>
#include <queue>
#include <algorithm>

using namespace std;

// ==========================================
// MODELS & DATA STRUCTURES
// ==========================================

struct Post {
    int postId;
    int userId;
    string content;
    int likes;
    long timestamp;
    Post* next; 

    Post(int pid, int uid, string txt, int lk, long time) 
        : postId(pid), userId(uid), content(txt), likes(lk), timestamp(time), next(nullptr) {}
        
    // TODO: LAB 3 - Implement Scoring Logic
    double getScore() {
        double hoursOld = (time(0) - timestamp)/3600.0;  // Calculates number of hours old the post is
        double score = (likes * 10) + (1000/(hoursOld + 1));  // Formula for scoring
        return score; 
    }
};

// TODO: LAB 1 - Linked List
class Timeline {
public:
    Post* head;
    Timeline() : head(nullptr) {}

    // Task: Add a new post to the FRONT of the list (O(1))
    void addPost(int pid, int uid, string content, int likes, long time) {
        Post* new_Post = new Post(pid, uid, content, likes, time); // Creates new post object
        new_Post->next = head;  // Connects the new post to the head of the linked list
        head = new_Post;  // The new post becomes the head of the linked list
    }

    void printTimeline() {
        Post* current = head;
        if (!current) { cout << "  (No posts yet)" << endl; return; }
        else {
            for (Post* p = head; p != nullptr; p = p->next){  // Traverse the linked list
                cout << p->content << endl;  // Print out the content of each post
            }
        }
    }
};

// Forward Declaration
class User;

// TODO: LAB 4 - Binary Search Tree
struct BSTNode {
    User* user;
    BSTNode* left;
    BSTNode* right;
    BSTNode(User* u) : user(u), left(nullptr), right(nullptr) {}
};

class FriendBST {
public:
    BSTNode* root;
    FriendBST() : root(nullptr) {}

    BSTNode* insert(BSTNode* node, User* u) ;

    void printInOrder(BSTNode* node);

    void addFriend(User* u) { root = insert(root, u); }

    void printFriends() {
        if (root == nullptr) cout << "  (No friends yet)" << endl;
        else printInOrder(root);
    }
};

class User {
public:
    int userId;
    string username;
    int techScore, artScore, sportScore;
    
    Timeline timeline;       // Lab 1
    vector<User*> friends;   // Graph
    FriendBST friendTree;    // Lab 4

    User(int id, string name, int t, int a, int s) 
        : userId(id), username(name), techScore(t), artScore(a), sportScore(s) {}

    void addPost(int pid, string content, int likes, long time) {
        timeline.addPost(pid, userId, content, likes, time);
    }

    void addFriend(User* u) {
        friends.push_back(u);       
        friendTree.addFriend(u);    
    }
    
    vector<User*> getFriendsList() { return friends; }
};

// BST Implementation
BSTNode* FriendBST::insert(BSTNode* node, User* u) {
    if(node == nullptr){  // Place to put the user was found
        struct BSTNode* newFriend = new BSTNode(u);  // Create BSTNode for the user    
        return newFriend;
    };
    
    if(u->username < node->user->username){  // The user should go to the left of the current node
        node->left = insert(node->left, u);
    }else if(u->username > node->user->username){  // The user should go to the right of the current node
        node->right = insert(node->right, u);
    }
    return node;  // Return the node if the user is already in the BST
}
void FriendBST::printInOrder(BSTNode* node) {
    if(node == nullptr) return;  // Base case

    printInOrder(node->left);  // Process the left node
    cout << node->user->username << endl;  // Process (print) the current node
    printInOrder(node->right);  // Process the right node
    return;
}

// TODO: LAB 3 - Max Heap
class FeedHeap {
private:
    Post* heap[1000]; 
    int size;

    void heapifyDown(int index) { 
        int leftchild = 2*index + 1;
        if(leftchild >= size){  // If the node does not have a left child
            return;
        }
        int bigchild = leftchild;
        int rightchild = 2*index + 2;
        if(rightchild < size){  // If the node has a right child
            if(heap[rightchild]->getScore() > heap[leftchild]->getScore()){  // If the right child's score 
                                                                             // is bigger than the left child's score
                bigchild = rightchild;
            }
        }
        // If the biggest child's score is greater than the current node's score,
        // swap the nodes and check if the current node needs to be heapified down again
        if(heap[bigchild]->getScore() > heap[index]->getScore()){   
            Post *temp = heap[index];
            heap[index] = heap[bigchild];
            heap[bigchild] = temp;
            heapifyDown(bigchild);
        }
        return;
    }
    void heapifyUp(int index) { 
        if(index == 0){  // If node to heapifyUp is at the top of the heap, return
            return;
        }
        while(index != 0){
            int parent = (int)((index-1)/2);  
            // If the current node's score is higher than it's parent, swap the nodes
            // and parent node becomes the new current node 
            if(heap[index]->getScore() > heap[parent]->getScore()){ 
                Post *temp = heap[parent];
                heap[parent] = heap[index];
                heap[index] = temp;
                index = parent;
            }else{  // No need to heapify up
                break;
            }
        }
        return;
    }

public:
    FeedHeap() : size(0) {}
    void push(Post* p) {
        heap[size] = p;  // Add post to the end of the heap
        size++;  // Increment size
        heapifyUp(size-1);  // Heapify up the appended post
    }
    Post* popMax() {
        if(size == 0){  // If the array is empty
            return nullptr;
        }
        Post *maxPost = heap[0];  // Save the post with the highest score
        heap[0] = heap[size-1];  // Put the last post at the top of the heap
        size--;
        if(size != 0){  // If the heap isn't empty after removing the post with the highest score
            heapifyDown(0);  // Heapify down the last post at the top of the heap
        }
        return maxPost;
    }
    bool isEmpty() { return size == 0; }
};

vector<User*> allUsers;

// TODO: LAB 2 - Hash Map
struct HashNode {
    string key;
    User* value;
    HashNode* next;
    HashNode(string k, User* v) : key(k), value(v), next(nullptr) {}
};

class UserMap {
private:
    static const int TABLE_SIZE = 10007; 
    HashNode** table;

    unsigned long hashFunction(string key) {
        size_t hashValue = 0;
        size_t base = 37;  // prime number
        size_t basePow = 1;
        // Do a polynomial rolling hash through each character of the username
        for(int i = 0; i < key.length(); i++){
            unsigned char charVal = key[i];
            hashValue = (hashValue + charVal*basePow) % TABLE_SIZE;
            basePow = (basePow * base) % TABLE_SIZE;
        }

        return (unsigned long) hashValue; 
    }

public:
    UserMap() {
        table = new HashNode*[TABLE_SIZE];
        for (int i = 0; i < TABLE_SIZE; i++) table[i] = nullptr;
    }

    void put(string key, User* user) { 
        unsigned long hash = hashFunction(key);  // Get the hash value of the username
        HashNode* newNode = new HashNode(key,user);  // Create new user
        newNode->next = table[hash];  // Add new user to the front of the LL at the index 
        table[hash] = newNode;        // that equals the hash value
    }

    User* get(string key) {
        unsigned long hash = hashFunction(key);
        HashNode* curr = table[hash];  // Find the table entry that corresponds
                                       // with the hash value
        // Loop through the LL until the node's key matches the key it is looking for
        // and return that node's value (User profile)
        while(curr != nullptr){
            if(curr->key == key){
                return curr->value;
            }
            curr = curr->next;
        }

        return nullptr;  // If the key is not found, return nullptr 
    }
};

UserMap userMap;

// ==========================================
// UTILITY FUNCTIONS
// ==========================================

vector<string> split(string s) {
    vector<string> tokens;
    string token;
    bool inQuotes = false;
    
    for (char c : s) {
        if (c == '"') {
            inQuotes = !inQuotes; // Toggle quote state
            continue; // Skip the quote character itself
        }
        
        if (c == ',' && !inQuotes) {
            // Found a delimiter outside of quotes -> New Token
            tokens.push_back(token);
            token.clear();
        } else {
            // Regular character or comma inside quotes
            token += c;
        }
    }
    tokens.push_back(token); // Add last token
    return tokens;
}


int GLOBAL_POST_ID_COUNTER = 1;

Post* findPostById(int id) {
    for (User* u : allUsers) {
        Post* curr = u->timeline.head;
        while (curr != nullptr) {
            if (curr->postId == id) return curr;
            curr = curr->next;
        }
    }
    return nullptr;
}

void createNewPost(User* author, string content) {
    int postId = GLOBAL_POST_ID_COUNTER++;
    long timestamp = time(0); 
    author->addPost(postId, content, 0, timestamp);
    cout << "\n[SUCCESS] Post saved to timeline." << endl;
}

void registerNewUser(string username, int tech, int art, int sport) {
    int newId = allUsers.size() + 1; 
    User* newUser = new User(newId, username, tech, art, sport);
    allUsers.push_back(newUser);
    userMap.put(username, newUser);
    cout << "\n[SUCCESS] Account created." << endl;
}

void addFriendship(User* requester, User* target) {
    requester->addFriend(target);
    target->addFriend(requester);
    cout << "\n[SUCCESS] You are now friends with @" << target->username << endl;
}

// TODO: LAB 5 - Breadth First Search
void recommendFriends(User* startUser) {
    cout << "\n[GRAPH ANALYSIS] Finding friends of friends..." << endl;
    if(startUser->friendTree.root == nullptr){  // Check if the user has any friends
        cout << "You have no friends :(" << endl;
        return;
    }
    
    set<int> friendsSeen;  // Set for user IDs that are friends or have been recommended
    queue<User*> directFriends;  // Queue for users that are direct friends of startUser
    // For every direct friend, add them to the queue and their user ID to the set
    for(User* f : startUser->friends){  
        directFriends.push(f);
        friendsSeen.insert(f->userId);
    }

    User* currentFriend;
    bool recommendedFriends = false;  
    while(directFriends.empty() == false){ 
        currentFriend = directFriends.front();  // Set currentFriend to the user at the front of the queue
        directFriends.pop();  
        for(User* g : currentFriend->friends){  // For all of currentFriend's friends
            // If the user is not startUser and is not already friends or been recommended to startUser
            if(g != startUser && friendsSeen.find(g->userId) == friendsSeen.end()){
                friendsSeen.insert(g->userId);  // Add the user's user id to the set
                cout << g->username << endl;  // Print out the user's username as a recommendation
                recommendedFriends = true;  // A friend recommendation has been found
            }
        }
    }
    if(recommendedFriends == false){  // Friend recommendations have not been found
        cout << "We could not find any friend recommendations." << endl;
    }
    return;
}

// ==========================================
// FILE I/O 
// ==========================================

void loadData() {
    cout << "Loading data from CSV files..." << endl;
    ifstream userFile("users.csv");
    string line;
    if (userFile.is_open()) {
        getline(userFile, line); 
        while (getline(userFile, line)) {
            vector<string> row = split(line);
            if (row.size() < 5) continue;
            User* newUser = new User(stoi(row[0]), row[1], stoi(row[2]), stoi(row[3]), stoi(row[4]));
            allUsers.push_back(newUser);
            userMap.put(row[1], newUser); 
        }
        userFile.close();
    }
    ifstream relFile("relations.csv");
    if (relFile.is_open()) {
        getline(relFile, line); 
        while (getline(relFile, line)) {
            vector<string> row = split(line);
            if (row.size() < 2) continue;
            int u1 = stoi(row[0]); int u2 = stoi(row[1]);
            if (u1 <= allUsers.size() && u2 <= allUsers.size()) {
                allUsers[u1-1]->addFriend(allUsers[u2-1]);
                allUsers[u2-1]->addFriend(allUsers[u1-1]);
            }
        }
        relFile.close();
    }
    ifstream postFile("posts.csv");
    if (postFile.is_open()) {
        getline(postFile, line); 
        while (getline(postFile, line)) {
            vector<string> row = split(line);
            if (row.size() < 5) continue;
            int pid = stoi(row[0]); int uid = stoi(row[1]);
            if (uid <= allUsers.size()) {
                allUsers[uid-1]->addPost(pid, row[2], stoi(row[3]), stol(row[4])); 
                if (pid >= GLOBAL_POST_ID_COUNTER) GLOBAL_POST_ID_COUNTER = pid + 1;
            }
        }
        postFile.close();
    }
}

void saveData() {
    // SAFETY CHECK: If no users exist, do not overwrite files!
    if (allUsers.empty()) {
        cout << "[SAFETY] No data in memory. Skipping save to prevent file wipe." << endl;
        return;
    }

    cout << "Saving data..." << endl;
    ofstream userFile("users.csv");
    userFile << "user_id,username,tech_score,art_score,sport_score\n";
    for (User* u : allUsers) {
        userFile << u->userId << "," << u->username << "," << u->techScore << "," << u->artScore << "," << u->sportScore << "\n";
    }
    userFile.close();

    ofstream relFile("relations.csv");
    relFile << "user_id_1,user_id_2\n";
    for (User* u : allUsers) {
        for (User* f : u->friends) {
            if (u->userId < f->userId) relFile << u->userId << "," << f->userId << "\n";
        }
    }
    relFile.close();

    ofstream postFile("posts.csv");
    postFile << "post_id,user_id,content,likes,timestamp\n";
    for (User* u : allUsers) {
        vector<Post*> temp;
        Post* curr = u->timeline.head;
        while (curr) { temp.push_back(curr); curr = curr->next; }
        for (int i = temp.size() - 1; i >= 0; i--) {
            Post* p = temp[i];
            string safeContent = p->content;
            if (safeContent.find(',') != string::npos) {
                safeContent = "\"" + safeContent + "\"";
            } 
            postFile << p->postId << "," << p->userId << "," << safeContent << "," << p->likes << "," << p->timestamp << "\n";
        }
    }
    postFile.close();
    cout << "Done." << endl;
}

// ==========================================
// MAIN MENU (UI)
// ==========================================

void clearScreen() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

void showUserDashboard(User* currentUser) {
    int choice = 0;
    while (choice != 7) {
        cout << "\n--- Welcome, @" << currentUser->username << " ---" << endl;
        cout << "1. View My Post (Lab 1)" << endl;
        cout << "2. Create New Post (Lab 1)" << endl;
        cout << "3. Add Friend (Lab 2)" << endl;
        cout << "4. Algorithmic Feed (Lab 3)" << endl;
        cout << "5. View Friends Sorted (Lab 4)" << endl;
        cout << "6. Get Friend Recommendations (Lab 5)" << endl;
        cout << "7. Logout" << endl;
        cout << "Select >> ";
        cin >> choice;

        if (choice == 1) {
            cout << "\n[MY POST]" << endl;
            currentUser->timeline.printTimeline();
        }
        else if (choice == 2) {
            cout << "\nEnter post content: ";
            cin.ignore(); 
            string content;
            getline(cin, content);
            createNewPost(currentUser, content);
        }
        else if (choice == 3) {
            string friendName;
            cout << "Enter username to add: "; cin >> friendName;
            User* target = userMap.get(friendName);
            if(target && target != currentUser) addFriendship(currentUser, target);
            else cout << "Invalid user (or Hash Map not implemented)." << endl;
        }
        else if (choice == 4) {
            cout << "\n[ALGORITHMIC FEED]" << endl;
            FeedHeap feed;
            vector<User*> friends = currentUser->getFriendsList();
            for(User* f : friends) {
                Post* p = f->timeline.head;
                int limit = 0;
                while(p != nullptr && limit < 5) {
                    feed.push(p);
                    p = p->next;
                    limit++;
                }
            }
            int count = 0;
            while(!feed.isEmpty() && count < 10) {
                Post* top = feed.popMax();
                if(top){
                    cout << "  > [ID: " << top->postId << "] [Score: " << (int)top->getScore() << "] @" 
                         << allUsers[top->userId - 1]->username << ": " << top->content 
                         << " (" << top->likes << " likes)" << endl;
                    count++;
                }
            }
            if(count == 0) cout << "  No posts found." << endl;
            else {
                cout << "\nDo you want to like a post? (y/n): ";
                char resp; cin >> resp;
                if (resp == 'y' || resp == 'Y') {
                    int pid; cout << "Enter Post ID: "; cin >> pid;
                    Post* p = findPostById(pid);
                    if (p) { p->likes++; cout << "Liked!" << endl; }
                }
            }
        }
        else if (choice == 5) {
            cout << "\n[MY FRIENDS]" << endl;
            currentUser->friendTree.printFriends();
        }
        else if (choice == 6) {
             recommendFriends(currentUser);
        }
        else if (choice == 7) {
            cout << "Logging out..." << endl;
        }
    }
}

void showMainMenu() {
    int choice = 0;
    while (choice != 3) {
        cout << "\n=== CONNECT-U ===" << endl;
        cout << "1. Login" << endl;
        cout << "2. Register" << endl;
        cout << "3. Exit & Save" << endl;
        cout << "Select >> ";
        cin >> choice;

        if (choice == 1) {
            string username;
            cout << "Username: "; cin >> username;
            User* user = userMap.get(username);
            if (user) showUserDashboard(user);
            else cout << "User not found." << endl;
        } 
        else if (choice == 2) {
            string username;
            int t, a, s;
            cout << "Username: "; cin >> username;
            cout << "Tech/Art/Sport (1-10): "; cin >> t >> a >> s;
            registerNewUser(username, t, a, s);
        }
        else if (choice == 3) {
            saveData(); 
            cout << "Goodbye! " << endl;
        }
    }
}

int main() {
    loadData(); 
    clearScreen();
    showMainMenu();
    return 0;
}