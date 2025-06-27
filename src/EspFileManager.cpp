#include "EspFileManager.h"
// #include "page.h"
#include "webPage.h"


#define _storage LittleFS
EspFileManager::EspFileManager(/* args */)
{

}

EspFileManager::~EspFileManager()
{
}


bool EspFileManager::initSDCard()
{


    if (!_storage.begin())
    // if (!_storage.begin())
    {
        DEBUGLF("Card Mount Failed");
        memory_ready = false;
        return false;
    }

    
    if(memory_ready)
    {
        // uint32_t cardSize = _storage.cardSize() / (1024 * 1024);
        // DEBUGX("SD Card Size: %lluMB\n", cardSize);
        // DEBUGL2("SD Card Size: ", cardSize);
    }
    return true;
}


void EspFileManager::listDir(const char * dirname, uint8_t levels)
{
    DEBUGX("Listing directory: %s\n", dirname);

    File root = _storage.open(dirname);
    if(!root){
        DEBUGLF("Failed to open directory");
        return;
    }
    if(!root.isDirectory()){
        DEBUGLF("Not a directory");
        return;
    }

    bool first_files = true;
    str_data = "";
    File file = root.openNextFile();
    while(file){
        if (first_files)
            first_files = false;
        else 
            str_data += ":";

        if(file.isDirectory()){
            // DEBUGF("  DIR : ");
            // DEBUGL(file.name());
            str_data += "1,";
            str_data += file.name();
            // if(levels){
            //     listDir(file.path(), levels -1);
            // }
        } else {
            // DEBUG("  FILE: ");
            // DEBUG(file.name());
            // DEBUG("  SIZE: ");
            // DEBUGL(file.size());
            str_data += "0,";
            str_data += file.name();
        }
        file = root.openNextFile();
    }

    file.close();
    // DEBUGL2("Folder string ", str_data);
}

void EspFileManager::setServer(AsyncWebServer *server)
{
    if (server == nullptr) {
        DEBUGLF("Server is null!");
        return;
    }
    _server = server;

    _server->on("/file", HTTP_GET, [&](AsyncWebServerRequest *request){ 
            AsyncWebServerResponse *response = request->beginResponse_P(200, "text/html", html_page, html_page_len);
            response->addHeader("Content-Encoding", "gzip");
            request->send(response);
            // request->send(200, "text/html", html_page); 
            // request->send(200, "text/plain", "Test route working");
        });

    server->on("/get-folder-contents", HTTP_GET, [&](AsyncWebServerRequest *request){
        DEBUGL2("path:", request->arg("path").c_str());
        listDir(request->arg("path").c_str(), 0);
        request->send(200, "text/plain", str_data);
    });

    server->on("/upload", HTTP_POST, [&](AsyncWebServerRequest *request)
        { request->send(200, "application/json", "{\"status\":\"success\",\"message\":\"File upload complete\"}"); }, [&](AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final){
        String file_path;

        file_path = "/";
        file_path += filename;

        if(!index)
        {
            DEBUGX("UploadStart: %s\n", file_path.c_str());
            if(_storage.exists(file_path)) 
            {
                _storage.remove(file_path);
            }
        }

        File file = _storage.open(file_path, FILE_APPEND);
        if(file) 
        {
            if(file.write(data, len) != len)
            {
                DEBUGLF("File write failed");
            }
            file.close();
        }
        if(final)
        {
            DEBUGX("UploadEnd: %s, %u B\n", file_path.c_str(), index+len);
        } 
    });

    server->on("/delete", HTTP_GET, [&](AsyncWebServerRequest *request){
        String path;
        if (request->hasParam("path")) 
        {
            path = request->getParam("path")->value();
        } 
        else 
        {
            request->send(400, "application/json", "{\"status\":\"error\",\"message\":\"Path not provided\"}");
            return;
        }

        DEBUGL2("Deleting File: ", path);
        if (_storage.exists(path)) 
        {
            _storage.remove(path);
            request->send(200, "application/json", "{\"status\":\"success\",\"message\":\"File deleted successfully\"}");
        } 
        else 
        {
            request->send(404, "application/json", "{\"status\":\"error\",\"message\":\"File not found\"}");
        } 
    });

    server->on("/download", HTTP_GET, [&](AsyncWebServerRequest *request){
        String path;
        if (request->hasParam("path")) 
        {
            path = request->getParam("path")->value();
        } 
        else 
        {
            request->send(400, "application/json", "{\"status\":\"error\",\"message\":\"Path not provided\"}");
            return;
        }
        DEBUGL2("Downloading File: ", path);
        if (_storage.exists(path)) 
        {
            request->send(_storage, path, String(), true);
        } 
        else 
        {
            request->send(404, "application/json", "{\"status\":\"error\",\"message\":\"File not found\"}");
        } 
    });
}
