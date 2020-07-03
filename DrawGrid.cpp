#include "stdafx.h"
#include "DrawGrid.h"



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
		unsigned short width=CLayer::WW()/CLayer::CW();
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
		
		for(int y=0;y<height;y++){
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

CDrawGrid::CDrawGrid(int width, int height)
	:CLayer(width,height)
{
}


CDrawGrid::~CDrawGrid(void)
{
}

void CDrawGrid::OnDrawMini(Graphics& graphics,float x,float y,float sx,float sy){
	USES_CONVERSION;
	int i,j;
	map<int,map<int,int>>::iterator iter = m_cells.begin();
	for(;iter!=m_cells.end();iter++)
	{
		i = iter->first;
		map<int,int> row = iter->second;
		map<int,int>::iterator iterx = row.begin();
		for(;iterx!=row.end();iterx++)
		{
			j = iterx->first;
			int& mask = iterx->second;
			
			//if((CLayer::GetType()==MAP_BLOCK || !CLayer::GetType()) &&  mask & (1<<MAP_BLOCK))
			if(mask & (1<<MAP_BLOCK)){
				graphics.FillRectangle(GetBrush(Color::MakeARGB(0x66,0x88,0x99,0x12)), x+(j*CLayer::CW())*sx, y+(i*CLayer::CH())*sy, CLayer::CW()*sx, CLayer::CH()*sy);
			}
					
			//if((CLayer::GetType()==MAP_MARK || !CLayer::GetType()) &&  mask & (1<<MAP_MARK))
			if( mask & (1<<MAP_MARK)){
				graphics.FillRectangle(GetBrush(Color::MakeARGB(0x66,0xff,0x99,0x12)), x+(j*CLayer::CW())*sx, y+(i*CLayer::CH())*sy, CLayer::CW()*sx, CLayer::CH()*sy);
			}
		}
	}

	Gdiplus::FontFamily fontFamily(L"����");
	Gdiplus::Font font(&fontFamily, 16, Gdiplus::FontStyleRegular, Gdiplus::Unit::UnitPixel);

	bstr_t text = A2W(m_filename.c_str());
	if(wcslen(text)>0){
		switch (CLayer::GetType())
		{
		case MAP_BLOCK:
			text+="_";
			text+="�ϰ���";
		break;
		case MAP_MARK:
			text+="_";
			text+="�ڵ���";
		break;
		default:
			text+="_";
			text+="����";
			break;
		}

		if(CLayer::GetType()!=0){
			text+="[";
			text+=CLayer::isErase()?"��Ƥ":"Ǧ��";
			text+="]";
		}
	}

	graphics.DrawString(text, (INT)wcslen(text), &font,Gdiplus::PointF(x, y),GetBrush(Color::MakeARGB(0xFF,0x00,0xFF,0x00)));
}

void CDrawGrid::OnDraw(HDC dc,int x,int y){
	Graphics graphics(dc);


	int left = (x/CLayer::CW());
	int top = (y/CLayer::CH());

	int width = CLayer::width()+1;
	int height= CLayer::height()+1;

	float cw = (float)CLayer::CW()/CLayer::WS();
	float ch = (float)CLayer::CH()/CLayer::WS();

	float sx = (float)(-x%CLayer::CW())/CLayer::WS();
	float sy = (float)(-y%CLayer::CH())/CLayer::WS();


	if(!CLayer::GetType())return;
	for(int i=0;i<=height/ch+1;i++)
	{
		map<int,map<int,int>>::iterator iter = m_cells.find(i+top);
		if(iter!=m_cells.end()){
			for(int j=0;j<=width/cw+1;j++)
			{
				map<int,int>::iterator iterx = iter->second.find(j+left);

				if(iterx!=iter->second.end()){
					int& mask = iterx->second;
					
					//if((CLayer::GetType()==MAP_BLOCK || !CLayer::GetType()) &&  mask & (1<<MAP_BLOCK))
					if(mask & (1<<MAP_BLOCK)){
						graphics.FillRectangle(GetBrush(Color::MakeARGB(0x66,0x88,0x99,0x12)), sx+(j*cw), sy+(i*ch), cw, ch);
					}
					
					//if((CLayer::GetType()==MAP_MARK || !CLayer::GetType()) &&  mask & (1<<MAP_MARK))
					if(mask & (1<<MAP_MARK)){
						graphics.FillRectangle(GetBrush(Color::MakeARGB(0x66,0xFF,0x99,0x12)), sx+(j*cw), sy+(i*ch), cw, ch);
					}
				}
			}
		}
	}


	//ͨ��ѭ����������
	float offset = 0.0f;
	for(int i=0;i<=height;i++)
	{
		graphics.DrawLine(GetPen(Color::Black), 0.0f, sy+offset, (float)CLayer::width(), sy+offset);
		offset+=ch;
	}

	//ͨ��ѭ����������
	offset = 0.0f;
	for(int i=0;i<=width;i++)
	{
		graphics.DrawLine(GetPen(Color::Black), sx+offset, 0.0f, sx+offset, (float)CLayer::height());
		offset+=cw;
	}


	graphics.DrawRectangle(GetPen(Color::MakeARGB(0xFF,0xFF,0x00,0x00)),sx+((cursorX-left)*cw),sy+((cursorY-top)*ch),cw,ch);
}

void CDrawGrid::Hover(int x,int y){
	cursorX = x/CLayer::CW();
	cursorY = y/CLayer::CH();
}

void CDrawGrid::SetMaskOne(int x2,int y2,MAP_STATE a,bool set){
	if(x2<0 || y2<0)return;
	x2 /= CLayer::CW();
	y2 /= CLayer::CH();
	x2 -= CLayer::GetPenSize()/2;
	y2 -= CLayer::GetPenSize()/2;

	for(int i=0;i<CLayer::GetPenSize();i++){
		for(int j=0;j<CLayer::GetPenSize();j++){
			SetMask(x2+i,y2+j,a,set);
		}
	}
}

void CDrawGrid::SetMasks(int x2,int y2,MAP_STATE a,bool set){
	if(x2<0 || y2<0)return;
	x2/=CLayer::CW();
	y2/=CLayer::CH();
	int x1 = cursorX;
	int y1 = cursorY;
	if(cursorX>x2){
		x1 = x2;
		x2 = cursorX;
	}
	if(cursorY>y2){
		y1 = y2;
		y2 = cursorY;
	}
	
	for(int i = x1;i<=x2;i++){
		for(int j = y1;j<=y2;j++){
			SetMask(i,j,a,set);
		}
	}
}
