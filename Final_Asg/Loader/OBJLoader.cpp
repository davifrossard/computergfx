#include "OBJLoader.h"

bool loadOBJ(const char * path, vector<vec3> & out_vertices, vector<vec2> & out_uvs, vector<vec3> & out_normals)
{
	FILE * file = fopen(path, "r");
	if(file == NULL)
    return false;

	vector<unsigned int> vIndices, uvIndices, nIndices;
	vector<vec3> temp_vertices, temp_normals;
	vector<vec2> temp_uvs;
	int res;
	char header[256];
	while((res = fscanf(file, "%s", header)) != EOF){
		if(strcmp(header, "v") == 0) {
	    vec3 vertex;
	    fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
	    temp_vertices.push_back(vertex);
		} else if (strcmp(header, "vt") == 0 ) {
      vec2 uv;
    	fscanf(file, "%f %f\n", &uv.x, &uv.y);
    	temp_uvs.push_back(uv);
		} else if (strcmp(header, "vn") == 0 ) {
    	vec3 normal;
    	fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z );
    	temp_normals.push_back(normal);
		}	else if (strcmp(header, "f") == 0 ) {
	    string vertex1, vertex2, vertex3;
	    unsigned int vIndex[3], uvIndex[3], nIndex[3];
	    int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vIndex[0], &uvIndex[0], &nIndex[0], &vIndex[1], &uvIndex[1], &nIndex[1], &vIndex[2], &uvIndex[2], &nIndex[2]);
	    vIndices.push_back(vIndex[0]);
	    vIndices.push_back(vIndex[1]);
	    vIndices.push_back(vIndex[2]);
	    uvIndices.push_back(uvIndex[0]);
	    uvIndices.push_back(uvIndex[1]);
	    uvIndices.push_back(uvIndex[2]);
	    nIndices.push_back(nIndex[0]);
	    nIndices.push_back(nIndex[1]);
	    nIndices.push_back(nIndex[2]);
		}
	}

	for(unsigned int i=0; i<vIndices.size(); i++ ){
		unsigned int vIndex = vIndices[i];
		vec3 vertex = temp_vertices[vIndex-1];
		out_vertices.push_back(vertex);

		unsigned int uvIndex = uvIndices[i];
		vec2 uv = temp_uvs[uvIndex-1];
		out_uvs.push_back(uv);

		unsigned int nIndex = nIndices[i];
		vec3 normal = temp_normals[nIndex-1];
		out_normals.push_back(normal);
	}

	return 1;
}
