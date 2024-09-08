#include <iostream>
#include <unordered_map>
#include <vector>
#include <string>
#include <fstream>
#include <queue>
#include <bits/stdc++.h>

using namespace std;

struct Proceso{
	int id, bt, at, p, remaining;
	int start, end, waiting, tat = 0;
};

bool compareProcesos(Proceso p1, Proceso p2){
	if(p1.at!=p2.at){
		return (p1.at<p2.at);
	}else{
		return (p1.id<p2.id);
	}
}

vector<string> split(const std::string& cadena) {
    vector<string> resultado;
    string tempo = "";
    int i = 0;
    while (i < cadena.size()) {
        if (cadena[i] == ' ') {
            if (!tempo.empty()) {
                resultado.push_back(tempo);
                tempo = ""; 
            }
        }else {
            tempo += cadena[i];
        }
        i++;
    }if (!tempo.empty()) {
        resultado.push_back(tempo);
    }

    return resultado;
}

vector<Proceso> leerMLQ(const string& nombreArchivo){
    vector<Proceso> resultado;
    ifstream archivo(nombreArchivo);
    string linea;
    if (archivo.is_open()) {
        while (getline(archivo, linea)) {
            vector<string> inProceso = split(linea);
            Proceso temp = {stoi(inProceso[0]),stoi(inProceso[1]),stoi(inProceso[2]),stoi(inProceso[3])};
            temp.start = -1;
            temp.end, temp.waiting, temp.tat = 0;
            temp.remaining = temp.bt;
            resultado.push_back(temp);
        }
        archivo.close();
    } else {
        cout << "No se pudo abrir el archivo." << std::endl;
    }
    return resultado;
}

vector<Proceso> leerMLFQ(const string& nombreArchivo){
    vector<Proceso> resultado;
    ifstream archivo(nombreArchivo);
    string linea;
    if (archivo.is_open()) {
        while (getline(archivo, linea)) {
            vector<string> inProceso = split(linea);
            Proceso temp = {stoi(inProceso[0]),stoi(inProceso[1]),stoi(inProceso[2])};
            temp.start = -1;
            temp.end, temp.waiting, temp.tat = 0;
            temp.remaining = temp.bt;
            resultado.push_back(temp);
        }
        archivo.close();
    } else {
        cout << "No se pudo abrir el archivo." << std::endl;
    }
    return resultado;
}

vector<double> calculate(vector<Proceso> procesosEjecutados){
	vector<double> waitTAT = {0, 0};
	cout << "Id"<< "\t" 
             << "AT" << "\t" 
             << "BT" << "\t"
             << "RT" << "\t"
             << "CT" << "\t"
             << "WT" << "\t"
             << "TAT" << "\n";
	for(Proceso pro:procesosEjecutados){
		cout << pro.id << "\t" 
             << pro.at << "\t" 
             << pro.bt << "\t"
             << pro.start << "\t"
             << pro.end << "\t"
             << pro.waiting << "\t"
             << pro.tat << "\n";
        
        waitTAT[0] += pro.waiting;
        waitTAT[1] += pro.tat;
	}
	return waitTAT;
}

void simularFCFS(queue<Proceso> &cola, int &time, vector<Proceso> &procesosEjecutados){
	while(!cola.empty()){
		Proceso p = cola.front();
		cola.pop();
		
		p.start = max(time, p.at);
		p.end = p.start + p.bt;
		p.waiting = p.start - p.at;
		p.tat = p.end - p.at;
		
		time = p.end;
		
		procesosEjecutados.push_back(p);
		
		cout << "El proceso " << p.id << " fue atendido en el tiempo " << p.start << " y finalizo en tiempo " << p.end << endl;
		
	}
}

void simularRoundRobin(queue<Proceso> &cola, int &time, vector<Proceso> &procesosEjecutados, int quantum){
	queue<Proceso> temp;
	while(!cola.empty() || !temp.empty()){
		if (cola.empty() &&  !temp.empty()){
			cola = temp;
			temp = queue<Proceso>();
		}
		
		Proceso p = cola.front();
		cola.pop();
		
		if (p.start == -1){
			p.start = max(time, p.at);
		}
		
		int timeSlice = min(quantum, p.remaining);
		
		time += timeSlice;
		p.remaining -= timeSlice;
		
		if(p.remaining == 0){
			p.end = time;
			p.tat = p.end - p.at;
			p.waiting = p.tat - p.bt;
			
			procesosEjecutados.push_back(p);
			
			cout << "El proceso " << p.id << " termino en el tiempo " << p.end << endl;
		}else{
			temp.push(p);
		}
	}
}

void simularMLFQ(vector<queue<Proceso>> &colas, vector<int> &quantums, int &time, vector<Proceso> &procesosEjecutados){
	int numColas = colas.size();
	bool todoVacio = true;
	
	while(todoVacio){
		for(int i = 0; i<numColas; ++i){
			if(!colas[i].empty()){
				todoVacio = false;
				
				if (i==numColas-1){
					// Si es la ultima cola, usa FCFS
					cout << "Simulando FCFS en la ultima cola"<<endl;
					simularFCFS(colas[i], time, procesosEjecutados);
				}else{
					cout << "Simulando cola "<<i+1<< " Round Robin con quantum "<<quantums[i]<<endl;
					queue<Proceso> temp;
					while(!colas[i].empty()){
						Proceso pro = colas[i].front();
						colas[i].pop();

						if(pro.start == -1){
							// Los procesos con inicio -1 no han sido atendidos
							pro.start = max(time, pro.at);
						}
						int timeSlice = min(quantums[i], pro.remaining);
						time += timeSlice;
						pro.remaining -= timeSlice;
						
						if(pro.remaining ==0){
							// Si el proceso termina, lo agregamos a los procesos ejecutados
							pro.end = time;
							pro.tat = pro.end-pro.at;
							pro.waiting = pro.tat-pro.bt;
							
							procesosEjecutados.push_back(pro);
							cout << "El proceso " << pro.id << " finalizo en tiempo " << pro.end << endl;
						}else{
							// Si no termina, pasa a la cola de menor prioridad
							if(i<numColas-1){
								colas[i+1].push(pro);
							}else{
								// Mantener en la misma cola si es la ultima
								temp.push(pro);
							}
						}
					}
					
					if(!temp.empty()){
						while(!temp.empty()){
							colas[i].push(temp.front());
							temp.pop();
						}
					}
				}
			}
		}
		if (todoVacio) break;
	}
}

vector<double> MLFQ(){
	int numColas;
	do{
		cout << "Ingrese la cantidad de colas a simular MLFQ ";
		cin >> numColas;
		if(numColas<2) cout << endl<<"El minimo de colas a ejecutar es 2!" <<endl<<endl;
	}while(numColas<2);
	
	vector<queue<Proceso>> colas(numColas);
	vector<Proceso> procesos = leerMLFQ("prueba.txt");
	sort(procesos.begin(), procesos.end(), compareProcesos);
	
	vector<int> quantums(numColas-1);
	
	for(Proceso pro:procesos){
		colas[0].push(pro);
	}
	
	for(int i = 0; i < numColas-1; i++){
		cout << "Ingresa el quantum para la cola " << i+1 << " ";
		cin >> quantums[i];
	}
	cout << "La cola " << numColas << " correra el algoritmo First come, first served" << endl <<endl;
	
	int time = 0;
	vector<Proceso> procesosEjecutados;
	
	simularMLFQ(colas, quantums, time, procesosEjecutados);
	
	vector<double> responses = calculate(procesosEjecutados);
	responses.push_back(procesosEjecutados.size());
	return responses;
}

vector<double> MLQ(){
	int numColas;
	do{
		cout << "Ingrese la cantidad de colas a simular MLQ ";
		cin >> numColas;
		if(numColas<2) cout << endl<<"El minimo de colas a ejecutar es 2!" <<endl<<endl;
	}while(numColas<2);
	
	vector<queue<Proceso>> colas(numColas);
	vector<Proceso> procesos = leerMLQ("prueba.txt");
	sort(procesos.begin(), procesos.end(), compareProcesos);
	
	
	for (Proceso pro: procesos){
		colas[pro.p-1].push(pro);
	}
	
	int schedulingTypes[numColas];
	string schedulingNames[2];
	int quantum[numColas];
	
	for (int i = 0; i < numColas; i++) {
        do{
        	cout << "Elige el scheduling para la cola " << i+1 << " (1. FCFS o 2. RR): ";
        	cin >> schedulingTypes[i];
        	if (schedulingTypes[i]!=1 && schedulingTypes[i]!=2) cout <<endl<< "Elige una opcion valida!"<<endl<< endl;
		}while(schedulingTypes[i]!=1 && schedulingTypes[i]!=2);
        if (schedulingTypes[i] == 2) {
            cout << "Ingresa el quantum para la cola " << i+1 << ": ";
            cin >> quantum[i];
        }
    }
    
	
	int time = 0;
	vector<Proceso> procesosEjecutados;
	for(int i = 0; i<numColas; i++){
		if(!colas[i].empty()){
			cout << "Corriendo cola " << i+1 << " con scheduling " << schedulingNames[schedulingTypes[i]-1] <<endl;
			if (schedulingTypes[i] == 1) {
                simularFCFS(colas[i], time, procesosEjecutados);
            } else if (schedulingTypes[i] == 2) {
                simularRoundRobin(colas[i], time, procesosEjecutados, quantum[i]);
            }
		}
	}
	
	vector<double> responses = calculate(procesosEjecutados);
	responses.push_back(procesosEjecutados.size());
	return responses;
}



int main(){
	int numColas, opcScheduling;
	
	do{
		cout << "Elije el scheduling a ejecutar (1. MLQ o 2. MLFQ)" <<endl;
		cin >> opcScheduling;
		if(opcScheduling!=1 && opcScheduling!=2) cout << endl<<"Elije una opcion valida!" <<endl<<endl;
	}while(opcScheduling!=1 && opcScheduling!=2);
	
	vector<double> rsp;
	
	if(opcScheduling==1){
		rsp = MLQ();	
	}else{
		rsp = MLFQ();	
	}
	
	
	double avgWaiting = rsp[0]/rsp[2];
	double avgTAT = rsp[1]/rsp[2];
	
	cout << "El tiempo de espera promedio fue de: " << avgWaiting <<endl;
	cout << "El TAT promedio fue de: " << avgTAT <<endl;
	
	return 0;
}
