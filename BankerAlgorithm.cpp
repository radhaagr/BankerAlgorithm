// ConsoleApplication19.cpp : Defines the entry point for the console application.
//


#include <sstream>
#include <string>
#include <fstream>
#include <vector>
#include <set>
#include <map>
#include <iostream>
#include <queue>

#define debug 0

using namespace std;
 struct compare  
 {  
   bool operator()(const int& l, const int& r)  
   {  
       return l > r;  
   }  
 };  


typedef map<int,vector<int> > processRData_t;
typedef vector<int> procListAtT_t;
typedef	vector<procListAtT_t> timeVec_t;
typedef map<int, vector<vector<int> > > QuerySet_t;
typedef vector<vector<int> > OneProcessQuerySet_t;

void readProcessResources(processRData_t& allocateR,  std::ifstream& infile, vector<int>& procid, int& noResources)
{
	string line;
	std::istringstream iss;
	debug && cout << "Allocate/Max resource for current processes : " << endl ;
	//vector<vector<int> > allocateR(noPros, vector<int>(noResources,0));
	{
		for(size_t i = 0; i < procid.size() ; i++)
		{
			iss.clear();
			std::getline(infile, line);
			iss.str(line);
			int curPid = procid[i];
			vector<int>& vecAllocR =	allocateR[curPid];
			debug && cout <<  "[" << curPid << "] : ";
			for(int j = 0; j < noResources ; j++)
			{
				int k = 0;
				iss >> k;
				vecAllocR.push_back(k);
				debug && cout << k << " ";
			}
			debug && cout << endl;
		}
	}
}

void getNeed(processRData_t& maxR, processRData_t& allocateR , vector<int>& procid, int& noResources,processRData_t& needR)
{
	debug && cout << "Need Matrix resource for current processes : " << endl ;
	for(size_t i = 0; i < procid.size() ; i++)
	{
		int curPid = procid[i];
		vector<int> vecNeedR; ;
		debug && cout << curPid << " : ";
		for(int j = 0; j < noResources ; j++)
		{	
			int needValue = maxR[curPid][j] - allocateR[curPid][j];
			vecNeedR.push_back(needValue);
			debug && cout << needValue << " ";
		}
		debug && cout << endl;
		needR.insert(make_pair( curPid, vecNeedR));
	}
}

template <typename T> 
std::vector<T>& operator+=(std::vector<T>& aVector,std::vector<T>& bVector)
{
    for(size_t i=0; i < aVector.size(); i++ )
	{
		aVector[i] =  aVector[i] + bVector[i];
	}
    return aVector;
}

template <typename T> 
std::vector<T>& operator-=(std::vector<T>& aVector, std::vector<T>& bVector)
{
    for(size_t i=0; i < aVector.size(); i++ )
	{
		aVector[i] =  aVector[i] - bVector[i];
	}
    return aVector;
}


int main(int argc, char* argv[])
{
	string  inFileName("");
	string  outFileName("");
	if (argc > 1) 
	{
	   inFileName.append(argv[1]);
	}
	else{
	   cout << "Input file is not passed as parameter" << endl;
	   return 0;
 	}
	if (argc > 2)
	{ 
	    outFileName.append(argv[2]);
	}else
	{		
		outFileName.append("output.txt");
	}
	std::ifstream infile(inFileName.c_str());
	if (! infile.good())
	{
		cout << "Problem opening input file" << endl;
		return 0;
	}
	std::ofstream outFile(outFileName.c_str());
	std::string line;
	std::getline(infile, line);
	std::istringstream iss(line);
	
	int noPros, noResources, noQueries;
	{
		iss >> noPros >> noResources >> noQueries;
	}
	iss.clear();
	vector<int> procPid;
	vector<int> resPid;
	processRData_t  allocateR; //(noPros, vector<int>(noResources,0));
	processRData_t  maxR; //(noPros, vector<int>(noResources,0));
	processRData_t  needR; //(noPros, vector<int>(noResources,0));
	priority_queue<int, std::vector<int>, compare > myQ;
	queue<int > waitQ;

	{
		std::getline(infile, line);
		iss.str(line);
		for(int i = 0; i < noPros ; i++)
		{
			int k = 0;
			iss >> k;
			procPid.push_back(k);
			//procPid.insert(k);
		}
	}

	int maxT = -1;
	//getting all the time values needed for cur work
	

		vector<int> time;
		{
			iss.clear();
			std::getline(infile, line);
			iss.str(line);
			for(int i = 0; i < noPros ; i++)
			{
				int k = 0;
				iss >> k;
				//procPid.push_back(k);
				time.push_back(k);
				if(maxT < k)
				{
					maxT = k;
				}
			}
		}
		timeVec_t timeVec(maxT+1);
		//create a vector of time vs processes arriving at that time
		for(size_t t=0; t< procPid.size(); t++)
		{
			int curTime = time[t];
			procListAtT_t& procsAtT = timeVec[curTime];
			//procsAtT.insert(procPid[t]);
			procsAtT.push_back(procPid[t]);
		}

	
	vector<int> availableR(noResources,0);
	{
		iss.clear();
		std::getline(infile, line);
		iss.str(line);
		for(int i = 0; i < noResources ; i++)
		{
			int k = 0;
			iss >> k;
			availableR[i] = k;
		}
	}
	vector<int> workR(noResources,0);
	workR = availableR;

	readProcessResources(maxR,  infile, procPid, noResources);
	readProcessResources(allocateR,  infile, procPid, noResources);
	getNeed( maxR, allocateR ,  procPid,  noResources, needR);

	//reading all the query set in a vector<vector<vector<int>>> queries;
	//  0 - 0 { {1,2 3} , 1 { 2,3,4}
	// 1- (1,1,1) , (1, 1,1) 
	//these queries will be processes sequentially
	QuerySet_t queryData;
	{
		for(int i = 0; i < noQueries ; i++)
		{
			iss.clear();
			std::getline(infile, line);
			iss.str(line);
			int procid = 0;
			iss >> procid;
			vector<int> curData(noResources,0);
			for(int i = 0; i < noResources  ; i++)
			{
				int curQueryRValue = 0;
				iss >> curQueryRValue;
				curData[i] = curQueryRValue;
			}
			QuerySet_t::iterator itr = queryData.find(procid);
			if(itr != queryData.end())
			{
				OneProcessQuerySet_t& curVec = itr->second ;
				curVec.push_back(curData);
			}
			else
			{
				OneProcessQuerySet_t newVec;
				newVec.push_back(curData);
				queryData.insert(make_pair(procid, newVec));
			}
		}
	}


	/*
		create a pq at t = 0
		and insert all the element of t = 0 in it
	*/
	int startT = 0;
	int processLeft = noPros;
	bool noSafe = false;
	while( queryData.size() > 0)
	{
		bool grant = false;
		//at t = 0 create a priority queue of all elements from the vector of PIDs at t =0
		//query the process 1 query
		//increment the time and insert the element in PQ of time t =1 
		//timeVec;
		if(startT < timeVec.size())
		{
			procListAtT_t& allProcatT = timeVec[startT];
			int curPid ;
			for(size_t i = 0; i < allProcatT.size() ; i++)
			{
				curPid =	allProcatT[i];
				myQ.push(curPid);
			}
		}
		//insert element from waitQ as well 
		while(waitQ.empty() == false)
		{
			int topPid = waitQ.front();
			myQ.push(topPid);
			waitQ.pop();
		}

		while( grant == false && (myQ.size() > 0) )
		{
			//get the topmost priority element 
			//process each element from priority queue unless you find one element
			//for which it can be granted add these element in to wait queue
			int curPid = myQ.top();
			//check the query set of this process
			OneProcessQuerySet_t& curProcQueries = queryData[curPid];
			vector<int> curResourceQuery;
			if(curProcQueries.size() > 0)
			{
				curResourceQuery = curProcQueries[0];
			}
			//if query is greater than need exit the system
			if(curResourceQuery > needR[curPid] )
			{
				cout << "Not safe" << endl;
				return 0;
			}
			//if current query of the process is less then available than grant
			if(curResourceQuery <= availableR )
			{
				outFile << "Grant" << endl;
				if( curResourceQuery == needR[curPid])
				{
					//return allocated resources to available resources
					//remove process from priorityQueue
					availableR += allocateR[curPid];
					myQ.pop();
					processLeft--;
				}
				else if(  curResourceQuery < needR[curPid])
				{
					//reduce the available resouces and need resources
					availableR -=   curResourceQuery;
					needR[curPid] -= curResourceQuery;
				}
				curProcQueries.erase(curProcQueries.begin()+0);
				if(curProcQueries.size() == 0)
				{
					//remove this from map
					queryData.erase(curPid);
				}
				grant = true;
				resPid.push_back(curPid);
			}
			else
			{
				outFile <<"Deny" << endl;
				myQ.pop();
				waitQ.push(curPid);
				grant = false;
			}
		}
		if(startT > maxT && (grant == false) &&  (processLeft == waitQ.size()))
		{
			outFile << "Not safe";
			noSafe = true;
			break;
		}
		startT++;
		//now check the query set of that process
	}

	if(noSafe == false)
	{
		for(size_t k = 0; k <resPid.size() ; k++)
		{
			if(k == resPid.size()-1)
			{
				outFile << resPid[k] ;
			}
			else
			{
				outFile << resPid[k] << " ";
			}
		}
	}
 	return 0;
}
