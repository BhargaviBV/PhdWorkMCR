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
//HashBFS test::murmurhash set len=3 , tested on ER graph with 100  MCR queries without clustering
//test3::Murmurhash corrected by setting len=2; tested MCA code without clustering on own example. working well
//to do: have to add edge check constraint in future
//test4:include supergraph without constraints
using namespace std; 

map<long,bool> satTable;

unsigned int MurmurHash ( const void * key, int len, unsigned int seed )
{
	// 'm' and 'r' are mixing constants generated offline.

	const unsigned int m = 0xc6a4a793;
	const int r = 16;//seed=1;

	// Initialize the hash to a 'RANDOM' value

	unsigned int h = seed ^ (len * m);

	// Mix 4 bytes at a time into the hash

	const unsigned char * data = (const unsigned char *)key;

	while(len >= 4)
	{
			h += *(unsigned int *)data;
			h *= m;
			h ^= h >> r;

			data += 1;
			len -= 4;
	}

	// Handle the last few bytes of the input array

	switch(len)
	{
	case 3:
			h += data[2] << 16;
	case 2:
			h += data[1] << 8;
	case 1:
			h += data[0];
			h *= m;
			h ^= h >> r;
	};

	// Do a few final mixes of the hash to ensure the last few
	// bytes are well-incorporated.

	h *= m;
	h ^= h >> 10;
	h *= m;
	h ^= h >> 17;

	return h;
}


long getAttributeHash(int vertexId, long graph_hash[])
{
	//return graph_hash[vertexId-1];
	return graph_hash[vertexId];
}

bool checkAttribute(string attribute, string constraint)
{

	const char* constr = constraint.c_str();
	const char* attr = attribute.c_str();

	if(strcmp(attr,constr) == 0)
		return true;

	else
		return false;
}


bool checkConstraint(int vertexId, string constraint, long graph_hash[], MYSQL *conn)
{

	//MYSQL *conn;
   	MYSQL_RES *res;
   	MYSQL_ROW row;

   	long hash = getAttributeHash(vertexId, graph_hash);



   	//cout<<"hash:"<<vertexId<<":"<<hash<<endl;

   	string hashString;
	stringstream mystream;
	mystream << hash;
	hashString = mystream.str();


	const char* constraints = constraint.c_str();
	long hashConstraint = MurmurHash((const void *)(constraints),3,0);	

	//cout<<"hashConstraint of"<<constraints<<":"<<hashConstraint<<endl;

	if(hash != hashConstraint) return false;

	//map<long,bool> satTable;
	map<long,bool>::iterator itr;

	itr = satTable.find(hash);

	//cout<<hash<<endl;
	//cout<<itr->first<<" "<<itr->second<<endl;

	

	if(itr->second == 0 )
	{
		

		string query1 = string("SELECT attr, count FROM IFile where v_attr_hash = '") + hashString+"'";
		const char * query2 = query1.c_str();

		//cout<<"if"<<endl;

		

		//cout<<query1<<endl;

		if(mysql_query(conn,query2))
		{	
			//cout<<"success2"<<endl;
			cout << mysql_error(conn) << endl;	
      		exit(1);
		}

		//cout<<query1<<endl;
		


		res=mysql_store_result(conn);
		row=mysql_fetch_row(res);

		string attribute = row[0];

		//cout<<attribute<<" "<<constraint<<endl;
		int count = atoi(row[1]);



		if(count == 1 )
		{
			//cout<<"if-if"<<endl;

			if(checkAttribute(attribute, constraint) == true)
			{
				//cout<<"if-if-if"<<endl;
				satTable.insert({ hash, true });
				return true; 
			}

			else if(checkAttribute(attribute, constraint) == false)
			{
				//cout<<"if-if-else"<<endl;
				satTable.insert({ hash, false });
				return false; 
			}
		}

		else
		{
			
			//cout<<"if-else"<<endl;
			string vID;
	    	stringstream mystream;
	    	mystream << vertexId;
	    	vID = mystream.str();

			string query1 = string("SELECT * FROM Vertices2 where vID = ") + vID;//to be modified, to generalize and use vertices2 table
			const char * query2=query1.c_str();	

			mysql_query(conn,query2);
			res=mysql_store_result(conn);
			row=mysql_fetch_row(res);


			string attribute=string(row[1]) +','+ row[2] +','+row[3];

			if(checkAttribute(attribute, constraint) == true)
			{
				return true;
			}

			else
			{
				return false;
			}
		}
	}


	return itr->second;
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
int  createsupergraph(vector<int> sgadj[])
{
	set<int> v1;	
	//ifstream myfile ("/home/bbv1814/Desktop/j2/graphs/ex1super.edge");
	ifstream myfile ("/home/bbv1814/Desktop/j2/graphs/egv1/egv1supergboth.txt");
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
				int src=0,dst;
				src=stoi(ar[0]);
				dst=stoi(ar[1]);
				v1.insert(src);v1.insert(dst);
				add_edged(sgadj,src,dst);
			
			}
   		 }
 	 }
	cout<<"\nsupergraph created with k:"<<v1.size()+1<<endl;
	return v1.size()+1;
}
int findSuperVertex(int v)
{
	int sv=0;
	//cout<<"Inside findsupervertex():"<<endl;
	//ifstream myfile ("/home/bbv1814/Desktop/j2/graphs/ex1cluster");
	ifstream myfile ("/home/bbv1814/Desktop/j2/graphs/egv1/egv1clusterboth");
	string line;	
	if (myfile.is_open())                     //if the file is open
  	{
    		while (! myfile.eof() )                 //while the end of file is NOT reached
    		{
      			getline (myfile,line);
			std::vector<std::string> ar;
                	boost::split(ar, line, [](char c){return c == ' ';});
			//cout<<ar[0]<<"and"<<ar[1]<<endl;
			if(ar.size()!=1)
			{ 
				int src=0,dst;
				string tmp1=ar[0];				
				tmp1.erase(std::remove(tmp1.begin(),tmp1.end(),'\"'),tmp1.end());			
				ar[0]=tmp1;
				src=stoi(ar[0]);
				dst=stoi(ar[1]);
				if(src==v)
				return dst;
			
			}
   		 }
 	 }
	return sv;
}
bool BFSSuperg(vector<int> adj[],int src,int dest,int pred[],int k)
{
	
	bool visited[k];
	list<int> queue; 
	for (int i = 0; i < k; i++) 
	{ 
		visited[i] = false; 
		pred[i] = -1; 
	} 
 
	
	visited[src] = true; 
	queue.push_back(src); 

	while (!queue.empty()) 
	{ 
		int u = queue.front(); 
		queue.pop_front(); 
		for (int i = 0; i < adj[u].size(); i++) 
		{ 
			if (visited[adj[u][i]] == false) 
			{ 
				visited[adj[u][i]] = true; 
				pred[adj[u][i]] = u; 


				//cout<<"adjacent vertex to be pushed:"<<adj[u][i]<<endl;
				//if(checkConstraint(adj[u][i], constraints, graph_hash, conn) == true)
				{
					queue.push_back(adj[u][i]);
					//cout<<"In queue adjacent vertex :"<<adj[u][i]<<endl;
					//cout<<"true"<<endl;

				}
				if (adj[u][i] == dest) return true; 

			} 
		} 
	} 

	return false; 

}
bool BFSG(vector<int> adj[], long graph_hash[], string constraints, int src, int dest, int v, int pred[], int dist[], MYSQL *conn) 
{ 

	MYSQL_RES *res;
   	MYSQL_ROW row;

	list<int> queue; 

	bool visited[v]; 
	//cout<<"inside BFSG:"<<endl;
	//int supersrc,superdst,pred2[2000]={-1};
	//supersrc=findSuperVertex(src);
	//superdst=findSuperVertex(dest);
	/*cout<<"Super vertex of "<<src<<":"<<supersrc<<endl;
	cout<<"Super vertex of "<<dest<<":"<<superdst<<endl;*/
	//bool b1=BFSSuperg(sgadj,supersrc,superdst,pred2,k);
	//cout<<"BFS through supergraph returns:"<<b1<<endl;
	//if(!b1) return false;
	vector<int> path; 
	/*int crawl = superdst; 
	path.push_back(crawl); 

	while (pred2[crawl] != -1) { 
		path.push_back(pred2[crawl]); 
		crawl = pred2[crawl]; 
	} */

	/*cout << "Super Path is::"; 
	for (int i = path.size() - 1; i >= 0; i--) 
		cout << path[i] << " "; 
	cout<<endl;*/
		
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
				//int superiv=findSuperVertex(adj[u][i]); 
				//cout<<"Supervertex of "<<adj[u][i]<<" is"<<superiv<<endl;
	//if(find(path.begin(), path.end(),superiv)!=path.end())//if super vertex of adjacent vertex is in superpath then only,it is included
				{				
				visited[adj[u][i]] = true; 
				dist[adj[u][i]] = dist[u] + 1; 
				pred[adj[u][i]] = u;
				
			
					//cout<<"adjacent vertex to be pushed is in superpath"<<endl;
					//cout<<"adjacent vertex:"<<adj[u][i]<<endl;
					if(checkConstraint(adj[u][i], constraints, graph_hash, conn) == true)
					{
							queue.push_back(adj[u][i]);
						//cout<<"In queue adjacent vertex :"<<adj[u][i]<<endl;
					//cout<<"true"<<endl;
					if (adj[u][i] == dest) return true; 

					}
					
				}

			} 
		} 
	} 

	return false; 
} 
bool BFS(vector<int> adj[], long graph_hash[], string constraints, int src, int dest, int v, int pred[], int dist[], MYSQL *conn) 
{ 

	MYSQL_RES *res;
   	MYSQL_ROW row;

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
				visited[adj[u][i]] = true; 
				dist[adj[u][i]] = dist[u] + 1; 
				pred[adj[u][i]] = u; 


				cout<<"adjacent vertex to be pushed:"<<adj[u][i]<<endl;
				if(checkConstraint(adj[u][i], constraints, graph_hash, conn) == true)
				{
					queue.push_back(adj[u][i]);
					cout<<"In queue adjacent vertex :"<<adj[u][i]<<endl;
					//cout<<"true"<<endl;

				  if (adj[u][i] == dest) return true; 
				}

			} 
		} 
	} 

	return false; 
} 
int printShortestDistanceg(vector<int> adj[], long graph_hash[], string constraints, int s, int dest, int v, MYSQL *conn) 
{ 

	int pred[v], dist[v]; 

	//cout<<"in printshortestdistance() function"<<endl;
	if (BFSG(adj, graph_hash, constraints, s, dest, v, pred, dist, conn) == false) 
	{ 

		map<long,bool>::iterator itr;
		/*for (itr = satTable.begin(); itr != satTable.end(); ++itr) 
		{ 
        	cout << '\t' << itr->first<< '\t' << itr->second << '\n'; 
   		} */

		//cout << "not connected"; 
		return 0; 
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
	return(dist[dest]);
	//cout << "Path is::"; 
	//for (int i = path.size() - 1; i >= 0; i--) 
		//cout << path[i] << " "; 


	/*map<long,bool>::iterator itr;
	for (itr = satTable.begin(); itr != satTable.end(); ++itr) { 
        cout << '\t' << itr->first << '\t' << itr->second << '\n'<<endl; 
    } */
} 




void printShortestDistance(vector<int> adj[], long graph_hash[], string constraints, int s, int dest, int v, MYSQL *conn) 
{ 

	int pred[v], dist[v]; 

	//cout<<"success"<<endl;
	//if (BFSG(adj, graph_hash, constraints, s, dest, v, pred, dist, conn) == false) 
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




int conHashIndex(long graph_hash[], MYSQL *conn)
{
	//cout<<"success"<<endl;

	MYSQL_RES *res;
   	MYSQL_ROW row;

   	long hash_addr[18]={0};

   	mysql_query(conn,"drop table IFile");

   	if(mysql_query(conn,"create table IFile(v_attr_hash varchar(12) not null unique, attr varchar(100),count varchar(12))"))
 	{
		cout << mysql_error(conn) << endl;
  		exit(1);
	}

   	mysql_query(conn,"SELECT * FROM Vertices");
    res=mysql_store_result(conn);

    while(row=mysql_fetch_row(res))
    {
        int k=atoi(row[0]);
        long hash = graph_hash[k-1];

        string hashString;
    	stringstream mystream;
    	mystream << hash;
    	hashString = mystream.str();

    	string attribute=string(row[1]) + row[2] +row[3];
		if(hash_addr[k-1]==0)
		{

			ostringstream temp;  //temp as in temporary
			temp<<1;

			string query1 = string("insert into IFile (v_attr_hash, attr, count) values ('") + hashString + "', '" + attribute+ "', '" + temp.str() + "')";
			
			const char * query2=query1.c_str();
			
			if(mysql_query(conn,query2))
			{
				//cout << mysql_error(conn2) << endl;
  				//exit(1);
			}   
		}

		else
		{
			string query1 = string("SELECT attr, count FROM IFile where v_attr_hash = '")+ hashString+"'";
			const char* query2 = query1.c_str();

			mysql_query(conn,query2);
    		res=mysql_store_result(conn);
    		row=mysql_fetch_row(res);
    		string temp_attr = string(row[0]);
    		int temp_count = atoi(row[1]) + 1;

    		string new_count;
	    	stringstream mystream;
	    	mystream << temp_count;
	    	new_count = mystream.str();


    		



			query1=string("update IFile set attr ='") + temp_attr + "', count = '"+ new_count + "' where v_attr_hash = '" +hashString+"'";
			const char * query3=query1.c_str();
			if(mysql_query(conn,query3))	
			{
				cout << mysql_error(conn) << endl;
  				exit(1);
			}	
		}
	}
}
int conHashIndexg(long graph_hash[], MYSQL *conn,int n)
{
	//cout<<"success"<<endl;

	MYSQL_RES *res;
   	MYSQL_ROW row;

   	long hash_addr[n]={0};

   	mysql_query(conn,"drop table IFile");

   	if(mysql_query(conn,"create table IFile(v_attr_hash varchar(12) not null unique, attr varchar(100),count varchar(12))"))
 	{
		cout << mysql_error(conn) << endl;
  		exit(1);
	}

   	mysql_query(conn,"SELECT * FROM Vertices2");
    res=mysql_store_result(conn);

    while(row=mysql_fetch_row(res))
    {
        int k=atoi(row[0]);
        //long hash = graph_hash[k-1];
	long hash=graph_hash[k];
        string hashString;
    	stringstream mystream;
    	mystream << hash;
    	hashString = mystream.str();

    	string attribute=string(row[1]) + row[2];// +row[3];
		if(hash_addr[k]==0)
		{

			ostringstream temp;  //temp as in temporary
			temp<<1;

			string query1 = string("insert into IFile (v_attr_hash, attr, count) values ('") + hashString + "', '" + attribute+ "', '" + temp.str() + "')";
			
			const char * query2=query1.c_str();
			
			if(mysql_query(conn,query2))
			{
				//cout << mysql_error(conn2) << endl;
  				//exit(1);
			}   
		}

		else
		{
			string query1 = string("SELECT attr, count FROM IFile where v_attr_hash = '")+ hashString+"'";
			const char* query2 = query1.c_str();

			mysql_query(conn,query2);
    			res=mysql_store_result(conn);
    			row=mysql_fetch_row(res);
    			string temp_attr = string(row[0]);
    			int temp_count = atoi(row[1]) + 1;

    			string new_count;
	    		stringstream mystream;
	    		mystream << temp_count;
	    		new_count = mystream.str();


    		



			query1=string("update IFile set attr ='") + temp_attr + "', count = '"+ new_count + "' where v_attr_hash = '" +hashString+"'";
			const char * query3=query1.c_str();
			if(mysql_query(conn,query3))	
			{
				cout << mysql_error(conn) << endl;
  				exit(1);
			}	
		}
	}
}
void Reachabilityg(long graph_hash[], MYSQL *conn, int n)
{

	MYSQL_RES *res;
   	MYSQL_ROW row;

	int number_of_vertices = n; 

	vector<int> adj[number_of_vertices+1]; 

	


	mysql_query(conn,"SELECT Src_vertex, Dest_vertex FROM Edges2");
	res=mysql_store_result(conn);



    while(row=mysql_fetch_row(res))
    {

    	int i = atoi(row[0]);
    	int j = atoi(row[1]);

    	add_edged(adj, i, j);
    }
	
   	//vector<int> sgadj[1000];//need to generalize
	//int k=createsupergraph(sgadj);
  	string line;
	int source, destination; 
	//string country= job, religion;
  	string constraint="";
	int count=0;
	ofstream resultsFile ("/home/bbv1814/Desktop/j2/graphs/egv1/HGBFSETime");
	double sum=0.0,sum2=0;
	double max=0;
	double fnr;
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
				int d=printShortestDistanceg(adj, graph_hash, constraint, source, destination, number_of_vertices, conn);  
				clock_t stop=clock();
				double etime=	(double)(stop-start)/CLOCKS_PER_SEC;
				if(d>0){count++;				
				if(resultsFile.is_open()){	
				resultsFile<<etime<<"\n";}
				sum+=etime;
				if(etime>max)max=etime;
				cout<<"execution time ="<<etime<<endl;}
			}
		}	
	}
	
	cout<<"False negative ratio:"<<(double)(100-count)/100<<endl;	
	double ave=sum/count;
	cout<<"Average execution time:"<<ave<<endl;
	cout<<"Maximum execution time:"<<max<<endl;
	resultsFile.close();	
}

void Reachability(long graph_hash[18], MYSQL *conn)
{

	MYSQL_RES *res;
   	MYSQL_ROW row;

	int number_of_vertices = 24; 

	vector<int> adj[number_of_vertices]; 

	


	mysql_query(conn,"SELECT Src_vertex, Dest_vertex FROM Edges");
	res=mysql_store_result(conn);



    while(row=mysql_fetch_row(res))
    {

    	int i = atoi(row[0]);
    	int j = atoi(row[1]);

    	add_edge(adj, i, j);
    }
	
   	
  
	int source, destination; 
	//string country= job, religion;
  	string constraint="";


	cout << "enter the source vertex: ";
  	cin >> source;

  	cout << "enter the destination vertex: ";
  	cin >> destination;

  	cout<< "enter the constraints:"<<endl;
  	cout<< "country: (1)Ca \t (2)Cb \t (3)Cc \t (4)Cd"<<endl;
  //	cin>>country;
  	cout<< "job: (1)Eng \t (2)IT \t (3)Fin"<<endl;
  //	cin>>job;
  	cout<< "religion: (1)R1 \t (2)R2"<<endl;
 // 	cin>>religion;


  	cin>>constraint;

	//long hash = MurmurHash((const void *)(constraints),16,0);
	int item = 0;
	//cout<<constraint<<endl;
	

	/*while(item != 999)
	{
		cout<<"size:"<<adj[item].size()<<endl;
		cout<<"item:"<<endl;
		cin>>item;
		
	}
	*/

	//cout<<"success"<<endl;
	//cout<<"success"<<endl;
	printShortestDistance(adj, graph_hash, constraint, source, destination, number_of_vertices, conn); 
}

int createVETables(MYSQL *conn)
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
	mysql_query(conn,"drop table Edges2");
	if(mysql_query(conn,"create table Edges2(Src_vertex varchar(20), Dest_vertex varchar(20), Label varchar(100))"))
 	{
		cout << mysql_error(conn) << endl;
  		exit(1);
	}


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
			
			//string query1 = string("insert into Vertices2 (vid) values ('") + ar[0] + "')";
			
			string query2 = string("insert into Edges2 (Src_vertex, Dest_vertex, Label) values ('") + ar[0] + "', '" + ar[1]+ "', '" + ar[2] + "')";
			//cout<<query2<<endl;			
			const char * query3=query2.c_str();
			//var1.insert(ar[0]);var1.insert(ar[1]);
			if(mysql_query(conn,query3))
			{
				//cout << mysql_error(conn2) << endl;
  				//exit(1);
			}  
			
		}
    }
  }
	/*set<string> ::iterator it;	
	for(it=var1.begin();it!=var1.end();it++)
	{
		string tmp1=*it;		
		string query1 = string("insert into Vertices2 (vid) values ('") + tmp1 + "')";
		const char * query3=query1.c_str();
			
			if(mysql_query(conn,query3))
			{
				//cout << mysql_error(conn2) << endl;
  				//exit(1);
			} 

	}*/
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

	cout<<"VE tables created\n";
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
	/*else
	{
		long graph_hash[18]={0};
		mysql_query(conn,"SELECT * FROM Vertices");
        res=mysql_store_result(conn);
        while(row=mysql_fetch_row(res))
        {	
        	int k = atoi(row[0]);
        	string attribute = string(row[1]) + row[2] +row[3];//3 attributes
        	const char* constraints = attribute.c_str();
        	graph_hash[k-1] = MurmurHash((const void *)(constraints),4,0);	
        }

        for(int i =0; i<18; i++)
        {
        	cout<<i+1<<":"<<graph_hash[i]<<endl;
        }        

		conHashIndex(graph_hash,conn);

		Reachability(graph_hash,conn);

	}*/

	int vc=createVETables(conn);
	int n;
	/*mysql_query(conn,"SELECT COUNT(*) FROM Vertices2");
	 	res=mysql_store_result(conn);
		while(row=mysql_fetch_row(res))
        {
		n=atoi(row[0]);
	}
   	cout<<"Number of vertices:"<<n<<endl;*/
	mysql_query(conn,"SELECT MAX(vid) FROM Vertices2");
	 	res=mysql_store_result(conn);
		while(row=mysql_fetch_row(res))
        {
		n=atoi(row[0]);
	}
   	cout<<"Max of vertices:"<<n<<endl;
	long graph_hash2[n]={0};
	mysql_query(conn,"SELECT * FROM Vertices2");
        res=mysql_store_result(conn);
        while(row=mysql_fetch_row(res))
        {	
        	int k = atoi(row[0]);
        	string attribute = string(row[1]) + row[2];//2 attributes
        	const char* constraints = attribute.c_str();
        	//graph_hash2[k-1] = MurmurHash((const void *)(constraints),3,0);	
		graph_hash2[k] = MurmurHash((const void *)(constraints),3,0);	
        }

        for(int i =0; i<n; i++)
        {
        	cout<<i<<":"<<graph_hash2[i]<<endl;
        }  
	conHashIndexg(graph_hash2,conn,n);  
	Reachabilityg(graph_hash2,conn,n);  

	return 0;
}
