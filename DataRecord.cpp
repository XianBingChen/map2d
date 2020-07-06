#include "stdafx.h"
#include "DataRecord.h"


CDataRecord::CDataRecord(void)
{
}


CDataRecord::~CDataRecord(void)
{
}

int CDataRecord::area(int x, int y){
	map<int,map<int,int>>::iterator iter = m_cells.find(y);
	if(iter!=m_cells.end()){
		map<int,int>::iterator iterx = iter->second.find(x);
		if(iterx!=iter->second.end()){
			return iterx->second;
		}
	}

	return 0;
}

void CMapinfo::CreatRes(){
	if(m_path.empty())return;
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

	FILE* fp;
	fopen_s(&fp,m_path.c_str(),"rb");
	if( !fp )return;

	unsigned short version=0;
	unsigned short gwidth=0;
	unsigned short gheight=0;
	unsigned short width=0;
	unsigned short height=0;
	fread(&version,2,1,fp);
	fread(&gwidth,2,1,fp);
	fread(&gheight,2,1,fp);
	fread(&width,2,1,fp);
	fread(&height,2,1,fp);
	fclose(fp);


	int mRealWidth = width*CLayer::CW();
	int mRealHeight = height*CLayer::CH();
	int wc = mRealHeight/512;
	int hc = mRealWidth/512;
	Bitmap* bmpCombine =new Bitmap(mRealWidth,mRealHeight);
	for(int i=0;i<=mRealHeight/512;i++){
		for(int j=0;j<=mRealWidth/512;j++){
			char path[256]={0};
			sprintf(path,"%s\\%s\\%s_r%d_c%d.jpg",fullpath,file,file,i+1,j+1);
			CLayer::CombinePic(*bmpCombine,path,j*512,i*512);
		}
	}
	CLayer::SavePicture(*bmpCombine,"image/jpeg",outpath.c_str());
	delete bmpCombine;
}

void CMapinfo::Save(string path){
	SetPath(path);
	FILE* file;
	fopen_s(&file,m_path.c_str(),"wb");
	if( file )
	{
		unsigned short version = 4;
		unsigned short width = CLayer::WW()/CLayer::CW();
		unsigned short height = CLayer::WH()/CLayer::CH();
		unsigned short gwidth=0,gheight=0;
		unsigned char tile_dir=0;
		int size = 0;
		fwrite(&version,2,1,file);
		fwrite(&gwidth,2,1,file);
		fwrite(&gheight,2,1,file);
		fwrite(&width,2,1,file);
		fwrite(&height,2,1,file);
		fwrite(&tile_dir,1,1,file);
		fwrite(&size,4,1,file);

		int pos =ftell(file)-4;

		int p=0;
		int l=0;
		int b = area(0,0);
		
		for(int y=0;y<=height;y++){
			for(int x=0;x<width;x++){
				int b2 = area(x,y);
				if(b2!=b || l==255){
					if(b & (1<<MAP_BLOCK)){
						fputc(MAP_BLOCK,file);
					}
					else
					if(b & (1<<MAP_MARK)){
						fputc(MAP_MARK,file);
					}					
					else{
						fputc(0,file);
					}
					fputc(l,file);

					l = 0;
					b = b2;
					size+=2;
				}
				l++;
			}
		}

		fseek(file,pos,SEEK_SET);		
		fwrite(&size,4,1,file);
		fclose(file);

	}
}
	
string CMapinfo::GetResPath(){
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

string CMapinfo::GetResOut(){
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

void CMapinfo::ReSetMask(opstatus& ret){
	int& i = m_cells[ret.y][ret.x];
	int o = i;
	i = ret.v;
	ret.v = o;

}

bool CMapinfo::SetMask(opstatus& ret,MAP_STATE a,bool set){
	if(a>0){
		int& v = m_cells[ret.y][ret.x];
		ret.v = v;
		if(set){
			//i=(i|(1<<a));
			v=(1<<a);
		}
		else{
			v=(v&(~(1<<a)));
		}
		if(ret.v!=v){
			return true;
		}
	}
	return false;
}
	
void CMapinfo::SetPath(string path){
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

bool CMapinfo::FromFile(string path){
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
		
		m_cells.clear();

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
					m_cells[sj][si]=(1<<b);
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