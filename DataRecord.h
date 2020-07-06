#pragma once

enum MAP_STATE
{
	MAP_NORMAL=0,
	MAP_BLOCK=1,
	MAP_MARK=2,
	MAP_WATER=3,
};

typedef struct {
	int x;
	int y;
	int v;
}opstatus;

class CDataRecord
{
protected:
	map<int,map<int,int>> m_cells;

public:
	CDataRecord(void);
	~CDataRecord(void);
	int area(int x, int y);
	virtual bool FromFile(string)=0;
	virtual void Save(string)=0;
};


class CMapinfo:public CDataRecord{
private:
	string m_path;
protected:
	string m_filename;
public:
	string GetResPath();
	string GetResOut();
	void SetPath(string path);
	bool FromFile(string path);
	void CreatRes();
	void Save(string);
	inline void Clear(){
		m_cells.clear();
	}
	inline string GetSavePath(){
		return m_path;
	}
	void ReSetMask(opstatus& o);	
	bool SetMask(opstatus& o,MAP_STATE a,bool set);
};