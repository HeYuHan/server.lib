#include "Student.h"
#include<iostream>
using namespace std;

Student::Student()
{
}

void Student::Run()
{
	cout << "Student Run t=>"<< t << endl;
}

void Student::Run(int a)
{
	cout << "Student Run " << a << endl;
}

const char * Student::GetRequireScript()
{
	return NULL;
}

const char * Student::GetNativeTypeName()
{
	return "Student";
}

Student::~Student()
{
}
