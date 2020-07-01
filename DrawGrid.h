#pragma once


class CMapinfo:public CDataRecord{
private:
	string m_path;
protected:
	string m_filename;
public:
	inline string GetSavePath(){
		return m_path;
	}
	
	inline string GetResPath(){
		if(m_path.empty())return "";
		char fullpath[1024];
		strcpy(fullpath,m_path.c_str());
		char* ext = strchr(fullpath,'.');
		if(ext)*ext='\0';
		char* file = strrchr(fullpath,'\\');
		if(file){
			*file='\0';
			file++;
		}
		string outpath(file);
		outpath+=".jpg";
		return outpath;
	}

	inline string GetResOut(){
		char path[1024];
		char fullpath[1024];
		strcpy(fullpath,m_path.c_str());
		char* ext = strchr(fullpath,'.');
		if(ext)*ext='\0';
		char* file = strrchr(fullpath,'\\');
		if(file){
			*file='\0';
			file++;
		}

		sprintf(path,"%s\\%s\\%s",fullpath,file,file);
		return path;
	}

	void SetMask(int x,int y,MAP_STATE a,bool set){
		if(a>0){
			int& i = m_cells[y][x];
			if(set){
				//i=(i|(1<<a));
				i=(1<<a);
			}
			else{
				i=(i&(~(1<<a)));
			}
		}
	}
	
	void SetPath(string path){
		m_path = path;

		m_path = path;
		char fullpath[1024];
		strcpy(fullpath,m_path.c_str());
		char* ext = strchr(fullpath,'.');
		if(ext)*ext='\0';
		char* filename = strrchr(fullpath,'\\');
		if(filename){
			*filename='\0';
			filename++;
		}
		m_filename = filename;
	}

	bool FromFile(string path){
		SetPath(path);

		FILE* file;
		fopen_s(&file,m_path.c_str(),"rb");
		if( file )
		{
			int size=0;
			unsigned short version=0;
			unsigned short gwidth=0;
			unsigned short gheight=0;
			unsigned short width=0;
			unsigned short height=0;
			unsigned char tile_dir=0;
			fread(&version,2,1,file);
			fread(&gwidth,2,1,file);
			fread(&gheight,2,1,file);
			fread(&width,2,1,file);
			fread(&height,2,1,file);
			fread(&tile_dir,1,1,file);
			fread(&size,4,1,file);
			
			if( version != 4 )
			{
				MessageBox(NULL,"版本不支持",NULL,NULL);
			}

			int p=0;
			char b;unsigned char l;
			int si=0;
			int sj=0;
			while(p<size)
			{
                fread(&b,1,1,file);
				fread(&l,1,1,file);
				for(int i = 0;i < l;i++)
				{
					if(b>0){
						SetMask(si,sj,(MAP_STATE)b,true);
					}
					si++;
					if(si>=width){
						sj++;
						si=0;
					}
				}
				p+=2;
			}

			fclose(file);
			return true;
		}
		return false;
	}

	void CreatRes();
	void Save(string);
	inline void Clear(){
		m_cells.clear();
	}
};

class CDrawGrid:public CLayer,public CMapinfo
{
private:
	bool doMask;
	int cursorX;
	int cursorY;
public:
	CDrawGrid(int width, int height);
	~CDrawGrid(void);
	void OnDraw(HDC dc,int x,int y);
	void OnDrawMini(Graphics& g,float x,float y,float sx,float sy);
	void Hover(int x,int y);
	void SetMasks(int x,int y,MAP_STATE,bool);
	inline void GetPosition(int &x,int &y){
		x = cursorX;
		y = cursorY;
	}
	inline bool IsMaskNow(){return doMask;};
};

