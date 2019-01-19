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


	ProcessTetrahedron(mesh, gradients, v0, p0);
	ProcessTetrahedron(mesh, gradients, v1, p1);
	ProcessTetrahedron(mesh, gradients, v2, p2);
	ProcessTetrahedron(mesh, gradients, v3, p3);
	ProcessTetrahedron(mesh, gradients, v4, p4);
	ProcessTetrahedron(mesh, gradients, v5, p5);
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

	int addBoundingBox = sampling;

	vector<float> MPUValues((sampling + 1 + 2 * addBoundingBox) * (sampling + 1 + 2 * addBoundingBox) * (sampling + 1 + 2 * addBoundingBox));

	int i = 0; int j = 0; int k = 0;
	int totalZeros = 0;

	root.b = space;

	// Construction of the space
	for (float x = space.x - addBoundingBox * (space.lx/sampling); x <= space.x + space.lx + addBoundingBox * (space.lx/sampling); x+= space.lx/sampling){
		// cout << "Tree creation i" << i << " out of " << sampling << endl;
		j = 0;
		cout << "------------------------------------------------------" << endl;
		for (float y = space.y - addBoundingBox * (space.ly/sampling); y <= space.y + space.ly + addBoundingBox * (space.ly/sampling); y+= space.ly/sampling){
			// cout << "Tree creation j " << j << " out of " << sampling << endl;
			k = 0;
			for (float z = space.z - addBoundingBox * (space.lz/sampling); z <= space.z + space.lz + addBoundingBox * (space.lz/sampling); z+= space.lz/sampling){
				// cout << "Tree creation k " << k << " out of " << sampling << endl;
				MPUValues[i * (sampling + 1 + 2 * addBoundingBox) * (sampling + 1 + 2 * addBoundingBox) + j * (sampling + 1 + 2 * addBoundingBox) + k] =
					evaluateMPUapprox(mesh, vec3(x, y, z), eps0);
				// cout << "value " << MPUValues[i * (sampling + 1) * (sampling + 1) + j * (sampling + 1) + k] << endl;
				if (MPUValues[i * (sampling + 1 + 2 * addBoundingBox) * (sampling + 1 + 2 * addBoundingBox) + j * (sampling + 1 + 2 * addBoundingBox) + k] > 0){
					cout << "+ ";
                    // MPUValues[i * (sampling + 5) * (sampling + 5) + j * (sampling + 5) + k] = 1;
				} else if (MPUValues[i * (sampling + 1 + 2 * addBoundingBox) * (sampling + 1 + 2 * addBoundingBox) + j * (sampling + 1 + 2 * addBoundingBox) + k] < 0) {
					cout << "- ";
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

	int res = 1;

	for (int i = 0; i < sampling + 2 * addBoundingBox - res; i+=res){
		cout << "Marching cubes " << i << " out of " << sampling << endl;
		for (int j = 0; j < sampling + 2 * addBoundingBox - res; j+=res){
			for (int k = 0; k < sampling + 2 * addBoundingBox - res; k+=res){
				Box b(space.x + (i - addBoundingBox) * (float)space.lx/sampling,
					space.y + (j - addBoundingBox) * (float)space.ly/sampling,
					space.z + (k - addBoundingBox) * (float)space.lz/sampling,
					res * space.lx/sampling, res * space.ly/sampling, res * space.lz/sampling);
				vector<float> values(8);
				vector<vec3> points = b.getListPoints();
				vector<vec3> gradients(8);
				for (int x = 0; x < 2; x++){
					for (int y = 0; y < 2; y++){
						for (int z = 0; z < 2; z++){
							values[x * 2 * 2 + y * 2 + z] = MPUValues[(x * res+i) *
								(sampling + 1 + 2 * addBoundingBox) * (sampling + 1 + 2 * addBoundingBox) +
								(y*res+j) * (sampling + 1 + 2 * addBoundingBox) + k+z*res];
							float x_i_1 = MPUValues[(x*res+i+1) *
								(sampling + 1 + 2 * addBoundingBox) * (sampling + 1 + 2 * addBoundingBox) + (y*res+j) *
								(sampling + 1 + 2 * addBoundingBox) + k+z*res];
							float x_i = MPUValues[(x*res+i) *
									(sampling + 1 + 2 * addBoundingBox) * (sampling + 1 + 2 * addBoundingBox) +
									(y*res+j) * (sampling + 1 + 2 * addBoundingBox) + k+z*res];
							float y_i_1 = MPUValues[(x*res+i) *
									(sampling + 1 + 2 * addBoundingBox) * (sampling + 1 + 2 * addBoundingBox) +
									(y*res+j+1) * (sampling + 1 + 2 * addBoundingBox) + k+z*res];
							float z_i_1 = MPUValues[(x*res+i) *
									(sampling + 1 + 2 * addBoundingBox) * (sampling + 1 + 2 * addBoundingBox) +
									(y*res+j) * (sampling + 1 + 2 * addBoundingBox) + k+z*res+1];
							vec3 g = vec3(x_i_1 - x_i, y_i_1 - x_i, z_i_1 - x_i); // discretisation of the gradient
							gradients[x * 2 * 2 + y * 2 + z] = g;
						}
					}
				}
				mesh.ProcessCube(mesh, gradients, values, points);
			}
		}
	}
}

vec3 findRoot(const float v0, const float v1, const vec3& p0, const vec3& p1, unsigned nb_iter = 10)
{
    vec3 p00;
    vec3 p10;

	if (v1 < v0){
		p00 = p1;
		p10 = p0;
	} else {
		p00 = p0;
		p10 = p1;
	}

	if (v0 == 0){
		return p00;
	}

	float total = abs(v0) + abs(v1);
    vec3 p = p0 + (abs(v0)/total) * (p1 - p0);

	// cout << "values " << v0 << " " << v1 << " " << total << endl;
	// cout << "p1 " << p1[0] << " " << p1[1] << " " << p1[2] << endl;
	// cout << "p0 " << p0[0] << " " << p0[1] << " " << p0[2] << endl;
	// cout << "p " << p[0] << " " << p[1] << " " << p[2] << endl;

    return p;
}

void Mesh_Reconstruction::ProcessTetrahedron(Mesh& mesh, const vector<vec3> gradients, const float MPU_values[], const vec3 p[])
{
    bool b[4] = {MPU_values[0] > 0, MPU_values[1] > 0, MPU_values[2] > 0, MPU_values[3] > 0};

    unsigned int N = mesh.m_positions.size();
    if((!b[0] && !b[1] && !b[2] && !b[3]) || (b[0] && b[1] && b[2] && b[3]))
    {
        return;
    }

    for(unsigned int i=0; i<4; i++)
    {
        if(((MPU_values[i] > 0) && (MPU_values[(i+1)%4] < 0) && (MPU_values[(i+2)%4] < 0) && (MPU_values[(i+3)%4] < 0)) ||
			((MPU_values[i] < 0) && (MPU_values[(i+1)%4] > 0) && (MPU_values[(i+2)%4] > 0) && (MPU_values[(i+3)%4] > 0)))
        {

            vec3 p0 = findRoot(MPU_values[i], MPU_values[(i+1)%4], p[i], p[(i+1)%4]);
            vec3 p1 = findRoot(MPU_values[i], MPU_values[(i+2)%4], p[i], p[(i+2)%4]);
            vec3 p2 = findRoot(MPU_values[i], MPU_values[(i+3)%4], p[i], p[(i+3)%4]);

            vec3 n0 = glm::normalize(-(gradients[i] * abs(MPU_values[(i+1)%4]) + gradients[(i+1)%4] *
				abs(MPU_values[i]))/(abs(MPU_values[(i+1)%4]) + abs(MPU_values[i])));
            vec3 n1 = glm::normalize(-(gradients[i] * abs(MPU_values[(i+2)%4]) + gradients[(i+2)%4] *
				abs(MPU_values[i]))/(abs(MPU_values[(i+2)%4]) + abs(MPU_values[i])));
            vec3 n2 = glm::normalize(-(gradients[i] * abs(MPU_values[(i+3)%4]) + gradients[(i+3)%4] *
				abs(MPU_values[i]))/(abs(MPU_values[(i+3)%4]) + abs(MPU_values[i])));

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


            if(((MPU_values[i] > 0) && (MPU_values[j] > 0) && (MPU_values[k] < 0) && (MPU_values[l] < 0)) ||
				((MPU_values[i] < 0) && (MPU_values[j] < 0) && (MPU_values[k] > 0) && (MPU_values[l] > 0)))
            {
                vec3 p0 = findRoot(MPU_values[i], MPU_values[k], p[i], p[k]);
                vec3 p1 = findRoot(MPU_values[i], MPU_values[l], p[i], p[l]);
                vec3 p2 = findRoot(MPU_values[j], MPU_values[k], p[j], p[k]);
                vec3 p3 = findRoot(MPU_values[j], MPU_values[l], p[j], p[l]);

                vec3 n0 = glm::normalize(-(gradients[i] * abs(MPU_values[k]) + gradients[k] *
					abs(MPU_values[i]))/(abs(MPU_values[k]) + abs(MPU_values[i])));
                vec3 n1 = glm::normalize(-(gradients[i] * abs(MPU_values[l]) + gradients[l] *
					abs(MPU_values[i]))/(abs(MPU_values[l]) + abs(MPU_values[i])));
                vec3 n2 = glm::normalize(-(gradients[j] * abs(MPU_values[k]) + gradients[k] *
					abs(MPU_values[j]))/(abs(MPU_values[k]) + abs(MPU_values[j])));
                vec3 n3 = glm::normalize(-(gradients[j] * abs(MPU_values[l]) + gradients[l] *
					abs(MPU_values[j]))/(abs(MPU_values[l]) + abs(MPU_values[j])));

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

    // cerr << "no solution found in marching tetrahedron !!" << endl;
}
