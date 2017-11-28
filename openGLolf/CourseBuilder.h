#pragma once

#include "Definitions.h"

class CourseBuilder
{
public:
	
	int m_naMapData[10][10];
	
	CourseBuilder();
	~CourseBuilder();

	void ReadMap(std::string a_sLoc);

	void BuildMap();


};

