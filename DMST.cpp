#include <iostream>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <vector>
#include <map>
#include <unordered_map>
#include <queue>
#include <ctime>
#include <string>
using namespace std;
// #define TEST
const int INTMAX = 99999999;
//test
int VNUM, MNUM;
//usairport
// const int VNUM = 1574+10, MNUM = 28236+10;
//lexical network EAT
// const int VNUM = 23132+10, MNUM = 312342+10;
//web-berkstan
// const int VNUM = 334856+10, MNUM = 4523219+10;
//pokec
/* const int VNUM = 1304536+10, MNUM = 29183654+10; */
//wikicat
//  const int VNUM = 1791489+10, MNUM = 28508141+10;
//livejournal 4000k
// const int VNUM = 3828682 + 10, MNUM = 65825429 + 10;
//wikitalk 111k
// const int VNUM = 111881 + 10, MNUM = 1477893 + 10;
//epinion 32k
// const int VNUM = 32223 + 10, MNUM = 443506 + 10;
//wikiVote 1.3k
// const int VNUM = 1300  + 10, MNUM = 39456 + 10;

int n, m, edge_num;
int* V;
int* E_target;
int* E_cost;
int* list;
int* O_E;
int _n, _m, _edge_num;
int* _V;
int* _E_target;
int* _E_cost;
int* _list;
int* _O_E;
int* E_replace;
int* _E_replace;
bool* E_delete;
int report_replace;
int report_leaf_replace;
int report_cirlce;
int report_NumEdge;
//辅助空间
//1号位记录起始cycle编号 2号位记录当前的cycle编号
vector<vector<int> > nodeInCycleFirLas;//O(n)
//记录cycle在直接父亲节点的inEdge
vector<int> inEdge_of_cycle;//O(n)
//记录cycle在直接父亲节点的inEdge对应在cycle的删除掉的边
vector<int> corDelEdge_inEdge_of_cycle;
//记录直接父亲
vector<int> parent_of_cycle;//O(n)
//// 记录cycle内的边<head, <tail, O_E[]>>
//记录cycle内的<head, <O_E[], O_cycle>>
vector<unordered_map<int, pair<int, int>>> cycle;//O(n)
//记录cycle中的hook点对应的原始环，用来找树儿子节点中和父亲入边中对应的点
//<cycleID, hoodVertexId>
vector<unordered_map<int, int>> cycleRvs;
//记录每一轮新vertexid对应的cycleid
vector<int> vtocyle;
FILE* fin_G;
FILE* fout_G;

struct Graph{
	int n, m;
	int* E_target;
	int* E_cost;
	int* E_source;
	int* E_number;
	vector<int>* E_replace;
}G;

int* chosen;
int* mark;
bool* is_in_circle;
int* new_number;
vector<vector<int > > circle;
//vector< vector< pair <int, int> > > save;
vector< vector< int > > save;

void readGraph();
void markRound(int& round);
void buildIndex(int& round);
void indexOnEdge();
void queryByIndex(int& round);
void queryByIndex_Oh(int qNode);

void readGraph(){
	fscanf(fin_G, "%d %d", &n, &m);
	if (n == 0 && m == 0)
		return;
	G.n = n;
	G.m = m;
	edge_num = 1;
	for (int i = 0; i <= n; i++){
		V[i] = 0;
		chosen[i] = 0;
		mark[i] = 0;
	}
	for (int i = 0; i < m; i++)
	{
		int x, y, z;
		fscanf(fin_G, "%d %d %d", &y, &x, &z);

		G.E_source[i] = y;
		G.E_target[i] = x;
		G.E_cost[i] = z;

		edge_num++;
		E_target[edge_num] = y;
		E_replace[edge_num] = i; // initialize E_replace
		E_cost[edge_num] = z;
		O_E[edge_num] = i;
		list[edge_num] = V[x];
		V[x] = edge_num;
	}
	for (int i = 0; i <= m; i++)
	{
		G.E_number[i] = INTMAX;
		G.E_replace[i].clear(); // initialize G.E_replace
	}
}
void markRound(int& round){
	round++;
	for (int i = 1; i <= n; i++)
	{
		chosen[i] = V[i];
		for (int j = V[i]; j != 0; j = list[j])
		{
			if (E_cost[j] < E_cost[chosen[i]])
				chosen[i] = j;
		}

		if (chosen[i] != 0)
		{
			G.E_number[O_E[chosen[i]]] = round;
			// int curTrueNodeID = G.E_target[O_E[chosen[i]]];
			// if(-1 != nodeInCycleFirLas[curTrueNodeID][1]){

			// }
			// cout << "chosen e# " << O_E[chosen[i]] + 1 << " round " << round << endl;
		}
	}

	// cout << "#####" << endl;
	for (int i = 0; i <= n; i++)
		mark[i] = 0;
	circle.clear();
	for (int i = 1; i <= n; i++)
	{
		for (int v = i;; v = E_target[chosen[v]])
		{
			if (chosen[v] == 0)
				break;
			if (mark[v] > 0)
			{
				if (mark[v] == i)
				{
					vector<int> c;
					unordered_map<int, pair<int,int>> um_OE_inEdge;
					unordered_map<int, int> um_cycle_inVertex;
					c.clear();
					c.push_back(v);
					// um_OE_inEdge.emplace(make_pair(O_E[chosen[v]], make_pair(G.E_target[O_E[chosen[v]]], G.E_source[O_E[chosen[v]]])));
					// cout << "in cir " << E_target[chosen[v]] << " ";
					for (int s = E_target[chosen[v]]; s != v; s = E_target[chosen[s]])
					{
						// cout << E_target[chosen[s]] << " ";
						c.push_back(s);
						// report_NumEdge++;
					}
					// cout << endl;
					circle.push_back(c);
					report_cirlce++;
					for(auto s:c){
						int curTrueHeadID = G.E_target[O_E[chosen[s]]];
						int curTrueTailID = G.E_source[O_E[chosen[s]]];
						//建立父亲环指向儿子环<头, <原边号,对应环号>>
						um_OE_inEdge.emplace(make_pair(curTrueHeadID, make_pair(O_E[chosen[s]], vtocyle[s])));
#ifdef TEST
						cout<<"cycle "<<report_cirlce<<" head "<<curTrueHeadID<<" tail "<<curTrueTailID<<" true edge "<<O_E[chosen[s]]+1<<" child cycle "<<vtocyle[s]<<endl;
#endif
						// if (-1 != nodeInCycleFirLas[curTrueHeadID][1])
						// {
						// 	int initCycle = nodeInCycleFirLas[curTrueHeadID][0];
						// 	while(-1!=parent_of_cycle[initCycle]){
						// 		initCycle = parent_of_cycle[initCycle];
						// 	}
						// 	parent_of_cycle[initCycle] = report_cirlce;
						// 	cout<<"node "<<curTrueHeadID<<" node last in cycle "<<nodeInCycleFirLas[curTrueHeadID][1]<<" "<<report_cirlce<<endl;
						// 	inEdge_of_cycle[initCycle] = O_E[chosen[s]]; 
						// }
						if(-1 != vtocyle[s]){
                            //儿子环对应的节点在本轮环中对应入边的原边号
							inEdge_of_cycle[vtocyle[s]] = O_E[chosen[s]];
                            //s之前所在的环号是本轮的环号的儿子
							parent_of_cycle[vtocyle[s]] = report_cirlce; 
							//这个点不是儿子中的点，从孙子节点递归上来
							if(cycle[vtocyle[s]].find(curTrueHeadID) == cycle[vtocyle[s]].end()){
								int curCycle = nodeInCycleFirLas[curTrueHeadID][0];
								//游标的父亲未指向当前环儿子
								while(parent_of_cycle[curCycle]!=vtocyle[s]){
									curCycle = parent_of_cycle[curCycle];
								}
								//儿子环在当前环对应的收缩点v'所对应入边，该入边导致儿子环中一条边的删除，我们记录被删的边
								corDelEdge_inEdge_of_cycle[vtocyle[s]] = inEdge_of_cycle[curCycle];
							}else{
								corDelEdge_inEdge_of_cycle[vtocyle[s]] = cycle[vtocyle[s]][curTrueHeadID].first;
							}
						}
						//不是树叶，且有儿子
						if(round>1 && -1!=vtocyle[s]){
                            //点所在的<叶子环，点>
							um_cycle_inVertex.emplace(make_pair(nodeInCycleFirLas[curTrueHeadID][0],curTrueHeadID));
						}
						// cout<<"in edge of cycle "<<nodeInCycleFirLas[curTrueHeadID][1]<<" is "<<inEdge_of_cycle[nodeInCycleFirLas[curTrueHeadID][1]]<<endl;

						if (-1 == nodeInCycleFirLas[curTrueHeadID][0])
						{
							nodeInCycleFirLas[curTrueHeadID][0] = report_cirlce;
							nodeInCycleFirLas[curTrueHeadID][1] = report_cirlce;
						}
						else
						{
							nodeInCycleFirLas[curTrueHeadID][1] = report_cirlce;
						}
#ifdef TEST
						cout<<"node "<<curTrueHeadID<<" fir "<<nodeInCycleFirLas[curTrueHeadID][0]<<" las "<<nodeInCycleFirLas[curTrueHeadID][1]<<endl;
#endif
					}
					cycle.push_back(um_OE_inEdge);
						report_NumEdge+=um_OE_inEdge.size();
					cycleRvs.push_back(um_cycle_inVertex);
				}
				break;
			}
			mark[v] = i;
		}
	}
	if (circle.size() == 0){
		return;
	}
	for (int i = 0; i <= n; i++)
		is_in_circle[i] = false;
	for (int i = 0; i < circle.size(); i++)
	{
		for (int j = 0; j < circle[i].size(); j++)
			is_in_circle[circle[i][j]] = true;
	}

	_n = n;
	_m = m;
	_edge_num = edge_num;
	for (int i = 0; i <= n; i++)
		_V[i] = V[i];
	for (int i = 0; i <= edge_num; i++)
	{
		_E_target[i] = E_target[i];
		_E_cost[i] = E_cost[i];
		_E_replace[i] = E_replace[i]; // initialize _E_replace
		_list[i] = list[i];
		_O_E[i] = O_E[i];
	}

	n = 0;
	for (int i = 1; i <= _n; i++){
		if (is_in_circle[i] == false)
		{
			new_number[i] = ++n;
			if(-1 != vtocyle[i]){
				vtocyle[new_number[i]] = vtocyle[i];
			}
		}
	}
	for (int i = 0; i < circle.size(); i++)
	{
		++n;
		vtocyle[n] = report_cirlce-circle.size()+i+1;
#ifdef TEST
		cout<<"new vertex id "<<n<<" to cycle "<<vtocyle[n]<<endl;
#endif
		for (int j = 0; j < circle[i].size(); j++)
		{
			new_number[circle[i][j]] = n;
		}
	}
	// cout<<"__________vtocycle"<<endl;
	// for(auto ele:vtocyle){
	// 	cout<<ele<<" ";
	// }
	// cout<<endl;
	// cout<<"vtocycle________________"<<endl;
	edge_num = 1;
	for (int i = 0; i <= _n; i++){
		V[i] = 0;
	}
	for (int i = 1; i <= _n; i++){
		for (int j = _V[i]; j != 0; j = _list[j])
		{
			if (new_number[i] == new_number[_E_target[j]])
				continue;
			edge_num++;
			E_target[edge_num] = new_number[_E_target[j]];
			E_cost[edge_num] = _E_cost[j] - _E_cost[chosen[i]];
			O_E[edge_num] = _O_E[j];
			list[edge_num] = V[new_number[i]];
			// if (is_in_circle[i]&&G.E_number[_O_E[j]] != INTMAX)
			// if (is_in_circle[i])
			// {
			// 	// cout<<"真边"<<G.E_number[_O_E[j]]<<endl;
			// 	E_replace[edge_num] = _O_E[chosen[i]]; 
			// 	G.E_replace[_O_E[j]].push_back(_O_E[chosen[i]]);
			// 	report_replace++;
			// 	// cout <<"round "<<round<< " i " << i << " replace " << _O_E[j] + 1 << " with " << _O_E[chosen[i]] + 1 << endl;
			// }
			// else{
			// 	E_replace[edge_num] = _E_replace[j];
			// }
			V[new_number[i]] = edge_num;
		}
	}
}
void buildIndex(int& round){
		readGraph();	
		cout<<"read Graph Fin!"<<endl;
		report_cirlce = 0;
		report_NumEdge = 0;
		report_replace = 0;
		report_leaf_replace = 0;
		for(int i=0; i<=n+1;i++){
			nodeInCycleFirLas.push_back(vector<int>(2, -1));
			inEdge_of_cycle.push_back(-1);
			corDelEdge_inEdge_of_cycle.push_back(-1);
			parent_of_cycle.push_back(-1);
			vtocyle.push_back(-1);
		}
		cycle.push_back(unordered_map<int, pair<int,int>>());
		cycleRvs.push_back(unordered_map<int, int>());
		while (true){
			markRound(round);
			if(circle.size() == 0){
#ifdef TEST
				cout<<"mark Round Fin!"<<endl;
				cout<<"__________--------------__________"<<endl;
				for(int i=0;i<parent_of_cycle.size(); i++){
					cout<<"c"<<i<<" parent is "<<parent_of_cycle[i]<<endl;
				}	
				for(int i=0;i<nodeInCycleFirLas.size(); i++){
					cout<<"node "<<i<<" in cycle first "<<nodeInCycleFirLas[i][0]<<" in last "<<nodeInCycleFirLas[i][1]<<endl;
				}
				for(int i=0;i<inEdge_of_cycle.size(); i++){
					cout<<"cycle "<<i<<" in edge is "<<inEdge_of_cycle[i]+1<<endl;
				}
				for(int i=0; i<cycle.size(); i++){
					cout<<"cycle "<<i<<" "<<endl;
					for (auto ele : cycle[i])
					{
						cout << "head " << ele.first << " inedge# " <<ele.second.first+1<<" child cycle "<<ele.second.second<<endl;
					}
				}
				cout<<"__________--------------__________"<<endl;
#endif
				indexOnEdge();
#ifdef TEST
				cout<<"index on edge Finish!"<<endl;
				for(int i=0; i<G.m; i++){
					cout<<"arc# "<<i+1<<" replace ";
					for(auto ele:G.E_replace[i]){
						cout<<ele+1<<" ";
					}
					cout<<endl;
				}
#endif
				return;
			}
		}
}
void indexOnEdge(){
	for(int i=0; i<cycle.size(); i++){
		// cout<<"iiiii "<<i<<endl;
		for(auto edge: cycle[i]){
			int toNode = edge.first;
			int fromNode = G.E_source[edge.second.first];
			int toInEdge = edge.second.first;
			int childCycle = edge.second.second;
			int firCycle = nodeInCycleFirLas[toNode][0];
			//v6在2环跳过;v7在4环跳过
			// if(i == firCycle) continue;
			// cout<<"to edge "<<toInEdge+1<<" first cycle "<<firCycle<<" "
			// <<"to node"<<toNode<<" "<<cycle[firCycle][toNode].second+1<<endl;
			if(-1 == childCycle) continue;
			//叶子上的inEdge要删掉
			G.E_replace[toInEdge].push_back(cycle[firCycle][toNode].first);	
			report_leaf_replace++;
#ifdef TEST
			cout<<"toInEdge "<<toInEdge+1<<" delete "<<cycle[firCycle][toNode].first+1<<endl;
#endif
			// cout<<"to node "<<toNode<<" inEdge# "<<toInEdge+1<<" replace "<<cycle[firCycle][toNode].first+1<<endl;
			// while(toInEdge!=inEdge_of_cycle[firCycle]){
				// cout<<"cycle "<<firCycle<<endl;
			//如果儿子就是叶子，跳过
			if(firCycle == childCycle) continue;
			// int vhookChildtoParent = cycleRvs[childCycle][firCycle];
			// int childcorrespondingVInEdge = cycle[childCycle][vhookChildtoParent].first;
			//找儿子节点中，需要打开的环，对应的入边
			// G.E_replace[toInEdge].push_back(childcorrespondingVInEdge);
			G.E_replace[toInEdge].push_back(corDelEdge_inEdge_of_cycle[childCycle]);
			report_replace++;
			// cout<<"toisCycle "<<childCycle<<endl;
			// cout<<"to node "<<toNode<<" inEdge# "<<toInEdge+1<<" replace "<<cycle[childCycle][toNode].first+1<<"hook "<<vhookChildtoParent<<" "<<childcorrespondingVInEdge+1<<endl;
			// if(G.E_replace[childcorrespondingVInEdge].size() > 1){
			// 	for(int i=1; i<G.E_replace[childcorrespondingVInEdge].size(); i++){
			// 		if(toInEdge == childcorrespondingVInEdge) continue;
			// 		G.E_replace[toInEdge].push_back(G.E_replace[childcorrespondingVInEdge][i]);
			// 		cout<<"push_back "<<G.E_replace[childcorrespondingVInEdge][i]+1<<"   "<<toInEdge+1<<endl;
			// 	}
			// }
			
			// int childInEdge = cycle[toisCycle][toNode].first;				
			// G.E_replace[toInEdge].push_back(childInEdge);
			// for(auto ele:G.E_replace[childInEdge]){
			// 	G.E_replace[toInEdge].push_back(ele);
			// }
				// cout<<"true edge "<<toInEdge+1<<" in edge of cycle "<<inEdge_of_cycle[firCycle]+1<<endl;
				//解决v6向上到根
				// G.E_replace[toInEdge].push_back(inEdge_of_cycle[firCycle]);
				//找父亲节点
				// firCycle = parent_of_cycle[firCycle];
			// }
		}
	}
}
void queryByIndex(int& round, int qNode){
	int result = 0;
	n = G.n;
	m = G.m;
	edge_num = 1;
	for (int i = 0; i <= n; i++)
		V[i] = 0;
	for (int i = 0; i < m; i++)
	{
		edge_num++;
		E_target[edge_num] = G.E_target[i];
		E_cost[edge_num] = G.E_cost[i];
		O_E[edge_num] = G.E_number[i];
		list[edge_num] = V[G.E_source[i]];
		/* cout<<"nodeid "<<G.E_source[i]<<" V[nodeid] "<<V[G.E_source[i]]<<endl; */
		//		E_replace[edge_num] = G.E_replace[i] + 2;
		E_delete[edge_num] = false;
		V[G.E_source[i]] = edge_num;
		/* cout<<"nodeid "<<G.E_source[i]<<" V[nodeid] "<<V[G.E_source[i]]<<" edg# "<<edge_num<<" list[e#] "<<list[edge_num]<<endl; */
		/* cout<<"_"<<endl; */
	}
	// for(int i=0; i<m; i++){
	// 	cout<<i+1<<" "<<O_E[i+2]<<endl;
	// }
	// cout<<endl;
	/*____________*/
	/* for(int i=0;i<20;i++){ */
	/*     cout<<list[i]<<" "; */
	/* } */
	/* cout<<endl; */
	/* cout<<endl; */
	/* cout<<"______"<<endl; */
	// for(int i=1;i<=n;i++){
	//     cout<<"node "<<i<<" ";
	//     for(int outE=V[i];0!=outE;outE=list[outE]){
	//         cout<<outE-1<<" ";
	//     }
	//     cout<<endl;
	// }
	/*____________*/

	int count = 1;
	for (int i = 0; i <= n; i++)
		mark[i] = 0;
	mark[qNode] = 1;
	save.clear();
	for (int i = 0; i <= n; i++)
	{
		vector<int> v;
		v.clear();
		save.push_back(v);
	}

	for (int i = V[qNode]; i != 0; i = list[i]){
		if (O_E[i] < INTMAX)
		{
			save[O_E[i]].push_back(i);
			/* cout<<"save# "<<i-1<<" oei "<<O_E[i]<<endl; */
		}
	}
	// for(int i=0; i<=round; i++){
	// 	cout<<"round "<<i<<": ";
	// 	for(int j=0; j<save[i].size(); j++){
	// 		cout<<save[i][j]-1<<" ";
	// 	}
	// 	cout<<endl;
	// }
	
	queue<int> Q;
	while (!Q.empty())
		Q.pop();
	int _round = round;
	for (int round = 1; round <= _round; round++)
	{
		//		printf("round: %d\n", round);
		if (count >= n)
			break;
		for (int i = 0; i < save[round].size(); i++)
		{
			Q.push(save[round][i]);
			// cout << "round " << round << " e# " << save[round][i] - 1 << endl;
		}
		while (!Q.empty())
		{
			int _i = Q.front();
			int x = E_target[_i], y = E_cost[_i];
			Q.pop();
			if (mark[x] > 0)
			{
				// cout << "continued x " << x << endl;
				continue;
			}
			// cout << "x " << x << endl;

			//		printf("E_source[_i] E_target[_i] E_cost[_i]: %d %d %d\n", G.E_source[_i - 2], E_target[_i], E_cost[_i]);
			if (round > 1)
			{
				//	E_delete[E_replace[_i]] = true;
				for (int i = 0; i < G.E_replace[_i - 2].size(); i++){

					E_delete[G.E_replace[_i - 2][i] + 2] = true;
					// cout<<"round "<<round<<" delete "<<G.E_replace[_i - 2][i] + 1<<endl;
					// printf("(x1, y1) -> (x2, y2): (%d, %d) -> (%d, %d)\n", G.E_source[_i - 2], G.E_target[_i - 2],
					// 		G.E_source[E_replace[_i] - 2], G.E_target[E_replace[_i] - 2]);
				}
			}
			#ifdef TEST
			cout << "add edge " <<_i-1<<" "<< G.E_source[_i-2] << "->" << G.E_target[_i-2]<<" cost "<<y << endl;
			#endif
			result += y;
			mark[x] = 1;
			count++;

			// cout<<"cur node "<<x<<endl;
			for (int i = V[x]; i != 0; i = list[i])
			{
				// cout << "x " << x << " outEdge " << i - 1 << " E_DELETE " << E_delete[i] << endl;
				if (E_delete[i])
					continue;
				/* if (E_target[i] == 3) { */
				/*     cout<<"!!!to root skip "<<endl; */
				/*     continue; */
				/* } */
				// cout<<"O_E[i] "<<O_E[i]<<" round "<<round<<endl;
				if (O_E[i] <= round)
				{
					Q.push(i);
					// cout<<"QPUSH round"<< round <<endl;
					// cout<<" i "<<i-1<<endl;
					// cout<<"Q---"<<endl;
				}
				else
				{
					if (O_E[i] < INTMAX){
						save[O_E[i]].push_back(i);
						// cout<<"save push "<<O_E[i]<<endl;
						// cout<<" i "<<i-1<<endl;
						// cout<<"rs---"<<endl;
					}
				}
			}
		}
	}
	// cout<<"SAVE"<<endl;
	// for(int i=0; i<=round; i++){
	// 	for(int j=0; j<=save[i].size(); j++){
	// 		cout<<save[i][j]<<" ";
	// 	}
	// 	cout<<endl;
	// }
	// for(int i=0; i<=round; i++){
	// 	cout<<"round "<<i<<": ";
	// 	for(int j=0; j<save[i].size(); j++){
	// 		cout<<save[i][j]-1<<" ";
	// 	}
	// 	cout<<endl;
	// }
	//  if (count < n){ 
	// 	printf("impossible\n"); 
	//      return; 
	//  } else{
	// 	printf("%d\n", result);
	//  } 
		fprintf(fout_G, "%d %d\n", qNode, result);
}
// void querybyIndex_Oh(int qNode){
// 	for(int i=0; i<G.m ;i++){
// 		E_delete[i] = false;
// 	}
// 	int initCycle = nodeInCycleFirLas[qNode][0];
// 	int toInEdge = cycle[initCycle][qNode].second;
// 	E_delete[toInEdge] = true;
// 	while(true){
// 		for(auto ele:cycle[initCycle]){
// 			if(E_delete[ele.second.second]) continue;
// 			E_delete[ele.second.second] = true;
// 		}
// 		if(-1 == parent_of_cycle[initCycle]) break;
// 		E_delete[inEdge_of_cycle[initCycle]] = true;
// 		initCycle = parent_of_cycle[initCycle];
// 	}
// }
int char2int(char* qi){
	int i=0;
	while(qi[i]!='\0'){
		i++;
	}
	int res = 0;
	for(int cnt = i-1; cnt>=0; cnt--){
		res+=((qi[cnt]-'0')*pow(10, (i-cnt-1)));
	}
	return res;
}
void nchoosem(vector<int>& vq, int mchoose, int fromn){
	for(int i=0; i<fromn; i++){
		if(rand()%(fromn-i)<mchoose){
			vq.push_back(i+1);
			mchoose--;
		}
	}
} 
int main(int argc, char** argv){
	srand((int)time(0));
	char* in_graph = argv[1];
	string filename = in_graph;
	char* out_res = argv[2];
	//test
	// const int VNUM = 20, MNUM = 200;
	//usairport
	// const int VNUM = 1574+10, MNUM = 28236+10;
	//lexical network EAT
	// const int VNUM = 23132+10, MNUM = 312342+10;
	//web-berkstan
	// const int VNUM = 334856+10, MNUM = 4523219+10;
	//pokec
	/* const int VNUM = 1304536+10, MNUM = 29183654+10; */
	//wikicat
	//  const int VNUM = 1791489+10, MNUM = 28508141+10;
	//livejournal 4000k
	// const int VNUM = 3828682 + 10, MNUM = 65825429 + 10;
	//wikitalk 111k
	// const int VNUM = 111881 + 10, MNUM = 1477893 + 10;
	//epinion 32k
	// const int VNUM = 32223 + 10, MNUM = 443506 + 10;
	//wikiVote 1.3k
	// const int VNUM = 1300  + 10, MNUM = 39456 + 10;

	if(filename.find("test")!=string::npos){
		VNUM = 20, MNUM = 200;
	}
	if(filename.find("liveJournal")!=string::npos){
		VNUM = 3828682 + 10, MNUM = 65825429 + 10;
	}
	if(filename.find("pokec")!=string::npos){
		VNUM = 1304536+10, MNUM = 29183654+10;
	}
	if(filename.find("berkstan")!=string::npos){
		VNUM = 334856+10, MNUM = 4523219+10;
	}
	if(filename.find("wikiCat")!=string::npos){
		VNUM = 1791489+10, MNUM = 28508141+10;
	}
	if(filename.find("wikiTalk")!=string::npos){
		VNUM = 111881 + 10, MNUM = 1477893 + 10;
	}
	if(filename.find("epinion")!=string::npos){
		VNUM = 32223 + 10, MNUM = 443506 + 10;
	}
	if(filename.find("wikiVote")!=string::npos){
		VNUM = 1300  + 10, MNUM = 39456 + 10;
	}
	if(filename.find("eat")!=string::npos){
		VNUM = 23132+10, MNUM = 312342+10;
	}
	if(filename.find("usairport")!=string::npos){
		VNUM = 1574+10, MNUM = 28236+10;
	}
	V = (int* ) malloc(VNUM* sizeof(int ));
	E_target = (int*)malloc(MNUM*sizeof(int) );
	E_cost = (int*)malloc(MNUM*sizeof(int));
	list = (int*)malloc(MNUM*sizeof(int));
	O_E = (int*)malloc(MNUM*sizeof(int));
	_V = (int* ) malloc(VNUM* sizeof(int ));
	_E_target = (int*)malloc(MNUM*sizeof(int));
	_E_cost = (int*)malloc(MNUM*sizeof(int));
	_list = (int*)malloc(MNUM*sizeof(int));
	_O_E = (int*)malloc(MNUM*sizeof(int));
	E_replace = (int*)malloc(MNUM*sizeof(int));
	_E_replace = (int*)malloc(MNUM*sizeof(int));
	E_delete = (bool*)malloc(MNUM*sizeof(bool));
	
	G.E_target = (int*)malloc(MNUM*sizeof(int));
	G.E_cost = (int*)malloc(MNUM*sizeof(int));
	G.E_source = (int*)malloc(MNUM*sizeof(int));
	G.E_number = (int*)malloc(MNUM*sizeof(int));
	G.E_replace = new vector<int>[MNUM];

	chosen = (int* ) malloc(VNUM* sizeof(int ));
	mark = (int* ) malloc(VNUM* sizeof(int ));

	is_in_circle = (bool*)malloc(VNUM*sizeof(bool));
	new_number = (int* ) malloc(VNUM* sizeof(int ));



 	// fin_G = freopen("in.txt", "r",stdin);
	// fout_G = freopen("out.txt", "w",stdout);
 	// fin_G = freopen("inZurich.txt", "r",stdin);
	// fout_G = freopen("outZurich.txt", "w",stdout);
 	// fin_G = freopen("inReno.txt", "r",stdin);
	// fout_G = freopen("outReno.txt", "w",stdout);
 	// fin_G = fopen("scc_LiveJournal1.gr", "r");
	// fout_G = fopen("CLE_LiveJournal1_DMST.Res", "w");
 	/* fin_G = fopen("scc_pokec.gr", "r"); */
	/* fout_G = fopen("CLE_pokec_DMST.Res", "w"); */
 	// fin_G = fopen("scc_web-berkstan.gr", "r");
	// fout_G = fopen("CLE_web-berkstan_DMST.Res", "w");
 	//  fin_G = fopen("scc_wikiCat.gr", "r");
	//  fout_G = fopen("CLE_wikiCat_DMST.Res", "w");
 	// fin_G = fopen("scc_wikiTalk.gr", "r");
	// fout_G = fopen("CLE_WikiTalk_DMST.Res", "w");
 	// fin_G = fopen("scc_Epinion.gr", "r");
	// fout_G = fopen("CLE_Epinion_DMST.Res", "w");
 	// fin_G = fopen("scc_wikiVote.gr", "r");
	// fout_G = fopen("CLE_WikiVote_DMST.Res", "w");
 	// fin_G = fopen("scc_eat.gr", "r");
	// fout_G = fopen("CLE_eat_DMST.Res", "w");
 	// fin_G = fopen("scc_usairport.gr", "r");
	// fout_G = fopen("CLE_usairport_DMST.Res", "w");
 	fin_G = fopen(in_graph, "r");
	fout_G = fopen(out_res, "w");
	cout<<"start"<<endl;
	int round=0;
	// int qNode = char2int(argv[1]);
	clock_t idxstart = clock();
	buildIndex(round);
	clock_t idxend = clock();
	cout<<"index fininsh!"<<endl;
	fprintf(fout_G, "edge index size = %ld ,edge index size = %0.3lf GB, indexing time %0.6lf secs\n", \
	report_replace, report_replace*4.0/1000000000.0, (double)(idxend-idxstart)/CLOCKS_PER_SEC);
	fprintf(fout_G, "tree size size = %ld ,tree space size = %0.3lf GB, indexing time %0.6lf secs\n", \
	report_NumEdge, report_NumEdge*4.0/1000000000.0, (double)(idxend-idxstart)/CLOCKS_PER_SEC);
	fprintf(fout_G, "tree height %d, cycles %d, leaf_edges %d\n", round, report_cirlce, report_leaf_replace);
	// cout<<"AI~~~~~~~~~~~"<<endl;
	// for(int i=0; i<_m; i++){
	// 	cout<<"i "<<i+1<<" replace ";
	// 	for(int j=0; j<G.E_replace[i].size(); j++){
	// 		cout<<G.E_replace[i][j]+1<<" ";
	// 	}	
	// 	cout<<endl;
	// }
	// cout<<"hierarchical cycles "<<round<<endl;
	// for(int i=0; i<=m; i++){
	// 	cout<<i+1<<" "<<G.E_number[i]<<endl;
	// }
	// cout<<endl;
	vector<int> vq;
	clock_t nchoosemStart = clock();
	nchoosem(vq, 100, G.n);
	clock_t nchoosemEnd = clock();
	printf("query time %0.3lf\n", (double)(nchoosemEnd-nchoosemStart)/CLOCKS_PER_SEC);
	// cout<<"vq size "<<vq.size()<<endl;
	// fprintf(fout_G, "vq size %d\n", vq.size());
	cout<<"queryset Finish!"<<endl;
	clock_t queryStart = clock();
	for(auto qNode:vq){
		queryByIndex(round, qNode);	
	}
	clock_t queryEnd = clock();
	fprintf(fout_G, "query time %0.3lf\n", (double)(queryEnd-queryStart)/CLOCKS_PER_SEC);
	fclose(fout_G);
	fclose(fin_G);
	cout<<"Success!"<<endl;
} 
