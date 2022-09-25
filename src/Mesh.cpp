#include "Mesh.h"


void Mesh::loadOBJ(std::string filename)
{
	/**
	 * OBJ file format:
	 * '#'  = comments
	 * 'v'  = vertex coordinates: 3 floats x-y-z
	 * 'vt' = vertex texture coordinates: 2 floats u-v
	 * 'vn' = vertex normals: 3 floats x-y-z
	 * 'f'  = faces are represented by a set of id numbers separated by a "/" and space :vertex_id/texture_id/normal_id
	 *  For example: f v1/vt1/vn1 v2/vt2/vn2 v3/vt3/vn3
	 */

	std::ifstream filestream;
	filestream.open(filename.c_str());
	
	std::string line_stream;
	while(std::getline(filestream, line_stream))
	{
		std::stringstream str_stream(line_stream);
		std::string type_str;
		str_stream >> type_str;
	
		if(type_str == "v")
		{
			Vector3f position;
			str_stream >> position.x >> position.y >> position.z;
			positions.push_back(position);
		}
		else if(type_str == "vt")
		{
			Vector2f texture;
			str_stream >> texture.x >> texture.y;
			texcoords.push_back(texture);
		}
		else if(type_str == "vn")
		{
			Vector3f normal;
			str_stream >> normal.x >> normal.y >> normal.z;
			normals.push_back(normal);
		}
		else if(type_str == "f")
		{
			char temp;
			Face face;
			unsigned int v1,v2,v3;
			for(int i = 0; i < 3; ++i)
			{
				str_stream >> v1 >> temp >> v2  >> temp >> v3;
				face.position_index.push_back(v1-1);
				face.texturecoord_index.push_back(v2-1);
				face.normal_index.push_back(v3-1);
			}
			faces.push_back(face);
		}
	}
	
	// Explicit closing of the file
	filestream.close();

	// Report input
	std::cout 	<< "Loaded " 			<< filename 		<< "\n" 
				<< "\t Positions: " 	<< positions.size() << "\n" 
				<< "\t Normals: " 		<< normals.size() 	<< "\n" 
				<< "\t Tex Coords: " 	<< texcoords.size() << "\n" 
				<< "\t Faces: " 		<< faces.size() 	<< "\n" << std::endl;
				
				
	initBuffers();
}

// Initialise vertex array buffers
void Mesh::initBuffers()
{
	std::cout << "Start Init Mesh Buffers" << std::endl;
	
	// Initialise Buffers
	glGenBuffers(1, &positionBuffer);
	glGenBuffers(1, &normalBuffer);
	glGenBuffers(1, &texcoordBuffer);
	
	// Data
	std::vector<GLfloat> vertexPositionData;
	std::vector<GLfloat> vertexNormalData;
	std::vector<GLfloat> vertexTexcoordData;

	// Go Through each Face and Add to Data
	for(int face_i = 0 ; face_i < faces.size(); face_i++)
	{
		Face face = faces[face_i];
		
		for(int vertex_i = 0 ; vertex_i < 3; vertex_i++)
		{
			// Add Positions
			if(positions.size() > 0)
			{
				Vector3f v = positions[face.position_index[vertex_i]];
				vertexPositionData.push_back(v.x);
				vertexPositionData.push_back(v.y);
				vertexPositionData.push_back(v.z);

				// Determine Minimum Point
				if (v.x < min.x)
					min.x = v.x;
				if (v.y < min.y)
					min.y = v.y;
				if (v.z < min.z)
					min.z = v.z;

				// Determine Maximum Point
				if (v.x > max.x)
					max.x = v.x;
				if (v.y > max.y)
					max.y = v.y;
				if (v.z > max.z)
					max.z = v.z;
			}

			// Add Normals
			if(normals.size() > 0)
			{
				Vector3f n = normals[face.normal_index[vertex_i]];
				vertexNormalData.push_back(n.x);
				vertexNormalData.push_back(n.y);
				vertexNormalData.push_back(n.z);
			}
			
			// Add Texture Coordinates
			if(texcoords.size() > 0)
			{
				Vector2f t = texcoords[face.texturecoord_index[vertex_i]];
				vertexTexcoordData.push_back(t.x);
				vertexTexcoordData.push_back(t.y);
			}
		}
	}
	
	// Set Data for Position Buffer
	if(positions.size() > 0)
	{
		glBindBuffer(GL_ARRAY_BUFFER, positionBuffer);
		glBufferData(
			GL_ARRAY_BUFFER,
			vertexPositionData.size() * sizeof(GLfloat),
			&vertexPositionData[0],
			GL_STATIC_DRAW
		);
	}
	
	// Set Data for Normal Buffer
	if(normals.size() > 0)
	{
		glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
		glBufferData(
			GL_ARRAY_BUFFER, 
			vertexNormalData.size() * sizeof(GLfloat), 
			&vertexNormalData[0], 
			GL_STATIC_DRAW
		);
	}

	// Set Data for Texture Coordinates Buffer
	if(texcoords.size() > 0)
	{
		glBindBuffer(GL_ARRAY_BUFFER, texcoordBuffer);
		glBufferData(GL_ARRAY_BUFFER,
			vertexTexcoordData.size() * sizeof(GLfloat),
			&vertexTexcoordData[0],
			GL_STATIC_DRAW
		);
	}

	std::cout << "End Init Mesh Buffers" << std::endl;

}

// Function to Draw a Mesh 
void Mesh::draw(GLuint vertexPositionAttribute, GLuint vertexNormalAttribute, GLuint vertexTexcordAttribute)
{
	// Vertex Position Attribute and Buffer
	if(positions.size() > 0)
	{
		glEnableVertexAttribArray(vertexPositionAttribute);
		glBindBuffer(GL_ARRAY_BUFFER, positionBuffer);
		glVertexAttribPointer(
			vertexPositionAttribute, 		// The attribute we want to configure
			3,                  			// Size
			GL_FLOAT,        			    // Type
			GL_FALSE,           			// Normalized?
			0,                  			// Stride
			(void*)0            			// Array buffer offset
		);
	}

	if(normals.size() > 0 && vertexNormalAttribute != -1)
	{
		glEnableVertexAttribArray(vertexNormalAttribute);
		glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
		glVertexAttribPointer(
			vertexNormalAttribute, 		// The attribute we want to configure
			3,                 		 	// Size
			GL_FLOAT,           		// Type
			GL_FALSE,           		// Normalized?
			0,                  		// Stride
			(void*)0           			// Array buffer offset
		);
	}

	if(texcoords.size() > 0 && vertexTexcordAttribute != -1)
	{
		glEnableVertexAttribArray(vertexTexcordAttribute);
		glBindBuffer(GL_ARRAY_BUFFER, texcoordBuffer);
		glVertexAttribPointer(
			vertexTexcordAttribute, 	// The attribute we want to configure
			2,                  		// Size
			GL_FLOAT,           		// Type
			GL_FALSE,          			// Normalized?
			0,                 			// Stride
			(void*)0           			// Array buffer offset
		);
	}

	// Draw Arrays
	glDrawArrays(GL_TRIANGLES, 0, faces.size() * 3); 
	
	// Disable Vertex Psition Array
	if(positions.size() > 0)
	{
		glDisableVertexAttribArray(vertexPositionAttribute);
	}
	
	// Disable Vertex Normal Array
	if(normals.size() > 0  && vertexNormalAttribute != -1)
	{
		glDisableVertexAttribArray(vertexNormalAttribute);
	}

	// Disable Vertex Texture Coordinates Array
	if(texcoords.size() > 0 && vertexTexcordAttribute != -1)
	{
		glDisableVertexAttribArray(vertexTexcordAttribute);
	}
}



void Mesh::drawAABB(GLuint vertexPositionAttribute)
{
	// Vertex position of bounding box
	GLfloat vertexPositionData[] =
	{
		// Back surface
		max.x, max.y, max.z,		// Coordinate - Back Top Right (Max)
		min.x, max.y, max.z,		// Coordinate - Back Top Left
		min.x, min.y, max.z,		// Coordinate - Back Bottom Left
		max.x, min.y, max.z,		// Coordinate - Back Bottom Right

		// Front surface
		max.x, max.y, min.z,		// Coordinate - Front Top Right
		min.x, max.y, min.z,		// Coordinate - Front Top Left
		min.x, min.y, min.z,		// Coordinate - Front Bottom Left (Min)
		max.x, min.y, min.z,		// Coordinate - Front Bottom Right

		// Right surface
		max.x, max.y, max.z,		// Coordinate - Back Top Right (Max)
		max.x, min.y, max.z,		// Coordinate - Back Bottom Right
		max.x, min.y, min.z,		// Coordinate - Front Bottom Right
		max.x, max.y, min.z,		// Coordinate - Front Top Right

		// Left surface
		min.x, max.y, max.z,		// Coordinate - Back Top Left
		min.x, min.y, max.z,		// Coordinate - Back Bottom Left
		min.x, min.y, min.z,		// Coordinate - Front Bottom Left (Min)
		min.x, max.y, min.z,		// Coordinate - Front Top Left	
	};

	// Create Vertex Buffer and Upload Data
	glGenBuffers(1, &AABBPositionBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, AABBPositionBuffer);
	glBufferData(GL_ARRAY_BUFFER,
		sizeof(vertexPositionData),
		vertexPositionData, 
		GL_STATIC_DRAW
	);

	// Enable vertexPositionAttribute and bind vertex position buffer to vertexPositonAttribute
	glEnableVertexAttribArray(vertexPositionAttribute);
	glBindBuffer(GL_ARRAY_BUFFER, AABBPositionBuffer);
	glVertexAttribPointer(
		vertexPositionAttribute,	// The attribute we want to configure
		3,							// size
		GL_FLOAT,					// type
		GL_FALSE,					// normalized?
		0,							// stride
		(void*)0					// array buffer offset
	);

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glDrawArrays(GL_QUADS, 0, 16);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	// Disable VertexPositionAttribute
	glDisableVertexAttribArray(vertexPositionAttribute);
}

void Mesh::transformAABB(Vector3f translate, Vector3f scale)
{
	transformedMin.x = (min.x) + (translate.x * scale.x);
	transformedMin.y = (min.y) + (translate.y * scale.y);
	transformedMin.z = (min.z) + (translate.z * scale.z);

	transformedMax.x = (max.x) + (translate.x * scale.x);
	transformedMax.y = (max.y) + (translate.y * scale.y);
	transformedMax.z = (max.z) + (translate.z * scale.z);
}
