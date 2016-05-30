#pragma once

//=============================================================================================================

enum FindPathResult
{
	NotDone,
	Failed,
	Success,
};

//=============================================================================================================

typedef unsigned T_GraphIndex;

const T_GraphIndex INVALID_INDEX = static_cast<T_GraphIndex>(-1);
const float COST_DEFAULT = 1.0f;

//=============================================================================================================

//search
const int VISITED = 0;
const int UNVISITED = -1;
const T_GraphIndex NO_PARRENT = static_cast<T_GraphIndex>(-2);

//=============================================================================================================

class Heuristic_Dijkstra
{
public:
	template <class graph_type>
	static float Calculate(const graph_type& g, T_GraphIndex nd1, T_GraphIndex nd2)
	{
			return 0;
	}
};

//=============================================================================================================

class Heuristic_Euclid
{
public:
	template <class graph_type>
	static float Calculate(const graph_type& g, T_GraphIndex nd1, T_GraphIndex nd2)
	{
		return (ensure_ref(g.GetNode(nd1)).position - ensure_ref(g.GetNode(nd2)).position).GetLength();
	}
};

//=============================================================================================================
//=============================================================================================================
//=============================================================================================================
//=============================================================================================================
//=============================================================================================================

#include <vector>

//=============================================================================================================

template<class KeyType>
class IndexedPriorityQLow
{
private:
	IndexedPriorityQLow operator=(const IndexedPriorityQLow&);
	std::vector<KeyType> & m_vecKeys;
	std::vector<int> m_Heap;
	std::vector<int> m_invHeap;
	int m_iSize;
	int m_iMaxSize;

	void Swap(int a, int b)
	{
		int temp = m_Heap[a];
		m_Heap[a] = m_Heap[b];
		m_Heap[b] = temp;

		//change the handles too
		m_invHeap[m_Heap[a]] = a;
		m_invHeap[m_Heap[b]] = b;
	}

	void ReorderUpwards(int nd)
	{
		while ( (nd>1) && (m_vecKeys[m_Heap[nd/2]] > m_vecKeys[m_Heap[nd]]) )
		{      
			Swap(nd/2, nd);
			nd /= 2;
		}
	}

	void ReorderDownwards(int nd, int HeapSize)
	{
		while (2*nd <= HeapSize)
		{
			int child = 2 * nd;
			if ((child < HeapSize) && (m_vecKeys[m_Heap[child]] > m_vecKeys[m_Heap[child+1]]))
			{
				++child;
			}

			if (m_vecKeys[m_Heap[nd]] > m_vecKeys[m_Heap[child]])
			{
				Swap(child, nd);
				nd = child;
			}
			else
			{
				break;
			}
		}
	}

public:

	IndexedPriorityQLow(std::vector<KeyType>& keys, int MaxSize)
		:m_vecKeys(keys),
			m_iMaxSize(MaxSize),
			m_iSize(0)
	{
		m_Heap.assign(MaxSize+1, 0);
		m_invHeap.assign(MaxSize+1, 0);
	}

	bool empty()const
	{
		return (m_iSize == 0);
	}

	void insert(const int idx)
	{
		check (m_iSize+1 <= m_iMaxSize && "Maximum reached!");
		++m_iSize;
		m_Heap[m_iSize] = idx;
		m_invHeap[idx] = m_iSize;
		ReorderUpwards(m_iSize);
	}

	int Pop()
	{
		Swap(1, m_iSize);
		ReorderDownwards(1, m_iSize-1);
		return m_Heap[m_iSize--];
	}

	void ChangePriority(const int idx)
	{
		ReorderUpwards(m_invHeap[idx]);
	}
};

