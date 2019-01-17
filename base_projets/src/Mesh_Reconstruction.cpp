#include "Mesh_Reconstruction.h"

// Marching cubes from the marching tetrahedrons
void Mesh_Reconstruction::ProcessCube(Mesh& mesh, const vector<vec3> gradients, vector<float> MPU_values, const vector<vec3> points){
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


	// ProcessTetrahedron(mesh, gradients, v0, p0);
	// ProcessTetrahedron(mesh, gradients, v1, p1);
	// ProcessTetrahedron(mesh, gradients, v2, p2);
	// ProcessTetrahedron(mesh, gradients, v3, p3);
	// ProcessTetrahedron(mesh, gradients, v4, p4);
	// ProcessTetrahedron(mesh, gradients, v5, p5);
}

// MPU approximation
float Mesh_Reconstruction::evaluateMPUapprox(Mesh& mesh, glm::vec3 x, float eps0){

	// First we need to recreate the root node.
	// cout << "Box parent" << broot << endl;

	root.initializeAsRoot(mesh.m_positions.size());

	vec2 SwqSw = root.MPUapprox(x, eps0, mesh.m_positions, mesh.m_normals);

	if ((std::isinf(SwqSw[0])) || (std::isinf(SwqSw[1]))){
		cout << "NAN";
	}

	// cout << "test " << SwqSw[1] << " " <<SwqSw[0] << " 0" << endl;

	if ((SwqSw[0] == 0) || (std::isinf(SwqSw[0])) || (std::isinf(SwqSw[1]))){
		//cout << "d0"; 
		return 0;
	}

	// cout << "test " << SwqSw[1] << " " <<SwqSw[0] << " " << SwqSw[1]/SwqSw[0] << endl;

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

	vector<float> MPUValues((sampling + 5) * (sampling + 5) * (sampling + 5));
	vector<vec3> points; 

	int i = 0; int j = 0; int k = 0;
	int totalZeros = 0;

	root.b = space;

	// Construction of the space
	for (float x = space.x - 2 * (space.lx/sampling); x <= space.x + space.lx + 2 * (space.lx/sampling); x+= space.lx/sampling){
		// cout << "Tree creation i" << i << " out of " << sampling << endl;
		j = 0;
		cout << "------------------------------------------------------" << endl;
		for (float y = space.y - 2 * (space.ly/sampling); y <= space.y + space.ly + 2 * (space.ly/sampling); y+= space.ly/sampling){
			// cout << "Tree creation j " << j << " out of " << sampling << endl;
			k = 0;
			for (float z = space.z - 2 * (space.lz/sampling); z <= space.z + space.lz + 2 * (space.lz/sampling); z+= space.lz/sampling){
				// cout << "Tree creation k " << k << " out of " << sampling << endl;
				MPUValues[i * (sampling + 5) * (sampling + 5) + j * (sampling + 5) + k] =
					evaluateMPUapprox(mesh, vec3(x, y, z), eps0);
				// cout << "value " << MPUValues[i * (sampling + 1) * (sampling + 1) + j * (sampling + 1) + k] << endl;
				if (MPUValues[i * (sampling + 5) * (sampling + 5) + j * (sampling + 5) + k] > 0){
					cout << "+ ";
                    // MPUValues[i * (sampling + 5) * (sampling + 5) + j * (sampling + 5) + k] = 1; 
					points.push_back(vec3(x, y, z)); 
				} else if (MPUValues[i * (sampling + 5) * (sampling + 5) + j * (sampling + 5) + k] < 0) {
					cout << "- ";
					points.push_back(vec3(x, y, z)); 
                    // MPUValues[i * (sampling + 5) * (sampling + 5) + j * (sampling + 5) + k] = -1; 
				} else {
					cout << "  ";
					totalZeros ++;
				}
				k ++;
			}
			cout << "|" << endl;
			j++;
		}
		cout << endl;
		i++;
	}

	cout << endl << endl << "WARNING THERE WERE " << totalZeros << " ZEROS " << endl << endl;

	// We destroy the indices and the vertices of m
	mesh.clearIndicesAndVertices();

	MPUFunction f; 
	f.space = space; 
	f.values = MPUValues; 

	vec3 points_arr[points.size()];
	copy(points.begin(), points.end(), points_arr);

	mesh.ProcessTetrahedronR(mesh, f, 0, points_arr); 
}

vec3 findRootR(const ImplicitFunction& function, const float isoValue, const vec3& p0, const vec3& p1, unsigned nb_iter = 10)
{
    vec3 p00 = p0;
    vec3 p10 = p1;
    if(function.Eval(p0) > function.Eval(p1))
    {
        swap(p00, p10);
    }

	float evalp0 = function.Eval(p0); 
	float evalp1 = function.Eval(p1); 

	float total = abs(evalp0) + abs(evalp1);
    vec3 p = p0 + (abs(evalp0)/total) * (p1 - p0);

    return p;
}

void Mesh_Reconstruction::ProcessTetrahedronR(Mesh& mesh, const ImplicitFunction& function, const float isoValue, const vec3 p[])
{
    bool b[4] = {function.Eval(p[0]) > isoValue, function.Eval(p[1]) > isoValue, function.Eval(p[2]) > isoValue, function.Eval(p[3]) > isoValue};

    unsigned int N = mesh.m_positions.size();
    if(!b[0] && !b[1] && !b[2] && !b[3] || b[0] && b[1] && b[2] && b[3])
    {
        return;
    }

    for(unsigned int i=0; i<4; i++)
    {
        if(b[i] && !b[(i+1)%4] && !b[(i+2)%4] && !b[(i+3)%4] || !b[i] && b[(i+1)%4] && b[(i+2)%4] && b[(i+3)%4])
        {

            vec3 p0 = findRootR(function, isoValue, p[i], p[(i+1)%4]);
            vec3 p1 = findRootR(function, isoValue, p[i], p[(i+2)%4]);
            vec3 p2 = findRootR(function, isoValue, p[i], p[(i+3)%4]);


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
                vec3 p0 = findRootR(function, isoValue, p[i], p[k]);
                vec3 p1 = findRootR(function, isoValue, p[i], p[l]);
                vec3 p2 = findRootR(function, isoValue, p[j], p[k]);
                vec3 p3 = findRootR(function, isoValue, p[j], p[l]);

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

