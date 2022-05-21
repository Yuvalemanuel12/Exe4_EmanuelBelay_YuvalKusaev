#define _CRT_SECURE_NO_WARNINGS
/*#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>*/ //uncomment this block to check for heap memory allocation leaks.
// Read https://docs.microsoft.com/en-us/visualstudio/debugger/finding-memory-leaks-using-the-crt-library?view=vs-2019
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef struct StudentCourseGrade
{
	char courseName[35];
	int grade;
}StudentCourseGrade;

typedef struct Student
{
	char name[35];
	StudentCourseGrade* grades; //dynamic array of courses
	int numberOfCourses;
}Student;


//Part A
void countStudentsAndCourses(const char* fileName, int** coursesPerStudent, int* numberOfStudents);
int countPipes(const char* lineBuffer, int maxCount);
char*** makeStudentArrayFromFile(const char* fileName, int** coursesPerStudent, int* numberOfStudents);
void printStudentArray(const char* const* const* students, const int* coursesPerStudent, int numberOfStudents);
void factorGivenCourse(char** const* students, const int* coursesPerStudent, int numberOfStudents, const char* courseName, int factor);
void studentsToFile(char*** students, int* coursesPerStudent, int numberOfStudents);

//Part B
Student* transformStudentArray(char*** students, const int* coursesPerStudent, int numberOfStudents);
void writeToBinFile(const char* fileName, Student* students, int numberOfStudents);
Student* readFromBinFile(const char* fileName);

int main()
{
	//Part A
	char lineBuffer[1023];
	int numMax = 456;
	countPipes(lineBuffer, numMax);


	int* coursesPerStudent = NULL;
	int numberOfStudents = 0;
	countStudentsAndCourses("studentList.txt",
		&coursesPerStudent, &numberOfStudents);


	char*** students = makeStudentArrayFromFile("studentList.txt", &coursesPerStudent, &numberOfStudents);
	factorGivenCourse(students, coursesPerStudent, numberOfStudents, "Advanced Topics in C", +5);
	printStudentArray(students, coursesPerStudent, numberOfStudents);
	studentsToFile(students, coursesPerStudent, numberOfStudents); //this frees all memory. Part B fails if this line runs. uncomment for testing (and comment out Part B)

	//Part B
	Student* transformedStudents = transformStudentArray(students, coursesPerStudent, numberOfStudents);
	writeToBinFile("students.bin", transformedStudents, numberOfStudents);
	Student* testReadStudents = readFromBinFile("students.bin");



	/*_CrtDumpMemoryLeaks();*/ //uncomment this block to check for heap memory allocation leaks.
	// Read https://docs.microsoft.com/en-us/visualstudio/debugger/finding-memory-leaks-using-the-crt-library?view=vs-2019

	return 0;
}
void countStudentsAndCourses(const char* fileName, int** coursesPerStudent, int* numberOfStudents)
{
	int countp = 0;
	int countc = 0;
	char oneline[1023];
	FILE* fin = fopen(*fileName, "rt");
	if (fin == NULL) {
		printf("Unable to open file\n"); exit(1);
	}
	while (fgets(oneline, 1023, fin) != NULL) {
		countp++;
	}
	*numberOfStudents = countp;
	*coursesPerStudent = (int)malloc(sizeof(int) * countp);
	rewind(fin);
	for (int i = 0; i < countp; i++) {
		fgets(oneline, 1023, fin);
		coursesPerStudent[i] = countPipes(oneline, strlen(oneline));
	}
	fclose(fin);

}
int countPipes(const char* lineBuffer, int maxCount)
{
	if (lineBuffer == NULL) return -1;
	if (maxCount <= 0) return 0;
	int count = 0;
	for (int i = 0; (i < maxCount) || (lineBuffer[i] != "\n"); i++) {
		if (lineBuffer[i] == "|") count++;
	}
	return count;
}

char*** makeStudentArrayFromFile(const char* fileName, int** coursesPerStudent, int* numberOfStudents)
{
	FILE* fin = fopen(fileName, "rt");
	char*** students = (char***)malloc(sizeof(char***) * (*numberOfStudents));
	for (int i = 0; i < *numberOfStudents; i++) {
		students[i] = (char*)malloc(sizeof(char*) * (((*coursesPerStudent[i]) * 2) + 1));
	}
	char* temp = 0;
	fgets(temp, 1023, fin);
	char* piece = strtok(temp, "|,");
	while (piece != NULL)
	{
		piece = strtok(temp, "|,");
	}
	for (int i = 0; i < *numberOfStudents; i++) {
		for (int j = 0; j < *coursesPerStudent[i]; j++) {
			*students[j] = (char)malloc(sizeof(char) * (strlen(temp) + 1));
			strcpy(*students[j], temp);
			temp = temp + strlen(temp) + 1;
		}
	}
	fclose(fin);
	return students;
}

void factorGivenCourse(char** const* students, const int* coursesPerStudent, int numberOfStudents, const char* courseName, int factor)
{
	if (factor > 20 || factor < -20)exit(1);
	for (int i = 0; i < numberOfStudents; i++) {
		for (int j = 0; j < (coursesPerStudent[i] * 2) + 1; j++) {
			if (!strcmp((*(students[j]) + i), courseName)) {
				int val = atoi(*students[j + 1]);
				val = val + factor;
				if (val > 100)val = 100;
				if (val < 0)val = 0;
				atoi(val, *students[j + 1], 10);

			}
		}
	}
}





void printStudentArray(const char* const* const* students, const int* coursesPerStudent, int numberOfStudents)
{
	for (int i = 0; i < numberOfStudents; i++)
	{
		printf("name: %s\n*********\n", students[i][0]);
		for (int j = 1; j <= 2 * coursesPerStudent[i]; j += 2)
		{
			printf("course: %s\n", students[i][j]);
			printf("grade: %s\n", students[i][j + 1]);
		}
		printf("\n");
	}
}

void studentsToFile(char*** students, int* coursesPerStudent, int numberOfStudents)
{
	FILE* pFile = fopen("t.studentList", "w");
	char* temparr; char* A = " | "; char* B = ",";
	for (int j = 0; j < numberOfStudents; j++) {

		for (int i = 0; i < (coursesPerStudent[j] * 2) + 1; i++) {

			strcat(temparr, students[j][i]);
			free(*students[j][i]);
			free(students[j][i]);
			if (i == 0) strcat(temparr, A);
			if (i % 2 == 0 && i != 0)strcat(temparr, B);
		}
		int x = fputs(temparr, pFile);
		if (x == -1) printf("input to file not seccseded ");
		free(students[j]);
	}
	free(students);
	free(coursesPerStudent);
	fclose(pFile);
}

void writeToBinFile(const char* fileName, Student* students, int numberOfStudents)
{
	FILE* pFile = fopen(*fileName, "wb");
	char* temp;
	itoa(numberOfStudents, temp, 10);
	fputs(temp, pFile);
	for (int i = 0; i < numberOfStudents; i++) {
		temp = students[i].name;
		fputs(temp, pFile);
		itoa(students[i].numberOfCourses, temp, 10);
		fputs(temp, pFile);
		for (int j = 0; j < students[i].numberOfCourses; j++) {
			fputs(students[i].grades[j].courseName, pFile);
			itoa(students[i].grades[j].grade, temp, 10);
			fputs(temp, pFile);
		}
	}
}

Student* readFromBinFile(const char* fileName)
{
	Student* stu1[] = (Student*)malloc(sizeof(Student) * 1023);
	FILE* newbinFile = fopen("fileName", "rb");
	if (newbinFile == NULL)
	{
		printf("File open error!\n");
		exit(1);

	}
	else
	{
		return fread(stu1, sizeof(Student) * 1023, strlen(stu1), newbinFile);

	}
}


Student* transformStudentArray(char*** students, const int* coursesPerStudent, int numberOfStudents)
{
	Student* structarr = (Student*)malloc(sizeof(Student) * numberOfStudents);
	for (int i = 0; i < numberOfStudents; i++) {
		strcpy(structarr[i].name, students[i]);
		structarr[i].grades = (StudentCourseGrade*)malloc(sizeof(StudentCourseGrade) * coursesPerStudent[i]);
		structarr[i].numberOfCourses = coursesPerStudent[i];
	}
	int x = 1;
	for (int i = 0; i < numberOfStudents; i++) {
		for (int j = 0; j < coursesPerStudent[i]; j++) {
			x = x + 2;
			strcpy(structarr[i].grades[j].courseName, students[i][j + x]);
			structarr[i].grades[j].grade = atoi(students[i][j + x + 1]);
			++x;
		}
	}
	return structarr;
}
