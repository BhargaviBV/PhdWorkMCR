#include <bits/stdc++.h> 
#include <mysql/mysql.h>
#include <iostream> 
#include <list> // for list operations
#include <string>
//#include <stdio.h>
//#include <stdlib.h>
#include <cstring>
//#include <typeinfo>
#include <sstream>
#include<bits/stdc++.h> 
#include <boost/algorithm/string.hpp> 
//#define ALLOW_UNALIGNED_READS 0
//only bfs
using namespace std; 

map<long,bool> satTable;

bool checkAttribute(string attribute, string constraint)
{

	const char* constr = constraint.c_str();
			string tmp1=attribute;				
			tmp1.erase(std::remove(tmp1.begin(),tmp1.end(),'\"'),tmp1.end());			
			attribute=tmp1;	
	const char* attr = attribute.c_str();

	if(strcmp(attr,constr) == 0)
		return true;

	else
		return false;
}

bool checkConstraint(int v,string constraints)
{
	ifstream myfile ("/home/bbv1814/Desktop/j2/graphs/egv1/egv1vertexinfofull");          //opening the file having vertices
	string line; 
 if (myfile.is_open())                     //if the file is open
  {
    while (! myfile.eof() )                 //while the end of file is NOT reached
    {
      getline (myfile,line);
	std::vector<std::string> ar;
                boost::split(ar, line, [](char c){return c == ' ';});
		if(ar.size()!=1)
		{ 
			int i = stoi(ar[0]);
			if(i==v)
			{	
				string j1=ar[1];
				string j2=ar[2];
				string tmp=j1+j2;
				if(checkAttribute(tmp,constraints)==true)return true;
				else return false;
			}
		}
    }
  }

}

void add_edge(vector<int> adj[], int src, int dest) 
{ 
	adj[src].push_back(dest); 
	adj[dest].push_back(src); 
} 
void add_edged(vector<int> adj[], int src, int dest) 
{ 
	adj[src].push_back(dest); 
	//adj[dest].push_back(src); 
} 


bool BFSonly(vector<int> adj[], string constraints, int src, int dest, int v, int pred[], int dist[]) 
{ 


	list<int> queue; 

	bool visited[v]; 

	
 

		
	for (int i = 0; i < v; i++) 
	{ 
		visited[i] = false; 
		dist[i] = INT_MAX; 
		pred[i] = -1; 
	} 

	visited[src] = true; 
	dist[src] = 0; 
	queue.push_back(src); 

	while (!queue.empty()) 
	{ 
		int u = queue.front(); 
		queue.pop_front(); 
		for (int i = 0; i < adj[u].size(); i++) 
		{ 
			if (visited[adj[u][i]] == false) 
			{ 
				
				{				
				visited[adj[u][i]] = true; 
				dist[adj[u][i]] = dist[u] + 1; 
				pred[adj[u][i]] = u;
				
			
					
					if(checkConstraint(adj[u][i], constraints) == true)
					{
							queue.push_back(adj[u][i]);
						//cout<<"In queue adjacent vertex :"<<adj[u][i]<<endl;
					//cout<<"true"<<endl;

					}
					if (adj[u][i] == dest) return true; 
				}

			} 
		} 
	} 

	return false; 
} 

void printShortestDistanceonly(vector<int> adj[], string constraints, int s, int dest, int v) 
{ 

	int pred[v], dist[v]; 

	//cout<<"in printshortestdistance() function"<<endl;
	if (BFSonly(adj, constraints, s, dest, v, pred, dist) == false) 
	{ 

		map<long,bool>::iterator itr;
		/*for (itr = satTable.begin(); itr != satTable.end(); ++itr) 
		{ 
        	cout << '\t' << itr->first<< '\t' << itr->second << '\n'; 
   		} */

		cout << "not connected"; 
		return; 
	} 

	vector<int> path; 
	int crawl = dest; 
	path.push_back(crawl); 

	/*for(int i=0;i<v;i++)
	{
		cout<<i<<" "<<pred[i]<<endl;	
	}*/


	while (pred[crawl] != -1) { 
		path.push_back(pred[crawl]); 
		crawl = pred[crawl]; 
	} 

	//cout << "Shortest path length is : "<< dist[dest]<<endl; 

	//cout << "Path is::"; 
	//for (int i = path.size() - 1; i >= 0; i--) 
		//cout << path[i] << " "; 


	/*map<long,bool>::iterator itr;
	for (itr = satTable.begin(); itr != satTable.end(); ++itr) { 
        cout << '\t' << itr->first << '\t' << itr->second << '\n'<<endl; 
    } */
} 



void Reachabilityonly( int n)
{

	int number_of_vertices = n; 
	vector<int> adj[number_of_vertices+1]; 
	//cout<<"inside reachabilityonly"<<endl;
  ifstream myfile ("/home/bbv1814/Desktop/j2/graphs/egv1/egV1kD2L8exp.edge");          //opening the file having vertices
	string line; 
 if (myfile.is_open())                     //if the file is open
  {
    while (! myfile.eof() )                 //while the end of file is NOT reached
    {
      getline (myfile,line);
	std::vector<std::string> ar;
                boost::split(ar, line, [](char c){return c == ' ';});
		if(ar.size()!=1)
		{ 
			int i = stoi(ar[0]);	
			int j = stoi(ar[1]);
			//cout<<"Edge added:"<<i<<"-"<<j<<endl;
			add_edged(adj, i, j);
		}
    }
  }
	
	//cout<<"after storing edges in array from table"<<endl;
  
	int source, destination; 
	//string country= job, religion;
  	string constraint="";

	

	ofstream resultsFile ("/home/bbv1814/Desktop/j2/graphs/egv1/BFSETime");
	double sum=0.0;
	double max=0;
	ifstream myfile2 ("/home/bbv1814/Desktop/j2/graphs/egv1/mcrqgen.true"); 
	if (myfile2.is_open())                     //if the file is open
  	{
    		while (! myfile2.eof() )                 //while the end of file is NOT reached
    		{
      			getline (myfile2,line);
			std::vector<std::string> ar;
                	boost::split(ar, line, [](char c){return c == ' ';});
			if(ar.size()!=1)
			{ 
				source=stoi(ar[0]);
				destination=stoi(ar[1]);
				constraint=ar[2];			
				clock_t start=clock();
				printShortestDistanceonly(adj, constraint, source, destination, number_of_vertices); 
				clock_t stop=clock();
				double etime=	(double)(stop-start)/CLOCKS_PER_SEC;
				if(resultsFile.is_open()){	
				resultsFile<<etime<<"\n";}
				sum+=etime;
				if(etime>max)max=etime;
				cout<<"execution time ="<<etime<<endl;
			}
		}	
	}
	
	double ave=sum/100;
	cout<<"Average execution time:"<<ave<<endl;
	cout<<"Maximum execution time:"<<max<<endl;
	resultsFile.close();
}


int createVTables(MYSQL *conn)
{

	MYSQL_RES *res;
   	MYSQL_ROW row;
	//set<string> var1;
		mysql_query(conn,"drop table Vertices2");
	if(mysql_query(conn,"create table Vertices2(vid varchar(20) not null unique, country varchar(10), region varchar(10))"))
 	{
		cout << mysql_error(conn) << endl;
  		exit(1);
	}
	string line;
	ifstream myfile2 ("/home/bbv1814/Desktop/j2/graphs/egv1/egv1vertexinfofull"); 
	if (myfile2.is_open())                     //if the file is open
  	{
    		while (! myfile2.eof() )                 //while the end of file is NOT reached
    		{
      			getline (myfile2,line);
			std::vector<std::string> ar;
                	boost::split(ar, line, [](char c){return c == ' ';});
			if(ar.size()!=1)
			{ 
			string tmp1=ar[0];				
			tmp1.erase(std::remove(tmp1.begin(),tmp1.end(),'\"'),tmp1.end());			
			ar[0]=tmp1;
			tmp1=ar[1];				
			tmp1.erase(std::remove(tmp1.begin(),tmp1.end(),'\"'),tmp1.end());			
			ar[1]=tmp1;
			tmp1=ar[2];				
			tmp1.erase(std::remove(tmp1.begin(),tmp1.end(),'\"'),tmp1.end());			
			ar[2]=tmp1;			
			//cout<<ar[0]<<" "<<ar[1]<<endl;
			string query1 = string("insert into Vertices2 (vid, country, region) values ('") + ar[0] + "', '" + ar[1]+ "', '" + ar[2] + "')";

			
			//cout<<query1<<endl;			
			const char * query3=query1.c_str();
			//cout<<query3<<endl;
			if(mysql_query(conn,query3))
			{
				//cout << mysql_error(conn2) << endl;
  				//exit(1);
			}  
			
		}
    }
  }

	cout<<"V table created\n";
	return 0;
}

int main()
{

	MYSQL *conn;
   	MYSQL_RES *res;
   	MYSQL_ROW row;

   	const char *server = "localhost";
   	const char *user = "root";
   	const char *password = "cisbv102"; // set me first 
   	const char *database = "Attributed_Graph";

   	conn = mysql_init(NULL);

   	// Connect to database
   	if (!mysql_real_connect(conn, server, user, password, database, 0, NULL, 0)) 
   	{
    	cout << mysql_error(conn) << endl;	
      	exit(1);
	}
	

	int vc=createVTables(conn);
	int n;
	
	mysql_query(conn,"SELECT MAX(vid) FROM Vertices2");
	 	res=mysql_store_result(conn);
		while(row=mysql_fetch_row(res))
        {
		n=atoi(row[0]);
	}
   	cout<<"Max of vertices:"<<n<<endl;
	
	Reachabilityonly(n);  

	return 0;
}
