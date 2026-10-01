/*
 * Program: EECS 348 Assignment 3
 * Description:
 *   This program manages and prioritizes emails for a CEO.
 *   A MaxHeap is implemented from scratch using a vector.
 *   Emails are prioritized first by sender category and then
 *   by date, with newer emails receiving higher priority.
 *
 * Input:
 *   A text file containing EMAIL, NEXT, READ, and COUNT commands.
 *
 * Output:
 *   Displays the highest-priority email and the number of
 *   unread emails when requested.
 *
 * Collaborators: Trey Curtis
 * Other Sources: ChatGPT, Gemini
 * Author: Ethan Church
 * Creation Date: September 29, 2026
 * Revision Date: September 29, 2026
 * Revisions: input handling by removing extra spaces and checking commas exist
 */

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

/*
 * Email class
 * Stores the sender category, subject, and date for one email.
 */
class Email
{
private:
    string sender;          // Stores the sender category.
    string subject;         // Stores the email subject.
    string date;            // Stores the original date string.
    int dateValue;          // Stores a numeric version of the date for comparison.

public:
    // Default constructor creates an empty email.
    Email()
    {
        sender = "";        // Initialize sender.
        subject = "";       // Initialize subject.
        date = "";          // Initialize date.
        dateValue = 0;      // Initialize numeric date.
    }

    // Constructor creates an email using information read from the file.
    Email(string newSender, string newSubject, string newDate)
    {
        sender = newSender;              // Save sender category.
        subject = newSubject;            // Save subject.
        date = newDate;                  // Save original date.
        dateValue = convertDate(date);   // Convert date for comparisons.
    }

    // Converts MM-DD-YYYY into YYYYMMDD so dates can be compared numerically.
    int convertDate(string emailDate)
    {
        int month;                       // Stores month.
        int day;                         // Stores day.
        int year;                        // Stores year.
        char dash1;                      // Stores first '-'.
        char dash2;                      // Stores second '-'.

        stringstream ss(emailDate);      // Create stream from date.
        ss >> month >> dash1 >> day >> dash2 >> year; // Extract date parts.

        return year * 10000 + month * 100 + day; // Return sortable date value.
    }

    // Returns the priority number associated with the sender category.
    int getSenderPriority() const
    {
        if (sender == "Boss")            // Boss has highest priority.
        {
            return 5;
        }
        else if (sender == "Subordinate") // Subordinate is second.
        {
            return 4;
        }
        else if (sender == "Peer")       // Peer is third.
        {
            return 3;
        }
        else if (sender == "ImportantPerson") // ImportantPerson is fourth.
        {
            return 2;
        }
        else                             // OtherPerson has lowest priority.
        {
            return 1;
        }
    }

    // Determines whether this email has higher priority than another email.
    bool higherPriorityThan(const Email& other) const
    {
        if (getSenderPriority() != other.getSenderPriority())
        {
            return getSenderPriority() > other.getSenderPriority();
        }

        // If categories are equal, the newest email gets priority.
        return dateValue > other.dateValue;
    }

    // Displays the email using the assignment's required format.
    void display() const
    {
        cout << "Sender: " << sender << endl;   // Display sender.
        cout << "Subject: " << subject << endl; // Display subject.
        cout << "Date: " << date << endl;       // Display date.
    }
};

/*
 * MaxHeap class
 * Implements a MaxHeap from scratch using a vector.
 */
class MaxHeap
{
private:
    vector<Email> heap; // List-based storage for the MaxHeap.

    // Moves an email upward until the MaxHeap property is restored.
    void heapifyUp(int index)
    {
        while (index > 0)
        {
            int parent = (index - 1) / 2; // Calculate parent index.

            if (heap[index].higherPriorityThan(heap[parent]))
            {
                Email temp = heap[index]; // Temporarily save child.
                heap[index] = heap[parent]; // Move parent downward.
                heap[parent] = temp;      // Move child upward.

                index = parent;           // Continue from new position.
            }
            else
            {
                break;                    // Heap property has been restored.
            }
        }
    }

    // Moves an email downward until the MaxHeap property is restored.
    void heapifyDown(int index)
    {
        int size = heap.size();           // Store current heap size.

        while (true)
        {
            int left = 2 * index + 1;     // Calculate left child.
            int right = 2 * index + 2;    // Calculate right child.
            int largest = index;          // Assume current item has highest priority.

            if (left < size &&
                heap[left].higherPriorityThan(heap[largest]))
            {
                largest = left;           // Left child has higher priority.
            }

            if (right < size &&
                heap[right].higherPriorityThan(heap[largest]))
            {
                largest = right;          // Right child has higher priority.
            }

            if (largest == index)
            {
                break;                    // Heap property is correct.
            }

            Email temp = heap[index];     // Temporarily save current email.
            heap[index] = heap[largest];  // Move higher-priority child upward.
            heap[largest] = temp;         // Move current email downward.

            index = largest;              // Continue checking new position.
        }
    }

public:
    // Adds an email to the MaxHeap.
    void insert(const Email& email)
    {
        heap.push_back(email);            // Add email to end of list.
        heapifyUp(heap.size() - 1);       // Restore MaxHeap property.
    }

    // Displays the highest-priority email without removing it.
    void next() const
    {
        if (heap.empty())                 // Check for an empty inbox.
        {
            return;                       // Nothing can be displayed.
        }

        cout << "Next email:" << endl;    // Required NEXT heading.
        heap[0].display();                // Root contains highest-priority email.
    }

    // Removes the highest-priority email.
    void read()
    {
        if (heap.empty())                 // Check for an empty inbox.
        {
            return;                       // Nothing can be removed.
        }

        heap[0] = heap.back();            // Move final email to root.
        heap.pop_back();                  // Remove final position.

        if (!heap.empty())                // Check whether emails remain.
        {
            heapifyDown(0);               // Restore MaxHeap property.
        }
    }

    // Displays the number of unread emails.
    void count() const
    {
        cout << "There are " << heap.size()
             << " emails to read." << endl;
    }
};

/*
 * EmailManager class
 * Handles reading the command file and interacting with the MaxHeap.
 */
class EmailManager
{
private:
    MaxHeap inbox; // CEO's priority inbox.

    // Processes an EMAIL command.
    void processEmail(string line)
    {
        // Remove "EMAIL " from the beginning.
        string emailData = line.substr(6);

        // Locate the commas separating the three fields.
        size_t firstComma = emailData.find(',');
        size_t secondComma = emailData.find(',', firstComma + 1);

        // Extract sender category.
        string sender = emailData.substr(0, firstComma);

        // Extract subject, which may contain spaces.
        string subject = emailData.substr(
            firstComma + 1,
            secondComma - firstComma - 1
        );

        // Extract date.
        string date = emailData.substr(secondComma + 1);

        // Create an Email object.
        Email newEmail(sender, subject, date);

        // Insert it into the priority queue.
        inbox.insert(newEmail);
    }

public:
    // Reads and processes every command in the specified file.
    void run(string filename)
    {
        ifstream inputFile(filename);      // Open command file.

        if (!inputFile.is_open())         // Verify that the file opened.
        {
            cout << "Error opening file." << endl;
            return;
        }

        string line;                      // Stores each input line.

        while (getline(inputFile, line))  // Read file one line at a time.
        {
            if (line.rfind("EMAIL ", 0) == 0)
            {
                processEmail(line);       // Add new email.
            }
            else if (line == "NEXT")
            {
                inbox.next();             // Display highest-priority email.
            }
            else if (line == "READ")
            {
                inbox.read();             // Remove highest-priority email.
            }
            else if (line == "COUNT")
            {
                inbox.count();            // Display unread count.
            }
        }

        inputFile.close();                // Close input file.
    }
};

/*
 * Main program
 * Creates the EmailManager object and starts processing the input file.
 */
int main(int argc, char* argv[])
{
    // Require the test filename as a command-line argument.
    if (argc != 2)
    {
        cout << "Usage: " << argv[0] << " <test file>" << endl;
        return 1;
    }

    EmailManager manager;                 // Create email manager object.
    manager.run(argv[1]);                 // Process the supplied test file.

    return 0;                             // End program successfully.
}