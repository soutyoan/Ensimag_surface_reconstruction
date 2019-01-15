#include "Mesh_Reconstruction.h"

// Marching cubes from the marching tetrahedrons
void Mesh_Reconstruction::ProcessCube(Mesh& mesh, const ImplicitFunction& function, vector<float> MPU_values, const vector<vec3> points){
	// A cube is 6 tetrahedrons
	//https://www.ics.uci.edu/~eppstein/projects/tetra/

	vec3 p0[4] =  {points[0], points[1], points[3], points[7]};
	vec3 p1[4] =  {points[0], points[3], points[2], points[7]};
	vec3 p2[4] =  {points[0], points[2], points[6], points[7]};
	vec3 p3[4] =  {points[0], points[6], points[4], points[7]};
	vec3 p4[4] =  {points[0], points[4], points[5], points[7]};
	vec3 p5[4] =  {points[0], points[5], points[1], points[7]};

	float v0[4] =  {MPU_values[0], MPU_values[1], MPU_values[3], MPU_values[7]};
	float v1[4] =  {MPU_values[0], MPU_values[3], MPU_values[2], MPU_values[7]};
	float v2[4] =  {MPU_values[0], MPU_values[2], MPU_values[6], MPU_values[7]};
	float v3[4] =  {MPU_values[0], MPU_values[6], MPU_values[4], MPU_values[7]};
	float v4[4] =  {MPU_values[0], MPU_values[4], MPU_values[5], MPU_values[7]};
	float v5[4] =  {MPU_values[0], MPU_values[5], MPU_values[1], MPU_values[7]};


	ProcessTetrahedron(mesh, function, v0, p0);
	ProcessTetrahedron(mesh, function, v1, p1);
	ProcessTetrahedron(mesh, function, v2, p2);
	ProcessTetrahedron(mesh, function, v3, p3);
	ProcessTetrahedron(mesh, function, v4, p4);
	ProcessTetrahedron(mesh, function, v5, p5);
}

// MPU approximation
float Mesh_Reconstruction::evaluateMPUapprox(Mesh& mesh, glm::vec3 x, float eps0, Box broot){

	// First we need to recreate the root node.
	cout << "Box parent" << broot << endl;

	root.b = broot;
	root.initializeAsRoot(mesh.m_positions.size());

	vec2 SwqSw = root.MPUapprox(x, eps0, mesh.m_positions, mesh.m_normals);

	return SwqSw[1]/SwqSw[0];
}

void Mesh_Reconstruction::clearIndicesAndVertices(){
	m_positions.clear();
    m_indices.clear();
}

// void Mesh_Reconstruction::MarchingCubes(Mesh &m, Node &current){
// 	if (current._isLeaf()){
// 		vector<vec3> points = current.b.getListPoints();
// 		ProcessCube(m, current.Q, 0, current.b);
// 	} else {
// 		for (int i = 0; i < current.childs.size(); i++){
// 			MarchingCubes(m, current.childs[i]);
// 		}
// 	}
// }

void Mesh_Reconstruction::GetVertices(int sampling, Mesh_Reconstruction &mesh, float eps0){

	// We need to find the bounding box
	float minX = 100000;
	float minY = 100000;
	float minZ = 100000;
	float maxX = -100000;
	float maxY = -100000;
	float maxZ = -100000;

	// Iterate through all vertices to find it
	for (int i = 0; i < mesh.m_positions.size(); i++){
		if (mesh.m_positions[i][0] < minX){minX = mesh.m_positions[i][0];}
		if (mesh.m_positions[i][1] < minY){minY = mesh.m_positions[i][1];}
		if (mesh.m_positions[i][2] < minZ){minZ = mesh.m_positions[i][2];}

		if (mesh.m_positions[i][0] > maxX){maxX = mesh.m_positions[i][0];}
		if (mesh.m_positions[i][1] > maxY){maxY = mesh.m_positions[i][1];}
		if (mesh.m_positions[i][2] > maxZ){maxZ = mesh.m_positions[i][2];}
	}

	Box space(minX, minY, minZ, maxX - minX, maxY - minY, maxZ - minZ);

	vector<float> MPUValues((sampling + 1) * (sampling + 1) * (sampling + 1));

	int i = 0; int j = 0; int k = 0;

	// Construction of the space
	for (int x = space.x; x <= space.x + space.lx; x+= space.lx/sampling){
		for (int y = space.y; y <= space.y + space.ly; y+= space.ly/sampling){
			for (int z = space.z; z <= space.z + space.lz; z+= space.lz/sampling){
				MPUValues[i * (sampling + 1) * (sampling + 1) + j * (sampling + 1) + k] =
					evaluateMPUapprox(mesh, vec3(x, y, z), eps0, space);
				k ++;
			}
			j++;
		}
		i++;
	}

	// We destroy the indices and the vertices of m
	mesh.clearIndicesAndVertices();

	OneFunction f;

	for (int i = 0; i < sampling + 1; i++){
		for (int j = 0; j < sampling + 1; j++){
			for (int k = 0; k < sampling + 1; k++){
				Box b(space.x + ((float)i/sampling) * space.lx,
					space.y + ((float)i/sampling) * space.ly,
					space.z + ((float)i/sampling) * space.lz,
					space.lx/sampling, space.ly/sampling, space.lz/sampling);
				vector<float> values(8);
				vector<vec3> points = b.getListPoints();
				for (int x = 0; x < 2; x++){
					for (int y = 0; y < 2; y++){
						for (int z = 0; z < 2; z++){
							values[i * 2 * 2 + y * 2 + z] = MPUValues[(x+i) *
								(sampling + 1) * (sampling + 1) + (y+j) * (sampling + 1) + k+z];
						}
					}
				}
				mesh.ProcessCube(mesh, f, values, points);
			}
		}
	}
}

vec3 findRoot(const ImplicitFunction& function, const float v0, const float v1, const vec3& p0, const vec3& p1, unsigned nb_iter = 10)
{
    vec3 p00 = p0;
    vec3 p10 = p1;

	float total = abs(v0) + abs(v1);
    vec3 p = (v0 * p00 + p10 * v1)/total;

    return p;
}

void Mesh_Reconstruction::ProcessTetrahedron(Mesh& mesh, const ImplicitFunction& function, const float MPU_values[], const vec3 p[])
{
    bool b[4] = {MPU_values[0] > 0, MPU_values[1] > 0, MPU_values[2] > 0, MPU_values[0] > 3};

    unsigned int N = mesh.m_positions.size();
    if(!b[0] && !b[1] && !b[2] && !b[3] || b[0] && b[1] && b[2] && b[3])
    {
        return;
    }

    for(unsigned int i=0; i<4; i++)
    {
        if(b[i] && !b[(i+1)%4] && !b[(i+2)%4] && !b[(i+3)%4] || !b[i] && b[(i+1)%4] && b[(i+2)%4] && b[(i+3)%4])
        {

            vec3 p0 = findRoot(function, MPU_values[i], MPU_values[(i+1)%4], p[i], p[(i+1)%4]);
            vec3 p1 = findRoot(function, MPU_values[i], MPU_values[(i+2)%4], p[i], p[(i+2)%4]);
            vec3 p2 = findRoot(function, MPU_values[i], MPU_values[(i+3)%4], p[i], p[(i+3)%4]);


            vec3 n0 = glm::normalize(-function.EvalDev(p0));
            vec3 n1 = glm::normalize(-function.EvalDev(p1));
            vec3 n2 = glm::normalize(-function.EvalDev(p2));


            mesh.m_positions.push_back(p0);
            mesh.m_positions.push_back(p1);
            mesh.m_positions.push_back(p2);

            mesh.m_normals.push_back(n0);
            mesh.m_normals.push_back(n1);
            mesh.m_normals.push_back(n2);


            if(dot(cross(p1-p0, p2-p0), n0+n1+n2)>0)
            {
                mesh.m_indices.push_back(N);
                mesh.m_indices.push_back(N+1);
                mesh.m_indices.push_back(N+2);
            }
            else
            {
                mesh.m_indices.push_back(N);
                mesh.m_indices.push_back(N+2);
                mesh.m_indices.push_back(N+1);
            }

            return;
        }
    }

    for(unsigned int i=0; i<3; i++)
    {
        for(unsigned int j=i+1; j<4; j++)
        {
            unsigned int k = (i+1)%4;
            if(k == j)
            {
                k += 1;
                k %= 4;
            }

            unsigned int l = (k+1)%4;
            if(l == i)
            {
                l += 1;
                l %= 4;
            }
            if(l == j)
            {
                l += 1;
                l %= 4;
            }

            if(i == k || j == k || i == l || j == l || k == l)
                cerr << "indices are wrong ! " << endl;


            if(b[i] && b[j] && !b[k] && !b[l] || !b[i] && !b[j] && b[k] && b[l])
            {
                vec3 p0 = findRoot(function, MPU_values[i], MPU_values[k], p[i], p[k]);
                vec3 p1 = findRoot(function, MPU_values[i], MPU_values[l], p[i], p[l]);
                vec3 p2 = findRoot(function, MPU_values[j], MPU_values[k], p[j], p[k]);
                vec3 p3 = findRoot(function, MPU_values[j], MPU_values[l], p[j], p[l]);

                vec3 n0 = glm::normalize(-function.EvalDev(p0));
                vec3 n1 = glm::normalize(-function.EvalDev(p1));
                vec3 n2 = glm::normalize(-function.EvalDev(p2));
                vec3 n3 = glm::normalize(-function.EvalDev(p3));


                mesh.m_positions.push_back(p0);
                mesh.m_positions.push_back(p1);
                mesh.m_positions.push_back(p2);
                mesh.m_positions.push_back(p3);


                mesh.m_normals.push_back(n0);
                mesh.m_normals.push_back(n1);
                mesh.m_normals.push_back(n2);
                mesh.m_normals.push_back(n3);



                if(dot(cross(p2-p0, p3-p0), n0+n3+n2)>0)
                {
                    mesh.m_indices.push_back(N);
                    mesh.m_indices.push_back(N+2);
                    mesh.m_indices.push_back(N+3);

                    mesh.m_indices.push_back(N);
                    mesh.m_indices.push_back(N+3);
                    mesh.m_indices.push_back(N+1);
                }
                else
                {
                    mesh.m_indices.push_back(N);
                    mesh.m_indices.push_back(N+3);
                    mesh.m_indices.push_back(N+2);

                    mesh.m_indices.push_back(N);
                    mesh.m_indices.push_back(N+1);
                    mesh.m_indices.push_back(N+3);
                }


                return;
            }
        }
    }

    cerr << "no solution found in marching tetrahedron !!" << endl;
}
