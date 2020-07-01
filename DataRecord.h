#pragma once
class CDataRecord
{
protected:
	map<int,map<int,int>> m_cells;

public:
	CDataRecord(void);
	~CDataRecord(void);
	virtual bool FromFile(string)=0;
	virtual void Save(string)=0;
	inline int area(int x, int y){
		map<int,map<int,int>>::iterator iter = m_cells.find(y);
		if(iter!=m_cells.end()){
			map<int,int>::iterator iterx = iter->second.find(x);
			if(iterx!=iter->second.end()){
				return iterx->second;
			}
		}

		return 0;
	}
};

