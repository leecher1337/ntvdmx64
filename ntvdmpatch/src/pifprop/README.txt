How to compile?
---------------

You need 
 * Windows Server 2003 Platform SDK installed
 * Windows NT4 source
 * Windows 2000 source
 * If Windows NT4 or 2k source are in Archives, 7z installed and in search path.

1) prepare Path\to\NT4-src Path\to\Win2k-src
2) build Path\to\Win2k3-PlatformSDK

i.e.:

set PATH=%PATH%;c:\Program Files (x86)\7-zip
prepare x:\old-src.trunk.r687.20150728.7z x:\win2k-master.zip
build X:\PlatformSDK

