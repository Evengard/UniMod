local tr=table.remove
local ti=table.insert

tr(package.loaders,4)
tr(package.loaders,3) -- ����� ��� �������� �������
--tr(package.loaders,1) -- �������
ti(package.loaders,bz2Loader) -- ��������� ����
bz2Loader=nil