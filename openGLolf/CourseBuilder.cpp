#include "CourseBuilder.h"

CourseBuilder::CourseBuilder()
{
}


CourseBuilder::~CourseBuilder()
{
}

void CourseBuilder::ReadMap(std::string a_sLoc)
{
}

void CourseBuilder::BuildMap()
{
	for (int i = 0; i < 10; i++)
	{
		for (int j = 0; j < 10; j++)
		{
			int nType = m_naMapData[i][j];
			switch (nType)
			{
				//empty plane
			case 0: 

				//ball spawn
			case 1:	

				//hole loc
			case 2:	

				//obstacle
			case 3: 

				//wall 
			case 4: 
				if (i - 1 < 0 || m_naMapData[i - 1][j] == -1) {}
				if (i + 1 > 9 || m_naMapData[i + 1][j] == -1) {}
				if (j - 1 < 0 || m_naMapData[i][j - 1] == -1) {}
				if (j + 1 > 9 || m_naMapData[i][j + 1] == -1) {}
			}
		}
	}
}
