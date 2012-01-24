#include <iostream>
#include <fstream>

using namespace std;

int main(int argc, char * argv[]) {
	string name = "output.off";
	if(argc < 3) {
		cout << "Usage: " << argv[0] << " first_file second_file [output_file]" << endl;
		return 1;
    }
	if(argc == 4)
		name = argv[3];
	ifstream model(argv[1]);
	ifstream rays(argv[2]);
	ofstream output(name.c_str());
	
	string head;
	model >> head;
	if (head != "OFF")
        cout << "Model does not start with OFF!";
     
    rays >> head;
	if (head != "OFF") {
	    rays.close();
	    rays.open(argv[2]);
	}
	    
	output << "OFF\n";
	
	int nverts, nfaces, nedges;
	
	if (!(model >> nverts >> nfaces >> nedges))
        cout << "Could not read number of vertices, faces, edges of model";
	
	int nv, nf, ne;
	
	if (!(rays >> nv >> nf >> ne))
        cout << "Could not read number of vertices, faces, edges of rays file";
       
    output << nverts+nv << " " << nfaces+nf << " " << nedges + ne << endl;
    
    for (int i=0; i<nverts; ++i) {
    	double x, y, z;
        model >> x >> y >> z;
        output << x << " " << y << " " << z << endl;
    }
    
    for (int i=0; i<nv; ++i) {
    	double x, y, z;
        rays >> x >> y >> z;
        output << x << " " << y << " " << z << endl;
    }
    
    for (int i=0; i<nfaces; ++i) {
        int sz, v;
        model >> sz;
        output << sz;
        for(int j=0; j<sz; ++j) {
            model >> v;
            output << " " << v;
        }
        output << endl;
    }
    
    for (int i=0; i<nf; ++i) {
        int sz, v;
        rays >> sz;
        output << sz;
        for(int j=0; j<sz; ++j) {
            rays >> v;
            output << " " << v+nverts;
        }
        output << endl;
    }
        
	return 0;
}
