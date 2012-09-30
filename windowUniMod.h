// ������� ��� � ��������� ����, � ���� ��������. 

struct wddControl
{
  int flags0;
  int group;
  int controlType;
  int status;
  int wndPtr;
  int BgColor;
  void* imageH;
  int EnabledRectColor;
  void*  imageEnabledH;
  int HiliteColor;
  void*  imageHiliteH;
  int DisabledRectColor;
  void*  imageDisabledH;
  int SelectedColor;
  void*  imageSelectedH;
  int offsetX;
  int offsetY;
  int TextColor;
  wchar_t String[60];
  int field_C0;
  int field_C4;
  void *FontPtr;
  wchar_t tooltipStr[62];
  int field_148;
};
struct wndStruct
{
	int wndId;         
	int flags;         
	int width;         
	int height;        
	int screenLeft;    
	int screenTop;     
	int screenRight;     
	int screenBottom;    
	void *someData;                    
	wddControl drawData;  
	int wndIdMB;         
	void *wndProc;       
	void *wndProcPre;      
	void *drawFn;          
	void *onHoverFnMB;
	void *nextWnd;         
	void *nextSibilingMB;  
	void *parentWindow;    
	void *firstChildMB;
};


struct listBoxDataStruct 
{
	short maxLines; /// ������ ���������� ������� ���� ��
	short LineHeight; //������ ����� �������
	int Param_3;
	int Bool_4;/// ���� �� 0 - �� ������ ����� ������������ � �������� ������� 0x401B �������� (��� ����)
	int Param_5;// bool - ���� ���������
	int Param_6;//0x10
	int LineCanUnSelected;
	/// ��� ��� ����� �������� ( � ��������� �� �����, ��� �������)
	void *SomeDataPtr; // +18 - ��������� �� ����� � �������
	void *buttonUp; // ���� ���� ��������� �� ������ �����
	void *buttonDown; // ���� ���� ��������� �� ������ ����
	void *slider; // ��������� �� ��� ���������
	int unk28;
	short freeLinesCount2C; //+2C
	short var2E;// ����� ������/��������� ��������� � ������
	int lineSelectIdx;
	short unk_34;
	short unk_36;
};
struct editBoxDataStruct
{
	//���� ������ - ������ ���� ������� � � ������ �������
	wchar_t Buf[0x100];
	wchar_t Buf2[0x100];
	int Password; // ���������� *****
	int onlyNumbers; // ��������� ��� �����
	int onlyAlphaNum;  // ��������� ��� �����
	int Param_6;
	short maxLen;//0x410 - MaxLenMB
	short entryWidth; // ������ ����� ��� ����� (����� ������ - � ��������)
	int Param_x414;
	int Param_x418;
	int Param_x41C;//Len - �������� �������������
};
struct scrollBoxDataStruct
{
	int Param_1;
	int Param_2;
	int Param_3;
	int Param_4;
};
struct staticTextDataStruct
{
	//���� ������ - ������ ���� ������� � � ������ �������
	wchar_t*DataPtr;
	int BoolA;
	int BoolB;
	size_t Size;
	wchar_t Buf[]; // ����� ��������� ������������ - ������� ��� �����  ����� ������, �� �� ������� ������
};


extern wndStruct *wndGetHandleByLua(int idx);
extern void *(__cdecl *noxCallWndProc)(void* Window,int Msg,int A,int B);
extern void *(__cdecl *noxWndLoad)(char const *WndName,void *WndProc);
extern int (__cdecl *noxWndGetPostion) (void* Window,int *xLeft,int *yTop);