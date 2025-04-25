// EECS 348 Assignment 7
// Author: Aiman Boullaouz
// Description: This program connects to a MySQL database and performs various queries as described in Assignment 7.
// Inputs: None (all queries are predefined)
// Output: Results printed to console
// Created: April 24, 2025
// Collaborators: ChatGPT

#include <iostream>
#include <stdexcept>
#include <mysql_driver.h>
#include <mysql_connection.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>
#include <cppconn/resultset.h>
#include <cppconn/exception.h>

using namespace std;
using namespace sql;

void runQuery(Statement* stmt, const string& query) {
    try {
        ResultSet* res = stmt->executeQuery(query);
        ResultSetMetaData* meta = res->getMetaData();
        int columns = meta->getColumnCount();
        while (res->next()) {
            for (int i = 1; i <= columns; ++i)
                cout << meta->getColumnLabel(i) << ": " << res->getString(i) << "\t";
            cout << endl;
        }
        delete res;
    } catch (SQLException &e) {
        cerr << "Query failed: " << e.what() << endl;
    }
}

int main() {
    try {
        mysql::MySQL_Driver* driver = mysql::get_mysql_driver_instance();
        Connection* con = driver->connect("tcp://mysql.eecs.ku.edu", "348s25_yourid", "yourpassword");
        con->setSchema("348s25_yourid");

        Statement* stmt = con->createStatement();

        cout << "1. Students majoring in IS:\n";
        runQuery(stmt, "SELECT * FROM Student WHERE StdMajor = 'IS'");

        cout << "\n2. Students enrolled in more than 2 courses:\n";
        runQuery(stmt, "SELECT StdName FROM Enrollment GROUP BY StdSSN HAVING COUNT(CrsCode) > 2");

        cout << "\n3. Professors in Physics with >5 years:\n";
        runQuery(stmt, "SELECT * FROM Faculty WHERE Dept = 'Physics' AND YearsTeaching > 5");

        cout << "\n4. Departments with more than 50 students:\n";
        runQuery(stmt, "SELECT StdMajor, COUNT(*) AS Total FROM Student GROUP BY StdMajor HAVING COUNT(*) > 50");

        cout << "\n5. Data courses taught by Dr. Johnson:\n";
        runQuery(stmt, "SELECT Course.CrsCode, CrsName FROM Course JOIN Offering ON Course.CrsCode = Offering.CrsCode WHERE CrsName LIKE '%Data%' AND Instructor = 'Dr. Johnson'");

        cout << "\n6. Students not enrolled in past two semesters:\n";
        runQuery(stmt, "SELECT * FROM Student WHERE StdSSN NOT IN (SELECT DISTINCT StdSSN FROM Enrollment WHERE Semester IN ('Fall2024', 'Spring2025'))");

        cout << "\n7. Second highest GPA:\n";
        runQuery(stmt, "SELECT MAX(GPA) AS SecondHighest FROM Student WHERE GPA < (SELECT MAX(GPA) FROM Student)");

        cout << "\n8. Students who are TAs with GPA > 3.5:\n";
        runQuery(stmt, "SELECT * FROM Student WHERE StdSSN IN (SELECT TA_SSN FROM Offering) AND GPA > 3.5");

        cout << "\n9. Students and courses (after 2022):\n";
        runQuery(stmt, "SELECT Student.StdName, Enrollment.CrsCode FROM Student JOIN Enrollment ON Student.StdSSN = Enrollment.StdSSN WHERE Year > 2022");

        cout << "\n10. Top 3 highest paid professors:\n";
        runQuery(stmt, "SELECT FacName, Salary FROM Faculty ORDER BY Salary DESC LIMIT 3");

        cout << "\n11. Insert Alice Smith:\n";
        stmt->execute("INSERT INTO Student VALUES ('888-88-8888', 'Alice Smith', 'Topeka', 'KS', '66610', 'Computer Science', 3.85, 'Junior')");
        runQuery(stmt, "SELECT * FROM Student WHERE StdSSN = '888-88-8888'");

        cout << "\n12. Update Bob Norbert city/zip:\n";
        stmt->execute("UPDATE Student SET StdCity = 'Overland Park', StdZip = '66210' WHERE StdName = 'Bob Norbert'");
        runQuery(stmt, "SELECT * FROM Student WHERE StdName = 'Bob Norbert'");

        delete stmt;
        delete con;
    } catch (SQLException &e) {
        cerr << "MySQL error: " << e.what() << endl;
    }
    return 0;
}
