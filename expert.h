#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <unordered_map>

using namespace std;

// courseNode structure
struct courseNode
{
    int courseID;
    string type;
    string topic;
    string description;
    int price;
    int num_users;
    int enrollment_limit;
    string instructor_email;
    vector<string> materials;
    courseNode *left;
    courseNode *right;

    courseNode(int id, const string &t, const string &top, const string &desc, int p, int limit, const string &email, const vector<string> &mats, int n)
        : courseID(id), type(t), topic(top), description(desc), price(p), enrollment_limit(limit), instructor_email(email), materials(mats), left(nullptr), right(nullptr), num_users(n) {}
};

unordered_map<int, vector<string>> materialsMap;

// Splay Tree class
class SplayTree
{
public:
    SplayTree() : root(nullptr) {}

    // Insert a new node into the Splay Tree
    void insert(int courseID, const string &type, const string &topic, const string &description, int price, int enrollment_limit, const string &instructor_email, const vector<string> &materials, int num_users)
    {
        root = insert(root, courseID, type, topic, description, price, enrollment_limit, instructor_email, materials, num_users);
        root = splay(root, courseID);
    }

    // In-order traversal to print the tree
    void inorder()
    {
        inorder(root);
    }

private:
    courseNode *root;

    // Rotate right
    courseNode *rotateRight(courseNode *x)
    {
        courseNode *y = x->left;
        x->left = y->right;
        y->right = x;
        return y;
    }

    // Rotate left
    courseNode *rotateLeft(courseNode *x)
    {
        courseNode *y = x->right;
        x->right = y->left;
        y->left = x;
        return y;
    }

    // Splay operation
    courseNode *splay(courseNode *root, int courseID)
    {
        if (!root || root->courseID == courseID)
            return root;

        if (root->courseID > courseID)
        {
            if (!root->left)
                return root;

            if (root->left->courseID > courseID)
            {
                root->left->left = splay(root->left->left, courseID);
                root = rotateRight(root);
            }
            else if (root->left->courseID < courseID)
            {
                root->left->right = splay(root->left->right, courseID);
                if (root->left->right)
                    root->left = rotateLeft(root->left);
            }
            return (root->left == nullptr) ? root : rotateRight(root);
        }
        else
        {
            if (!root->right)
                return root;

            if (root->right->courseID > courseID)
            {
                root->right->left = splay(root->right->left, courseID);
                if (root->right->left)
                    root->right = rotateRight(root->right);
            }
            else if (root->right->courseID < courseID)
            {
                root->right->right = splay(root->right->right, courseID);
                root = rotateLeft(root);
            }
            return (root->right == nullptr) ? root : rotateLeft(root);
        }
    }

    // Insert helper
    courseNode *insert(courseNode *root, int courseID, const string &type, const string &topic, const string &description, int price, int enrollment_limit, const string &instructor_email, const vector<string> &materials, int num_users)
    {
        if (!root)
            return new courseNode(courseID, type, topic, description, price, enrollment_limit, instructor_email, materials, num_users);

        if (courseID < root->courseID)
        {
            root->left = insert(root->left, courseID, type, topic, description, price, enrollment_limit, instructor_email, materials, num_users);
        }
        else if (courseID > root->courseID)
        {
            root->right = insert(root->right, courseID, type, topic, description, price, enrollment_limit, instructor_email, materials, num_users);
        }

        return root;
    }

    // In-order traversal helper
    void inorder(courseNode *root)
    {
        if (root)
        {
            inorder(root->left);
            cout << "Course ID: " << root->courseID << " Type: " << root->type << " Topic: " << root->topic
                 << " Description: " << root->description << " Price: " << root->price << " Enrollment Limit: " << root->enrollment_limit
                 << " Instructor Email: " << root->instructor_email << " Materials: ";
            for (const auto &material : root->materials)
            {
                cout << material << " ";
            }
            cout << endl;
            inorder(root->right);
        }
    }
};

// CourseHeap class
class CourseHeap
{
private:
    vector<courseNode *> heap;

    // Function to heapify (max heap) based on number of users enrolled
    void heapify(int n, int i)
    {
        int largest = i;
        int left = 2 * i + 1;
        int right = 2 * i + 2;

        if (left < n && heap[left]->num_users > heap[largest]->num_users)
        {
            largest = left;
        }

        if (right < n && heap[right]->num_users > heap[largest]->num_users)
        {
            largest = right;
        }

        if (largest != i)
        {
            swap(heap[i], heap[largest]);
            heapify(n, largest);
        }
    }

public:
    CourseHeap() {}

    // Function to insert a node into the heap
    void insert(courseNode *node)
    {
        heap.push_back(node);
        int i = heap.size() - 1;
        while (i > 0 && heap[(i - 1) / 2]->num_users < heap[i]->num_users)
        {
            swap(heap[i], heap[(i - 1) / 2]);
            i = (i - 1) / 2;
        }
    }

    // Function to view the heap (max heap) based on number of users enrolled
    void viewHeap()
    {
        cout << "Heap (Course ID : Users Enrolled)" << endl;
        for (int i = 0; i < heap.size(); i++)
        {
            cout << heap[i]->courseID << " : " << heap[i]->num_users << endl;
        }
    }

    // Function to build the heap
    void buildHeap()
    {
        int n = heap.size();
        for (int i = n / 2 - 1; i >= 0; i--)
        {
            heapify(n, i);
        }
    }

    // Function to read course data from courses
    // Function to read course data from courses.txt and construct the heap
    void readCourseData(const string &filename)
    {
        ifstream file(filename);
        string line;

        while (getline(file, line))
        {
            istringstream iss(line);
            string token;
            vector<string> tokens;

            while (getline(iss, token, ','))
            {
                tokens.push_back(token);
            }

            try
            {
                if (tokens.size() == 8)
                {
                    int courseID = stoi(tokens[0]);
                    string type = tokens[1];
                    string topic = tokens[2];
                    string description = tokens[3];
                    int price = stoi(tokens[4]);
                    int enrollment_limit = stoi(tokens[5]);
                    string instructor_email = tokens[6];
                    int num_users = stoi(tokens[7]); // Assuming last column in courses.txt is number of users
                    vector<string> materials = materialsMap[courseID];
                    courseNode *node = new courseNode(courseID, type, topic, description, price, enrollment_limit, instructor_email, materials, num_users);
                    insert(node);
                }
            }
            catch (const std::invalid_argument &e)
            {
                //cout << "Invalid course ID format in file.\n";
            }
            catch (const std::out_of_range &e)
            {
                //cout << "Course ID out of range in file.\n";
            }
        }

        file.close();
    }
};

class Expert
{
private:
    string expertID;
    CourseHeap heap;

public:
    Expert(string ID) : expertID(ID){};
    void addReview()
    {
        ofstream expertFile("review.txt", ios::app); // Open file in append mode

        int courseID;
        string review;
        string status;

        // Get user input
        cout << "Enter Course ID: ";
        cin >> courseID;
        cin.ignore(); // Ignore the newline character left in the buffer
        cout << "Enter Review: ";
        getline(cin, review);
        cout << "Enter Status: ";
        getline(cin, status);

        // Write the review details to the file
        expertFile << expertID << "," << courseID << "," << review << "," << status << endl;

        expertFile.close();
        cout << "Review added successfully." << endl;
    }

    vector<int> fetchPendingCourseIDs()
    {
        ifstream reviewFile("review.txt");
        string line;
        vector<int> courseIDs;

        // Read review.txt
        while (getline(reviewFile, line))
        {
            vector<string> tokens = split(line, ',');
            try
            {
                if (tokens.size() == 4)
                {
                    string currentExpertID = (tokens[0]);
                    if (currentExpertID == expertID && tokens[3] == "pending")
                    {
                        int courseID = stoi(tokens[1]);
                        courseIDs.push_back(courseID);
                    }
                }
            }
            catch (const std::invalid_argument &e)
            {
                //cout << "Invalid course ID format in file.\n";
            }
            catch (const std::out_of_range &e)
            {
                //cout << "Course ID out of range in file.\n";
            }
        }

        return courseIDs;
    }

    courseNode *fetchCourseData(int courseID, const unordered_map<int, vector<string>> &materialsMap)
    {
        ifstream coursesFile("courses.txt");
        string line;

        // Read courses.txt
        while (getline(coursesFile, line))
        {
            vector<string> tokens = split(line, ',');
            try
            {
                if (tokens.size() == 8)
                {
                    int id = stoi(tokens[0]);
                    if (id == courseID)
                    {
                        string type = tokens[1];
                        string topic = tokens[2];
                        string description = tokens[3];
                        int price = stoi(tokens[4]);
                        int enrollment_limit = stoi(tokens[5]);
                        string instructor_email = tokens[6];
                        int num_users = stoi(tokens[7]);

                        vector<string> materials = materialsMap.at(courseID);

                        // Create and return a new node with fetched data
                        return new courseNode(courseID, type, topic, description, price, enrollment_limit, instructor_email, materials, num_users);
                    }
                }
            }
            catch (const std::invalid_argument &e)
            {
                //cout << "Invalid course ID format in file.\n";
            }
            catch (const std::out_of_range &e)
            {
                //cout << "Course ID out of range in file.\n";
            }
        }

        return nullptr; // Course ID not found
    }

    void insertFromReview(SplayTree &tree, const unordered_map<int, vector<string>> &materialsMap)
    {
        ifstream reviewFile("review.txt");
        string line;

        // Read review.txt
        while (getline(reviewFile, line))
        {
            vector<string> tokens = split(line, ',');
            if (tokens.size() == 4)
            {
                try
                {
                    int courseID = stoi(tokens[1]);
                    string review = tokens[2];
                    string status = tokens[3];
                    // Insert into splay tree if status is "pending"
                    if (status == "pending")
                    {
                        courseNode *node = fetchCourseData(courseID, materialsMap); // Pass materialsMap to fetchCourseData
                        tree.insert(courseID, node->type, node->topic, node->description, node->price, node->enrollment_limit, node->instructor_email, node->materials, node->num_users);
                    }
                }
                catch (const std::invalid_argument &e)
                {
                    //cout << "Invalid course ID format in file.\n";
                }
                catch (const std::out_of_range &e)
                {
                    //cout << "Course ID out of range in file.\n";
                }
            }
        }
    }

    void viewPriorityQueue()
    {
        heap.readCourseData("courses.txt");
        heap.viewHeap();
    }

    void menu(SplayTree &tree, const unordered_map<int, vector<string>> &materialsMap)
    {
        while (true)
        {
            cout << "1. Add Review\n";
            //cout << "2. View Recent Reviews\n";
            cout << "3. View Priority Queue\n";
            cout << "4. Exit\n";
            cout << "Enter your choice: ";
            int choice;
            cin >> choice;
            cin.ignore();

            switch (choice)
            {
            case 1:
                addReview();
                break;
            case 2:
                insertFromReview(tree, materialsMap);
                tree.inorder();
                break;
            case 3:
                viewPriorityQueue();
                break;
            case 4:
                return;
            default:
                cout << "Invalid choice. Please enter again.\n";
            }
        }
    }
};
