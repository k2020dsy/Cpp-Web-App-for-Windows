// 百度api.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//
#include "stdafx.h"
#include "utf-8.h"
string targetPic, templateImg;

string Base64Code(Base64 *base, string OriginalPicName) {
	string  encoded, encoddtest;
	ifstream f;
	//Open file
	f.open(OriginalPicName, ios_base::in | ios_base::binary);
	//Get file size
	int asd_file_size = getFile_size(OriginalPicName);
	if (!asd_file_size) MessageBox(NULL, "Cannot open file", "dialog", MB_ICONERROR);
	cout << "File_size1:" << asd_file_size << endl;
	unsigned char buffer1[1000000];
	//读取文件到buffer内
	f.seekg(0, std::ios_base::beg);     //读之前先将偏移量设置到文件开头
	f.read((char*)buffer1, asd_file_size);

	const unsigned char* b;
	//因为这个方法编码要求const unsigned char*，所以要将buffer转成相应的类型
	b = reinterpret_cast<const unsigned char*>(buffer1);
	//编码
	encoded = base->Encode(b, asd_file_size);
	cout << "Encode_Len:" << encoded.length() << endl;
	return encoded;

}

void UnCode(Base64 *base, string encoded, string resualSaveName, string OriginalPicName) {
	fstream fcache;
	fcache.open("base64cache.cache", ios::out);
	int asd_file_size = getFile_size(OriginalPicName.c_str());
	/*string normal;
	const char * str1 = encoded.c_str();
	cout << "UnCode()encoded:" << encoded << endl;
	normal = decode(encoded.c_str());//base->Decode(str1, strlen(encoded.data()));
	cout << "UnCode()encoded_Len:" << encoded.length() << endl;

	cout << "Decode:" << normal.c_str() << endl;
	//const char *qwe = normal.c_str();
	fb = fopen(resualSaveName.data(), "wb+");

	int asd_file_size = file_size((char*)OriginalPicName);

	cout << "OriginalFileSize:" << asd_file_size << endl;
	cout << "Decode_Len:" << normal.length() << endl;*/
	CChineseCode::GB2312ToUTF_8(encoded, (char*)encoded.c_str(), encoded.size());
	fcache << encoded;
	//fwrite(GBKToUTF8(encoded.c_str()).c_str(), asd_file_size, 1, fb);
	cout << encoded.c_str();
	//fclose(fb);
	cout << "Finish Cache,Running python to decode base64.\nPlease wait" << endl;
	string Commend("base64decoder ");
	Commend += resualSaveName=(strstr(resualSaveName.data(),".jpg"))? resualSaveName: resualSaveName+=".jpg";
	system(Commend.c_str());
}

// libcurl库下载链接：https://curl.haxx.se/download.html
// jsoncpp库下载链接：https://github.com/open-source-parsers/jsoncpp/
// 获取access_token所需要的url
const std::string access_token_url = "https://aip.baidubce.com/oauth/2.0/token?grant_type=client_credentials";
const static std::string request_url = "https://aip.baidubce.com/rest/2.0/face/v1/merge";
std::string faceMerge_result;
int faceMerge( const std::string &access_token);
// 回调函数获取到的access_token存放变量
// static std::string access_token_result;
/**
 * curl发送http请求调用的回调函数，回调函数中对返回的json格式的body进行了解析，解析结果储存在result中
 * @param 参数定义见libcurl库文档
 * @return 返回值定义见libcurl库文档
 */
static size_t callback(void *ptr, size_t size, size_t nmemb, void *stream) {
	// 获取到的body存放在ptr中，先将其转换为string格式
	std::string s((char *)ptr, size * nmemb);
	// 开始获取json中的access token项目
	Json::Reader reader;
	Json::Value root;
	// 使用boost库解析json
	reader.parse(s, root);
	std::string* access_token_result = static_cast<std::string*>(stream);
	*access_token_result = root["access_token"].asString();
	//cout<<"faceMerge return code:"<<faceMerge(*access_token_result)<<endl;
	faceMerge(*access_token_result);
	return size * nmemb;
}

/**
 * 用以获取access_token的函数，使用时需要先在百度云控制台申请相应功能的应用，获得对应的API Key和Secret Key
 * @param access_token 获取得到的access token，调用函数时需传入该参数
 * @param AK 应用的API key
 * @param SK 应用的Secret key
 * @return 返回0代表获取access token成功，其他返回值代表获取失败
 */
int get_access_token(std::string access_token, const std::string &AK, const std::string &SK) {
	CURL *curl;
	CURLcode result_code;
	int error_code = 0;
	curl = curl_easy_init();
	if (curl) {
		std::string url = access_token_url + "&client_id=" + AK + "&client_secret=" + SK;
		cout<< url <<endl;
		curl_easy_setopt(curl, CURLOPT_URL, url.data());
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0);
		std::string access_token_result;
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &access_token_result);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, callback);
		result_code = curl_easy_perform(curl);
		if (result_code != CURLE_OK) {
			fprintf(stderr, "get_access_token curl_easy_perform() failed: %s\n",
				curl_easy_strerror(result_code));
			return 1;
		}
		access_token = access_token_result;
		curl_easy_cleanup(curl);
		error_code = 0;
	}
	else {
		fprintf(stderr, "curl_easy_init() failed.");
		error_code = 1;
	}
	return error_code;
}

static size_t callbackF(void *ptr, size_t size, size_t nmemb, void *stream) {
	// 获取到的body存放在ptr中，先将其转换为string格式
	//faceMerge_result += std::string((char *)ptr, size * nmemb);

	std::string html_data(reinterpret_cast<const char*>(ptr), size * nmemb);

	*(reinterpret_cast<std::ostringstream*>(stream)) << html_data;

	cout<<"\n\nnmemb:"<< nmemb <<"\n"<<endl;

	return size * nmemb;
}
/**
 * 人脸融合
 * @return 调用成功返回0，发生错误返回其他错误码
 */
int faceMerge(const std::string &access_token) {
	
	
	
	Base64 *base = new Base64();
    std::string url = request_url + "?access_token=" + access_token;
	//cout << access_token<<endl;
    CURL *curl = NULL;
    CURLcode result_code;
	std::ostringstream writedata;
    int is_success;
    curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url.data());
        curl_easy_setopt(curl, CURLOPT_POST, 1);

		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0);
		
        curl_slist *headers = NULL;
        headers = curl_slist_append(headers, "Content-Type:application/json;charset=UTF-8");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
		string TargetImgDate = Base64Code(base, targetPic);
		string templateImgData = Base64Code(base, templateImg);
		char*JsonDate = (char*)malloc(templateImgData.size()+ TargetImgDate.size()+300*sizeof(char));
		sprintf(JsonDate, "{\"image_template\":{\"image\":\"%s\",\"image_type\":\"BASE64\",\"quality_control\":\"NONE\"},\"image_target\":{\"image\":\"%s\",\"image_type\":\"BASE64\",\"quality_control\":\"NONE\"}}", templateImgData.data(), TargetImgDate.data());
		//string json = "{\"image_template\":{\"image\":\"" + Base64Code(base, targetPic) + "\",\"image_type\":\"BASE64\",\"quality_control\":\"NONE\"},\"image_target\":{\"image\":\""+ Base64Code(base, templateImg)+"\",\"image_type\":\"BASE64\",\"quality_control\":\"NONE\"}}";
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, JsonDate);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &writedata);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, callbackF);
		//curl_easy_setopt(curl, CURLOPT_MAXREDIRS, 5);
		//curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);
		//curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0);
					
		///curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, callbackF);
		
		
        result_code = curl_easy_perform(curl);
		
		faceMerge_result+= writedata.str();

        if (result_code != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n",
                    curl_easy_strerror(result_code));
            is_success = 1;
            return is_success;
        }
		
	
	
	//cout << "This is json result:" << json_result << endl;

	//UnCode(base, json_result, "uncoderes.jpg", targetPic);

        curl_easy_cleanup(curl);
        is_success = 0;
		free(JsonDate); JsonDate = NULL;
		base = NULL;free(base);
		
    } else {
        fprintf(stderr, "curl_easy_init() failed.");
        is_success = 1;
    }
	
    return is_success;
}

#define AK 0
#define SK 1

int main()
{
	char assert;
	do{
		int ireadLine = 0;
		string resB64, save_fullpath, Key[2];

		fstream readKey("serveskey.set64");
		if (!readKey.good()) {
			cout << "Enter key confirm error.Fail to open confirm file" << endl;
			return -1;
		}
		while (getline(readKey, Key[ireadLine])) { ireadLine++; }
		//cout <<" AK: "<< Key[AK] << " SK: " << Key[SK] << endl;
		cout << "\n\n******直接将图片拖入(低版本Windows请粘贴图片完整路径),并且按下回车继续*****\n\n" << endl;
		cout << "头像图片路径(拖入):"; cin >> targetPic;
		cout << "要融合的背景图片路径(拖入):"; cin >> templateImg;
		cout << "保存为(Fullpath):"; cin >> save_fullpath;
		if (templateImg.length() || targetPic.length() || save_fullpath.length()) {
			int retc = get_access_token(access_token_url, Key[AK], Key[SK]);
			cout << "Token return code:" << retc << endl;
			if (retc != NULL) return -1;
		Base64 *base = new Base64();

		Json::Reader reader;
		Json::Value root;
		// 使用boost库解析json
		reader.parse(faceMerge_result, root);
		resB64 = root["result"]["merge_image"].asString();
		if (!resB64.length()) { cout << "unexpected Json:" << faceMerge_result << endl; return -1; }
		cout << "This is merge Base64 len:" << resB64.length() << endl;
		UnCode(base, resB64, save_fullpath, targetPic);
		base = NULL; free(base);
		}

		
		cout << "\n\n\PRESS 'E' TO EXIT,OTHERS TO BE CONTIENUE :" << endl;
		cin >> assert;

	} while (assert != 'E');

	getchar();
	return 0;
}
