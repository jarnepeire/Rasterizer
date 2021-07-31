#pragma once
#include "pch.h"
#include "ObjParser.h"
#include <iostream>
#include <fstream>
#include <sstream>

using namespace Elite;
ObjParser::ObjParser(const char* filepath, bool isDX)
	: m_IsDX(isDX)
	, m_FilePath(filepath)
	, m_IndexBuffer()
	, m_VertexBuffer()
{
	InitializeParsing();
}

void ObjParser::InitializeParsing()
{
	//Start parsing file
	std::ifstream file;
	file.open(m_FilePath, std::ios::in);

	if (!file.is_open())
	{
		std::cout << "Could not parse given file: \" " << m_FilePath << " \" \n";
		return;
	}

	//Variables to store information in
	std::vector<Elite::FPoint3> vertexPoints;
	std::vector<Elite::FVector3> vertexNormals;
	std::vector<Elite::FVector2> uvCoordinates;

	std::string line;
	while (!file.eof())
	{
		std::getline(file, line);
		std::stringstream ss{ line };

		if (line[0] == 'v')
		{
			if (line[1] == ' ') //-> Vertex Point
			{
				FPoint3 vertex;

				//Reusing string line to put the first letter in there, then put the 3 components in a point
				ss >> line >> vertex.x >> vertex.y >> vertex.z;
				
				if (m_IsDX)
					vertex.z = -vertex.z;

				vertexPoints.push_back(vertex);
			}
			else if (line[1] == 'n') //-> Vertex Normal
			{
				FVector3 vertexNormal;

				//Reusing string line to put the first letter in there, then put the 3 components in a vector
				ss >> line >> vertexNormal.x >> vertexNormal.y >> vertexNormal.z;

				if (m_IsDX)
					vertexNormal.z = -vertexNormal.z;

				vertexNormals.push_back(vertexNormal);
			}
			else if (line[1] == 't') //-> UV Coordinate
			{
				FVector2 uv;

				//Reusing string line to put the first letter in there, then put the 2 components in a vector
				ss >> line >> uv.x >> uv.y;
				uv.y = 1.f - uv.y;

				uvCoordinates.push_back(uv);
			}

		}
		else if (line[0] == 'f') //-> Meaning we're looking at different faces that indicate what vertex, vertex normal and UV it has
		{
			//To store the indices for the buffers in
			//Index 0: Position
			//Index 1: UV Coordinate
			//Index 2: Vertex Normal
			int indices[3];

			//Delim character that seperates these indices in the file
			char delim{ '/' };

			//For each triplet that forms a face:
			for (unsigned int i = 0; i < 3; ++i)
			{
				//Parse information
				if (i == 0)
					ss >> delim >> indices[0] >> delim >> indices[1] >> delim >> indices[2];
				else
					ss >> indices[0] >> delim >> indices[1] >> delim >> indices[2];

				//Create vertex accordingly
				Vertex_Input v;
				v.Position = vertexPoints[(indices[0] - 1)];
				v.UV = uvCoordinates[(indices[1] - 1)];
				v.VertexNormal = vertexNormals[(indices[2] - 1)];

				//Check for duplicate information (with vertex operator overloading function)
				size_t duplicateIndex = (size_t)-1;
				for (size_t j = 0; j < m_VertexBuffer.size(); ++j)
				{
					if (v == m_VertexBuffer[j])
					{
						duplicateIndex = j;
						break;
					}
				}

				//If there wasn't any, we'll simply push the vertex in the buffer and assign the according index to the index buffer
				if (duplicateIndex == -1)
				{
					m_IndexBuffer.push_back((uint32_t)m_VertexBuffer.size());
					m_VertexBuffer.push_back(v);
				}
				//If there was a duplicate, we'll push back the same index that refers to that vertex
				else
				{
					m_IndexBuffer.push_back((uint32_t)duplicateIndex);
				}
			}
		}
	}
	
	//After reading out file, we're going to loop over our vertices to determine the tangents per vertex
	for (size_t i = 0; i < m_IndexBuffer.size(); i += 3)
	{
		size_t idx0 = m_IndexBuffer[i];
		size_t idx1 = m_IndexBuffer[i + 1];
		size_t idx2 = m_IndexBuffer[i + 2];

		const FPoint3 p0 = FPoint3(m_VertexBuffer[idx0].Position);
		const FPoint3 p1 = FPoint3(m_VertexBuffer[idx1].Position);
		const FPoint3 p2 = FPoint3(m_VertexBuffer[idx2].Position);
		const FVector2& uv0 = m_VertexBuffer[idx0].UV;
		const FVector2& uv1 = m_VertexBuffer[idx1].UV;
		const FVector2& uv2 = m_VertexBuffer[idx2].UV;

		const FVector3 edge0 = p1 - p0;
		const FVector3 edge1 = p2 - p0;
		const FVector2 diffX = FVector2(uv1.x - uv0.x, uv2.x - uv0.x);
		const FVector2 diffY = FVector2(uv1.y - uv0.y, uv2.y - uv0.y);
		float r = 1.f / Cross(diffX, diffY);

		FVector3 tangent = (edge0 * diffY.y - edge1 * diffY.x) * r;
		if (m_IsDX)
			tangent.z = -tangent.z;
		m_VertexBuffer[idx0].Tangent += tangent;
		m_VertexBuffer[idx1].Tangent += tangent;
		m_VertexBuffer[idx2].Tangent += tangent;
	}

	//Create the tangents (reject vector) + fix tangents per vertex
	for (auto& v : m_VertexBuffer)
	{
		v.Tangent = GetNormalized(Reject(v.Tangent, v.VertexNormal));
	}
}