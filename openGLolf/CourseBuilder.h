#ifndef COURSEBUILDER_H
#define COURSEBUILDER_H

#include "Definitions.h"
#include <iostream>
#include <fstream>

class CourseBuilder
{
public:
	
	struct CourseControl
	{
		int type; //used to determine what type of entity to spawn

		 //location data
		float x;
		float z;

		//used exclusively for the walls, to determine if the wall must be rotated 90 degrees
		bool bRot90;
	};

	int m_naMapData[10][10];
	std::vector<CourseControl> m_oaCourse; 
	float fDisplace = 10.f;
	float fDisplaceHalf = 5.0f;

	CourseBuilder();
	~CourseBuilder();

	bool ReadMap(std::string a_sLoc);

	void BuildMap();

};


#endif // !COURSEBUILDER_H