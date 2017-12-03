#include "CourseBuilder.h"

CourseBuilder::CourseBuilder()
{
}


CourseBuilder::~CourseBuilder()
{
}

bool CourseBuilder::ReadMap(std::string a_sLoc)
{
	std::vector<std::string> asLines;
	std::string sLine;
	std::ifstream mapFile(a_sLoc);

	if (mapFile.is_open())
	{
		std::getline(mapFile, sLine); //ignore the sizing data

		//get each line of the map file and write it to the 
		while (std::getline(mapFile, sLine))
		{
			asLines.push_back(sLine);
		} //end while
		mapFile.close();

		for (int i = 0; i < asLines.size(); i++)
		{
			std::vector<char> cstr(asLines[i].c_str(), asLines[i].c_str() + asLines[i].size() + 1);
			int k = 0;
			std::string input;
			std::vector<int> temp;
			for (int j = 0; j < cstr.size(); j++)
			{
				//std::cout << cstr[j] << " || k is: " << k << std::endl;
				if (cstr[j] == ',' || j == cstr.size() -1) {temp.push_back( std::stoi(input)); k++; input = ""; }
				else input += cstr[j];
			} //end for j

			m_naMapData.push_back(temp); 
			temp.clear();
		} //end for i

		
		return true;
	} //end if
	else std::cout << "Cannot open file";
	return false;
}

void CourseBuilder::BuildMap()
{
	
	for (int i = 0; i < m_naMapData.size(); i++)
	{
		for (int j = 0; j < m_naMapData[i].size(); j++)
		{
			CourseControl temp;

			int nType = m_naMapData[i][j];
		
			//For each square that is a part of the course, make a greens tile. 
			//Special tiles will spawn additional entities ontop of the green.
			if (nType != -1)
			{
				temp.type = 0;
				temp.x = i * fDisplace;
				temp.z = j * fDisplace;
				temp.bRot90 = false;

				m_oaCourse.push_back(temp);
			}

			switch (nType)
			{
				//Hole
			case 0: 
				temp.type = 1;
				temp.x = i * fDisplace;
				temp.z = j * fDisplace;
				temp.bRot90 = false;
				m_oaCourse.push_back(temp);
				break;

				//Wall
				/*
				Wall cases are the most peculiar. You need to check each of the four surrounding tiles on your grid, and see if they are out of bounds (-1)
				If a bordering tile is out of bounds, you need to build a wall against that edge of the piece of the green.
				Alternatively, a wall tile may be on the edge of the playable space, at which point it MUST have an edge there.
				Check for this first, as it will skip checking an invalid index if it already knows to build the wall.
				*/
			case 1:

				//Left wall
				if (i - 1 < 0 || m_naMapData[i - 1][j] == -1) 
				{
					temp.type = 2;
					temp.x = (i * fDisplace) - fDisplaceHalf; //Center of the plane plus the width of the plane, backed into the scene
					temp.z = j * fDisplace; //Centered with the plane center for that column.
					temp.bRot90 = true; //is left right
					m_oaCourse.push_back(temp);
				}

				//Right wall
				else if (i + 1 > 9 || m_naMapData[i + 1][j] == -1) 
				{
					temp.type = 2;
					temp.x = (i * fDisplace) + fDisplaceHalf; //Center of the plane plus the width of the plane, backed into the scene
					temp.z = j * fDisplace; //Centered with the plane center for that column.
					temp.bRot90 = true; //is left right
					m_oaCourse.push_back(temp);
				}

				//Front
				if (j - 1 < 0 || m_naMapData[i][j - 1] == -1) 
				{
					temp.type = 2;
					temp.x = i * fDisplace; //Center of the plane
					temp.z = (j * fDisplace) - fDisplaceHalf; //Center of the plane minus half the width
					temp.bRot90 = false; //is top bottom
					m_oaCourse.push_back(temp);
				}

				//Back
				else if (j + 1 > 9 || m_naMapData[i][j + 1] == -1) 
				{
					temp.type = 2;
					temp.x = i * fDisplace; //Center of the plane
					temp.z = (j * fDisplace) + fDisplaceHalf; //Center of the plane plus half the width
					temp.bRot90 = false; //is top bottom
					m_oaCourse.push_back(temp);
				}

				break;

				//obstacle
			case 3: 
				temp.type = 3;
				temp.x = i * fDisplace;
				temp.z = j * fDisplace;
				temp.bRot90 = false;
				m_oaCourse.push_back(temp);
				break;

				//Spawn 
			case 4: 
				temp.type = 4;
				temp.x = i * fDisplace;
				temp.z = j * fDisplace;
				temp.bRot90 = false;

				m_oaCourse.push_back(temp);
				break;

				//Sliding Obstacle
			case 5:
				temp.type = 5;
				temp.x = i * fDisplace;
				temp.z = j * fDisplace;
				temp.bRot90 = false;
				temp.bLerp = true;

				m_oaCourse.push_back(temp);
				break;

				//Rotating Bridge
			case 6:
				temp.type = 6;
				temp.x = i * fDisplace;
				temp.z = j * fDisplace;
				temp.bRot90 = false;
				temp.bLerp = true;

				m_oaCourse.push_back(temp);
				break;

				//Portal
			case 7:
				temp.type = 7;
				temp.x = i * fDisplace;
				temp.z = j * fDisplace;
				temp.bRot90 = false;

				m_oaCourse.push_back(temp);
				break;

				//Water hazard
			case 8: 
				temp.type = 8;
				temp.x = i * fDisplace;
				temp.z = j * fDisplace;
				temp.bRot90 = false;

				m_oaCourse.push_back(temp);
				break;

				
			}
		}
	}
}