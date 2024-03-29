#include <string>
#include <stdio.h>
#include <tgbot/tgbot.h>
#include <curl/curl.h>
#include "json.hpp"

using namespace std;
using json = nlohmann::json; // docs: https://github.com/nlohmann/json

string start_message = "Hello, welcome to the <strong>nm80 dictionary bot</strong>.\nSend a word.";

string lowercase(string text){
    for(auto & c : text){
        if(c >= 65 && c <= 90){
            c += 32;
        }
    }
    return text;
}

// parse merriam-webster text formatting (https://dictionaryapi.com/products/json#sec-2.fmttokens)
string formatter(string text){
    string first, second;
    size_t pos;
    
    first = "{bc}";
    second = "<strong>: </strong>";
    while(string::npos != (pos = text.find(first))){
        text.replace(pos, first.size(), second);
    }
    
    first = "{it}";
    second = "<i>";
    while(string::npos != (pos = text.find(first))){
        text.replace(pos, first.size(), second);
    }
    
    first = "{/it}";
    second = "</i>";
    while(string::npos != (pos = text.find(first))){
        text.replace(pos, first.size(), second);
    }
    
    first = "{b}";
    second = "<strong>";
    while(string::npos != (pos = text.find(first))){
        text.replace(pos, first.size(), second);
    }
    
    first = "{/b}";
    second = "</strong>";
    while(string::npos != (pos = text.find(first))){
        text.replace(pos, first.size(), second);
    }

    first = "{phrase}";
    second = "<strong>";
    while(string::npos != (pos = text.find(first))){
        text.replace(pos, first.size(), second);
    }
    
    first = "{/phrase}";
    second = "</strong>";
    while(string::npos != (pos = text.find(first))){
        text.replace(pos, first.size(), second);
    }
    
    first = "{ldquo}";
    second = "\"";
    while(string::npos != (pos = text.find(first))){
        text.replace(pos, first.size(), second);
    }
    
    first = "{rdquo}";
    second = "\"";
    while(string::npos != (pos = text.find(first))){
        text.replace(pos, first.size(), second);
    }

    return text;
}

static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp){
  ((std::string*)userp)->append((char*)contents, size * nmemb);
  return size * nmemb;
}

string learner_webster(string search){
    try{
        CURL * curl;
        // CURLcode res;
        search = lowercase(search);
        std::string readBuffer;
        std::string key = getenv("LEARNER_WEBSTER_API");
        std::string link = "https://www.dictionaryapi.com/api/v3/references/learners/json/" + search + "?key=" + key;
        curl = curl_easy_init();
        if(curl){
            curl_easy_setopt(curl, CURLOPT_URL, link.c_str());
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
            // res = curl_easy_perform(curl); 
            curl_easy_perform(curl); 
            curl_easy_cleanup(curl);
        }
        ofstream dic_log("dic_log", ios::app);
        try{
            json j = json::parse(readBuffer);
            stringstream out;
            dic_log << "-------------------------------------------------\n";
            for(unsigned entry = 0; entry < j.size(); entry++){
                try{
                    if(!j[entry]["meta"]["app-shortdef"].empty()){
                        string entry_title = j[entry]["meta"]["app-shortdef"]["hw"].get<string>();
                        if(string::npos != lowercase(entry_title).find(search)){ // check for exact related ones
                            entry_title = entry_title.substr(0, entry_title.find(":"));
                            string functional_label;
                            if (!j[entry]["meta"]["app-shortdef"]["fl"].empty()){
                                functional_label = " (" + j[entry]["meta"]["app-shortdef"]["fl"].get<string>() + ")";
                            }
                            dic_log << entry_title << endl;
                            out << "<strong>" << formatter(entry_title)
                                    << functional_label
                                    << ": \n" << "</strong>";
                            for(unsigned def = 0; def < j[entry]["meta"]["app-shortdef"]["def"].size(); def++){
                                string app_def_title;
                                if("" == j[entry]["meta"]["app-shortdef"]["def"][def]){
                                    out << "<strong> :  </strong>" << formatter(j[entry]["shortdef"][def].get<string>()) << "\n";
                                    break;
                                }
                                app_def_title = j[entry]["meta"]["app-shortdef"]["def"][def].get<string>();
                                out << formatter(app_def_title);
                                dic_log << "\tdebug:\t" << entry << "\t" << app_def_title << endl;
                                if(!j[entry]["shortdef"][0].empty()){
                                    string def_title = j[entry]["shortdef"][0].get<string>();
                                    if(string("{/it}") == app_def_title.substr(app_def_title.size() - 5)){
                                        out << "<strong> :  </strong>" << formatter(def_title);
                                    }
                                }
                                out << "\n";
                            }
                        }
                    }
                }catch(...){
                    dic_log << "\nproblem happened here" << endl;
                }
            }
            dic_log.close();
            if(out.str().empty()){
                return "Exact result not found :(\nTry to search different";
            }else{
                return out.str();
            }
        }catch(...){
            return "Not found :(";
        }
    }catch(...){
        return "Sorry, Bot couldn't work correctly for this word.\nWe'll work to fix it as soon as possible";
    }
}

int main() {
    ofstream tel_log("tel_log", ios::app);
    std::string BOT_API = getenv("NM80_DICTIONARY_BOT_API");
    TgBot::Bot bot(BOT_API);
    bot.getEvents().onCommand("start", [&bot](TgBot::Message::Ptr message) {
        bot.getApi().sendMessage(message->chat->id, start_message,false,0,std::make_shared<TgBot::GenericReply>(),"html", false);
    });
    bot.getEvents().onAnyMessage([&bot](TgBot::Message::Ptr message) {
        // printf("User wrote %s\n", message->text.c_str());
        if (StringTools::startsWith(message->text, "/start")) {
            return;
        }
        // bypass seeing messages from log channel:
        std::string log_channel = getenv("LOG_CHANNEL_ID");
        if (std::to_string(message->chat->id) == log_channel){
            return;
        }
        bot.getApi().sendMessage(message->chat->id, "<strong>Meanings of \"" + message->text + "\" from Merriam Webster Learner's Dictionary: \n\n</strong>" + learner_webster(message->text),false,0,std::make_shared<TgBot::GenericReply>(),"html", false);
    });
    try {
        tel_log << "bot name: " << bot.getApi().getMe()->firstName.c_str() << endl;
        // printf("Bot name: %s\n", bot.getApi().getMe()->firstName.c_str());
        TgBot::TgLongPoll longPoll(bot);
        while (true) {
            tel_log << "Long poll started\n";
            longPoll.start();
        }
    } catch (TgBot::TgException& e) {
        tel_log << "error: " << e.what();
    }
}
