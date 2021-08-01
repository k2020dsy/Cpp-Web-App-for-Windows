import os,base64,sys

#start from here
#pack in python3

str1 = ''
Saveas = sys.argv[1]
# Read file
Base64FileCache = open("base64cache.cache", "r")

for line in Base64FileCache.readlines():                          #依次读取每行  
    line = line.strip()  #去掉每行头尾空白  
    str1 = str1 + line;
    print ("读取的数据为: %s" % (line))
 
# close file
Base64FileCache.close()

if len(Saveas) :# 把字符串还原成图片
    with open(Saveas,'wb') as str2image:
        str2image.write(base64.b64decode(str1))
        print("base64 decode finish||Python IDE")
        os.system("pause");
else:
    print("File reading ERROR,Cache is empty")

    





