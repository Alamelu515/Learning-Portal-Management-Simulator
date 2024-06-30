#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <map>
#include <vector>
#include <limits>
#include <iomanip>
#include <algorithm>
#include "user1.h"
#include "expert.h"

using namespace std;

class Instructor
{
public:
    string email;
    string password;
    string name;
    string type;
    Instructor(string email);
    virtual void menu() = 0;
};

class CourseCreator : public Instructor
{
public:
    void menu() override;
    void newCourse();
    void viewCourses();
    virtual void options(int courseID);
    virtual void getInfo();
    CourseCreator(string email) : Instructor(email){};

protected:
    int getNextID(const string &filename);
    void saveCourse(const string &courseType, const string &topic, const string &description, float price, int enrollmentLimit = 0, map<string, int> rubrics = {});
    void displayMenu();
    void uploadPractiseTests(int);
    void view(int);
    void uploadMaterial(int);
};

Instructor::Instructor(string gmailID)
{
    ifstream inFile("instructor.txt");
    if (inFile.is_open())
    {
        string line;
        while (getline(inFile, line))
        {
            vector<string> columns = split(line, ',');
            if (columns[0] == gmailID)
            {
                email = columns[0];
                type = columns[1];
                name = columns[2];
                password = columns[3];
                break;
            }
        }
        inFile.close();
    }
    else
    {
        std::cout << "Unable to open instructor file.\n";
    }
}

void CourseCreator::getInfo()
{
    string topic, desc;
    float price;
    std::cout << "\nTopic: ";
    std::cin.ignore();
    getline(std::cin, topic);
    std::cout << "Description: ";
    getline(std::cin, desc);
    std::cout << "Price: ";
    std::cin >> price;
    saveCourse("Predefined", topic, desc, price);
}

int CourseCreator::getNextID(const string &filename)
{
    ifstream file(filename);
    string lastLine;
    while (file.good())
    {
        string line;
        getline(file, line);
        if (!line.empty())
        {
            lastLine = line;
        }
    }
    file.close();
    if (lastLine.empty())
    {
        return 1;
    }
    else
    {
        istringstream iss(lastLine);
        string idStr;
        getline(iss, idStr, ',');
        int x;
        try
        {
            x = stoi(idStr) + 1;
        }
        catch (const std::invalid_argument &e)
        {
            //std::cout << "Invalid course ID format in file.\n";
        }
        catch (const std::out_of_range &e)
        {
            //std::cout << "Course ID out of range in file.\n";
        }
        return x;
    }
}

void CourseCreator::saveCourse(const string &courseType, const string &topic, const string &description, float price, int enrollmentLimit, map<string, int> rubrics)
{
    int courseID = getNextID("courses.txt");
    ofstream outFile("courses.txt", ios::app);
    if (outFile.is_open())
    {
        outFile << courseID << "," << courseType << "," << topic << "," << description << "," << price << "," << enrollmentLimit << "\n";
        outFile.close();
    }
    else
    {
        std::cout << "Unable to open file to save course.\n";
    }
}

class LiveMentor : public CourseCreator
{
public:
    void welcome()
    {
        std::cout << "\nHi\n";
    }
    LiveMentor(string email) : CourseCreator(email){};
    void seeStudents(int courseID)
    {
        std::cout << "Enrollment list for course " << courseID << ":\n";
        ifstream inFile("enrollment.txt");
        if (inFile.is_open())
        {
            string line;
            bool found = false;
            while (getline(inFile, line))
            {
                vector<string> columns = split(line, ',');
                try
                {
                    if (stoi(columns[0]) == courseID)
                    {
                        std::cout << columns[1] << "\n"; 
                        found = true;
                    }
                }
                catch (const std::invalid_argument &e)
                {
                    //std::cout << "Invalid course ID format in file.\n";
                }
                catch (const std::out_of_range &e)
                {
                    //std::cout << "Course ID out of range in file.\n";
                }
            }
            inFile.close();
            if (!found)
            {
                std::cout << "No students enrolled in this course.\n";
            }
        }
        else
        {
            std::cout << "Unable to open enrollment file.\n";
        }
    }

    void changeTestStatus(int testID, string status);
    void assignHomework(int courseID);
    void gradeAssignments(int courseID);
    void scheduleTest(int courseID);
    map<string, int> setRubrics();
    void enableTest(int testID);
    void disableTest(int testID);
    void getInfo() override;
    void options(int courseID) override;
    void viewScheduledTests(int courseID);
};

void LiveMentor::assignHomework(int courseID)
{
    string topic, desc, date;
    int marks;
    std::cout << "Topic: ";
    std::cin.ignore();
    getline(std::cin, topic);
    std::cout << "Description: ";
    getline(std::cin, desc);
    std::cout << "Due date: ";
    getline(std::cin, date);
    std::cout << "Marks: ";
    std::cin >> marks;
    ofstream outFile("assignments.txt", ios::app);
    if (outFile.is_open())
    {
        int assignmentID = getNextID("assignments.txt");
        outFile << assignmentID << "," << courseID << "," << topic << "," << desc << "," << date << "," << marks << "\n";
        outFile.close();
    }
    else
    {
        std::cout << "Unable to open file to save assignment.\n";
    }
}

void LiveMentor::viewScheduledTests(int courseID)
{
    std::ifstream inFile("livetests.txt");
    std::string line;

    if (inFile.is_open())
    {
        std::cout << std::left
                  << std::setw(10) << "Test ID"
                  << std::setw(15) << "Course ID"
                  << std::setw(30) << "Portions"
                  << std::setw(15) << "Date"
                  << std::setw(20) << "Question File"
                  << std::setw(10) << "Status"
                  << std::endl;

        std::cout << std::string(100, '-') << std::endl; 
        while (getline(inFile, line))
        {
            std::vector<std::string> columns = split(line, ',');
            try
            {
                if (std::stoi(columns[1]) == courseID)
                {
                    std::cout << std::setw(10) << columns[0]
                              << std::setw(15) << columns[1]
                              << std::setw(30) << columns[2]
                              << std::setw(15) << columns[3]
                              << std::setw(20) << columns[4]
                              << std::setw(10) << columns[5]
                              << std::endl;
                }
            }
            catch (const std::invalid_argument &e)
            {
                //std::cout << "Invalid course ID format in file.\n";
            }
            catch (const std::out_of_range &e)
            {
                //std::cout << "Course ID out of range in file.\n";
            }
        }
        inFile.close();
    }
    else
    {
        std::cout << "Unable to open file livetests.txt.\n";
    }
}

void LiveMentor::gradeAssignments(int courseID)
{
    int assignmentID, studentID, marks;
    std::cout << "Enter assignment ID: ";
    std::cin >> assignmentID;
    std::cout << "Enter student ID: ";
    std::cin >> studentID;
    std::cout << "Enter marks: ";
    std::cin >> marks;
    ofstream outFile("grades.txt", ios::app);
    if (outFile.is_open())
    {
        outFile << assignmentID << "," << studentID << "," << marks << "\n";
        outFile.close();
    }
    else
    {
        std::cout << "Unable to open file to save grade.\n";
    }
}

void LiveMentor::scheduleTest(int courseID)
{
    string portions, date;
    std::cout << "Portions for the test: ";
    std::cin.ignore();
    getline(std::cin, portions);
    std::cout << "Date: ";
    getline(std::cin, date);

    int testID = getNextID("livetests.txt");

    string questionFileName;
    std::cout << "Enter the name of the question file to upload: ";
    getline(std::cin, questionFileName);

    ofstream outFile("livetests.txt", ios::app);
    if (outFile.is_open())
    {
        outFile << testID << "," << courseID << "," << portions << "," << date << "," << questionFileName << ",disabled" << "\n";
        outFile.close();
        std::cout << "Test scheduled successfully(not enabled)\n";
    }
    else
    {
        std::cout << "Unable to open livetests.txt to save test details.\n";
    }
}

map<string, int> LiveMentor::setRubrics()
{
    map<string, int> rubrics;
    int assignments, tests, finalExam;
    std::cout << "\nEnter course rubrics:\n";
    std::cout << "Assignments: ";
    std::cin >> assignments;
    rubrics["assignments"] = assignments;
    std::cout << "Internal tests: ";
    std::cin >> tests;
    rubrics["tests"] = tests;
    std::cout << "Final test: ";
    std::cin >> finalExam;
    rubrics["final"] = finalExam;
    return rubrics;
}

void LiveMentor::getInfo()
{
    string topic, desc;
    float price;
    int limit;
    map<string, int> rubrics;

    std::cout << "\nTopic: ";
    std::cin.ignore();
    getline(std::cin, topic);
    std::cout << "Description: ";
    getline(std::cin, desc);
    std::cout << "Price: ";
    std::cin >> price;
    std::cout << "Enter enrollment limit: ";
    std::cin >> limit;
    rubrics = setRubrics();
    saveCourse("LiveMentor", topic, desc, price, limit, rubrics);
}

void LiveMentor::options(int courseID)
{
    int choice;
    do
    {
        std::cout << "\n1. View enrolled student list" << endl;
        std::cout << "2. Assign homework" << endl;
        std::cout << "3. Grade assignments" << endl;
        std::cout << "4. Schedule a test" << endl;
        std::cout << "5. Enable a test" << endl;
        std::cout << "6. Disable a test" << endl;
        std::cout << "7. Upload material" << endl;
        std::cout << "8. Upload practice test sheets" << endl;
        std::cout << "9. View Scheduled tests" << endl;
        std::cout << "10. View uploaded practice test sheets" << endl;
        std::cout << "11. Back" << endl;
        std::cout << "Enter your choice: ";
        std::cin >> choice;

        switch (choice)
        {
        case 1:
            seeStudents(courseID);
            break;
        case 2:
            assignHomework(courseID);
            break;
        case 3:
            gradeAssignments(courseID);
            break;
        case 4:
            scheduleTest(courseID);
            break;
        case 5:
            int testIDToEnable;
            std::cout << "Enter Test ID to enable: ";
            std::cin >> testIDToEnable;
            enableTest(testIDToEnable);
            break;
        case 6:
            int testIDToDisable;
            std::cout << "Enter Test ID to disable: ";
            std::cin >> testIDToDisable;
            disableTest(testIDToDisable);
            break;
        case 7:
            uploadMaterial(courseID);
            break;
        case 8:
            uploadPractiseTests(courseID);
            break;
        case 9:
            viewScheduledTests(courseID);
            break;
        case 10:
            view(courseID);
            break;
        case 11:
            return;
        default:
            std::cout << "Invalid choice, please try again.\n";
        }
    } while (true);
}

void CourseCreator ::uploadMaterial(int courseID)
{
    std::ofstream file;
    string filePath = "materials.txt";
    file.open(filePath, std::ios_base::app);  

    if (!file.is_open())
    {
        std::cerr << "Unable to open file: " << filePath << std::endl;
        return;
    }

    std::vector<std::string> filenames;
    std::string input;

    std::cout << "Enter filenames (type 'done' to finish):" << std::endl;
    std::cin.ignore();
    while (true)
    {
        std::cout << "Filename: ";

        std::getline(std::cin, input);
        if (input == "done")
        {
            break;
        }
        filenames.push_back(input);
    }

    file << courseID << ",";
    for (const auto &filename : filenames)
    {
        file << filename << ",";
    }
    file << endl;
    file.close();
}

void LiveMentor::changeTestStatus(int testID, string status)
{
    ifstream inFile("livetests.txt");
    ofstream tempFile("temp.txt");
    string line;

    if (inFile.is_open() && tempFile.is_open())
    {
        while (getline(inFile, line))
        {
            vector<string> columns = split(line, ',');
            try
            {
                if (stoi(columns[0]) == testID)
                {
                    columns[5] = status;
                }
            }
            catch (const std::invalid_argument &e)
            {
                //std::cout << "Invalid course ID format in file.\n";
            }
            catch (const std::out_of_range &e)
            {
                //std::cout << "Course ID out of range in file.\n";
            }
            tempFile << columns[0] << "," << columns[1] << "," << columns[2] << "," << columns[3] << "," << columns[4] << "," << columns[5] << "\n";
        }
        inFile.close();
        tempFile.close();
        remove("livetests.txt");
        rename("temp.txt", "livetests.txt");
        std::cout << "Test ID " << testID << " has been " << status << ".\n";
    }
    else
    {
        std::cout << "Unable to open file to change test status.\n";
    }
}

void LiveMentor::enableTest(int testID)
{
    changeTestStatus(testID, "enabled");
}

void LiveMentor::disableTest(int testID)
{
    changeTestStatus(testID, "disabled");
}

void CourseCreator::newCourse()
{
    int choice;
    std::cout << "\n1. Live mentoring" << endl;
    std::cout << "2. Predefined course" << endl;
    std::cout << "3. Back" << endl;
    std::cout << "Enter your choice: ";
    std::cin >> choice;

    switch (choice)
    {
    case 1:
    {
        LiveMentor mentor(this->email);
        mentor.getInfo();
        break;
    }
    case 2:
        getInfo();
        break;
    case 3:
        return;
    default:
        std::cout << "Invalid choice, please try again.\n";
    }
}

void CourseCreator::viewCourses()
{
    std::cout << "\nAll courses authored:\n";
    ifstream inFile("courses.txt");
    if (inFile.is_open())
    {
        string line;

        std::cout << left << setw(5) << "ID" << " | "
                  << setw(15) << "Type" << " | "
                  << setw(20) << "Topic" << " | "
                  << setw(10) << "Price" << " | "
                  << setw(15) << "Enrollment Limit" << endl;
        std::cout << string(100, '-') << endl;

        while (getline(inFile, line))
        {
            vector<string> columns = split(line, ',');

            if (columns[6] == this->email)
            {
                std::cout << left << setw(5) << columns[0] << " | "
                          << setw(15) << columns[1] << " | "
                          << setw(20) << columns[2] << " | "
                          << setw(10) << columns[4] << " | "
                          << setw(15) << columns[5] << endl;
            }
        }
    }
    else
    {
        std::cout << "Unable to open courses file.\n";
    }

    inFile.close();

    int option;
    std::cout << endl;
    std::cout << "1. Select" << endl;
    std::cout << "2. Back" << endl;
    std::cout << "Enter your choice: ";
    std::cin >> option;

    if (option == 1)
    {
        std::cout << "Enter course ID: ";
        int courseID;
        std::cin >> courseID;
        ifstream inFile("courses.txt");

        bool live = false;
        if (inFile.is_open())
        {
            string line;
            while (getline(inFile, line))
            {
                vector<string> columns = split(line, ',');
                try
                {
                    if (stoi(columns[0]) == courseID)
                    {
                        if (columns[1] == "LiveMentor")
                        {
                            live = true;
                        }
                    }
                }
                catch (const std::invalid_argument &e)
                {
                    //std::cout << "Invalid course ID format in file.\n";
                }
                catch (const std::out_of_range &e)
                {
                    //std::cout << "Course ID out of range in file.\n";
                }
            }
            inFile.close();
        }
        else
        {
            std::cout << "Unable to open courses file.\n";
        }

        if (live)
        {
            LiveMentor mentor(this->email);
            mentor.options(courseID);
        }
        else
        {
            options(courseID);
        }
    }
    inFile.close();
}

void CourseCreator::uploadPractiseTests(int courseID)
{
    string portions, date;
    std::cout << "Portions for the test: ";
    std::cin.ignore();
    getline(std::cin, portions);

    int testID = getNextID("tests.txt");

    string questionFileName;
    std::cout << "Enter the name of the question file to upload: ";
    getline(std::cin, questionFileName);

    ofstream outFile("tests.txt", ios::app);
    if (outFile.is_open())
    {
        outFile << testID << "," << courseID << "," << portions << "," << questionFileName << "\n";
        outFile.close();
        std::cout << "Test added.\n";
    }
    else
    {
        std::cout << "Unable to open tests.txt to save test details.\n";
    }
}

void CourseCreator::options(int courseID)
{
    int choice;
    do
    {
        std::cout << "\n1. Upload material" << endl;
        std::cout << "2. Practice tests" << endl;
        std::cout << "3. View " << endl;
        std::cout << "4. Back" << endl;
        std::cout << "Enter your choice: ";
        std::cin >> choice;

        switch (choice)
        {
        case 1:
            uploadMaterial(courseID);
            break;
        case 2:
            uploadPractiseTests(courseID);
            break;
        case 3:
            view(courseID);
            break;
        case 4:
            return;
        default:
            std::cout << "Invalid choice, please try again.\n";
        }
    } while (true);
}

void CourseCreator ::view(int courseID)
{
    std::cout << "Practice tests : " << endl;
    string line;
    ifstream inFile("tests.txt");
    if (inFile.is_open())
    {
        std::cout << std::left
                  << std::setw(10) << "Test ID"
                  << std::setw(15) << "Course ID"
                  << std::setw(30) << "Portions"
                  << std::setw(15) << "Date"
                  << std::endl;

        std::cout << std::string(100, '-') << std::endl;  

        while (getline(inFile, line))
        {
            std::vector<std::string> columns = split(line, ',');
            try
            {
                if (std::stoi(columns[1]) == courseID)
                {
                    std::cout << std::setw(10) << columns[0]
                              << std::setw(15) << columns[1]
                              << std::setw(30) << columns[2]
                              << std::setw(15) << columns[3]
                              << std::endl;
                }
            }
            catch (const std::invalid_argument &e)
            {
                //std::cout << "Invalid course ID format in file.\n";
            }
            catch (const std::out_of_range &e)
            {
                //std::cout << "Course ID out of range in file.\n";
            }
        }
        inFile.close();
    }
    else
    {
        std::cout << "Unable to open tests.txt to save test details.\n";
    }
}

void CourseCreator::displayMenu()
{
    int choice;
    do
    {
        std::cout << "\n1. New course" << endl;
        std::cout << "2. View my courses" << endl;
        std::cout << "3. Exit" << endl;
        std::cout << "Enter your choice: ";
        std::cin >> choice;

        switch (choice)
        {
        case 1:
            newCourse();
            break;
        case 2:
            viewCourses();
            break;
        case 3:
            return;
        default:
            std::cout << "Invalid choice, please try again.\n";
        }
    } while (true);
}

void CourseCreator::menu()
{
    displayMenu();
}

int main()
{
    int choice;
    do
    {
        std::cout << endl;
        std::cout << "1. Admin" << endl;
        std::cout << "2. Course creator" << endl;
        std::cout << "3. Expert" << endl;
        std::cout << "4. User" << endl;
        std::cout << "5. Exit " << endl;
        std::cout << "Enter your choice: ";
        std::cin >> choice;
        if (choice == 1 || choice == 2 || choice == 3 || choice == 4 || choice == 5)
        {
            if (choice == 1)
            {
                Admin admin;
                admin.displayMenu();
            }
            if (choice == 2)
            {
                CourseCreator creator("kashyap@gmail.com");
                creator.menu();
            }
            if (choice == 3)
            {
                Expert expert("alamelu@gmail.com");
                SplayTree tree;

                expert.menu(tree, materialsMap);
            }
            if (choice == 4)
            {
                User user("vedha@gmail.com"); 
                user.userMenu();
            }
            if (choice == 5)
            {
                std::cout << "Exiting..." << endl;
                return 0;
            }
        }
        else
        {
            std::cout << "Invalid choice, please try again.\n";
        }
    } while (choice != 5);
}
