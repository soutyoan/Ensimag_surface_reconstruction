#include "EvaluationTree.h"
#include "EvaluationTreeD.h"
#include "FileManager.h"
#include "Bloomenthal.h"
#include "ROct.h"
#include "RCross.h"
#include "RCube.h"
#include "RSix.h"
#include <time.h>
#include <stdlib.h>

int main(int argc, char** argv){
  char* in;
  char* out;
  float resolution;
  float accuracy;

  cout << argc << endl;

  if(argc != 4){
    in = argv[1];
    out = argv[2];
    resolution = (float)atof(argv[3]);
    accuracy = (float)atof(argv[4]);
  }
  else if(argc == 4){
    in = argv[1];
    out = argv[2];
  }
  else{
    printf("You need to specify data as arguments;\n");
    printf("input output resolution accuracy\n");
    printf("OR \n input output mesh-file\n\n");
    return -1;
  }

  EvaluationTreeD* func = FileManager::readRbfFile(in);
  Mesh* mesh;

  if(argc != 4){
    polygonizer::Bloomenthal poly;
    poly.func = new RSix; //RCube; //func;
    //float box[] = {func->max[0], func->min[0], func->max[1], func->min[1], func->max[2], func->min[2]};
	float box[] = {10, -10, 10, -10, 10, -10};

    if(argc > 5){
      for(int i=5; i<11; i++){
        box[i-5] = (float)atof(argv[i]);
      }
    }
    else{
      printf("You can also specify a bounding box of the polygonization by appending;\n");
      printf("x_max x_min y_max y_min z_max z_min\n\n");
    }

    printf("Polygonization is started.\n");
    clock_t start = clock();

    mesh = new Mesh;
    poly.accuracy = accuracy;
    poly.polygonize(mesh, resolution, box, 0, 0, 0,
		            //func->seed[0], func->seed[1], func->seed[2],
                    /*0.5f*(box[0]+box[1]), 0.5f*(box[2]+box[3]), 0.5f*(box[4]+box[5]),*/ 1);

    clock_t duration = clock() - start;
    printf("Polygonization is finished.\n");
    printf("Polygonization time is %d sec.\n\n", duration/ CLOCKS_PER_SEC);

    FileManager::writeMeshFile(mesh, out);
  }
  else{
    mesh = FileManager::readMeshFile(argv[3]);
    mesh->rescale(20.0f);
    clock_t start = clock();
    FileManager::writeMeshWithRd(mesh, func /*new RCube*/, out);
    //FileManager::writeMeshWithTensorFile(mesh, new ROct/*func*/, out);
    clock_t duration = clock() - start;
    printf("Curvature-sampling time is %d sec.\n\n", duration/ CLOCKS_PER_SEC);
  }
  //func->removeInvalidTriangles(mesh, 12);

  //FileManager::writeMeshFile(mesh, out);
  //FileManager::writeMeshWithTensorFile(mesh, func, out);
  //FileManager::writeMeshWithHkFile(mesh, func, out);
}
