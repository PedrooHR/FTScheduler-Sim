#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <map>  
#include <vector>

using namespace std;

string line;
ifstream inputfile;
ofstream datafile;
ofstream datafilecfg;
ofstream dagfile;
ofstream dagfileinv;

vector <vector <int>> Dependencies;
map <string,int> TaskIds;
int curr_id = 0;

void createtaskcfg(){
    for (int i = 0 ; i < Dependencies.size(); i++){
        datafilecfg << Dependencies[i].size() << " ";
        for (int j = 0; j < Dependencies[i].size(); j++){
            datafilecfg << Dependencies[i][j] << " ";
        }
        datafilecfg << "\n";
    }
}

void mountdag(string line){
    int id = line.find("ref=");
    int first = line.find('"', id);
    int second = line.find('"', first+1);
    string idstr = line.substr(first + 1, second - first - 1);
    int localTaskId = TaskIds[idstr];

    string lline;
    while ( getline(inputfile, lline)) {
        int prev = lline.find('<');
        int curr = lline.find(' ', prev);
        string preamble = lline.substr(prev+1, curr - prev - 1);
        if (strcmp("/child>", preamble.c_str()) == 0){
            return;
        } else {
            id = lline.find("ref=");
            first = lline.find('"', id);
            second = lline.find('"', first+1);
            string parent = lline.substr(first + 1, second - first - 1);
            dagfile << "\t" << idstr << " -> " << parent << " ;" << '\n';
            dagfileinv << "\t" << parent << " -> " << idstr << " ;" << '\n';
            int parent_id = TaskIds[parent];
            Dependencies[localTaskId].push_back(parent_id);
        }
    }
}

void mountjob(string line){
    long int inputsize = 0, outputsize = 0;
    float rt;
    int id = line.find("id=");
    int first = line.find('"', id);
    int second = line.find('"', first+1);
    string idstr = line.substr(first + 1, second - first - 1);
    TaskIds.insert(pair<string,int>(idstr,curr_id++));
    Dependencies.resize(curr_id);

    id = line.find("runtime=");
    first = line.find('"', id);
    second = line.find('"', first+1);
    string rtstr = line.substr(first + 1, second - first - 1);
    rt = stof(rtstr);

    string lline;
    while ( getline(inputfile, lline)) {
        int prev = lline.find('<');
        int curr = lline.find(' ', prev);
        string preamble = lline.substr(prev+1, curr - prev - 1);
        if (strcmp("/job>", preamble.c_str()) == 0){
            datafile << TaskIds[idstr] << " " << idstr << " " << abs(rt) << " " << max((long int)1, inputsize / (1024 * 1024)) << " " << max((long int)1, outputsize / (1024 * 1024)) << '\n';
            return;
        } else {
            id = lline.find("link=");
            first = lline.find('"', id);
            second = lline.find('"', first+1);
            string type = lline.substr(first + 1, second - first - 1);
            id = lline.find("size=");
            first = lline.find('"', id);
            second = lline.find('"', first+1);
            string sizedata = lline.substr(first + 1, second - first - 1);
            long long int i = stoll(sizedata);
            if (strcmp("input", type.c_str()) == 0){
                inputsize += i;
            } else {
                outputsize += i;
            }
        }
    }
}

int main (int argc, char * argv[]) {
    int i = 1;
    while (i < argc){
        curr_id = 0;
        Dependencies.clear();
        TaskIds.clear();
        string path = argv[i];
        string line;
        path = path.substr(0, path.size()-4);
        inputfile.open(path+".xml");
        datafile.open("output/Datafiles/"+path+".txt");
        datafilecfg.open("output/Datafiles/"+path+".cfg");
        dagfile.open("output/Dag/"+path+"_dagfile.gv");
        dagfile << "digraph {" << "\n";
        dagfileinv.open("output/Dag/"+path+"_dagfileinv.gv");
        dagfileinv << "digraph {" << "\n";

        while ( getline(inputfile, line)) {
            int prev = line.find('<');
            int curr = line.find(' ', prev);
            string preamble = line.substr(prev+1, curr - prev - 1);

            if (strcmp("job", preamble.c_str()) == 0){
                mountjob(line);
            } else if (strcmp("child", preamble.c_str()) == 0) {
                mountdag(line);
            }
        }
        createtaskcfg();
        dagfile << "}" << "\n";
        dagfileinv << "}" << "\n";
        inputfile.close();
        datafilecfg.close();
        dagfile.close();
        dagfileinv.close();
        datafile.close();
        i++;
    }
    return 0;
}
