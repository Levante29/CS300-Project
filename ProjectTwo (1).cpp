//============================================================================
// Name        : ProjectTwo.cpp
// Author      : Levante Lanier
// Version     : 1.0
// Description : ABCU Advising Assistance Program
//============================================================================

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <set>
#include <algorithm>
#include <limits>

using namespace std;

// Course structure holds course information
struct Course
{
    string courseNumber;
    string courseTitle;
    vector<string> prerequisites;
};

// Node structure for Binary Search Tree
struct Node
{
    Course course;
    Node *left;
    Node *right;

    Node(Course aCourse)
    {
        course = aCourse;
        left = nullptr;
        right = nullptr;
    }
};

// Binary Search Tree class
class BinarySearchTree
{
private:
    Node *root;

    // Recursively delete nodes
    void deleteTree(Node *node)
    {
        if (node != nullptr)
        {
            deleteTree(node->left);
            deleteTree(node->right);
            delete node;
        }
    }

    // In-order traversal prints courses in alphanumeric order
    void inOrder(Node *node)
    {
        if (node != nullptr)
        {
            inOrder(node->left);
            cout << node->course.courseNumber << ", " << node->course.courseTitle << endl;
            inOrder(node->right);
        }
    }

    // Recursive search helper
    Course *search(Node *node, string courseNumber)
    {
        if (node == nullptr)
        {
            return nullptr;
        }

        if (node->course.courseNumber == courseNumber)
        {
            return &(node->course);
        }
        else if (courseNumber < node->course.courseNumber)
        {
            return search(node->left, courseNumber);
        }
        else
        {
            return search(node->right, courseNumber);
        }
    }

public:
    BinarySearchTree()
    {
        root = nullptr;
    }

    ~BinarySearchTree()
    {
        deleteTree(root);
    }

    // Clear the current tree
    void clear()
    {
        deleteTree(root);
        root = nullptr;
    }

    // Insert a course into the BST
    void insert(Course course)
    {
        Node *newNode = new Node(course);

        if (root == nullptr)
        {
            root = newNode;
            return;
        }

        Node *current = root;

        while (current != nullptr)
        {
            if (course.courseNumber < current->course.courseNumber)
            {
                if (current->left == nullptr)
                {
                    current->left = newNode;
                    return;
                }
                current = current->left;
            }
            else
            {
                if (current->right == nullptr)
                {
                    current->right = newNode;
                    return;
                }
                current = current->right;
            }
        }
    }

    // Print all courses in alphanumeric order
    void printCourseList()
    {
        inOrder(root);
    }

    // Search for a course
    Course *searchCourse(string courseNumber)
    {
        return search(root, courseNumber);
    }

    // Check if tree is empty
    bool isEmpty()
    {
        return root == nullptr;
    }
};

// Remove extra spaces from a string
string trim(string value)
{
    size_t first = value.find_first_not_of(" \t\r\n");

    if (first == string::npos)
    {
        return "";
    }

    size_t last = value.find_last_not_of(" \t\r\n");
    return value.substr(first, last - first + 1);
}

// Convert user input and course numbers to uppercase
string toUpperCase(string value)
{
    transform(value.begin(), value.end(), value.begin(), ::toupper);
    return value;
}

// Split a CSV line by commas
vector<string> splitLine(string line)
{
    vector<string> tokens;
    string token;
    stringstream ss(line);

    while (getline(ss, token, ','))
    {
        tokens.push_back(trim(token));
    }

    return tokens;
}

// Load course data from file into BST
bool loadCourses(string fileName, BinarySearchTree &courses)
{
    ifstream file(fileName);

    if (!file.is_open())
    {
        cout << "Error: File could not be opened." << endl;
        return false;
    }

    vector<vector<string>> allRows;
    set<string> courseNumbers;
    string line;

    // First pass: read all rows and collect course numbers
    while (getline(file, line))
    {
        if (trim(line).empty())
        {
            continue;
        }

        vector<string> tokens = splitLine(line);

        if (tokens.size() < 2 || tokens[0].empty() || tokens[1].empty())
        {
            cout << "Error: Each course must include a course number and course title." << endl;
            file.close();
            return false;
        }

        tokens[0] = toUpperCase(tokens[0]);
        courseNumbers.insert(tokens[0]);
        allRows.push_back(tokens);
    }

    file.close();

    // Clear existing tree before loading new data
    courses.clear();

    // Second pass: validate prerequisites and insert courses
    for (vector<string> tokens : allRows)
    {
        Course course;
        course.courseNumber = toUpperCase(tokens[0]);
        course.courseTitle = tokens[1];

        for (size_t i = 2; i < tokens.size(); i++)
        {
            string prerequisite = toUpperCase(trim(tokens[i]));

            // Ignore blank values caused by extra commas
            if (prerequisite.empty())
            {
                continue;
            }

            // Validate that prerequisite exists in the file
            if (courseNumbers.find(prerequisite) == courseNumbers.end())
            {
                cout << "Error: Prerequisite " << prerequisite
                     << " does not exist in the course file." << endl;
                courses.clear();
                return false;
            }

            course.prerequisites.push_back(prerequisite);
        }

        courses.insert(course);
    }

    cout << "Data loaded successfully." << endl;
    return true;
}

// Print one course with prerequisite numbers and titles
void printCourseInformation(BinarySearchTree &courses, string courseNumber)
{
    courseNumber = toUpperCase(trim(courseNumber));

    Course *course = courses.searchCourse(courseNumber);

    if (course == nullptr)
    {
        cout << "Course not found." << endl;
        return;
    }

    cout << course->courseNumber << ", " << course->courseTitle << endl;

    if (course->prerequisites.empty())
    {
        cout << "Prerequisites: None" << endl;
    }
    else
    {
        cout << "Prerequisites:" << endl;

        for (string prerequisiteNumber : course->prerequisites)
        {
            Course *prerequisiteCourse = courses.searchCourse(prerequisiteNumber);

            if (prerequisiteCourse != nullptr)
            {
                cout << prerequisiteCourse->courseNumber << ", "
                     << prerequisiteCourse->courseTitle << endl;
            }
            else
            {
                cout << prerequisiteNumber << endl;
            }
        }
    }
}

// Display menu
void displayMenu()
{
    cout << endl;
    cout << "1. Load Data Structure." << endl;
    cout << "2. Print Course List." << endl;
    cout << "3. Print Course." << endl;
    cout << "9. Exit" << endl;
    cout << "What would you like to do? ";
}

int main()
{
    BinarySearchTree courses;
    bool loaded = false;
    int choice = 0;
    string fileName;
    string courseNumber;

    cout << "Welcome to the course planner." << endl;

    while (choice != 9)
    {
        displayMenu();

        if (!(cin >> choice))
        {
            cout << "Invalid input. Please enter a number." << endl;
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }

        switch (choice)
        {
        case 1:
            cout << "Enter file name: ";
            cin >> fileName;

            loaded = loadCourses(fileName, courses);
            break;

        case 2:
            if (!loaded || courses.isEmpty())
            {
                cout << "Please load the course data first." << endl;
            }
            else
            {
                cout << "Here is a sample schedule:" << endl;
                courses.printCourseList();
            }
            break;

        case 3:
            if (!loaded || courses.isEmpty())
            {
                cout << "Please load the course data first." << endl;
            }
            else
            {
                cout << "What course do you want to know about? ";
                cin >> courseNumber;
                printCourseInformation(courses, courseNumber);
            }
            break;

        case 9:
            cout << "Thank you for using the course planner!" << endl;
            break;

        default:
            cout << choice << " is not a valid option." << endl;
            break;
        }
    }

    return 0;
}