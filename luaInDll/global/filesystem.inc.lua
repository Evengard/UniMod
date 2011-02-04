local tr=table.remove
local ti=table.insert

tr(package.loaders,4)
tr(package.loaders,3) -- нафиг нам двоичные лоадеры
--tr(package.loaders,1) -- прелоад
ti(package.loaders,bz2Loader) -- добавляем свой
bz2Loader=nil