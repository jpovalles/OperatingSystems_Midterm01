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
            resultado.push_back(temp);
        }
        archivo.close();
    } else {
        cout << "No se pudo abrir el archivo." << std::endl;
    }
    return resultado;
}

vector<int> calculate(vector<Proceso> procesosEjecutados){
	vector<int> waitTAT = {0, 0};
	for(Proceso pro:procesosEjecutados){
		cout << pro.id << "\t" 
             << pro.at << "\t" 
             << pro.bt << "\t"
             << pro.start << "\t"
             << pro.end << "\t"
             << pro.waiting << "\t"
             << pro.tat << "\n";
        
        waitTAT[1] += pro.waiting;
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



vector<int> MLQ(int numColas){
	vector<queue<Proceso>> colas(numColas);
	vector<Proceso> procesos = leerMLQ("prueba.txt");
	sort(procesos.begin(), procesos.end(), compareProcesos);
	
	for (Proceso pro: procesos){
		colas[pro.p].push(pro);
	}
	
	
	string schedulingTypes[numColas];
	int quantum[numColas];
	
	for (int i = 1; i <= numColas; i++) {
        cout << "Elige el scheduling para la cola " << i << " (FCFS or RR): ";
        // Entradas esperados: FCFS / RR
        cin >> schedulingTypes[i];
        if (schedulingTypes[i] == "RR") {
            cout << "Ingresa el quantum para la cola " << i << ": ";
            cin >> quantum[i];
        }
    }
	
	int time = 0;
	vector<Proceso> procesosEjecutados;
	for(int i = 1; i<=numColas; i++){
		if(!colas[i].empty()){
			cout << "Corriendo cola " << i << " con scheduling " << schedulingTypes[i] <<endl;
			if (schedulingTypes[i] == "FCFS") {
				cout <<"pass";
                simularFCFS(colas[i], time, procesosEjecutados);
            } else if (schedulingTypes[i] == "RR") {
            	cout <<"pass";
                //simularRoundRobin(colas[i], time, procesosEjecutados, quantum[i]);
            }
		}
	}
	
	vector<int> responses = calculate(procesosEjecutados);
	responses.push_back(procesosEjecutados.size());
	return responses;
}



int main(){
	int numColas;
	do{
		cout << "Ingrese la cantidad de colas a simular MLQ ";
		cin >> numColas;
	}while(numColas<1);
	vector<int> rsp = MLQ(numColas);
	
	double avgWaiting = rsp[0]/rsp[2];
	double avgTAT = rsp[1]/rsp[2];
	
	cout << "El tiempo de espera promedio fue de: " << avgWaiting <<endl;
	cout << "El TAT promedio fue de: " << avgTAT <<endl;
	
	return 0;
}